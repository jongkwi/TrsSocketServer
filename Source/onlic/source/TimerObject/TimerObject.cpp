
#include "TimerObject.h"

int m_g_timer_index = 1;

void RunTimerThread(void *vHandle)
{
	CTimerObject *pTimer_obj = (CTimerObject *)vHandle;
	int i = 0, nCnt = 0, waitType, nFind = FALSE;
	int nWaitTime = 10, nRet;
	char szTimerName[51];
	TIMER_STRUCT tmObject;
	strcpy_st(szTimerName, pTimer_obj->__timer_name);
	if(pTimer_obj->timer_pid <= 0) pTimer_obj->TimerSleep(100);

	for ( ; ; ) {
		if(pTimer_obj->timer_thread_start != TRUE) break;
		if(nFind == TRUE) waitType = pTimer_obj->TimerWaitStruct((DWORD)10, &tmObject);
		else waitType = pTimer_obj->TimerWaitStruct((DWORD)100, &tmObject);
		if(waitType <= 0){
			if(pTimer_obj->timer_thread_start != TRUE) break;
			nFind = pTimer_obj->RunTimerProc();
			pTimer_obj->TimerSleep(10);
			continue;
		}
		else{
			if(pTimer_obj->timer_thread_start != TRUE) break;
			nRet = pTimer_obj->TimerSetListProc(&tmObject);
			pTimer_obj->TimerSleep(10);
			if(nRet < 0) break;
			nFind = TRUE;
		}
	}

	pTimer_obj->timer_thread_start = FALSE;
	pTimer_obj->timer_pid = 0;
}

CTimerObject::CTimerObject()
{
	m_pEventTimer = NULL;
	m_timer_event = NULL;
	timer_thread_init = FALSE;
	timer_thread_start = FALSE;
}

CTimerObject::~CTimerObject()
{
	TimerObjectKill();
}

void CTimerObject::TimerObjectInit(char *szTimerName)
{
	if(timer_thread_init == TRUE) return;

	timer_cond_attr = NULL;
	timer_mutex_attr = NULL;

	Timer_Pthread_cond_init(&timer_cond, timer_cond_attr);
	Timer_Pthread_mutex_init(&timer_mutex, timer_mutex_attr);

	timer_action = FALSE;
	timer_count = 0;
	__timer_id = m_g_timer_index++;
	strcpy_st(__timer_name, szTimerName);

	m_timer_event = new TIMER_VECTOR();
	timer_list_object.ListObjectInit();
	timer_thread_init = TRUE;
}

void CTimerObject::TimerObjectStart(char *szTimerName, void* pTimer)
{

	timer_thread_start = TRUE;

	m_pEventTimer = pTimer;

	TimerObjectInit(szTimerName);

	Timer_Pthread_create(&timer_pid, (void*(*)(void*))RunTimerThread, this);
}

int CTimerObject::TimerWaitStruct(DWORD nWaitTime, TIMER_STRUCT *pTData)
{
	int nWaitType;
	if(timer_thread_init != TRUE) return FALSE;
	if(timer_thread_start != TRUE) return FALSE;

#ifdef _WIN32
	timespec *tsptr = (timespec*)(nWaitTime);
	if(nWaitTime <= 0){
		// 무제한 기다린다.
		nWaitType = Timer_Pthread_cond_wait(&timer_cond, &timer_mutex);
	}
	else{
		// 일정시간 기다린다.
		if(timer_count > 0) nWaitType = TRUE;
		else nWaitType = Timer_Pthread_cond_waittime(&timer_cond, &timer_mutex, tsptr);
	}
	if(nWaitType == TRUE){
		Timer_Pthread_mutex_lock(&timer_mutex);
		nWaitType = TimerWaitGetMsg(pTData);
		if(timer_count > 0) timer_count--;
		timer_action = FALSE;
		Pthread_mutex_unlock(&timer_mutex);
	}

#else
	timespec tsptr;
	long lNanoDelay;
	int nRet = -1;

	Timer_Pthread_mutex_lock(&timer_mutex);

	if(nWaitTime <= 0){
		// 무제한 기다린다.
		while(timer_action == FALSE)
		{
			Timer_Pthread_cond_wait(&timer_cond, &timer_mutex);
		}
		nWaitType = TRUE;
	}
	else
	{
		// 일정시간 기다린다.
		clock_gettime(CLOCK_REALTIME, &tsptr);
		tsptr.tv_sec += nWaitTime/1000;
		lNanoDelay = (nWaitTime%1000) * 1000000;
		tsptr.tv_nsec += lNanoDelay;
		if(tsptr.tv_nsec >= 1000000000){
			++tsptr.tv_sec;
			tsptr.tv_nsec -= 1000000000;
		}
		if(timer_count > 0) nWaitType = TRUE;
		else{
			while(timer_action == FALSE && nRet != ETIMEDOUT){
				nRet = Timer_Pthread_cond_waittime(&timer_cond, &timer_mutex, &tsptr);
			}
			nWaitType = (nRet == ETIMEDOUT) ? FALSE : TRUE;
		}
	}
	
	if(nWaitType == TRUE){
		nWaitType = TimerWaitGetMsg(pTData);
		// 자동 리셋으로 설정되어 있다면 상태를 리셋해야 한다.
		if(timer_count > 0) timer_count--;
		if(nWaitType == FALSE) timer_action = FALSE;
	}

	Timer_Pthread_mutex_unlock(&timer_mutex);
#endif

	return nWaitType;
}

