/*
**	SockProcLic.cpp : 6321 라이선스 체크 워커 처리
**	프레임 payload: "IVR"|"VRS" 0xFF <MAC> "IP:" <ip>   (docs/프로토콜명세.md §3)
**	응답: raw "Run"(3) / "Close"(5), 프레이밍 없음. 5초 이내 응답 (클라이언트 select 5s)
*/
#include "onlicDefine.h"
#include "RecvWaitStack.h"
#include "SockProcLic.h"

void LicSocketProc(CRecvWait *pWait, SOCK_RECV_WAIT *pItem)
{
	char strType[8] = "", strMac[128] = "", strIP[64] = "";
	const char *p = NULL;
	int nFindPos = -1, nLen = 0, nMacLen = 0, nRet = 0;
	long nCnt = -1;
	int64_t t0 = GetMilliSencondProc();

	if(pWait == NULL || pItem == NULL) return;

	/* TcpLogging(:699) "IP:%s, Port:%d, Data:%s" */
	OnlicLogf(LOG_TCP, "IP:%s, Port:%u, Data:%s", pItem->remote_ip, pItem->remote_port, pItem->buff);

	nLen = strlen_st(pItem->buff);
	p = strstr(pItem->buff, "IP:");
	nFindPos = (p != NULL) ? (int)(p - pItem->buff) : -1;

	/* :704 if(nFindPos > 3) - 아니면 무응답 (원본 동일) */
	if(nFindPos > 3){
		strncpy_st(strType, pItem->buff, 3); strType[3] = 0;				/* Mid(0,3) */
		nMacLen = nFindPos - 4;												/* Mid(4, nFindPos-4) : index 3 은 0xFF */
		if(nMacLen > (int)sizeof(strMac) - 1) nMacLen = sizeof(strMac) - 1;
		if(nMacLen > 0){ memcpy(strMac, pItem->buff + 4, nMacLen); strMac[nMacLen] = 0; }
		strncpy_st(strIP, pItem->buff + nFindPos + 3, sizeof(strIP) - 1);	/* Mid(nFindPos+3) */

		if(strlen_st(strIP) > 0){
			if(DbLic_IpcsLogInsert(pWait->m_pLicDB, strMac, strIP, strType, pItem->remote_ip) != TRUE)
				OnlicLogf(LOG_SQLERR, "IPCS_LOG insert fail mac:%s ip:%s type:%s rip:%s", strMac, strIP, strType, pItem->remote_ip);
		}

		/* :719 lic_access_list. 실패(-1) 도 Close (결정 B2 현행 유지) */
		nCnt = DbLic_AccessCount(pWait->m_pLicDB, pItem->remote_ip);

		if(pItem->handle > 0){
			if(nCnt <= 0) nRet = (int)send(pItem->handle, LIC_RESP_CLOSE, 5, MSG_NOSIGNAL);
			else nRet = (int)send(pItem->handle, LIC_RESP_RUN, 3, MSG_NOSIGNAL);
		}
		OnlicLogf(LOG_TCP, "LIC %s rip:%s type:%s mac:%s ip:%s cnt:%ld send:%d elapsed:%lldms",
			(nCnt <= 0) ? LIC_RESP_CLOSE : LIC_RESP_RUN, pItem->remote_ip, strType, strMac, strIP, nCnt, nRet, (long long)(GetMilliSencondProc() - t0));
	}else{
		OnlicLogf(LOG_TCP, "LIC ignore (no 'IP:' or pos<=3) rip:%s len:%d", pItem->remote_ip, nLen);
	}
}
