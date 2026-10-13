
#include "LogObject.h"

int g_log_index = 0;

pthread_mutex_t file_mutex;
pthread_mutexattr_t *file_mutex_attr = NULL;

void RunLogThread(void *vHandle)
{
	CLogObject *pLog_obj = (CLogObject *)vHandle;
	pLog_obj->ObjectLogEvent();
}

CLogObject::CLogObject()
{
	m_log_event = NULL;
}

CLogObject::~CLogObject()
{
}

void CLogObject::ObjectLogEvent()
{
	int i = 0, nCnt = 0;
	DWORD nWaitTime = 200;
	LOG_STRUCT *pLogST;

	for ( ; ; ) {
		if(m_start != TRUE) break;
		pLogST = LogWaitStruct(nWaitTime);
		if(pLogST == NULL){
			continue;
		}
		else{
			if(pLogST->log_yn == LOG_YN_EXIT) m_start = FALSE;
			if(m_start != TRUE) break;
			ObjectLogWrite(pLogST);
		}
	}
	ObjectLogTerminate();

	m_pid = 0;
}

void CLogObject::ObjectLogInit(const char *szName, unsigned short nBufCnt, const char *szLogPath)
{
	size_t i, nLen, nFind = FALSE;
	char szTmpCwd[MAX_STR], cCmp;

	strcpy_st(m_log_name, szName);
	cond_attr = NULL;
	sig_mutex_attr = NULL;
	m_pid = 0;

	Pthread_cond_init(&cond, cond_attr);
	Pthread_mutex_init(&sig_mutex, sig_mutex_attr);
	Pthread_mutex_init(&file_mutex, file_mutex_attr);

	m_log_event = new LOG_VECTOR();

	m_index = g_log_index++;
	m_buf_cnt = nBufCnt;
	m_log_pos = 0;
	sig_action = FALSE;
	sig_count = 0;
	__ago_day = 7;
	if(strcmp(szLogPath, "") == FALSE) strcpy_st(m_log_path, "../log");
	else strcpy_st(m_log_path, szLogPath);

	m_log_buff = new LOG_STRUCT[m_buf_cnt+1];
	memset(m_log_buff, 0, sizeof(LOG_STRUCT)*(m_buf_cnt+1));

#ifdef _WIN32
    GetCurrentDirectory(MAX_STR, szTmpCwd);
	cCmp = '\\';
#else
	getcwd(szTmpCwd, MAX_STR);
	cCmp = '/';
#endif

	nLen = strlen_st(szTmpCwd);
	for(i = nLen-1; i >= 0; i--){
		if(szTmpCwd[i] == cCmp) nFind = TRUE;
		if(nFind == TRUE){
			strncpy_st(m_szCurPath, szTmpCwd, i);
			m_szCurPath[i] = '\0';
			break;
		}
	}

	GetLocalDayProc(m_fd_date);
	m_fd_run = m_fd_sock = m_fd_sockerr = m_fd_sql = m_fd_sqlerr = 0; 
	m_fd_agt = m_fd_ssw = m_fd_ivr = m_fd_rec = m_fd_web = m_fd_etc = 0;
	m_fd_exc = m_fd_rsv = m_fd_link = 0;
	m_fd_pds = m_fd_pds_err = 0;

	ObjectLogDeleteFile();

}

void CLogObject::LogSetAgoday(unsigned int ago_day)
{
	__ago_day = ago_day;
}

void CLogObject::LogObjectStart(const char *szName, unsigned short nBufCnt, const char *szLogPath)
{
	if(m_start == TRUE) return;
	m_start = TRUE;
	if(m_init == TRUE) return;

	ObjectLogInit(szName, nBufCnt, szLogPath);

	ObjectLogInitDirectory();

	Pthread_create(&m_pid, (void*(*)(void*))RunLogThread, this);
	m_init = TRUE;
}