int CTimerObject::TimerWaitGetMsg(TIMER_STRUCT *pTData)
{
	TIMER_STRUCT *timerTData = NULL;
	TIMER_ITER iter;
	int nCnt = 0;

	if(timer_thread_init != TRUE) return FALSE;
	if(timer_thread_start != TRUE) return FALSE;

	if(m_timer_event->empty() == FALSE){
		iter = m_timer_event->begin();
		timerTData = *iter;
		m_timer_event->erase(iter);

		if(timerTData != NULL){ // 평촌성심 free 할때 죽는 경우 발생 - 210406
			memcpy(pTData, timerTData, sizeof(TIMER_STRUCT));
			free(timerTData);
			timerTData = NULL;
		}		
		return TRUE;
	}
	return FALSE;
}

int CTimerObject::TimerSetListProc(TIMER_STRUCT *pTData)
{
	int nRet = 0;

	if(timer_thread_init != TRUE) return FALSE;
	if(timer_thread_start != TRUE) return FALSE;

	switch(pTData->timer_type){
	  case T_TYPE_SET:
		TimerListAddTail(pTData);
		nRet = TRUE;
		break;
	  case T_TYPE_KILL:
		TimerListRemove(pTData);
		nRet = FALSE;
		break;
	  case T_NUM_KILL:
		TimerListNumRemove(pTData);
		nRet = FALSE;
		break;
	  case T_TYPE_END:
		TimerListRemoveAll();
		nRet = -1;
		break;
	  default:
		printf("timer error nTimerId:%d, szNumber:%s, timer_type:%d, lPeriod:%d\n",
					  pTData->nTimerId, pTData->szNumber, pTData->timer_type, pTData->lPeriod);
		break;
	}

	return nRet;
}

void CTimerObject::TimerListAddTail(TIMER_STRUCT *pTData)
{
	LISTPOS pos = NULL;
	int i = 0, nCnt = 0, nFind = FALSE;
	TIMER_STRUCT *pNewTData = NULL;
	TIMER_STRUCT *pTmpTObject = NULL;
 
	nCnt = timer_list_object.GetCount();
	pos = timer_list_object.GetHeadPosition();
	for(i = 0; i < nCnt; ){
		if(pos != NULL) pTmpTObject = (TIMER_STRUCT *)timer_list_object.GetAt(pos);
		else break;
		if(pTmpTObject != NULL){
			if(pTData->nTimerId == pTmpTObject->nTimerId && (strcmp(pTData->szNumber, pTmpTObject->szNumber) == FALSE)){
				nFind = TRUE;
				break;
			}
		}
		++i;
		timer_list_object.GetNext(pos);
	}

	// 기존에 있는 타이머인지 검사해서 없다면 새로 만들고 있다면 값을 입력한다.
	if(nFind == FALSE){
		pNewTData = (TIMER_STRUCT*)Malloc(sizeof(TIMER_STRUCT));
		if(pNewTData != NULL){ // 평촌성심 free 할때 죽는 경우 발생 - 210406
			memcpy(pNewTData, pTData, sizeof(TIMER_STRUCT));
			timer_list_object.AddTail(pNewTData);
		}
	}
	else{
		memcpy(pTmpTObject, pTData, sizeof(TIMER_STRUCT));
	}
}

