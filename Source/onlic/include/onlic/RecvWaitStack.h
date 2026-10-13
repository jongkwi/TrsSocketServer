/*
**	RecvWaitStack.h : 워커 큐 (onmw CAgtRecvWait 패턴: mutex + cond + 링 버퍼 + vector)
**	- 라이선스 워커 1개(__lic_recv), 이벤트 워커 1개(__event_recv). 워커마다 DB 핸들 보유 (리눅스참조구조 §3)
**	- Windows 는 단일 UI 스레드에서 직렬 처리했으므로 워커 1개 = 처리 순서 보존
*/
#ifndef ___ONLIC_RECV_WAIT_STACK_H___
#define ___ONLIC_RECV_WAIT_STACK_H___

#include <vector>

#ifndef MAX_RECV_WAIT_CNT
#define MAX_RECV_WAIT_CNT	2000
#endif

class CRecvWait;
typedef void (*RECV_HANDLER)(CRecvWait *pWait, SOCK_RECV_WAIT *pItem);
typedef std::vector<SOCK_RECV_WAIT*> RECV_WAIT_VECTOR;
typedef std::vector<SOCK_RECV_WAIT*>::iterator RECV_WAIT_ITER;

class CRecvWait
{
public:
	CRecvWait();
	virtual ~CRecvWait();

	int		Start(const char *szName, RECV_HANDLER fnHandler, int nUseLicDB, int nUseAtalkDB, int nUseSmsDB);
	void	Push(const SOCK_RECV_WAIT &item);			/* 소켓 스레드 → 워커 */
	int		Wait(SOCK_RECV_WAIT *pItem, DWORD nWaitMs);	/* 워커 내부 */
	int		GetListCount();
	void	Kill();

	char			m_szName[31];
	pthread_t		m_wait_pid;
	int				m_bExit;
	RECV_HANDLER	m_fnHandler;

	/* 워커 전용 DB 핸들 (연결 실패해도 사용 시점에 재접속) */
	CDirectODBC		*m_pLicDB;
	CDirectODBC		*m_pAtalkDB;
	CDirectODBC		*m_pSmsDB;

	/* 통계 */
	long			m_lPushCnt;
	long			m_lDropCnt;
	long			m_lProcCnt;

private:
	int		GetMsg(SOCK_RECV_WAIT *pItem);
	pthread_cond_t		m_cond;
	pthread_mutex_t		m_mutex;
	int					m_sig_action;
	int					m_sig_count;
	int					m_run;
	SOCK_RECV_WAIT		m_wait_obj[MAX_RECV_WAIT_CNT+1];
	int					m_nPos;
	RECV_WAIT_VECTOR	*m_list;
};

void*	RunRecvWaitThread(void *arg);

extern CRecvWait	*__lic_recv;
extern CRecvWait	*__event_recv;

#endif /* RecvWaitStack.h */
