/*
**	AlarmProc.cpp : UDP 이벤트·링크 감시 업무 로직. 각 블록 주석의 :NNN 은 TrsSocketServer.cpp 원본 라인
**	원본과의 차이 (호환에 영향 없음)
**	  - 알림톡(_ATALK)/SMS(#else) 컴파일 분기 → ini [ATALK] USE / [SMS] USE 런타임 (둘 다 켜면 둘 다)
**	  - 신규 사이트(nUserCnt==0) 경로의 newSite001 수신자: 원본은 이전 이벤트의 m_pSmsSiteUsers 잔존값을 사용(결함) → 방금 복사한 기본 수신자
**	  - 알람 문구는 AlarmText() (ini [ALARM_MSG] 로 교체 가능, 기본값 = 원본 리터럴)
*/
#include "onlicDefine.h"
#include "RecvWaitStack.h"
#include "SocketThread.h"
#include "HttpWaitStack.h"
#include "AlarmProc.h"

/* ---------------- 알람 문구 ---------------- */
typedef struct { const char *key; const char *def; } ALARM_TXT_DEF;
static const ALARM_TXT_DEF s_txt_def[TXT_MAX] = {
	{ "TABLE_CHECK",		"CallCenter_Link 테이블 이름 설정 확인\n시스템IP:%s\n공인IP:%s" },
	{ "PROC_ERROR",			"%s-%s에서 %s 프로세스 장애가 발생 했습니다." },
	{ "PROC_SUCC",			"%s-%s에서 %s 프로세스 장애가 복구 됬습니다." },
	{ "CPU_ERROR",			"%s-%s에서 CPU 장애가 발생 했습니다." },
	{ "CPU_SUCC",			"%s-%s에서 CPU 장애가 복구 됬습니다." },
	{ "DISK_ERROR",			"%s-%s에서 %s DISK 장애가 발생 했습니다. 용량을 확인해 주세요." },
	{ "DISK_SUCC",			"%s-%s에서 %s DISK 장애가 복구 됬습니다." },
	{ "MEM_ERROR",			"%s-%s에서 MEM 장애가 발생 했습니다. 가상 메모리가 부족 합니다." },
	{ "MEM_SUCC",			"%s-%s에서 MEM 장애가 복구 됬습니다." },
	{ "SOCK_CON_ERROR",		"%s-%s에서 %s SOCKET 연결 장애가 발생 했습니다." },
	{ "SOCK_CON_SUCC",		"%s-%s에서 %s SOCKET에 연결 되었습니다." },
	{ "SOCK_CHECK_ERROR",	"%s-%s에서 %s SOCKET LINK CHECK에 장애가 발생 했습니다." },
	{ "SOCK_CLOSE_ERROR",	"%s-%s에서 %s SOCKET Close 장애가 발생 했습니다." },
	{ "ICMP_ERROR",			"%s-%s에서 PING 전송이 실패 되고 있습니다. 네트워크 검사" },
	{ "ICMP_SUCC",			"%s-%s에서 PING 전송이 성공 되고 있습니다." },
	{ "DATA_ERROR",			"%s-%s에서 %s 로그 장애가 발생 했습니다. 해당 프로세스 검사" },
	{ "DATA_ERROR_PAUSE",	"%s-%s에서 %s 로그 장애가 발생 했습니다. - 문자전송 일시정지중! " },
	{ "LINK_RETURN",		"%s-%s에서 링크데이터가 수신 되고 있습니다." },
	{ "NAS_CON_FAIL_MSG",	"%s-%s에서 NAS 연결 장애가 발생하였습니다.(%s)" },
	{ "NAS_CON_FAIL_CNT",	"%s-%s에서 NAS 연결 장애가 발생 했습니다.(%d회)" },
	{ "NAS_CON_SUCC",		"%s-%s에서 NAS 연결 되었습니다." },
	{ "NAS_UPLOAD_FAIL",	"%s-%s에서 NAS 업로드가 실패 하고 있습니다." },
	{ "NAS_UPLOAD_SUCC",	"%s-%s에서 NAS 업로드가 성공 되고 있습니다." },
	{ "BACKUP_MESSAGE",		"%s-%s에서 %s" },
	{ "BACKUP_DISKFULL",	"%s-%s에서 녹취 백업 DISK용량 초과 장애가 발생하였습니다." },
	{ "FILE_REMOVE",		"%s-%s에서 녹취용량(%s%%) 초과로 녹취파일이 삭제 되었습니다" },
	{ "DISK_FULL",			"%s-%s에서 녹취용량(%s%%) 초과 장애입니다." },
	{ "NEW_SITE",			"콜센터 신규 사이트 감지\n시스템IP:%s\n공인IP:%s" },
	{ "LINK_ERROR",			"%s-%s에서 링크데이터가 수신되고 있지 않습니다." },
};
static char s_txt[TXT_MAX][512];
static int  s_txt_loaded = FALSE;

/* ini 값의 "\n" 두 글자를 개행으로 */
static void AlarmText_Unescape(char *s)
{
	int i = 0, j = 0;
	for(i = 0; s[i] != 0; i++){
		if(s[i] == '\\' && s[i+1] == 'n'){ s[j++] = '\n'; i++; }
		else s[j++] = s[i];
	}
	s[j] = 0;
}