void CTimerObject::TimerListRemove(TIMER_STRUCT *pTData)
{
	LISTPOS pos = NULL;
	int i = 0, nCnt = 0;
	TIMER_STRUCT *pTmpTObject = NULL;

	nCnt = timer_list_object.GetCount();
	pos = timer_list_object.GetHeadPosition();
	for(i = 0; i < nCnt; ){
		if(pos != NULL) pTmpTObject = (TIMER_STRUCT *)timer_list_object.GetAt(pos);
		else break;
		if(pTmpTObject != NULL){
			if(pTData->nTimerId == pTmpTObject->nTimerId && (strcmp(pTData->szNumber, pTmpTObject->szNumber) == FALSE)){
				timer_list_object.RemoveAt(pos);
				free(pTmpTObject);
				pTmpTObject = NULL;

				i = 0;
				nCnt = timer_list_object.GetCount();
				pos = timer_list_object.GetHeadPosition();
				continue;
			}
		}
		++i;
		timer_list_object.GetNext(pos);
	}
}

void CTimerObject::TimerListNumRemove(TIMER_STRUCT *pTData)
{
	LISTPOS pos = NULL;
	int i = 0, nCnt = 0;
	TIMER_STRUCT *pTmpTObject = NULL;

	nCnt = timer_list_object.GetCount();
	pos = timer_list_object.GetHeadPosition();
	for(i = 0; i < nCnt; ){
		if(pos != NULL) pTmpTObject = (TIMER_STRUCT *)timer_list_object.GetAt(pos);
		else break;
		if(pTmpTObject != NULL){
			if(strcmp(pTData->szNumber, pTmpTObject->szNumber) == FALSE){
				timer_list_object.RemoveAt(pos);
				free(pTmpTObject);
				pTmpTObject = NULL;
				
				i = 0;
				nCnt = timer_list_object.GetCount();
				pos = timer_list_object.GetHeadPosition();
				continue;
			}
		}
		++i;
		timer_list_object.GetNext(pos);
	}
}

void CTimerObject::TimerListRemoveAll()
{
	LISTPOS pos = NULL;
	TIMER_ITER iter;
	int i = 0, nCnt = 0;
	TIMER_STRUCT *pTmpTObject = NULL;
	
	if(timer_thread_init != TRUE) return;
	if(timer_thread_start != TRUE) return;

	if(m_timer_event != NULL){
		for( iter = m_timer_event->begin(); iter != m_timer_event->end() ;) { 
			pTmpTObject = *iter;
			m_timer_event->erase(iter);
			if(pTmpTObject != NULL){
				free(pTmpTObject);
				pTmpTObject = NULL;
			}
			iter++;
		}
		delete m_timer_event;
		m_timer_event = NULL;
	}

	nCnt = timer_list_object.GetCount();
	pos = timer_list_object.GetHeadPosition();
	for(i = 0; i < nCnt; ){
		if(pos != NULL) pTmpTObject = (TIMER_STRUCT *)timer_list_object.GetAt(pos);
		else break;
		if(pTmpTObject != NULL){
			timer_list_object.RemoveAt(pos);
			free(pTmpTObject);
			pTmpTObject = NULL;
			i = 0;
			nCnt = timer_list_object.GetCount();
			pos = timer_list_object.GetHeadPosition();
			continue;
		}
		++i;
		timer_list_object.GetNext(pos);
	}
}

int CTimerObject::RunTimerProc()
{
	LISTPOS pos = NULL;
	int i = 0, nCnt = 0, nFind = FALSE, nCorrectTime = 0;
	int64_t curTime;
	int64_t startTime;
	int64_t tm_delay;
    UINT *lo = ((UINT*) &curTime);
    UINT *hi = ((UINT*) &curTime) + 1;

	if(timer_thread_init != TRUE) return nFind;
	if(timer_thread_start != TRUE) return nFind;

	TIMER_STRUCT *pTmpTObject = NULL;

	nCnt = timer_list_object.GetCount();
	pos = timer_list_object.GetHeadPosition();

	for(i = 0; i < nCnt; ){
		if(pos != NULL) pTmpTObject = (TIMER_STRUCT *)timer_list_object.GetAt(pos);
		else break;
		if(pTmpTObject != NULL){
			curTime = TimerLocalTimeProc();

			//120814
			startTime = pTmpTObject->dwCurTime;
			nCorrectTime = (int)(curTime - startTime);
			if((nCorrectTime < 0) && (nCorrectTime > -2000)) {
				// 경과 시간이 제대로 되고 있는지 검사. 현재 시간이 시작 시간보다 작을 경우이다.200m 오차까지 허용

				++i;
				timer_list_object.GetNext(pos);
				continue;
			}

			if(curTime >= (pTmpTObject->dwCurTime+pTmpTObject->lPeriod)){

				tm_delay = curTime - (pTmpTObject->dwCurTime+pTmpTObject->lPeriod);
				pTmpTObject->dwCurTime = curTime;
				if(OnCallBackFunc != NULL) OnCallBackFunc(pTmpTObject->nTimerId, pTmpTObject->lPeriod, pTmpTObject->szNumber, m_pEventTimer, tm_delay);
				nFind = TRUE;
			}
		}
		++i;
		timer_list_object.GetNext(pos);
	}
	return nFind;
}

