/*
**	SocketThread.h : 리슨 소켓·select 루프·세션 리스트 (onmw SocketThread/RunAgtSocketThread 패턴)
**	Windows: CListenSocket/CClientSocket/CUdpSocket + CreateSocketProc/ProcessPendingAccept/CloseSocket/AlarmUser_Send
*/
#ifndef ___ONLIC_SOCKETTHREAD_H___
#define ___ONLIC_SOCKETTHREAD_H___

#define F_CREATE		1
#define F_READING		2
#define F_DONE			8

int		Tcp_CreateServerSocket(ONLIC_SOCKET &sock, const char *szIP, unsigned int nPort, int nPortType);
int		Udp_CreateServerSocket(ONLIC_SOCKET &sock, const char *szIP, unsigned int nPort);
ONLIC_SOCKET*	Tcp_Accept(ONLIC_SOCKET *pListen, int nPortType);
void	Tcp_TerminateClientSocket(ONLIC_SOCKET *pSock);
void	Tcp_TerminateSocket(ONLIC_SOCKET *pSock);

void	FdListSet(CListObject *pList, fd_set *rset);
int		GetSocketMaxFd(CListObject *pList, int nListenHandle);
int		ReloadFdSetSocket(CListObject *pList, fd_set *rset, int nListenHandle);
ONLIC_SOCKET*	IsClientSocket(CListObject *pList, fd_set *rset);
int		ClientSocketRemove(CListObject *pList, ONLIC_SOCKET *pSock);
void	RemoveAllSocket(CListObject *pList);

/* 6770 전체 푸시 (Windows AlarmUser_Send:1774). nCmd '1' 일반 / '2' 일시정지. 반환 = 송신 성공 세션 수 */
int		AlarmUser_Send(const char *szMessage, int nCmd = 1);

/* 포트별 소켓 스레드 (onlicmain RunOnlicSystem 에서 Pthread_create) */
void*	RunLicSocketThread(void *arg);
void*	RunAlarmSocketThread(void *arg);
void*	RunUdpSocketThread(void *arg);

extern pthread_mutex_t	lic_socklist_mutex;
extern pthread_mutex_t	alarm_socklist_mutex;

#endif /* SocketThread.h */
