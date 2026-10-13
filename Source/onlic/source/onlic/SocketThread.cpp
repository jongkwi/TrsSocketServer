/*
**	SocketThread.cpp : 리슨·select·세션 (onmw 패턴). 수신 바이트는 Framing_OnReceive 로 재조립 후 워커 큐로
**	onmw 와 다른 점: 세션별 잔여 버퍼(ONLIC_SOCKET::rdata), 6321 유휴 타임아웃, 포인터 0xcdcdcdcd 검사 제거
*/
#include "onlicDefine.h"
#include "SocketThread.h"
#include "RecvWaitStack.h"
#include <errno.h>

pthread_mutex_t	lic_socklist_mutex;
pthread_mutex_t	alarm_socklist_mutex;
static int		s_sock_seq = 0;

/* ---------------- 생성/종료 ---------------- */
int Tcp_CreateServerSocket(ONLIC_SOCKET &sock, const char *szIP, unsigned int nPort, int nPortType)
{
	struct sockaddr_in sa;
	const int sock_on = TRUE;
	int ret = 0;

	memset_st(&sock, 0, sizeof(ONLIC_SOCKET));
	memset_st(&sa, 0, sizeof(sa));
	sock.handle = socket(AF_INET, SOCK_STREAM, 0);
	if(sock.handle < 0){
		OnlicLogf(LOG_SOFT, "Tcp_CreateServerSocket socket() fail port:%u errno:%d", nPort, errno);
		return -1;
	}
	setsockopt(sock.handle, SOL_SOCKET, SO_REUSEADDR, &sock_on, sizeof(sock_on));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(nPort);
	if(szIP == NULL || strlen_st(szIP) <= 0) sa.sin_addr.s_addr = htonl(INADDR_ANY);	/* Windows Create(port) = INADDR_ANY */
	else sa.sin_addr.s_addr = inet_addr(szIP);
	ret = bind(sock.handle, (struct sockaddr*)&sa, sizeof(sa));
	if(ret < 0){
		OnlicLogf(LOG_SOFT, "Tcp_CreateServerSocket bind() fail port:%u errno:%d", nPort, errno);
		close(sock.handle); sock.handle = 0;
		return -1;
	}
	if(listen(sock.handle, LISTENQ) < 0){
		OnlicLogf(LOG_SOFT, "Tcp_CreateServerSocket listen() fail port:%u errno:%d", nPort, errno);
		close(sock.handle); sock.handle = 0;
		return -1;
	}
	sock.type = TCP_SOCKET;
	sock.port_type = nPortType;
	sock.local_sa = sa;
	sock.local_port = nPort;
	strcpy_st(sock.local_ip, (szIP != NULL) ? szIP : "");
	OnlicLogf(LOG_SOFT, "Create TCP Socket handle:%d %s:%u", sock.handle, (strlen_st(sock.local_ip) > 0) ? sock.local_ip : "ANY", nPort);
	return TRUE;
}

int Udp_CreateServerSocket(ONLIC_SOCKET &sock, const char *szIP, unsigned int nPort)
{
	const int sock_on = TRUE;
	int ret = 0;

	memset_st(&sock, 0, sizeof(ONLIC_SOCKET));
	sock.salen = sizeof(sock.local_sa);
	sock.handle = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock.handle < 0){
		OnlicLogf(LOG_SOFT, "Udp_CreateServerSocket socket() fail port:%u errno:%d", nPort, errno);
		return -1;
	}
	setsockopt(sock.handle, SOL_SOCKET, SO_REUSEADDR, &sock_on, sizeof(sock_on));
	sock.local_sa.sin_family = AF_INET;
	sock.local_sa.sin_port = htons(nPort);
	if(szIP == NULL || strlen_st(szIP) <= 0) sock.local_sa.sin_addr.s_addr = htonl(INADDR_ANY);
	else sock.local_sa.sin_addr.s_addr = inet_addr(szIP);
	ret = bind(sock.handle, (struct sockaddr*)&sock.local_sa, sock.salen);
	if(ret < 0){
		OnlicLogf(LOG_SOFT, "Udp_CreateServerSocket bind() fail port:%u errno:%d", nPort, errno);
		close(sock.handle); sock.handle = 0;
		return -1;
	}
	sock.type = UDP_SOCKET;
	sock.port_type = PORT_TYPE_UDP;
	sock.local_port = nPort;
	strcpy_st(sock.local_ip, (szIP != NULL) ? szIP : "");
	OnlicLogf(LOG_SOFT, "Create UDP Socket handle:%d %s:%u", sock.handle, (strlen_st(sock.local_ip) > 0) ? sock.local_ip : "ANY", nPort);
	return TRUE;
}