void CTimerObject::TimerObjectPush(TIMER_STRUCT *pTData)
{
	TIMER_STRUCT *pTObj = NULL;
	int nSize = 0;

	if(timer_thread_init != TRUE) return;
	if(timer_thread_start == FALSE) return;

	
	nSize = sizeof(TIMER_STRUCT);
	pTObj = (TIMER_STRUCT *)Malloc(nSize);

	if(pTObj != NULL){  // 평촌성심 free 할때 죽는 경우 발생 - 210406

		memcpy(pTObj, pTData, nSize);

		Timer_Pthread_mutex_lock(&timer_mutex);

		m_timer_event->push_back(pTObj);
		timer_action = TRUE;						//sig count 할동안은 Wait 로직에서 lock을 건다.
		timer_count++;
		Timer_Pthread_mutex_unlock(&timer_mutex);

		Pthread_cond_signal(&timer_cond);
	}
}

void CTimerObject::TimerObjectKill()
{
	int i;

	TIMER_STRUCT tSt;

	tSt.nTimerId = 0;
	tSt.timer_type = T_TYPE_END;
	TimerObjectPush(&tSt);

	for(i = 0; i < 10; i++){
		if(timer_pid <= 0) break;
		else TimerSleep(100);
	}
}



int64_t CTimerObject::TimerLocalTimeProc()
{
	int64_t ct;
	time_t  unixT;

	// 1970년 이후 유닉스 시간을 구한다.
    time(&unixT);
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	DWORD msecond = st.wMilliseconds;
#else
	timeval tv;
//	struct tm* today_tm;
	gettimeofday(&tv, NULL);
	DWORD msecond = (DWORD)(tv.tv_usec/1000);
#endif

	// 미리세컨단위로 변환한다.
	ct = ((int64_t)unixT*1000)+(int64_t)msecond;
	return ct;
}



void CTimerObject::TimerSleep(DWORD dwTime)
{
#ifdef _WIN32
	if(dwTime <= 0)
		dwTime = 0;
	Sleep(dwTime);
#else
	struct timespec interval;
	interval.tv_sec = (unsigned int)((dwTime)/1000);
	interval.tv_nsec = (((dwTime)-(interval.tv_sec*1000))*1000000);
	nanosleep(&interval, NULL);
#endif
}

void CTimerObject::Timer_Pthread_create(pthread_t *tid, void * (*func)(void *), void *arg)
{

#ifdef _WIN32
	long	th_id;
	th_id = _beginthread((void (__cdecl *)(void *))*func,  0, (void*)arg);
	if(tid)
		*tid = th_id;
#else
	int		n;
	if((n = pthread_create((pthread_t *)tid, 0, func, arg)) == 0){
		return;
	}
	printf("pthread_create error no:%d\n", n);
#endif
}

void CTimerObject::Timer_Pthread_cond_init(pthread_cond_t *__cond, pthread_condattr_t *__cond_attr)
{
#ifdef _WIN32
	__cond->InitEvent();
	return;
#else
#ifdef _FREEBSD
	*__cond = PTHREAD_COND_INITIALIZER;
	__cond_attr = NULL;
#else
	int		n;
	n = pthread_cond_init(__cond, __cond_attr);
	if(n == 0){
		return;
	}
	errno = n;
	printf("Pthread_cond_init error:%d\n", errno);
#endif
#endif
}

void CTimerObject::Timer_Pthread_mutex_init(pthread_mutex_t *__mutex, pthread_mutexattr_t *__mutex_attr)
{
#ifdef _WIN32
    InitializeCriticalSection(__mutex);
#else
#ifdef _FREEBSD
	*__mutex = PTHREAD_MUTEX_INITIALIZER;
	__mutex_attr = NULL;
#else
	int		n;
	n = pthread_mutex_init(__mutex, __mutex_attr);
	errno = n;
#endif
#endif
}

