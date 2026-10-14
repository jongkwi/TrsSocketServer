/*
**	RecvWaitStack.cpp : 워커 큐 구현 (onmw RecvAgtWaitStack.cpp 를 일반화)
*/
#include "onlicDefine.h"
#include "RecvWaitStack.h"
#include "SockProcLic.h"
#include "AlarmProc.h"
#include <errno.h>
#include <time.h>

CRecvWait	*__lic_recv = NULL;
CRecvWait	*__event_recv = NULL;

/* ---------------- 초기화/종료 (SystemLoad / onlicmain 에서 호출) ---------------- */
int LicRecvWaitStackInit()
{
	if(__lic_recv != NULL) return FALSE;
	__lic_recv = new CRecvWait();
	return __lic_recv->Start("LicRecv", LicSocketProc, TRUE, FALSE, FALSE);
}

void LicRecvWaitStackKill()
{
	if(__lic_recv != NULL){ __lic_recv->Kill(); delete __lic_recv; __lic_recv = NULL; }
}

int EventRecvWaitStackInit()
{
	if(__event_recv != NULL) return FALSE;
	__event_recv = new CRecvWait();
	return __event_recv->Start("EventRecv", EventUdpProc, TRUE, sys_conf.atalk_use, sys_conf.sms_use);
}

void EventRecvWaitStackKill()
{
	if(__event_recv != NULL){ __event_recv->Kill(); delete __event_recv; __event_recv = NULL; }
}

/* ---------------- 워커 스레드 ---------------- */
void* RunRecvWaitThread(void *arg)
{
	CRecvWait *pWait = (CRecvWait*)arg;
	SOCK_RECV_WAIT item;
	int waitType = 0;

	OnlicLogf(LOG_SOFT, "RunRecvWaitThread[%s] start", pWait->m_szName);
	for( ; ; ){
		if(pWait->m_bExit == TRUE || onlic_start == FALSE) break;
		memset_st(&item, 0, sizeof(item));
		waitType = pWait->Wait(&item, 200);
		if(waitType <= 0){
			WaitMSleep(10);
			continue;
		}
		if(pWait->m_bExit == TRUE) break;
		switch(item.evt_type){
		  case RECV_TYPE_DATA:
		  case RECV_TYPE_ACT:
			if(pWait->m_fnHandler != NULL) pWait->m_fnHandler(pWait, &item);
			pWait->m_lProcCnt++;
			break;
		  case RECV_TYPE_EXIT:
			pWait->m_bExit = TRUE;
			break;
		  default:
			break;
		}
	}
	OnlicLogf(LOG_SOFT, "RunRecvWaitThread[%s] exit push:%ld proc:%ld drop:%ld", pWait->m_szName, pWait->m_lPushCnt, pWait->m_lProcCnt, pWait->m_lDropCnt);
	pWait->m_wait_pid = 0;
	return NULL;
}

/* ---------------- CRecvWait ---------------- */
CRecvWait::CRecvWait()
{
	memset_st(m_szName, 0, sizeof(m_szName));
	m_wait_pid = 0;
	m_bExit = FALSE;
	m_fnHandler = NULL;
	m_pLicDB = NULL; m_pAtalkDB = NULL; m_pSmsDB = NULL;
	m_lPushCnt = m_lDropCnt = m_lProcCnt = 0;
	m_sig_action = FALSE;
	m_sig_count = 0;
	m_run = FALSE;
	memset_st(m_wait_obj, 0, sizeof(m_wait_obj));
	m_nPos = 0;
	m_list = NULL;
}

CRecvWait::~CRecvWait()
{
	Kill();
}

int CRecvWait::Start(const char *szName, RECV_HANDLER fnHandler, int nUseLicDB, int nUseAtalkDB, int nUseSmsDB)
{
	if(m_run == TRUE) return FALSE;
	strncpy_st(m_szName, szName, sizeof(m_szName)-1);
	m_fnHandler = fnHandler;
	m_bExit = FALSE;
	m_sig_action = FALSE;
	m_sig_count = 0;
	m_nPos = 0;
	Pthread_cond_init(&m_cond, NULL);
	Pthread_mutex_init(&m_mutex, NULL);
	m_list = new RECV_WAIT_VECTOR();
	m_run = TRUE;

	/* 워커 전용 DB 핸들. 실패해도 기동 (사용 시 재접속) */
	if(nUseLicDB == TRUE){
		m_pLicDB = new CDirectODBC();
		if(m_pLicDB->DbConnectODBC(sys_conf.db_lic, sys_conf.client_encoding) != TRUE)
			OnlicLogf(LOG_SOFT, "[%s] lic db connect fail - retry on use", m_szName);
	}
	if(nUseAtalkDB == TRUE){
		m_pAtalkDB = new CDirectODBC();
		if(m_pAtalkDB->DbConnectODBC(sys_conf.db_atalk, sys_conf.client_encoding) != TRUE)
			OnlicLogf(LOG_SOFT, "[%s] atalk db connect fail - retry on use", m_szName);
	}
	if(nUseSmsDB == TRUE){
		m_pSmsDB = new CDirectODBC();
		if(m_pSmsDB->DbConnectODBC(sys_conf.db_sms, sys_conf.client_encoding) != TRUE)
			OnlicLogf(LOG_SOFT, "[%s] sms db connect fail - retry on use", m_szName);
	}

	Pthread_create(&m_wait_pid, RunRecvWaitThread, this);
	return TRUE;
}