ONLIC_SOCKET* Tcp_Accept(ONLIC_SOCKET *pListen, int nPortType)
{
	ONLIC_SOCKET *pCli = NULL;
	struct sockaddr_in sa;
	socklen_t salen = sizeof(sa);
	int n = 0;

again:
	memset_st(&sa, 0, sizeof(sa));
	n = accept(pListen->handle, (struct sockaddr*)&sa, &salen);
	if(n < 0){
		if(errno == ECONNABORTED || errno == EINTR) goto again;
		OnlicLogf(LOG_SOFT, "accept error errno:%d port_type:%d", errno, nPortType);
		return NULL;
	}
	pCli = new ONLIC_SOCKET;
	memset_st(pCli, 0, sizeof(ONLIC_SOCKET));
	pCli->handle = n;
	pCli->type = TCP_SOCKET;
	pCli->port_type = nPortType;
	pCli->remote_sa = sa;
	pCli->salen = salen;
	strcpy_st(pCli->remote_ip, inet_ntoa(sa.sin_addr));
	pCli->remote_port = ntohs(sa.sin_port);
	pCli->local_port = pListen->local_port;
	pCli->sock_seq = ++s_sock_seq;
	pCli->sock_time = GetMilliSencondProc();
	pCli->last_recv_time = pCli->sock_time;
	pCli->rdata[0] = 0;
	pCli->rlen = 0;
	return pCli;
}

void Tcp_TerminateClientSocket(ONLIC_SOCKET *pSock)
{
	if(pSock == NULL || pSock->handle <= 0) return;
	shutdown(pSock->handle, SHUT_RDWR);
	close(pSock->handle);
	pSock->handle = 0;
}

void Tcp_TerminateSocket(ONLIC_SOCKET *pSock)
{
	if(pSock == NULL || pSock->handle <= 0) return;
	close(pSock->handle);
	pSock->handle = 0;
}

/* ---------------- fd_set / 리스트 ---------------- */
void FdListSet(CListObject *pList, fd_set *rset)
{
	ONLIC_SOCKET *s = NULL;
	LISTPOS pos = pList->GetHeadPosition();
	int i = 0, cnt = pList->GetCount();
	for(i = 0; i < cnt && pos != NULL; i++){
		s = (ONLIC_SOCKET*)pList->GetAt(pos);
		if(s != NULL && s->handle > 0) FD_SET(s->handle, rset);
		pList->GetNext(pos);
	}
}

int GetSocketMaxFd(CListObject *pList, int nListenHandle)
{
	ONLIC_SOCKET *s = NULL;
	LISTPOS pos = pList->GetHeadPosition();
	int i = 0, cnt = pList->GetCount(), nMax = nListenHandle;
	for(i = 0; i < cnt && pos != NULL; i++){
		s = (ONLIC_SOCKET*)pList->GetAt(pos);
		if(s != NULL && s->handle > nMax) nMax = s->handle;
		pList->GetNext(pos);
	}
	return nMax;
}

int ReloadFdSetSocket(CListObject *pList, fd_set *rset, int nListenHandle)
{
	FD_ZERO(rset);
	FD_SET(nListenHandle, rset);
	FdListSet(pList, rset);
	return GetSocketMaxFd(pList, nListenHandle);
}

ONLIC_SOCKET* IsClientSocket(CListObject *pList, fd_set *rset)
{
	ONLIC_SOCKET *s = NULL;
	LISTPOS pos = pList->GetHeadPosition();
	int i = 0, cnt = pList->GetCount();
	for(i = 0; i < cnt && pos != NULL; i++){
		s = (ONLIC_SOCKET*)pList->GetAt(pos);
		if(s != NULL && s->handle > 0 && FD_ISSET(s->handle, rset)) return s;
		pList->GetNext(pos);
	}
	return NULL;
}

int ClientSocketRemove(CListObject *pList, ONLIC_SOCKET *pSock)
{
	ONLIC_SOCKET *s = NULL;
	LISTPOS pos = pList->GetHeadPosition();
	int i = 0, cnt = pList->GetCount();
	for(i = 0; i < cnt && pos != NULL; i++){
		s = (ONLIC_SOCKET*)pList->GetAt(pos);
		if(s == pSock){
			pList->RemoveAt(pos);
			Tcp_TerminateClientSocket(s);
			delete s;
			return TRUE;
		}
		pList->GetNext(pos);
	}
	return FALSE;
}