/* %s/%d 개수가 기본값과 다르면 사용하지 않음 (포맷 문자열 안전) */
static int AlarmText_CountFmt(const char *s)
{
	int n = 0, i = 0;
	for(i = 0; s[i] != 0; i++){
		if(s[i] == '%'){
			if(s[i+1] == '%'){ i++; continue; }
			n++;
		}
	}
	return n;
}

int AlarmProc_Init()
{
	int i = 0;
	char szVal[512] = "";
	for(i = 0; i < TXT_MAX; i++){
		strcpy_st(s_txt[i], s_txt_def[i].def);
		if(strlen_st(sys_load.m_szConfig) > 0){
			szVal[0] = 0;
			GetPrivateProfileStringUsr((char*)"ALARM_MSG", (char*)s_txt_def[i].key, "", szVal, sizeof(szVal)-1, (char*)sys_load.m_szConfig);
			if(strlen_st(szVal) > 0){
				AlarmText_Unescape(szVal);
				if(AlarmText_CountFmt(szVal) == AlarmText_CountFmt(s_txt_def[i].def)){
					strcpy_st(s_txt[i], szVal);
					OnlicLogf(LOG_SOFT, "[ALARM_MSG] %s = %s", s_txt_def[i].key, szVal);
				}else{
					OnlicLogf(LOG_SOFT, "[ALARM_MSG] %s ignored (format count mismatch)", s_txt_def[i].key);
				}
			}
		}
	}
	s_txt_loaded = TRUE;
	return TRUE;
}

const char* AlarmText(int nIdx)
{
	if(nIdx < 0 || nIdx >= TXT_MAX) return "";
	return (s_txt_loaded == TRUE) ? s_txt[nIdx] : s_txt_def[nIdx].def;
}

/* ---------------- 보조 (Windows 동명) ---------------- */
/* :1352 GetUserString - "이름^번호|이름^번호" */
void GetUserString(char *szUsers, int nUsersSize, const SMS_SITE_USERS *pUsers, int nUserCnt, int nUseType)
{
	char szTmp[2048] = "";
	int j = 0, ok = 0;
	szUsers[0] = 0;
	if(pUsers == NULL) return;
	for(j = 0; j < nUserCnt; j++){
		ok = (nUseType == USE_ALL) ? (pUsers[j].sms_use[0] == 'Y' || pUsers[j].sms_use[0] == 'T') : (pUsers[j].sms_use[0] == 'Y');
		if(ok){
			if(strlen_st(szTmp) > 3) strncat_st(szTmp, "|", sizeof(szTmp) - strlen_st(szTmp) - 1);
			strncat_st(szTmp, pUsers[j].sms_user_name, sizeof(szTmp) - strlen_st(szTmp) - 1);
			strncat_st(szTmp, "^", sizeof(szTmp) - strlen_st(szTmp) - 1);
			strncat_st(szTmp, pUsers[j].sms_user_phone, sizeof(szTmp) - strlen_st(szTmp) - 1);
		}
	}
	if(strlen_st(szTmp) > 3){ strncpy_st(szUsers, szTmp, nUsersSize - 1); szUsers[nUsersSize - 1] = 0; }
	else szUsers[0] = 0;
}

/* :1392 GetUserInfoIdx */
int GetUserInfoIdx(const SMS_SITE_USERS *pUsers, int usrIdx, char *szName, char *szNumber, int nUserCnt, int nUseType)
{
	int ok = 0;
	if(pUsers == NULL || usrIdx >= nUserCnt) return FALSE;
	ok = (nUseType == USE_ALL) ? (pUsers[usrIdx].sms_use[0] == 'Y' || pUsers[usrIdx].sms_use[0] == 'T') : (pUsers[usrIdx].sms_use[0] == 'Y');
	if(ok && strlen_st(pUsers[usrIdx].sms_user_name) > 0 && strlen_st(pUsers[usrIdx].sms_user_phone) > 0){
		strcpy_st(szName, pUsers[usrIdx].sms_user_name);
		strcpy_st(szNumber, pUsers[usrIdx].sms_user_phone);
		return TRUE;
	}
	return FALSE;
}