void CRecvWait::Push(const SOCK_RECV_WAIT &item)
{
	SOCK_RECV_WAIT *p = NULL;
	if(m_run != TRUE) return;
	Pthread_mutex_lock(&m_mutex);
	if((int)m_list->size() >= MAX_RECV_WAIT_CNT){
		m_lDropCnt++;			/* 링 버퍼 덮어쓰기 방지: 가득 차면 버림 (onmw 는 조용히 덮어씀) */
		Pthread_mutex_unlock(&m_mutex);
		OnlicLogf(LOG_SOFT, "[%s] queue full, drop (size:%d)", m_szName, (int)m_list->size());
		return;
	}
	p = &m_wait_obj[m_nPos++];
	if(m_nPos >= MAX_RECV_WAIT_CNT) m_nPos = 0;
	memcpy(p, &item, sizeof(SOCK_RECV_WAIT));
	m_list->push_back(p);
	m_sig_action = TRUE;
	++m_sig_count;
	m_lPushCnt++;
	Pthread_mutex_unlock(&m_mutex);
	Pthread_cond_signal(&m_cond);
}

int CRecvWait::Wait(SOCK_RECV_WAIT *pItem, DWORD nWaitMs)
{
	struct timespec ts;
	int nRet = -1, nWaitType = FALSE;

	if(m_run != TRUE){ WaitSleep(1); return -1; }
	Pthread_mutex_lock(&m_mutex);
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += nWaitMs / 1000;
	ts.tv_nsec += (nWaitMs % 1000) * 1000000L;
	if(ts.tv_nsec >= 1000000000L){ ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
	if(m_sig_count > 0) nWaitType = TRUE;
	else{
		while(m_sig_action == FALSE && nRet != ETIMEDOUT){
			nRet = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
		}
		nWaitType = (nRet == ETIMEDOUT) ? FALSE : TRUE;
	}
	if(nWaitType == TRUE){
		nWaitType = GetMsg(pItem);
		if(m_sig_count > 0) m_sig_count--;
		if(nWaitType == FALSE) m_sig_action = FALSE;
	}
	Pthread_mutex_unlock(&m_mutex);
	return nWaitType;
}

/* m_mutex 보유 상태에서 호출 */
int CRecvWait::GetMsg(SOCK_RECV_WAIT *pItem)
{
	SOCK_RECV_WAIT *p = NULL;
	if(m_list == NULL || m_list->empty()) return FALSE;
	p = m_list->front();
	m_list->erase(m_list->begin());
	if(p == NULL) return FALSE;
	memcpy(pItem, p, sizeof(SOCK_RECV_WAIT));
	memset_st(p, 0, sizeof(SOCK_RECV_WAIT));
	return TRUE;
}

int CRecvWait::GetListCount()
{
	int n = 0;
	Pthread_mutex_lock(&m_mutex);
	n = (m_list != NULL) ? (int)m_list->size() : 0;
	Pthread_mutex_unlock(&m_mutex);
	return n;
}

void CRecvWait::Kill()
{
	int i = 0;
	pthread_t tid = 0;
	if(m_run != TRUE) return;
	m_bExit = TRUE;
	Pthread_mutex_lock(&m_mutex);
	m_sig_action = TRUE; ++m_sig_count;
	Pthread_mutex_unlock(&m_mutex);
	Pthread_cond_signal(&m_cond);
	/* 워커가 DB 호출 중일 수 있다 (로그인 30초 + 문장 10초). 종료를 기다려 join 한다 (L5).
	** 45초가 지나도 안 끝나면 핸들러가 아직 DB 핸들을 쓰는 중이므로 해제하지 않고 둔다 (use-after-free 방지, 프로세스 종료 시 회수) */
	tid = m_wait_pid;
	for(i = 0; i < 450; i++){ if(m_wait_pid == 0) break; WaitMSleep(100); }
	if(m_wait_pid != 0){
		OnlicLogf(LOG_SOFT, "[%s] worker did not exit in 45s - queue/DB handles left for process exit", m_szName);
		m_run = FALSE;
		return;
	}
	if(tid != 0) pthread_join(tid, NULL);
	m_run = FALSE;
	if(m_list != NULL){ m_list->clear(); delete m_list; m_list = NULL; }
	if(m_pLicDB != NULL){ m_pLicDB->DisConnectODBC(); delete m_pLicDB; m_pLicDB = NULL; }
	if(m_pAtalkDB != NULL){ m_pAtalkDB->DisConnectODBC(); delete m_pAtalkDB; m_pAtalkDB = NULL; }
	if(m_pSmsDB != NULL){ m_pSmsDB->DisConnectODBC(); delete m_pSmsDB; m_pSmsDB = NULL; }
}