void RemoveAllSocket(CListObject *pList)
{
	ONLIC_SOCKET *s = NULL;
	LISTPOS pos = pList->GetHeadPosition();
	int i = 0, cnt = pList->GetCount();
	for(i = 0; i < cnt && pos != NULL; i++){
		s = (ONLIC_SOCKET*)pList->GetAt(pos);
		if(s != NULL){ Tcp_TerminateClientSocket(s); delete s; }
		pList->GetNext(pos);
	}
	pList->RemoveAll();
}

/* ---------------- 6770 전체 푸시 (AlarmUser_Send:1774-1822) ---------------- */
int AlarmUser_Send(const char *szMessage, int nCmd)
{
	char szMsg[ALARM_PUSH_MAX+1] = "", szTime[21] = "", Sdata[ALARM_PUSH_MAX+64] = "";
	ONLIC_SOCKET *s = NULL;
	LISTPOS pos = NULL;
	int i = 0, cnt = 0, ret = 0, nUserCount = 0, nLen = 0;

	strncpy_st(szMsg, szMessage, ALARM_PUSH_MAX);		/* 원본: 1000 바이트에서 절단 */
	szMsg[ALARM_PUSH_MAX] = 0;
	GetLocalTimeProc(szTime);							/* HH:MM:SS */
	nLen = Framing_BuildAlarmPush(Sdata, sizeof(Sdata), (char)('0' + nCmd), szMsg, szTime);

	Pthread_mutex_lock(&alarm_socklist_mutex);
	cnt = list_alarm_socket.GetCount();
	pos = list_alarm_socket.GetHeadPosition();
	for(i = 0; i < cnt && pos != NULL; i++){
		s = (ONLIC_SOCKET*)list_alarm_socket.GetAt(pos);
		if(s != NULL && s->handle > 0){
			ret = (int)send(s->handle, Sdata, nLen, MSG_NOSIGNAL);
			OnlicLogf(LOG_ALARM, "Send(%d) [%s:%u] %s", ret, s->remote_ip, s->remote_port, Sdata);
			if(ret > 0) nUserCount++;
		}
		list_alarm_socket.GetNext(pos);
	}
	Pthread_mutex_unlock(&alarm_socklist_mutex);

	if(nUserCount <= 0) OnlicLogf(LOG_ALARM, "SendErr List:%d Msg:%s", cnt, szMsg);
	return nUserCount;
}

/* ---------------- TCP 프레임 콜백 ---------------- */
/*
**	6321: 워커 큐로 (DB 조회가 있으므로 소켓 스레드에서 처리하지 않음)
**	6770: Windows ServerProc ALARM_PORT 분기 - 첫 글자 '3' 이면 즉시 ACK (DB 없음 → 인라인)
*/
static void OnTcpFrame(void *pCtx, int nCmd, const char *szPayload, int nLen)
{
	ONLIC_SOCKET *pSock = (ONLIC_SOCKET*)pCtx;
	SOCK_RECV_WAIT item;
	char Sdata[64] = "", szTime[21] = "", command[4] = "";

	if(pSock == NULL) return;

	if(pSock->port_type == PORT_TYPE_LIC){
		memset_st(&item, 0, sizeof(item));
		item.sock = pSock;
		item.handle = pSock->handle;
		item.port_type = PORT_TYPE_LIC;
		item.remote_sa = pSock->remote_sa;
		strcpy_st(item.remote_ip, pSock->remote_ip);
		item.remote_port = pSock->remote_port;
		item.local_port = pSock->local_port;
		item.evt_type = RECV_TYPE_DATA;
		item.cmd = nCmd;
		item.len = (nLen > MAX_RECV_BUFF) ? MAX_RECV_BUFF : nLen;
		memcpy(item.buff, szPayload, item.len); item.buff[item.len] = 0;
		if(__lic_recv != NULL) __lic_recv->Push(item);
		return;
	}

	if(pSock->port_type == PORT_TYPE_ALARM){
		strncpy_st(command, szPayload, 1); command[1] = 0;
		if(atoi(command) == 3){
			GetLocalTimeProc(szTime);
			Framing_BuildAlarmPush(Sdata, sizeof(Sdata), ALARM_CMD_HEARTBEAT_ACK, "", szTime);
			send(pSock->handle, Sdata, strlen_st(Sdata), MSG_NOSIGNAL);
		}
		return;
	}
}