/* :136 IsSmsExclude - 기동 시 캐시한 SITE_SmsExcludeTime 으로 판정 (HHMM 문자열 비교) */
int IsSmsExclude(int nType, const char *szMsg)
{
	int Exclude = FALSE, i = 0, nFindIndex = -1;
	int h = 0, m = 0, s = 0;
	char szCurrentTime[11] = "";
	SMS_EXCLUDE *pEx = sys_load.m_pSmsExclude;

	if(pEx != NULL){
		for(i = 0; i < sys_load.m_nSmsExcludeCnt && strlen_st(pEx[i].sm_stime) > 0; i++){
			if(strlen_st(pEx[i].sm_stime) == 4 && strlen_st(pEx[i].sm_etime) == 4){
				GetLocalTimeProc(h, m, s);
				nprintf(szCurrentTime, 10, "%02d%02d", h, m);
				nFindIndex = i;
				if(strcmp(pEx[i].sm_stime, szCurrentTime) <= 0 && strcmp(szCurrentTime, pEx[i].sm_etime) <= 0){
					if(pEx[i].sm_type == nType){
						switch(nType){
						  case SEND_TYPE_PROC_ERROR: case SEND_TYPE_PROC_SUCC:
						  case SEND_TYPE_SOCK_CON_ERROR: case SEND_TYPE_SOCK_CON_SUCC:
						  case SEND_TYPE_SOCK_CHECK_ERROR: case SEND_TYPE_SOCK_CLOSE_ERROR:
						  case SEND_TYPE_DATA_ERROR:
							if(strlen_st(szMsg) > 0 && strcmp(pEx[i].sm_msg, szMsg) == 0) Exclude = TRUE;
							break;
						  default:
							Exclude = TRUE;
							break;
						}
					}
				}
			}
		}
	}
	OnlicLogf(LOG_SMSEX, "IsSmsExclude:%d, type:%d, msg:%s, findidx:%d", Exclude, nType, szMsg, nFindIndex);
	return Exclude;
}

/* :1740 PauseCheckProc - cc_pause(YYYYMMDD) 가 오늘 이후면 문자 일시정지. 잘못되거나 지난 값은 지움 */
int PauseCheckProc(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char szToday[21] = "", szValue[129] = "";
	int nPause = FALSE, nLen = 0;

	DbLic_Today8(szToday);
	DbLic_GetPause(pDB, szIP, szRealIP, szValue, sizeof(szValue));
	nLen = strlen_st(szValue);
	if(nLen == 8 && strcmp(szToday, szValue) <= 0){
		OnlicLogf(LOG_UDP, "PauseCheck: Today:%s, Pause:%s", szToday, szValue);
		nPause = TRUE;
	}else if(nLen > 0){
		DbLic_ClearPause(pDB, szIP, szRealIP);
		OnlicLogf(LOG_UDP, "PauseCheckDel Today:%s, Pause:%s", szToday, szValue);
	}
	return nPause;
}

/* :1299 IVRSMSSendProc - nType 200/201 은 항상 SDK_SMS_SEND (알림톡 제외, 원본 주석) */
int IVRSMSSendProc(CDirectODBC *pSmsDB, char szResult[STR_SPLIT_MAX][STR_SPLIT_LEN], const char *szLogMsg)
{
	int nType = 0, nDbRet = FALSE;
	char szIP[21] = "", szDate[21] = "", szFrom[31] = "", szTo[31] = "", szMsg[81] = "", szSubject[51] = "", szUser[101] = "";

	nType = atoi(szResult[1]);
	strncpy_st(szIP, szResult[2], 20);
	strncpy_st(szMsg, szResult[3], 80);
	strncpy_st(szFrom, szResult[4], 30);
	strncpy_st(szTo, szResult[5], 30);
	DbLic_Now14(szDate);
	nprintf(szUser, 100, "%s^%s", "Onpia", szTo);
	switch(nType){
	  case SEND_TYPE_IVR_GPS:		strcpy_st(szSubject, "GPS위치"); break;
	  case SEND_TYPE_IVR_NORMAL:
	  default:						strcpy_st(szSubject, "IVR메세지"); break;
	}
	if(strlen_st(szMsg) > 0){
		if(sys_conf.sms_use == TRUE && pSmsDB != NULL) nDbRet = DbLic_SmsInsert(pSmsDB, szSubject, szMsg, szDate, szFrom, 1, szUser);
		else OnlicLog(LOG_UDP, "IVRSMSSendProc: [SMS] USE=0 - SDK_SMS_SEND skipped");
	}
	OnlicLogf(LOG_UDP, "%s, DB:%d, Value('%s', '%s', '%s', '%s', '%s', '%s', %d, '%s')", szLogMsg, nDbRet, sys_conf.sms_user_id, szSubject, szMsg, szDate, szDate, szFrom, 1, szUser);
	return FALSE;
}

/* 알림톡: 수신자별 1행 INSERT (원본 _ATALK 블록). [ATALK] USE=0 이면 아무것도 안 함 */
int AtalkSendUsers(CDirectODBC *pAtalkDB, const SMS_SITE_USERS *pUsers, int nUserCnt, int nUseType, const char *szTmplCode, const char *szSendData)
{
	char smsUsrName[51] = "", smsUsrNum[21] = "";
	int usrIdx = 0, nSent = 0;
	if(sys_conf.atalk_use != TRUE) return 0;
	if(pAtalkDB == NULL){ OnlicLog(LOG_SQLERR, "AtalkSendUsers: atalk db handle null"); return 0; }
	for(usrIdx = 0; usrIdx < nUserCnt; usrIdx++){
		if(GetUserInfoIdx(pUsers, usrIdx, smsUsrName, smsUsrNum, nUserCnt, nUseType) == TRUE){
			if(DbLic_AtalkInsert(pAtalkDB, szTmplCode, smsUsrNum, szSendData) != TRUE)
				OnlicLogf(LOG_SQLERR, "MTS_ATALK_MSG insert fail tmpl:%s phone:%s msg:%s", szTmplCode, smsUsrNum, szSendData);
			else nSent++;
		}
	}
	OnlicLogf(LOG_ALARM, "AtalkSendUsers tmpl:%s sent:%d/%d useType:%d msg:%s", szTmplCode, nSent, nUserCnt, nUseType, szSendData);
	return nSent;
}