void CTimerObject::Timer_Pthread_mutex_lock(pthread_mutex_t *__mutex)
{
	if(__mutex == NULL){
		printf("Timer pthread_mutex_lock is NULL\n");
		return;
	}
#ifdef _WIN32
    EnterCriticalSection(__mutex);
#else
	int		n;
	if((n = pthread_mutex_lock(__mutex)) == 0)
		return;
	printf("Timer pthread_mutex_lock error:%d\n", n);
#endif
}

void CTimerObject::Timer_Pthread_mutex_unlock(pthread_mutex_t *__mutex)
{
	if(__mutex == NULL){
		printf("pthread_mutex_unlock is NULL\n");
		return;
	}
#ifdef _WIN32
    LeaveCriticalSection(__mutex);
#else
	int		n;
	if((n = pthread_mutex_unlock(__mutex)) == 0)
		return;
	printf("pthread_mutex_unlock error:%d\n", n);
#endif
}

int CTimerObject::Timer_Pthread_cond_wait(pthread_cond_t* __cond, pthread_mutex_t* __mutex)
{
	int ret = 0;
#ifdef _WIN32
    if(!__cond)
        return -1;

	ret = WaitForSingleObject(__cond->m_hObject, INFINITE);

	switch(ret){
	  case WAIT_OBJECT_0:
		ret = TRUE;
		break;
	  case WAIT_TIMEOUT:
	  case WAIT_ABANDONED:
	  default:
		ret = -1;
		__cond->InitEvent();
		TimerSleep(10);
		break;
	}
#else
	ret = pthread_cond_wait(__cond, __mutex);
#endif
	return ret;
}

int CTimerObject::Timer_Pthread_cond_waittime(pthread_cond_t* __cond, pthread_mutex_t* __mutex, timespec *tsptr)
{
	int ret = 0;
#ifdef _WIN32
    if(!__cond)
        return -1;

	ret = WaitForSingleObject(__cond->m_hObject, (unsigned long)tsptr);

	switch(ret){
	  case WAIT_ABANDONED:
		ret = -1;
		__cond->InitEvent();
		TimerSleep(10);
		break;
	  case WAIT_OBJECT_0:
		ret = TRUE;
		break;
	  case WAIT_TIMEOUT:
	  default:
		ret = FALSE;
		break;
	}
#else
	ret = pthread_cond_timedwait(__cond, __mutex, tsptr);
#endif
	return ret;
}

void CTimerObject::Timer_Pthread_cond_signal(pthread_cond_t* __cond)
{
#ifdef _WIN32
	if(!__cond)
		return;
	__cond->SetEvent();
#else
	int n = pthread_cond_signal(__cond);
	if (n < 0)
		printf("Timer_Pthread_cond_signal error:%s\n", strerror(n));
#endif
}

int CTimerObject::Timer_printf(char *buf, size_t size, const char *fmt, ...)
{
	int			n;
	va_list		ap;

	--size;
	va_start(ap, fmt);
	vsprintf_st(buf, fmt, ap);	/* Sigh, some vsprintf's return ptr, not length */
	n = (int)strlen_st(buf);
	va_end(ap);
	if (n >= (signed)size){
		err_Timer_end("Timer_printf:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x, overflowed array",
				fmt[0], fmt[1], fmt[2], fmt[3], fmt[4], fmt[5], fmt[6], fmt[7], fmt[8], fmt[9]);
		return 0;
	}
	return(n);
}

int CTimerObject::Timer_check_printf(char *buf, size_t size, const char *fmt, ...)
{
	int			n;
	va_list		ap;
	char		*sBuf=NULL;
	int			bSTR = true;

	--size;
	va_start(ap, fmt);
	vsprintf_st(buf, fmt, ap);	/* Sigh, some vsprintf's return ptr, not length */
	n = (int)strlen_st(buf);

	va_end(ap);
	if (n >= (signed)size)
		err_Timer_end("Timer_printf:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x, overflowed array",
				fmt[0], fmt[1], fmt[2], fmt[3], fmt[4], fmt[5], fmt[6], fmt[7], fmt[8], fmt[9]);
	return(n);
}

void CTimerObject::err_Timer_end(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_Timer_doit(0, 3, fmt, ap);
	va_end(ap);
}

void CTimerObject::err_Timer_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int		errno_save, n;
	char	buf[2047];

	errno_save = errno;		/* value caller might want printed */

	vsprintf_st(buf, fmt, ap);					/* not safe */

	n = (int)strlen_st(buf);
	if (errnoflag)
		printf(": %s\n", strerror(errno_save));
	strcat_st(buf, "\n");

#ifdef _UNIX
	syslog(level, buf);

#endif

	return;
}