/* ---------------- TCP 공통 루프 ---------------- */
static void RunTcpSocketLoop(ONLIC_SOCKET *pListen, CListObject *pList, pthread_mutex_t *pMutex, int nPortType, const char *szName)
{
	struct timeval tval;
	fd_set rset_all, rset1;
	ONLIC_SOCKET *pCli = NULL, *pNew = NULL, *pChk = NULL;
	LISTPOS pos = NULL, oldPos = NULL;
	char buf[MAXLENGTH+1];
	int maxfd = 0, nready = 0, nRecLen = 0, i = 0, cnt = 0, nClose = 0;
	int64_t now = 0;
	int nIdleMs = (nPortType == PORT_TYPE_LIC) ? sys_conf.lic_idle_timeout_sec * 1000 : 0;

	FD_ZERO(&rset_all);
	FD_SET(pListen->handle, &rset_all);
	maxfd = pListen->handle;
	OnlicLogf(LOG_SOFT, "%s socket thread start handle:%d port:%u idle:%dms", szName, pListen->handle, pListen->local_port, nIdleMs);

	for( ; ; ){
		if(onlic_start == FALSE) break;
		memcpy(&rset1, &rset_all, sizeof(fd_set));
		tval.tv_sec = 1; tval.tv_usec = 0;
		nready = select(maxfd + 1, &rset1, NULL, NULL, &tval);
		if(nready < 0){
			if(errno == EINTR) continue;
			Pthread_mutex_lock(pMutex);
			maxfd = ReloadFdSetSocket(pList, &rset_all, pListen->handle);
			Pthread_mutex_unlock(pMutex);
			OnlicLogf(LOG_SOFT, "%s select error errno:%d cnt:%d", szName, errno, pList->GetCount());
			WaitMSleep(50);
			continue;
		}
		if(onlic_start == FALSE) break;

		/* 접속 (Windows ProcessPendingAccept) */
		if(nready > 0 && FD_ISSET(pListen->handle, &rset1)){
			pNew = Tcp_Accept(pListen, nPortType);
			if(pNew != NULL){
				Pthread_mutex_lock(pMutex);
				pList->AddTail(pNew);
				FD_SET(pNew->handle, &rset_all);
				maxfd = GetSocketMaxFd(pList, pListen->handle);
				cnt = pList->GetCount();
				Pthread_mutex_unlock(pMutex);
				OnlicLogf(LOG_TCP, "Socket Connect %s IP:%s,Port:%u handle:%d cnt:%d", szName, pNew->remote_ip, pNew->remote_port, pNew->handle, cnt);
			}
			nready--;
		}

		/* 수신 */
		while(nready > 0){
			Pthread_mutex_lock(pMutex);
			pCli = IsClientSocket(pList, &rset1);
			Pthread_mutex_unlock(pMutex);
			if(pCli == NULL) break;
			FD_CLR(pCli->handle, &rset1);
			nready--;

			memset_st(buf, 0, sizeof(buf));
			nRecLen = (int)recv(pCli->handle, buf, MAXLENGTH, 0);		/* Windows Receive(500), 이후 strlen 기준 */
			if(nRecLen <= 0){
				OnlicLogf(LOG_TCP, "Socket Close %s IP:%s,Port:%u handle:%d ret:%d", szName, pCli->remote_ip, pCli->remote_port, pCli->handle, nRecLen);
				Pthread_mutex_lock(pMutex);
				ClientSocketRemove(pList, pCli);
				maxfd = ReloadFdSetSocket(pList, &rset_all, pListen->handle);
				Pthread_mutex_unlock(pMutex);
				continue;
			}
			pCli->last_recv_time = GetMilliSencondProc();
			Framing_OnReceive(pCli->rdata, buf, OnTcpFrame, pCli);
		}

		/* 유휴 세션 정리 (신규, 결정 B6). 0 이면 Windows 와 같이 세션 유지 */
		if(nIdleMs > 0){
			now = GetMilliSencondProc();
			nClose = 0;
			Pthread_mutex_lock(pMutex);
			cnt = pList->GetCount();
			pos = pList->GetHeadPosition();
			for(i = 0; i < cnt && pos != NULL; i++){
				pChk = (ONLIC_SOCKET*)pList->GetAt(pos);
				oldPos = pos;
				pList->GetNext(pos);
				if(pChk != NULL && (now - pChk->last_recv_time) > nIdleMs){
					OnlicLogf(LOG_TCP, "Socket Idle Close %s IP:%s,Port:%u handle:%d idle:%lldms", szName, pChk->remote_ip, pChk->remote_port, pChk->handle, (long long)(now - pChk->last_recv_time));
					pList->RemoveAt(oldPos);
					Tcp_TerminateClientSocket(pChk);
					delete pChk;
					nClose++;
				}
			}
			if(nClose > 0) maxfd = ReloadFdSetSocket(pList, &rset_all, pListen->handle);
			Pthread_mutex_unlock(pMutex);
		}
	}

	Pthread_mutex_lock(pMutex);
	RemoveAllSocket(pList);
	Pthread_mutex_unlock(pMutex);
	Tcp_TerminateSocket(pListen);
	OnlicLogf(LOG_SOFT, "%s socket thread exit", szName);
}