/* SMS: 원본 #else 경로 (80 바이트 절단, 수신자 목록 1행). [SMS] USE=0 이면 아무것도 안 함 */
int SmsSendUsers(CDirectODBC *pSmsDB, const char *szSubject, const char *szMsg, int nUserCnt, const char *szUsers)
{
	char szSendData[81] = "", szDate[21] = "";
	if(sys_conf.sms_use != TRUE) return 0;
	if(pSmsDB == NULL){ OnlicLog(LOG_SQLERR, "SmsSendUsers: sms db handle null"); return 0; }
	Framing_HangulStrCut(szMsg, szSendData, 80);
	DbLic_Now14(szDate);
	if(DbLic_SmsInsert(pSmsDB, szSubject, szSendData, szDate, sys_conf.sms_callback, nUserCnt, szUsers) != TRUE){
		OnlicLogf(LOG_SQLERR, "SDK_SMS_SEND insert fail msg:%s users:%s", szSendData, szUsers);
		return 0;
	}
	return 1;
}

/* ---------------- UDP 이벤트 (ServerUdpProc :783-1295) ---------------- */
void EventUdpProc(CRecvWait *pWait, SOCK_RECV_WAIT *pItem)
{
	static char szResult[STR_SPLIT_MAX][STR_SPLIT_LEN];		/* 워커 1개 전용 (2.5MB 스택 회피) */
	char Sdata[MAX_RECV_BUFF+8] = "", szLog[2048] = "";
	char szIP[21] = "", szRealIP[51] = "", szRMsg[1024] = "", szUseIP[200] = "";
	char szMsg[1024] = "", szAlarm[1024] = "", szSendData[1025] = "", szUsers[2048] = "", szTempCode[128] = "";
	char szDay[21] = "", szNasUpFailDate[129] = "";
	int nIndex = 0, nType = 0, nUserCnt = 0, nUserBasicCnt = 0, i = 0, nNasFailCnt = 0, nSmsExclude = FALSE, nIsEmergency = FALSE;
	int nSendLen = 0, nSiteName = 0;
	long nSiteCnt = 0, nCnt = 0;
	SMS_SITE_USERS *pUsers = NULL;
	SMS_SITE_BASIC_USER *pBasic = NULL;
	const char *szSiteName = "";
	CDirectODBC *pDB = NULL;

	if(pWait == NULL || pItem == NULL) return;
	pDB = pWait->m_pLicDB;

	DbLic_TodayHyphen(szDay);			/* cc_nasupload_fail 비교값 (결정 B12 소스 형식) */
	Framing_StrSplit(pItem->buff, szResult, (char)PKT_SEP);
	nIndex = atoi(szResult[0]);
	nType = atoi(szResult[1]);
	strncpy_st(szIP, szResult[2], 20);
	strncpy_st(szRMsg, szResult[3], 1023);
	strncpy_st(szRealIP, pItem->remote_ip, 50);
	trim(szRealIP);
	if(strcmp(szRealIP, szIP) == 0) strcpy_st(szUseIP, szRealIP);
	else nprintf(szUseIP, 199, "%s(%s)", szRealIP, szIP);

	nprintf(szLog, 2047, "IP:%s, Port:%u, Data:%s", szRealIP, pItem->remote_port, pItem->buff);

	/* :824 IVR 200/201 → SMS 만, ACK 없음 */
	if(nType == SEND_TYPE_IVR_GPS || nType == SEND_TYPE_IVR_NORMAL){
		IVRSMSSendProc(pWait->m_pSmsDB, szResult, szLog);
		return;
	}

	/* :832 echo ACK 프레임 */
	nSendLen = nprintf(Sdata, sizeof(Sdata)-1, "%c%s%c", PKT_STX, pItem->buff, PKT_ETX);
#define UDP_ACK()	do{ if(pItem->handle > 0) sendto(pItem->handle, Sdata, strlen_st(Sdata), 0, (struct sockaddr*)&pItem->remote_sa, sizeof(pItem->remote_sa)); }while(0)

	/* :834-850 수신자 조회 (count + select) */
	nUserCnt = DbLic_GetSmsSiteUsers(pDB, szIP, szRealIP, &pUsers);

	if(nUserCnt > 0){
		szSiteName = pUsers[0].sms_site_name;
		nSiteCnt = DbLic_SiteCount(pDB, szIP, szRealIP);					/* :854 */
		if(nSiteCnt <= 0){
			/* :856-888 수신자는 있으나 CallCenter_Link 미등록: 등록 + tableCheck001 (ACK 없음) */
			DbLic_SiteInsertNew(pDB, szIP, szRealIP);
			nprintf(szMsg, 1023, AlarmText(TXT_TABLE_CHECK), szIP, szRealIP);
			Framing_HangulStrCut(szMsg, szSendData, 80);
			GetUserString(szUsers, sizeof(szUsers), pUsers, nUserCnt, USE_NORAML);
			OnlicLogf(LOG_UDP, "%s / site not registered -> insert, users:%s", szLog, szUsers);
			if(strlen_st(szUsers) > 0){
				AtalkSendUsers(pWait->m_pAtalkDB, pUsers, nUserCnt, USE_NORAML, "tableCheck001", szSendData);
				SmsSendUsers(pWait->m_pSmsDB, "장애발생", szSendData, nUserCnt, szUsers);
			}
			delete [] pUsers;
			return;
		}

		/* :890-899 cc_use != 'Y' → 무시 (ACK 없음) */
		if(DbLic_SiteCountUse(pDB, szIP, szRealIP) <= 0){
			OnlicLogf(LOG_UDP, "문자메시지 서비스 사용 안하는 아이피 입니다. 실IP:%s, 공인IP:%s, sms_site_name:%s", szIP, szRealIP, szSiteName);
			delete [] pUsers;
			return;
		}
		/* :902-909 '신규1' + 3시간 경과 → 무시 (ACK 없음) */
		if(DbLic_IsNewSiteExpired(pDB, szIP, szRealIP, sys_conf.new_site_skip_hour) == TRUE){
			OnlicLogf(LOG_UDP, "신규1 -- 미변경 사이트 : 실IP:%s, 공인IP:%s, sms_site_name:%s", szIP, szRealIP, szSiteName);
			delete [] pUsers;
			return;
		}

		/* :911 UdpLogging */
		OnlicLog(LOG_UDP, szLog);
		szTempCode[0] = 0;

		switch(nType){
		  case SEND_TYPE_PROC_ERROR:							/* :916 */
			nprintf(szMsg, 1023, AlarmText(TXT_PROC_ERROR), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "procError001");
			AlarmWebTh_EmailPush(pDB, SEND_TYPE_PROC_ERROR, szIP, szRealIP, szSiteName, szRMsg, szMsg);
			break;
		  case SEND_TYPE_PROC_SUCC:
			nprintf(szMsg, 1023, AlarmText(TXT_PROC_SUCC), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "procReturn001");
			AlarmWebTh_EmailPush(pDB, SEND_TYPE_PROC_SUCC, szIP, szRealIP, szSiteName, szRMsg, szMsg);
			break;
		  case SEND_TYPE_CPU_ERROR:
			nprintf(szMsg, 1023, AlarmText(TXT_CPU_ERROR), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "cpuError001");
			break;
		  case SEND_TYPE_CPU_SUCC:
			nprintf(szMsg, 1023, AlarmText(TXT_CPU_SUCC), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "cpuReturn001");
			break;
		  case SEND_TYPE_DISK_ERROR:
			nprintf(szMsg, 1023, AlarmText(TXT_DISK_ERROR), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "diskError001");
			nIsEmergency = TRUE;
			break;
		  case SEND_TYPE_DISK_SUCC:
			nprintf(szMsg, 1023, AlarmText(TXT_DISK_SUCC), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "diskReturn001");
			nIsEmergency = TRUE;
			break;
		  case SEND_TYPE_MEM_ERROR:
			nprintf(szMsg, 1023, AlarmText(TXT_MEM_ERROR), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "memError001");
			break;
		  case SEND_TYPE_MEM_SUCC:
			nprintf(szMsg, 1023, AlarmText(TXT_MEM_SUCC), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "memReturn001");
			break;
		  case SEND_TYPE_SOCK_CON_ERROR:
			nprintf(szMsg, 1023, AlarmText(TXT_SOCK_CON_ERROR), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "sockError001");
			break;
		  case SEND_TYPE_SOCK_CON_SUCC:
			nprintf(szMsg, 1023, AlarmText(TXT_SOCK_CON_SUCC), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "sockReturn001");
			break;
		  case SEND_TYPE_SOCK_CHECK_ERROR:
			nprintf(szMsg, 1023, AlarmText(TXT_SOCK_CHECK_ERROR), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "sockLinkError001");
			break;
		  case SEND_TYPE_SOCK_CLOSE_ERROR:
			nprintf(szMsg, 1023, AlarmText(TXT_SOCK_CLOSE_ERROR), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "sockDisconError001");
			break;
		  case SEND_TYPE_ICMP_ERROR:
			nprintf(szMsg, 1023, AlarmText(TXT_ICMP_ERROR), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "pingError001");
			break;
		  case SEND_TYPE_ICMP_SUCC:
			nprintf(szMsg, 1023, AlarmText(TXT_ICMP_SUCC), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "pingReturn001");
			break;
		  case SEND_TYPE_USER_MSG:								/* :999 템플릿 코드 없음 (빈 값) */
			nprintf(szMsg, 1023, "%s", szRMsg);
			UDP_ACK();
			break;
		  case SEND_TYPE_DATA_ERROR:							/* :1004 */
			if(PauseCheckProc(pDB, szIP, szRealIP) == FALSE){
				nprintf(szMsg, 1023, AlarmText(TXT_DATA_ERROR), szSiteName, szUseIP, szRMsg);
			}else{
				nprintf(szAlarm, 1023, AlarmText(TXT_DATA_ERROR), szSiteName, szUseIP, szRMsg);
				nprintf(szLog, 2047, AlarmText(TXT_DATA_ERROR_PAUSE), szSiteName, szUseIP, szRMsg);
				OnlicLog(LOG_UDP, szLog);
			}
			strcpy_st(szTempCode, "procLogError001");
			UDP_ACK();
			break;
		  case SEND_TYPE_LINK_CHECK:							/* :1016 ACK 없음 */
			nCnt = DbLic_SiteCountLinkError(pDB, szIP, szRealIP);
			if(nCnt > 0){
				nprintf(szMsg, 1023, AlarmText(TXT_LINK_RETURN), szSiteName, szUseIP);
				strcpy_st(szTempCode, "linkDataReturn001");
				AlarmWebTh_EmailPush(pDB, SEND_TYPE_NET_SUCC, szIP, szRealIP, szSiteName, szMsg, "");
			}
			{ char szDate[21] = ""; DbLic_Now14(szDate); DbLic_UpdateTimeSite(pDB, szDate, szIP, szRealIP); }
			nIsEmergency = TRUE;
			break;
		  case SEND_TYPE_NAS_CON_FAIL:							/* :1035 */
			nNasFailCnt = DbLic_GetNasFailCnt(pDB, szIP, szRealIP);
			if(nNasFailCnt < 0) nNasFailCnt = 0;				/* GetUnsignedIntValue 실패(-1) → 원본은 -1 그대로 ++ → 0. 동일 결과 */
			if(nNasFailCnt > 0) nIsEmergency = TRUE;			/* 최초 1회는 긴급 아님 */
			if(nNasFailCnt > sys_conf.nas_fail_count_max) nNasFailCnt = sys_conf.nas_fail_count_max;
			++nNasFailCnt;
			DbLic_SetNasFailCnt(pDB, szIP, szRealIP, nNasFailCnt);
			if(nNasFailCnt <= sys_conf.nas_fail_notify_max){
				if(strlen_st(szRMsg) > 0){
					nprintf(szMsg, 1023, AlarmText(TXT_NAS_CON_FAIL_MSG), szSiteName, szUseIP, szRMsg);
					strcpy_st(szTempCode, "nasConError001");
				}else{
					nprintf(szMsg, 1023, AlarmText(TXT_NAS_CON_FAIL_CNT), szSiteName, szUseIP, nNasFailCnt);
					strcpy_st(szTempCode, "nasConError003");
				}
				AlarmWebTh_EmailPush(pDB, SEND_TYPE_NAS_CON_FAIL, szIP, szRealIP, szSiteName, szRMsg, szMsg);
			}
			UDP_ACK();
			break;
		  case SEND_TYPE_NAS_CON_SUCC:							/* :1062 */
			nprintf(szMsg, 1023, AlarmText(TXT_NAS_CON_SUCC), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "nasConReturn001");
			AlarmWebTh_EmailPush(pDB, SEND_TYPE_NAS_CON_SUCC, szIP, szRealIP, szSiteName, szRMsg, szMsg);
			nNasFailCnt = DbLic_GetNasFailCnt(pDB, szIP, szRealIP);
			if(nNasFailCnt > 1) nIsEmergency = TRUE;
			DbLic_SetNasFailCnt(pDB, szIP, szRealIP, 0);
			break;
		  case SEND_TYPE_NAS_UPLOAD_FAIL:						/* :1084 */
			nprintf(szMsg, 1023, AlarmText(TXT_NAS_UPLOAD_FAIL), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "nasUploadError002");
			AlarmWebTh_EmailPush(pDB, SEND_TYPE_NAS_UPLOAD_FAIL, szIP, szRealIP, szSiteName, szRMsg, szMsg);
			DbLic_GetNasUploadFail(pDB, szIP, szRealIP, szNasUpFailDate, sizeof(szNasUpFailDate));
			if(strcmp(szDay, szNasUpFailDate) != 0) nIsEmergency = TRUE;
			break;
		  case SEND_TYPE_NAS_UPLOAD_SUCC:						/* :1099 */
			nprintf(szMsg, 1023, AlarmText(TXT_NAS_UPLOAD_SUCC), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "nasUploadReturn001");
			AlarmWebTh_EmailPush(pDB, SEND_TYPE_NAS_UPLOAD_SUCC, szIP, szRealIP, szSiteName, szRMsg, szMsg);
			DbLic_GetNasUploadFail(pDB, szIP, szRealIP, szNasUpFailDate, sizeof(szNasUpFailDate));
			if(strcmp(szDay, szNasUpFailDate) != 0){
				DbLic_SetNasUploadFail(pDB, szIP, szRealIP, szDay);
				nIsEmergency = TRUE;
			}
			break;
		  case SEND_TYPE_BACKUP_MESSAGE:						/* :1115 */
			nprintf(szMsg, 1023, AlarmText(TXT_BACKUP_MESSAGE), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "unknownCmd003");
			break;
		  case SEND_TYPE_BACKUP_DISKFULL:
			nprintf(szMsg, 1023, AlarmText(TXT_BACKUP_DISKFULL), szSiteName, szUseIP);
			UDP_ACK();
			strcpy_st(szTempCode, "nasDiskError001");
			break;
		  case SEND_TYPE_FILE_REMOVE:
			nprintf(szMsg, 1023, AlarmText(TXT_FILE_REMOVE), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "recDiskDel001");
			break;
		  case SEND_TYPE_DISK_FULL:
			nprintf(szMsg, 1023, AlarmText(TXT_DISK_FULL), szSiteName, szUseIP, szRMsg);
			UDP_ACK();
			strcpy_st(szTempCode, "recDiskThres001");
			break;
		  default:												/* :1139 */
			OnlicLogf(LOG_UDP, "UDP_PORT 알 수 없는 형식 입니다. nType:%d", nType);
			UDP_ACK();
			strcpy_st(szTempCode, "unknownCmd001");
			break;
		}

		/* :1153-1206 알람 푸시 + 웹푸시 + 알림톡/SMS */
		if(strlen_st(szMsg) > 0){
			AlarmUser_Send(szMsg, 1);
			AlarmWebTh_Push(szMsg, 1);
			Framing_HangulStrCut(szMsg, szSendData, 1024);					/* _ATALK: 1024 */
			GetUserString(szUsers, sizeof(szUsers), pUsers, nUserCnt, (nIsEmergency == TRUE) ? USE_ALL : USE_NORAML);
			if(strlen_st(szUsers) > 0){
				nSmsExclude = IsSmsExclude(nType, szRMsg);
				if(nSmsExclude == FALSE){
					AtalkSendUsers(pWait->m_pAtalkDB, pUsers, nUserCnt, (nIsEmergency == TRUE) ? USE_ALL : USE_NORAML, szTempCode, szSendData);
					SmsSendUsers(pWait->m_pSmsDB, "장애발생", szMsg, nUserCnt, szUsers);
				}
			}
		}else if(strlen_st(szAlarm) > 0){
			AlarmUser_Send(szAlarm, 2);										/* :1208 일시정지 중 - 뷰어에만 cmd 2 */
		}
		delete [] pUsers;
	}
	else{
		/* :1216-1290 수신자 없는 IP */
		nUserBasicCnt = (int)DbLic_SmsBasicUserCount(pDB);
		if(nUserBasicCnt > 0){
			nSiteCnt = DbLic_SiteCount(pDB, szIP, szRealIP);
			if(nSiteCnt <= 0){
				/* 신규 사이트: 기본 수신자 복사 + 등록 + newSite001 */
				nUserBasicCnt = DbLic_GetSmsBasicUsers(pDB, &pBasic);
				for(i = 0; i < nUserBasicCnt; i++) DbLic_SmsUserInsert(pDB, szIP, szRealIP, pBasic[i]);
				DbLic_SiteInsertNew(pDB, szIP, szRealIP);
				nprintf(szMsg, 1023, AlarmText(TXT_NEW_SITE), szIP, szRealIP);
				AlarmUser_Send(szMsg, 1);
				AlarmWebTh_Push(szMsg, 1);
				Framing_HangulStrCut(szMsg, szSendData, 1024);
				/* 방금 복사한 기본 수신자로 수신자 목록 구성 (원본은 이전 사이트의 잔존 배열 사용 - 결함) */
				if(nUserBasicCnt > 0){
					pUsers = new SMS_SITE_USERS[nUserBasicCnt + 1];
					memset_st(pUsers, 0, sizeof(SMS_SITE_USERS) * (nUserBasicCnt + 1));
					for(i = 0; i < nUserBasicCnt; i++){
						strcpy_st(pUsers[i].sms_site_ip, szIP);
						strcpy_st(pUsers[i].sms_site_real_ip, szRealIP);
						strcpy_st(pUsers[i].sms_user_name, pBasic[i].site_sms_name);
						strcpy_st(pUsers[i].sms_user_phone, pBasic[i].site_sms_telnum);
						strcpy_st(pUsers[i].sms_use, pBasic[i].site_sms_use);
						strcpy_st(pUsers[i].sms_site_name, "신규1");
					}
					GetUserString(szUsers, sizeof(szUsers), pUsers, nUserBasicCnt, USE_ALL);
					OnlicLogf(LOG_UDP, "%s / new site registered basic users:%d -> %s", szLog, nUserBasicCnt, szUsers);
					if(strlen_st(szUsers) > 0){
						AtalkSendUsers(pWait->m_pAtalkDB, pUsers, nUserBasicCnt, USE_ALL, "newSite001", szSendData);
						SmsSendUsers(pWait->m_pSmsDB, "장애발생", szMsg, nUserBasicCnt, szUsers);
					}
					delete [] pUsers;
				}
				if(pBasic != NULL) delete [] pBasic;
			}else{
				OnlicLogf(LOG_UDP, "등록 되었지만 알 수 없는 아이피 입니다. CallCenter_Link, SITE_SMS_USER 테이블 확인 해 주세요. szIP:%s, szRealIP:%s, port:%u, nType:%d", szIP, szRealIP, pItem->local_port, nType);
			}
			UDP_ACK();
		}else{
			OnlicLogf(LOG_UDP, "szIP 알 수 없는 아이피 입니다. szIP:%s, szRealIP:%s, port:%u, nType:%d", szIP, szRealIP, pItem->local_port, nType);
			UDP_ACK();
		}
	}
#undef UDP_ACK
}

