/*
 * pthreads wrapper functions.
 */

#include "ShareLib.h"

#ifdef _WIN32
TEventDummy::TEventDummy()
{
    m_hObject = ::CreateEvent(NULL, 0, 0, NULL);
    if(m_hObject)
        ::ResetEvent(m_hObject);
}

void TEventDummy::InitEvent()
{
    m_hObject = ::CreateEvent(NULL, 0, 0, NULL);
    if(m_hObject)
        ::ResetEvent(m_hObject);
}
BOOL TEventDummy::SetEvent()
{
    if(m_hObject)
        return ::SetEvent(m_hObject);
    return FALSE;
}
#endif

void Pthread_create(pthread_t *tid, void * (*func)(void *), void *arg)
{

#ifdef _WIN32
	long	th_id;
	th_id = _beginthread((void (__cdecl *)(void *))*func,  0, (void*)arg);
	if(tid)
		*tid = th_id;
#else
	CError err;
	int		n;
	if((n = pthread_create((pthread_t *)tid, 0, func, arg)) == 0)
		return;
	errno = n;
	err.err_msg("pthread_create error no:%d", n);
#endif
}

void Pthread_join(pthread_t  __th, void** __thread_return)
{
#ifdef _WIN32
    WaitForSingleObject((HANDLE)__th, INFINITE);
#else
	CError err;
	int		n;
	if((n = pthread_join(__th, __thread_return)) == 0)
		return;
	errno = n;
	err.err_msg("Pthread_join error");
#endif
}

void Pthread_kill(pthread_t *tid, int signo)
{

#ifdef _WIN32
	TerminateThread(tid, signo); 
#else
	CError err;
	int		n;
	
	if ( (n = pthread_kill(*tid, signo)) == 0)
		return;
	
	errno = n;
	err.err_msg("pthread_kill error");
#endif
}

void Pthread_mutex_init(pthread_mutex_t *__mutex, pthread_mutexattr_t *__mutex_attr)
{
#ifdef _WIN32
    InitializeCriticalSection(__mutex);
#else
#ifdef _FREEBSD
	*__mutex = PTHREAD_MUTEX_INITIALIZER;
	__mutex_attr = NULL;
#else
	int		n;
	__mutex_attr = NULL;
	n = pthread_mutex_init(__mutex, __mutex_attr);
	errno = n;
	if (n < 0)
		printf("Pthread_mutex_init error:%s\n", strerror(errno));
#endif
#endif
}

void Pthread_mutex_lock(pthread_mutex_t *__mutex)
{
	if(__mutex == NULL){
		printf("pthread_mutex_lock is NULL");
		return;
	}
#ifdef _WIN32
    EnterCriticalSection(__mutex);
#else
	CError err;
	int		n;
	if((n = pthread_mutex_lock(__mutex)) == 0)
		return;
	errno = n;
	err.err_msg("pthread_mutex_lock error:%d", n);
#endif
}

void Pthread_mutex_unlock(pthread_mutex_t *__mutex)
{
	if(__mutex == NULL){
		printf("pthread_mutex_unlock is NULL");
		return;
	}
#ifdef _WIN32
    LeaveCriticalSection(__mutex);
#else
	CError err;
	int		n;
	if((n = pthread_mutex_unlock(__mutex)) == 0)
		return;
	errno = n;
	err.err_msg("pthread_mutex_unlock error:%d", n);
#endif
}

void Pthread_cond_init(pthread_cond_t *__cond, pthread_condattr_t *__cond_attr)
{
#ifdef _WIN32
	__cond->InitEvent();
	return;
#else
#ifdef _FREEBSD
	*__cond = PTHREAD_COND_INITIALIZER;
	__cond_attr = NULL;
#else
	CError err;
	int		n;

	__cond_attr = NULL;
	n = pthread_cond_init(__cond, __cond_attr);
	if(n == 0){
		return;
	}
	errno = n;
	err.err_msg("Pthread_cond_init error");
#endif
#endif
}

int Pthread_cond_wait(pthread_cond_t* __cond, pthread_mutex_t* __mutex)
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
		WaitSleep(10);
		break;
	}
#else
	CError err;
	int		n;

	n = pthread_cond_wait(__cond, __mutex);
	switch(n){
	  case EINTR:
		ret = TRUE;
		break;
	  case ETIMEDOUT:
	  case EBUSY:
	  default:
		ret = -1;
		break;
	}
#endif
	return ret;
}

int Pthread_cond_waittime(pthread_cond_t* __cond, pthread_mutex_t* __mutex, timespec *tsptr)
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
		WaitSleep(10);
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
	CError err;
	int		n;

	n = pthread_cond_timedwait(__cond, __mutex, tsptr);
	switch(n){
	  case ETIMEDOUT:
		ret = 0;
		break;
	  case EINTR:
	  default:
		ret = TRUE;
		break;
	}
#endif
	return ret;
}

void Pthread_cond_signal(pthread_cond_t* __cond)
{
#ifdef _WIN32
	if(!__cond)
		return;
	__cond->SetEvent();
#else
	int n = pthread_cond_signal(__cond);
	if (n < 0)
		printf("Pthread_cond_signal error:%s\n", strerror(n));
#endif
}