void* RunLicSocketThread(void *arg)
{
	RunTcpSocketLoop((ONLIC_SOCKET*)arg, &list_lic_socket, &lic_socklist_mutex, PORT_TYPE_LIC, "LIC");
	lic_pid = 0;
	return NULL;
}

void* RunAlarmSocketThread(void *arg)
{
	RunTcpSocketLoop((ONLIC_SOCKET*)arg, &list_alarm_socket, &alarm_socklist_mutex, PORT_TYPE_ALARM, "ALARM");
	alarm_pid = 0;
	return NULL;
}

/* ---------------- UDP 루프 (Windows ReadUdpMsgProc:541-568) ---------------- */
void* RunUdpSocketThread(void *arg)
{
	ONLIC_SOCKET *pUdp = (ONLIC_SOCKET*)arg;
	struct timeval tval;
	fd_set rset;
	struct sockaddr_in from;
	socklen_t fromlen = 0;
	char buf[MAXLENGTH+1], payload[RESIDUAL_MAX+1];
	int nready = 0, nRecLen = 0, nLen = 0;
	SOCK_RECV_WAIT item;

	OnlicLogf(LOG_SOFT, "UDP socket thread start handle:%d port:%u", pUdp->handle, pUdp->local_port);
	for( ; ; ){
		if(onlic_start == FALSE) break;
		FD_ZERO(&rset);
		FD_SET(pUdp->handle, &rset);
		tval.tv_sec = 1; tval.tv_usec = 0;
		nready = select(pUdp->handle + 1, &rset, NULL, NULL, &tval);
		if(nready < 0){
			if(errno == EINTR) continue;
			OnlicLogf(LOG_SOFT, "UDP select error errno:%d", errno);
			WaitMSleep(50);
			continue;
		}
		if(nready <= 0 || !FD_ISSET(pUdp->handle, &rset)) continue;

		memset_st(buf, 0, sizeof(buf));
		memset_st(&from, 0, sizeof(from));
		fromlen = sizeof(from);
		nRecLen = (int)recvfrom(pUdp->handle, buf, MAXLENGTH, 0, (struct sockaddr*)&from, &fromlen);	/* ReceiveFrom(500): 초과분 절단 */
		if(nRecLen <= 0) continue;

		nLen = Framing_UdpExtract(buf, payload, sizeof(payload));
		if(nLen <= 0 && payload[0] == 0){
			/* 원본: STX/ETX 없으면 조용히 폐기. (STX ETX 만 있는 빈 프레임은 nLen 0 이지만 처리 대상) */
			if(!(nRecLen >= 2 && strchr(buf, PKT_STX) != NULL && strchr(buf, PKT_ETX) != NULL)) continue;
		}

		memset_st(&item, 0, sizeof(item));
		item.sock = NULL;
		item.handle = pUdp->handle;
		item.port_type = PORT_TYPE_UDP;
		item.remote_sa = from;
		strcpy_st(item.remote_ip, inet_ntoa(from.sin_addr));
		item.remote_port = ntohs(from.sin_port);
		item.local_port = pUdp->local_port;
		item.evt_type = RECV_TYPE_DATA;
		item.len = (nLen > MAX_RECV_BUFF) ? MAX_RECV_BUFF : nLen;
		memcpy(item.buff, payload, item.len); item.buff[item.len] = 0;
		if(__event_recv != NULL) __event_recv->Push(item);
	}
	Tcp_TerminateSocket(pUdp);
	OnlicLogf(LOG_SOFT, "UDP socket thread exit");
	udp_pid = 0;
	return NULL;
}