LOG_STRUCT * CLogObject::LogWaitStruct(DWORD nWaitTime)
{
	int nWaitType;
	LOG_STRUCT *pLog_obj = NULL;

	if(m_init != TRUE) return NULL;
	if(m_start != TRUE) return NULL;

#ifdef _WIN32
	timespec *tsptr = (timespec*)(nWaitTime);
	if(nWaitTime <= 0){
		// 무제한 기다린다.
		nWaitType = Pthread_cond_wait(&cond, &sig_mutex);
	}
	else{
		// 일정시간 기다린다.
		if(sig_count > 0) nWaitType = TRUE;
		else nWaitType = Pthread_cond_waittime(&cond, &sig_mutex, tsptr);
	}
	if(nWaitType == TRUE){
		Pthread_mutex_lock(&sig_mutex);
		pLog_obj = LogWaitGetMsg(1);
		if(sig_count > 0) sig_count--;
		sig_action = FALSE;
		Pthread_mutex_unlock(&sig_mutex);
	}

#else
	timespec tsptr;
	long lNanoDelay;
	int nRet = -1;

	Pthread_mutex_lock(&sig_mutex);

	if(nWaitTime <= 0){
		// 무제한 기다린다.
		while(sig_action == FALSE)
		{
			Pthread_cond_wait(&cond, &sig_mutex);
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

		if(sig_count > 0) nWaitType = TRUE;
		else{
			while(sig_action == FALSE && nRet != ETIMEDOUT){
				//nRet = Pthread_cond_waittime(&cond, &sig_mutex, &tsptr);
				nRet = pthread_cond_timedwait(&cond, &sig_mutex, &tsptr);
			}
			nWaitType = (nRet == ETIMEDOUT) ? FALSE : TRUE;
		}
	}

	if(nWaitType == TRUE){
		pLog_obj = LogWaitGetMsg(2);
		// 자동 리셋으로 설정되어 있다면 상태를 리셋해야 한다.
		if(sig_count > 0) sig_count--;
		if(pLog_obj == NULL) sig_action = FALSE;
	}

	Pthread_mutex_unlock(&sig_mutex);

#endif

	return pLog_obj;
}

void CLogObject::LogObjectPush(char *szLog, int nLogType, int log_yn)
{
	char szLocaltime[21] = "";
	LOG_STRUCT *logST = NULL;

	if(m_init != TRUE) return;
	if(m_start != TRUE) return;

	Pthread_mutex_lock(&sig_mutex);

	int log_pos = m_log_pos++;
	if(m_log_pos >= m_buf_cnt) m_log_pos = 0;
	logST = &m_log_buff[log_pos];
	logST->log_type = nLogType;
	logST->log_yn = log_yn;
	LogLocalTimeProc(szLocaltime);
	sprintf(logST->szLog, "%s %s\r\n", szLocaltime, szLog);
	m_log_event->push_back(logST);
	sig_action = TRUE;						//sig count 할동안은 Wait 로직에서 lock을 건다.
	sig_count++;
	Pthread_mutex_unlock(&sig_mutex);

	Pthread_cond_signal(&cond);
}

LOG_STRUCT * CLogObject::LogWaitGetMsg(int nIndex)
{
	int nCnt = 0;
	LOG_STRUCT *pLog_obj = NULL;
	LOG_ITER iter;

	if(m_log_event != NULL){
		if(m_log_event->empty() == FALSE){
			iter = m_log_event->begin();
			pLog_obj = *iter;
			m_log_event->erase(iter);
		}
	}

	return pLog_obj;
}

void CLogObject::ObjectLogInitDirectory()
{
	char szCmd[101] = "";
	char szDay[21] = "", szTime[21] = "";
	char szDir0[201] = "", szDir1[101] = "", szDir2[101] = "";

	LogLocalDayProc(szDay);
	LogLocalTimeProc(szTime);
	
#ifdef _WIN32
	CreateDirectory(m_log_path, NULL);
#else
	mkdir(m_log_path, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
#endif
}

void CLogObject::ObjectLogDeleteFile()
{
	char szCmd[101] = "";
	char szDay[21] = "";
	char szFile[201] = "";

	LogRemoveDayProc(szDay);

#ifdef _WIN32
	sprintf(szFile, "%s\\run_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\sock_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\sockerr_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);	
	sprintf(szFile, "%s\\sql_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\sqlerr_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\agt_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\ssw_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\ivr_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\rec_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\web_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\exc_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\rsv_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\link_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\pds_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\pdserr_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s\\etc_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
#else	
	sprintf(szFile, "%s/run_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/sock_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/sockerr_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/sql_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/sqlerr_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);		
	sprintf(szFile, "%s/agt_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/ssw_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/ivr_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/rec_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/web_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/exc_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/rsv_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/link_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/pds_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);
	sprintf(szFile, "%s/pdserr_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);	
	sprintf(szFile, "%s/etc_%s_%s.log", m_log_path, szDay, m_log_name); ObjectLogFileDelete(szFile);	
#endif


}

void CLogObject::ObjectLogFileDelete(char *filename)
{
#ifdef _WIN32
	DeleteFile(filename);
#else
	unlink(filename);
#endif
}

int CLogObject::ObjectLogCreateFile(int nType)
{
	int fd = 0;
	char filename[101] = "", szDay[21] = "", szFullFilename[101] = "";

	LogLocalDayProc(szDay);

	ObjectLogClose();

	switch(nType){
	  case LOG_OBJ_RUN: 
		sprintf(filename, "run_%s_%s.log", szDay, m_log_name);
		sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		fd = m_fd_run = ObjectLogOpen(szFullFilename);
		break;
	  case LOG_OBJ_SOCK:
		sprintf(filename, "sock_%s_%s.log", szDay, m_log_name);
		sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		fd = m_fd_sock = ObjectLogOpen(szFullFilename);
		break;
	  case LOG_OBJ_SOCK_ERR:
		sprintf(filename, "sockerr_%s_%s.log", szDay, m_log_name);
		sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		fd = m_fd_sockerr = ObjectLogOpen(szFullFilename);
		break;	 
	  case LOG_OBJ_SQL:
		sprintf(filename, "sql_%s_%s.log", szDay, m_log_name);
		sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		fd = m_fd_sql = ObjectLogOpen(szFullFilename);
		break;
	  case LOG_OBJ_SQL_ERR:
		sprintf(filename, "sqlerr_%s_%s.log", szDay, m_log_name);
		sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		fd = m_fd_sqlerr = ObjectLogOpen(szFullFilename);
		break;
	  case LOG_OBJ_AGT:
		sprintf(filename, "agt_%s_%s.log", szDay, m_log_name);
		sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		fd = m_fd_agt = ObjectLogOpen(szFullFilename);
		break;
	  case LOG_OBJ_SSW:
		  sprintf(filename, "ssw_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_ssw = ObjectLogOpen(szFullFilename);
		  break;
	  case LOG_OBJ_IVR:
		  sprintf(filename, "ivr_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_ivr = ObjectLogOpen(szFullFilename);
		  break;
	  case LOG_OBJ_REC:
		  sprintf(filename, "rec_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_rec = ObjectLogOpen(szFullFilename);
		  break;
	  case LOG_OBJ_WEB:
		  sprintf(filename, "web_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_web = ObjectLogOpen(szFullFilename);
		  break;
	  case LOG_OBJ_EXC:
		  sprintf(filename, "exc_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_exc = ObjectLogOpen(szFullFilename);
		  break;
	  case LOG_OBJ_RSV:
		  sprintf(filename, "rsv_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_rsv = ObjectLogOpen(szFullFilename);
		  break;
	  case LOG_OBJ_LINK:
		  sprintf(filename, "link_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_link = ObjectLogOpen(szFullFilename);
		  break;
	  case LOG_OBJ_PDS:
		  sprintf(filename, "pds_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_pds = ObjectLogOpen(szFullFilename);
		  break;
	  case LOG_OBJ_PDS_ERR:
		  sprintf(filename, "pdserr_%s_%s.log", szDay, m_log_name);
		  sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		  fd = m_fd_pds_err = ObjectLogOpen(szFullFilename);
		  break;
	  default:
		sprintf(filename, "etc_%s_%s.log", szDay, m_log_name);
		sprintf(szFullFilename, "%s/%s", m_log_path, filename);
		fd = m_fd_etc = ObjectLogOpen(szFullFilename);
		break;
	}
	return fd;
}

void CLogObject::ObjectLogGetFilename(char *szFullFilename, int nType)
{
	int i = 0;
	char filename[101] = "", szDay[21] = "";

	LogLocalDayProc(szDay);

	switch(nType){
		case LOG_OBJ_RUN: sprintf(filename, "run_%s_%s.log", szDay, m_log_name); break;			
		case LOG_OBJ_SOCK: sprintf(filename, "sock_%s_%s.log", szDay, m_log_name); break;	  
		case LOG_OBJ_SOCK_ERR: sprintf(filename, "sockerr_%s_%s.log", szDay, m_log_name); break;
		case LOG_OBJ_SQL: sprintf(filename, "sql_%s_%s.log", szDay, m_log_name); break;
		case LOG_OBJ_SQL_ERR: sprintf(filename, "sqlerr_%s_%s.log", szDay, m_log_name); break;	  
		case LOG_OBJ_AGT: sprintf(filename, "agt_%s_%s.log", szDay, m_log_name); break;
		case LOG_OBJ_SSW: sprintf(filename, "ssw_%s_%s.log", szDay, m_log_name); break;	  
		case LOG_OBJ_IVR: sprintf(filename, "ivr_%s_%s.log", szDay, m_log_name); break;	  	  
		case LOG_OBJ_REC: sprintf(filename, "rec_%s_%s.log", szDay, m_log_name); break;
		case LOG_OBJ_WEB: sprintf(filename, "web_%s_%s.log", szDay, m_log_name); break;	 
		case LOG_OBJ_EXC: sprintf(filename, "exc_%s_%s.log", szDay, m_log_name); break;	
		case LOG_OBJ_RSV: sprintf(filename, "rsv_%s_%s.log", szDay, m_log_name); break;	
		case LOG_OBJ_LINK: sprintf(filename, "link_%s_%s.log", szDay, m_log_name); break;
		case LOG_OBJ_PDS: sprintf(filename, "pds_%s_%s.log", szDay, m_log_name); break;
		case LOG_OBJ_PDS_ERR: sprintf(filename, "pdserr_%s_%s.log", szDay, m_log_name); break;
		default: sprintf(filename, "etc_%s_%s.log", szDay, m_log_name); break;
	}
	
#ifdef _WIN32
	sprintf(szFullFilename, "%s\\%s", m_log_path, filename);
#else
	sprintf(szFullFilename, "%s/%s", m_log_path, filename);
#endif
}

int CLogObject::ObjectLogGetFileFD(int nType)
{
	int fd = 0;
	char filename[101] = "";

	switch(nType){
	  case LOG_OBJ_RUN: fd = m_fd_run; break;
	  case LOG_OBJ_SOCK: fd = m_fd_sock; break;
	  case LOG_OBJ_SOCK_ERR: fd = m_fd_sockerr; break;
	  case LOG_OBJ_SQL: fd = m_fd_sql; break;
	  case LOG_OBJ_SQL_ERR: fd = m_fd_sqlerr; break;
	  case LOG_OBJ_AGT: fd = m_fd_agt; break;
	  case LOG_OBJ_SSW: fd = m_fd_ssw; break;
	  case LOG_OBJ_IVR: fd = m_fd_ivr; break;
	  case LOG_OBJ_REC: fd = m_fd_rec; break;
	  case LOG_OBJ_WEB: fd = m_fd_web; break;	
	  case LOG_OBJ_EXC: fd = m_fd_exc; break;
	  case LOG_OBJ_RSV: fd = m_fd_rsv; break;	
	  case LOG_OBJ_LINK: fd = m_fd_link; break;	
	  case LOG_OBJ_PDS: fd = m_fd_pds; break;	
	  case LOG_OBJ_PDS_ERR: fd = m_fd_pds_err; break;	
	  default: fd = m_fd_etc; break;
	}
	
	if(fd <= 0) fd = ObjectLogCreateFile(nType);
	return fd;
}

void CLogObject::LogObjectSIP(char *sipData, int nSend, int nLogType, char *szLocalIP, int nLocalPort, char *szRemoteIP, int nRemotePort)
{
	char szLog[MAX_LOG_CHAR] = "";

	if(m_init != TRUE) return;
	if(m_start != TRUE) return;

	if(nSend == TRUE){			//목적지를 이미 셋팅한 값으로 한다.
		sprintf(szLog, "send src:%s:%d -> dst:%s:%d %s",
			szLocalIP, nLocalPort, szRemoteIP, nRemotePort, sipData);
	}
	else{						//출발지를 새로 구한다.
		sprintf(szLog, "recv src:%s:%d -> dst:%s:%d %s",
			szRemoteIP, nRemotePort, szLocalIP, nLocalPort, sipData);
	}

//	LogObjectPush(szLog, LOG_OBJ_SIGNAL, nLogType);
}

int CLogObject::ObjectLogWrite(LOG_STRUCT *logST)
{
	int		fd;
	int		nWriteLen;
	char szDate[21];

	GetLocalDayProc(szDate);
	if(strcmp(m_fd_date, szDate) != FALSE){
		GetLocalDayProc(m_fd_date);
		ObjectLogClose();
		if(__ago_day > 0) ObjectLogDeleteFile();
	}

	switch(logST->log_yn){
	  case LOG_YN_NONE:
		break;

	  case LOG_YN_FILE:
		errno = 0;
		fd = ObjectLogGetFileFD(logST->log_type);		// 파일 핸들을 얻는다.
		nWriteLen = write(fd, logST->szLog, strlen_st(logST->szLog));
		break;

	case LOG_YN_PRINT:
		printf(logST->szLog);
		break;

	case LOG_YN_BOTH:
		printf(logST->szLog);
		fd = ObjectLogGetFileFD(logST->log_type);		// 파일 핸들을 얻는다.
		nWriteLen = write(fd, logST->szLog, strlen_st(logST->szLog));
		break;
	case LOG_YN_EXIT:
		return -1;
	default:
		printf("ObjectLogWrite error\n");
		break;
	}
	return TRUE;
}

void CLogObject::LogObjectExit()
{
	char szE[6] = "";
	strcpy_st(szE, (const char*)"exit");

	LogObjectPush(szE, LOG_OBJ_RUN, LOG_YN_EXIT);
	for(int i = 0; i < 50; i++){
		if(m_pid <= 0) break;
		else WaitMSleep(10);
	}
}

void CLogObject::ObjectLogTerminate()
{
	if(m_init == FALSE) return;
	m_init = FALSE;
	WaitMSleep(1);

	if(m_log_event != NULL){
		m_log_event->clear();
		delete m_log_event;
		m_log_event = NULL;
	}
	if(m_log_buff != NULL){
		delete m_log_buff;
		m_log_buff = NULL;
	}
	ObjectLogClose();
	
}

void CLogObject::LogObjectGetError(int nErrorValue, char *szObjectError)
{

	if(m_start != TRUE){
		strcpy_st(szObjectError, "SIP_ERROR_INIT_ERROR"); 
		return;
	}

	switch(nErrorValue){
	  case SIP_ERROR_NOERROR1:
		strcpy_st(szObjectError, "SIP_ERROR_NOERROR1"); 
		break;
	  case SIP_ERROR_NOERROR2:
		strcpy_st(szObjectError, "SIP_ERROR_NOERROR2"); 
		break;
	  case SIP_ERROR_NOERROR3:
		strcpy_st(szObjectError, "SIP_ERROR_NOERROR3"); 
		break;
	  case SIP_ERROR_NOERROR4:
		strcpy_st(szObjectError, "SIP_ERROR_NOERROR4"); 
		break;
	  case SIP_ERROR_NOERROR5:
		strcpy_st(szObjectError, "SIP_ERROR_NOERROR5"); 
		break;
	  case SIP_ERROR_SUCCESS:
		strcpy_st(szObjectError, "SIP_ERROR_SUCCESS"); 
		break;
	  case SIP_ERROR_SUCCESS2:
		strcpy_st(szObjectError, "SIP_ERROR_SUCCESS2"); 
		break;
	  case SIP_ERROR_BADREQUEST:
		strcpy_st(szObjectError, "SIP_ERROR_BADREQUEST"); 
		break;
	  case SIP_ERROR_UNAUTHORIZED:
		strcpy_st(szObjectError, "SIP_ERROR_UNAUTHORIZED"); 
		break;
	  case SIP_ERROR_PAYMENT:
		strcpy_st(szObjectError, "SIP_ERROR_PAYMENT");
		break;
	  case SIP_ERROR_FORBIDDON:
		strcpy_st(szObjectError, "SIP_ERROR_FORBIDDON");
		break;
	  case SIP_ERROR_NOT_FOUND:
		strcpy_st(szObjectError, "SIP_ERROR_NOT_FOUND");
		break;
	  case SIP_ERROR_METHOD_NOT_ALLOWED:
		strcpy_st(szObjectError, "SIP_ERROR_METHOD_NOT_ALLOWED");
		break;
	  case SIP_ERROR_NOT_ACCEPTABLE:
		strcpy_st(szObjectError, "SIP_ERROR_NOT_ACCEPTABLE");
		break;
	  case SIP_ERROR_PROXY_AUTHREQUIRE:
		strcpy_st(szObjectError, "SIP_ERROR_PROXY_AUTHREQUIRE");
		break;
	  case SIP_ERROR_REQUEST_TIMEOUT:
		strcpy_st(szObjectError, "SIP_ERROR_REQUEST_TIMEOUT");
		break;
	  case SIP_ERROR_GONE:
		strcpy_st(szObjectError, "SIP_ERROR_GONE");
		break;
	  case SIP_ERROR_REQ_ENTITY_LARGE:
		strcpy_st(szObjectError, "SIP_ERROR_REQ_ENTITY_LARGE");
		break;
	  case SIP_ERROR_REQ_LONG:
		strcpy_st(szObjectError, "SIP_ERROR_REQ_LONG");
		break;
	  case SIP_ERROR_UNSUPPORT_MEDIA:
		strcpy_st(szObjectError, "SIP_ERROR_UNSUPPORT_MEDIA");
		break;
	  case SIP_ERROR_UNSUPPORT_SCHEME:
		strcpy_st(szObjectError, "SIP_ERROR_UNSUPPORT_SCHEME");
		break;
	  case SIP_ERROR_BAD_EXTENSION:
		strcpy_st(szObjectError, "SIP_ERROR_BAD_EXTENSION");
		break;
	  case SIP_ERROR_EXT_REQUIRED:
		strcpy_st(szObjectError, "SIP_ERROR_EXT_REQUIRED");
		break;
	  case SIP_ERROR_INTERVAL_BRIEF:
		strcpy_st(szObjectError, "SIP_ERROR_INTERVAL_BRIEF");
		break;
	  case SIP_ERROR_TEMPO_UNAVAILABLE:
		strcpy_st(szObjectError, "SIP_ERROR_TEMPO_UNAVAILABLE");
		break;
	  case SIP_ERROR_CALL_TRANSATION:
		strcpy_st(szObjectError, "SIP_ERROR_CALL_TRANSATION");
		break;
	  case SIP_ERROR_LOOP_DETECT:
		strcpy_st(szObjectError, "SIP_ERROR_LOOP_DETECT");
		break;
	  case SIP_ERROR_MANY_HOOP:
		strcpy_st(szObjectError, "SIP_ERROR_MANY_HOOP");
		break;
	  case SIP_ERROR_ADD_INCOMPLETE:
		strcpy_st(szObjectError, "SIP_ERROR_ADD_INCOMPLETE");
		break;
	  case SIP_ERROR_AMBIGOUS:
		strcpy_st(szObjectError, "SIP_ERROR_AMBIGOUS");
		break;
	  case SIP_ERROR_BUSY_HERE:
		strcpy_st(szObjectError, "SIP_ERROR_BUSY_HERE");
		break;
	  case SIP_ERROR_REQ_TERMINATE:
		strcpy_st(szObjectError, "SIP_ERROR_REQ_TERMINATE");
		break;
	  case SIP_ERROR_NOT_ACCEPTABLE_HERE:
		strcpy_st(szObjectError, "SIP_ERROR_NOT_ACCEPTABLE_HERE");
		break;
	  case SIP_ERROR_REQ_PENDING:
		strcpy_st(szObjectError, "SIP_ERROR_REQ_PENDING");
		break;
	  case SIP_ERROR_MIME_ERR:
		strcpy_st(szObjectError, "SIP_ERROR_MIME_ERR");
		break;
	  case SIP_ERROR_SERVER_INTERNAL:
		strcpy_st(szObjectError, "SIP_ERROR_SERVER_INTERNAL");
		break;
	  case SIP_ERROR_NOT_IMPLEMENTED:
		strcpy_st(szObjectError, "SIP_ERROR_NOT_IMPLEMENTED");
		break;
	  case SIP_ERROR_BAD_GATEWAY:
		strcpy_st(szObjectError, "SIP_ERROR_BAD_GATEWAY");
		break;
	  case SIP_ERROR_SERVICE_UNAVAILABLE:
		strcpy_st(szObjectError, "SIP_ERROR_SERVICE_UNAVAILABLE");
		break;
	  case SIP_ERROR_SERVER_TIMEOUT:
		strcpy_st(szObjectError, "SIP_ERROR_SERVER_TIMEOUT");
		break;
	  case SIP_ERROR_NOT_SUPPORT:
		strcpy_st(szObjectError, "SIP_ERROR_NOT_SUPPORT");
		break;
	  case SIP_ERROR_MSG_TOO_LARGE:
		strcpy_st(szObjectError, "SIP_ERROR_MSG_TOO_LARGE");
		break;
	  case SIP_ERROR_BUSY_EVERY_WHERE:
		strcpy_st(szObjectError, "SIP_ERROR_BUSY_EVERY_WHERE");
		break;
	  case SIP_ERROR_DECLINE:
		strcpy_st(szObjectError, "SIP_ERROR_DECLINE");
		break;
	  case SIP_ERROR_DOES_NOT_EXITS:
		strcpy_st(szObjectError, "SIP_ERROR_DOES_NOT_EXITS");
		break;
	  case SIP_ERROR_SERVER_NOT_ACCEPTABLE:
		strcpy_st(szObjectError, "SIP_ERROR_SERVER_NOT_ACCEPTABLE");
		break;
	  case SIP_ERROR_SEND_BUFFER:
		strcpy_st(szObjectError, "SIP_ERROR_SEND_BUFFER");
		break;
	  case SIP_ERROR_RECORD_STATE:
		strcpy_st(szObjectError, "SIP_ERROR_RECORD_STATE");
		break;
	  case SIP_ERROR_CANCEL:
		strcpy_st(szObjectError, "SIP_ERROR_CANCEL");
		break;
	  default:
		sprintf(szObjectError, "UnKnown Error code:%d", nErrorValue);
		break;
	}
}

#ifdef _WIN32
void CLogObject::dir_delete(char *rm_path)
{
    char fileFound[MAX_STR];
    char subFolder[MAX_STR];
    WIN32_FIND_DATA info;
    HANDLE hp = NULL;
	memset(&info, 0, sizeof(WIN32_FIND_DATA));

    sprintf(fileFound, "%s\\*.*", rm_path);
    hp = FindFirstFile(fileFound, &info);   //디렉토리에 파일이 있는지 첫번째 파일만.
    do{
        if (!((strcmp(info.cFileName, ".")==0)||(strcmp(info.cFileName, "..")==0)))
        {
            if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)  //Sub디렉토리가 존재하는경우 
            {
				strcpy_st(subFolder, rm_path);
				strcat_st(subFolder, "\\");
				strcat_st(subFolder, info.cFileName);
                dir_delete(subFolder); /// {return (_Ptr == 0 ? _Nullstr() : _Ptr); } 
                RemoveDirectory(subFolder);
            }
            else
            {
                sprintf(fileFound, "%s\\%s", rm_path, info.cFileName);
                BOOL retVal = DeleteFile(fileFound);
            }
        }
    }while(FindNextFile(hp, &info));

    FindClose(hp);
	RemoveDirectory(rm_path);
}

#else

void CLogObject::dir_delete(char *rm_path)
{
	int	state		= 0;
	char	file_path[MAX_STR];

	struct	dirent	*dir_ent;
	DIR		*dp;

	if ((dp = opendir(rm_path)) == NULL) return;

	/**********************************************************************
	 * dir_ent 구조체에 할당되는 디렉토리 내의 파일명을 이용하여 삭제한다.
	 **********************************************************************/

	while ((dir_ent = readdir(dp)) != NULL)
	{
		if (strcmp(dir_ent->d_name, ".") == 0 || strcmp(dir_ent->d_name, "..") == 0)
			continue;

		memset(&file_path, 0, MAX_STR);
		sprintf_st(file_path, "%s/%s", rm_path, dir_ent->d_name);
		state = unlink(file_path);
	}

	state = rmdir(rm_path);
}
#endif

void CLogObject::LogRemoveDayProc(char *szLocalDay)
{
	time_t	localt;
	struct tm today_tm;
	time(&localt);
	localt -= __ago_day * (60 * 60 * 24);
	today_tm = *localtime(&localt);

	sprintf(szLocalDay, "%04d-%02d-%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
}

void CLogObject::LogLocalDayProc(char *szLocalDay)
{
	time_t	localt;
	struct tm today_tm;
	time(&localt);
	today_tm = *localtime(&localt);

	sprintf(szLocalDay, "%04d-%02d-%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
}

void CLogObject::LogLocalTimeProc(char *szLocalTime)
{

#ifdef _WIN32
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szLocalTime, "%02d:%02d:%02d.%03d", 
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
	timeval tv;
	struct tm today_tm;

	gettimeofday(&tv, NULL);
	today_tm = *localtime((time_t*)&tv.tv_sec);

	sprintf(szLocalTime, "%02d:%02d:%02d.%03d", 
        today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec, (int)(tv.tv_usec/1000));
#endif
}

int CLogObject::ObjectLogOpen(char *szFileName)
{
	int nRet;
	
	Pthread_mutex_lock(&file_mutex);
#ifdef _WIN32
	nRet = _open(szFileName, _O_CREAT|O_WRONLY|_O_BINARY|O_APPEND, 0777);
#else
	nRet = open(szFileName, O_CREAT|O_WRONLY|O_NONBLOCK|O_APPEND, 0666);
#endif
	Pthread_mutex_unlock(&file_mutex);
	return nRet;
}

void CLogObject::ObjectLogClose()
{
	if(m_fd_run > 0){
		close(m_fd_run);
		m_fd_run = 0;
	}
	if(m_fd_sock > 0){
		close(m_fd_sock);
		m_fd_sock = 0;
	}
	if(m_fd_sockerr > 0){
		close(m_fd_sockerr);
		m_fd_sockerr = 0;
	}
	if(m_fd_sql > 0){
		close(m_fd_sql);
		m_fd_sql = 0;
	}
	if(m_fd_sqlerr > 0){
		close(m_fd_sqlerr);
		m_fd_sqlerr = 0;
	}	
	if(m_fd_agt > 0){
		close(m_fd_agt);
		m_fd_agt = 0;
	}
	if(m_fd_ssw > 0){
		close(m_fd_ssw);
		m_fd_ssw = 0;
	}
	if(m_fd_ivr > 0){
		close(m_fd_ivr);
		m_fd_ivr = 0;
	}
	if(m_fd_rec > 0){
		close(m_fd_rec);
		m_fd_rec = 0;
	}
	if(m_fd_web > 0){
		close(m_fd_web);
		m_fd_web = 0;
	}
	if(m_fd_exc > 0){
		close(m_fd_exc);
		m_fd_exc = 0;
	}
	if(m_fd_rsv > 0){
		close(m_fd_rsv);
		m_fd_rsv = 0;
	}
	if(m_fd_link > 0){
		close(m_fd_link);
		m_fd_link = 0;
	}
	if(m_fd_pds > 0){
		close(m_fd_pds);
		m_fd_pds = 0;
	}
	if(m_fd_pds_err > 0){
		close(m_fd_pds_err);
		m_fd_pds_err = 0;
	}
	if(m_fd_etc > 0){
		close(m_fd_etc);
		m_fd_etc = 0;
	}
}