/* ---------------- 링크 감시 (OnCheckLinkData :1425-1535, 타이머 스레드) ---------------- */
void LinkCheck_OnTimer()
{
	static CDirectODBC *s_pLicDB = NULL, *s_pAtalkDB = NULL, *s_pSmsDB = NULL;
	static int s_bInit = FALSE;
	CC_LINKINFO *pCCLink = NULL;
	SMS_SITE_USERS *pUsers = NULL;
	char szDate[21] = "", szLocalDay[21] = "", szMsg[1024] = "", szUsers[2048] = "", szUseIP[200] = "";
	const char *szSiteName = NULL;
	int i = 0, nCnt = 0, nUserCnt = 0;

	if(onlic_start != TRUE) return;

	/* 타이머 스레드 전용 핸들 (Windows 는 메인 스레드 m_dlgSql 공용) */
	if(s_pLicDB == NULL){
		s_pLicDB = new CDirectODBC();
		if(s_pLicDB->DbConnectODBC(sys_conf.db_lic, sys_conf.client_encoding) != TRUE) OnlicLog(LOG_SOFT, "LinkCheck lic db connect fail - retry on use");
		if(sys_conf.atalk_use == TRUE){
			s_pAtalkDB = new CDirectODBC();
			if(s_pAtalkDB->DbConnectODBC(sys_conf.db_atalk, sys_conf.client_encoding) != TRUE) OnlicLog(LOG_SOFT, "LinkCheck atalk db connect fail - retry on use");
		}
		if(sys_conf.sms_use == TRUE){
			s_pSmsDB = new CDirectODBC();
			if(s_pSmsDB->DbConnectODBC(sys_conf.db_sms, sys_conf.client_encoding) != TRUE) OnlicLog(LOG_SOFT, "LinkCheck sms db connect fail - retry on use");
		}
	}

	DbLic_Now14(szDate);
	if(s_bInit == FALSE){												/* :1440 최초 1회: 정상 사이트 기준시각 초기화 */
		DbLic_UpdateTimeNotError(s_pLicDB, szDate);
		s_bInit = TRUE;
	}
	DbLic_AgoMinute14(sys_conf.link_error_after_min, szLocalDay);		/* :1445 GetAgoTimeProc(21) */

	nCnt = DbLic_GetLinkErrorSites(s_pLicDB, szLocalDay, &pCCLink);	/* :1447-1458 */
	if(nCnt <= 0 || pCCLink == NULL) return;

	for(i = 0; i < nCnt; i++){
		szSiteName = (strlen_st(pCCLink[i].cc_site_name) > 0) ? pCCLink[i].cc_site_name : pCCLink[i].cc_name;
		if(strcmp(pCCLink[i].cc_ip, pCCLink[i].cc_real_ip) == 0) strcpy_st(szUseIP, pCCLink[i].cc_ip);
		else nprintf(szUseIP, 199, "%s(%s)", pCCLink[i].cc_real_ip, pCCLink[i].cc_ip);

		nprintf(szMsg, 1023, AlarmText(TXT_LINK_ERROR), szSiteName, szUseIP);	/* :1486 */
		OnlicLogf(LOG_ALARM, "LinkCheck error site idx:%d name:%s ip:%s rip:%s", pCCLink[i].cc_index, szSiteName, pCCLink[i].cc_ip, pCCLink[i].cc_real_ip);
		AlarmUser_Send(szMsg, 1);
		AlarmWebTh_Push(szMsg, 1);

		nUserCnt = DbLic_GetSmsSiteUsers(s_pLicDB, pCCLink[i].cc_ip, pCCLink[i].cc_real_ip, &pUsers);
		if(nUserCnt > 0){
			GetUserString(szUsers, sizeof(szUsers), pUsers, nUserCnt, USE_ALL);
			AtalkSendUsers(s_pAtalkDB, pUsers, nUserCnt, USE_ALL, "linkDataError001", szMsg);		/* :1510 szMsg 그대로 (절단 없음) */
			SmsSendUsers(s_pSmsDB, "장애발생", szMsg, nUserCnt, szUsers);
			delete [] pUsers; pUsers = NULL;
		}
		DbLic_SetLinkError(s_pLicDB, pCCLink[i].cc_ip, pCCLink[i].cc_real_ip);						/* :1523 */
		AlarmWebTh_EmailPush(s_pLicDB, SEND_TYPE_NET_ERROR, pCCLink[i].cc_ip, pCCLink[i].cc_real_ip, pCCLink[i].cc_name, "", "");	/* :1525 */
	}
	delete [] pCCLink;
}
