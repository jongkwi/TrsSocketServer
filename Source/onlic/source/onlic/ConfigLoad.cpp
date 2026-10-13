/*
**	ConfigLoad.cpp : cfg/onlic.ini 로더 + 전역 정의 (onmw ConfigLoad.cpp 패턴)
**	기본값은 Windows 소스의 하드코딩 값과 동일하게 둔다 (ini 가 없어도 같은 동작)
*/
#include "onlicDefine.h"

/* ---------------- 전역 정의 ---------------- */
ONLIC_CONF	sys_conf;
CSystemLoad	sys_load;
CLogObject	*lic_log_object = NULL;
CMwTimer	*licTm = NULL;
int			onlic_start = 0;
long		onlic_pid = 0;
int			daemon_start = TRUE;
int			daemon_proc = 0;
char		m_gPidFile[1024] = "";
ONLIC_SOCKET	m_SockLic, m_SockAlarm, m_SockUdp;
pthread_t	lic_pid = 0, alarm_pid = 0, udp_pid = 0;
CListObject	list_lic_socket, list_alarm_socket;

CConfigLoad::CConfigLoad(const char *szConfig)
{
	memset_st(m_strCfgFile, 0, sizeof(m_strCfgFile));
	strncpy_st(m_strCfgFile, szConfig, sizeof(m_strCfgFile)-1);
}

CConfigLoad::~CConfigLoad()
{
}

int CConfigLoad::GetConfigInfoProc(int nCfgType, char *szData)
{
	return GetConfigProc(nCfgType, szData);
}

int CConfigLoad::GetConfigInfoProc(int nCfgType, int &nValue)
{
	char szValue[256] = "";
	int nResult = GetConfigProc(nCfgType, szValue);
	nValue = atoi(szValue);
	return nResult;
}

int CConfigLoad::GetConfigInfoProc(int nCfgType, long &lValue)
{
	char szValue[256] = "";
	int nResult = GetConfigProc(nCfgType, szValue);
	lValue = atol(szValue);
	return nResult;
}

/*
**	키별 섹션/이름/기본값. 반환 FALSE = 파일 또는 키 없음 (기본값은 szData 에 들어 있음)
*/
int CConfigLoad::GetConfigProc(int nCfgType, char *szData)
{
	if(strlen_st(m_strCfgFile) <= 0) return FALSE;
	strcpy_st(szData, "");

	switch(nCfgType){
	  /* [IP_CONF] */
	  case CFG_MY_IP:				return (GetPrivateProfileStringLW("IP_CONF", "MY_IP", "", szData, 50) > 0);
	  case CFG_LIC_PORT:			return (GetPrivateProfileStringLW("IP_CONF", "LIC_PORT", "6321", szData, 10) > 0);
	  case CFG_ALARM_PORT:			return (GetPrivateProfileStringLW("IP_CONF", "ALARM_PORT", "6770", szData, 10) > 0);
	  case CFG_UDP_PORT:			return (GetPrivateProfileStringLW("IP_CONF", "EVENT_UDP_PORT", "5363", szData, 10) > 0);
	  /* [ROLE] */
	  case CFG_ROLE_LICENSE:		return (GetPrivateProfileStringLW("ROLE", "LICENSE", "1", szData, 10) > 0);
	  case CFG_ROLE_ALARM:			return (GetPrivateProfileStringLW("ROLE", "ALARM", "1", szData, 10) > 0);
	  case CFG_ROLE_LINK_CHECK:		return (GetPrivateProfileStringLW("ROLE", "LINK_CHECK", "1", szData, 10) > 0);
	  /* [PATH_CONF] */
	  case CFG_LOG_PATH:			return (GetPrivateProfileStringLW("PATH_CONF", "LOG_PATH", "../log", szData, 255) > 0);
	  case CFG_LOG_AGO_DAY:			return (GetPrivateProfileStringLW("PATH_CONF", "LOG_AGO_DAY", "30", szData, 10) > 0);
	  case CFG_PID_FILE:			return (GetPrivateProfileStringLW("PATH_CONF", "PID_FILE", PID_FILE, szData, 255) > 0);
	  /* [DATABASE_INFO] - 비밀번호는 로그에 남기지 않음 */
	  case CFG_LIC_DSN:				return (GetPrivateProfileStringLW("DATABASE_INFO", "LIC_DSN", "ipcs_log", szData, 50) > 0);
	  case CFG_LIC_ID:				return (GetPrivateProfileStringLW("DATABASE_INFO", "LIC_ID", "", szData, 50) > 0);
	  case CFG_LIC_PW:				return (GetPrivateProfileStringLW("DATABASE_INFO", "LIC_PW", "", szData, 50, TRUE) > 0);
	  case CFG_ATALK_DSN:			return (GetPrivateProfileStringLW("DATABASE_INFO", "ATALK_DSN", "onpia_check", szData, 50) > 0);
	  case CFG_ATALK_ID:			return (GetPrivateProfileStringLW("DATABASE_INFO", "ATALK_ID", "", szData, 50) > 0);
	  case CFG_ATALK_PW:			return (GetPrivateProfileStringLW("DATABASE_INFO", "ATALK_PW", "", szData, 50, TRUE) > 0);
	  case CFG_SMS_DSN:				return (GetPrivateProfileStringLW("DATABASE_INFO", "SMS_DSN", "onpia_sms", szData, 50) > 0);
	  case CFG_SMS_ID:				return (GetPrivateProfileStringLW("DATABASE_INFO", "SMS_ID", "", szData, 50) > 0);
	  case CFG_SMS_PW:				return (GetPrivateProfileStringLW("DATABASE_INFO", "SMS_PW", "", szData, 50, TRUE) > 0);
	  case CFG_STATEMENT_TIMEOUT:	return (GetPrivateProfileStringLW("DATABASE_INFO", "STATEMENT_TIMEOUT", "10", szData, 10) > 0);
	  case CFG_CLIENT_ENCODING:		return (GetPrivateProfileStringLW("DATABASE_INFO", "CLIENT_ENCODING", "EUCKR", szData, 30) > 0);
	  /* [LINK_CHECK] (TrsSocketServer.cpp OnCheckLinkData 상수) */
	  case CFG_LINK_CHECK_PERIOD_SEC:	return (GetPrivateProfileStringLW("LINK_CHECK", "CHECK_PERIOD_SEC", "5", szData, 10) > 0);
	  case CFG_LINK_ERROR_AFTER_MIN:	return (GetPrivateProfileStringLW("LINK_CHECK", "ERROR_AFTER_MIN", "21", szData, 10) > 0);
	  case CFG_NEW_SITE_SKIP_HOUR:		return (GetPrivateProfileStringLW("LINK_CHECK", "NEW_SITE_SKIP_HOUR", "3", szData, 10) > 0);
	  case CFG_NAS_FAIL_NOTIFY_MAX:		return (GetPrivateProfileStringLW("LINK_CHECK", "NAS_FAIL_NOTIFY_MAX", "10", szData, 10) > 0);
	  case CFG_NAS_FAIL_COUNT_MAX:		return (GetPrivateProfileStringLW("LINK_CHECK", "NAS_FAIL_COUNT_MAX", "999", szData, 10) > 0);
	  /* [ATALK] (TrsSocketServer.cpp:874 INSERT 고정값) */
	  case CFG_ATALK_USE:			return (GetPrivateProfileStringLW("ATALK", "USE", "1", szData, 10) > 0);
	  case CFG_ATALK_SENDER_KEY:	return (GetPrivateProfileStringLW("ATALK", "SENDER_KEY", "", szData, 100, TRUE) > 0);
	  case CFG_ATALK_CALLBACK:		return (GetPrivateProfileStringLW("ATALK", "CALLBACK", "15889310", szData, 20) > 0);
	  case CFG_ATALK_SUBJECT:		return (GetPrivateProfileStringLW("ATALK", "SUBJECT", "OnpiaATalk", szData, 50) > 0);
	  case CFG_ATALK_TRAN_TYPE:		return (GetPrivateProfileStringLW("ATALK", "TRAN_TYPE", "5", szData, 10) > 0);
	  /* [SMS] (SDK_SMS_SEND 고정값) */
	  case CFG_SMS_USE:				return (GetPrivateProfileStringLW("SMS", "USE", "0", szData, 10) > 0);
	  case CFG_SMS_USER_ID:			return (GetPrivateProfileStringLW("SMS", "USER_ID", "onpia", szData, 20) > 0);
	  case CFG_SMS_CALLBACK:		return (GetPrivateProfileStringLW("SMS", "CALLBACK", "15889310", szData, 20) > 0);
	  /* [HTTP] (AlramWeb.cpp URL) */
	  case CFG_WEBPUSH_URL:			return (GetPrivateProfileStringLW("HTTP", "WEBPUSH_URL", "http://onpia2.cafe24.com/onpia_push_system/send_push.php", szData, 511) > 0);
	  case CFG_MAIL_URL:			return (GetPrivateProfileStringLW("HTTP", "MAIL_URL", "http://www.onpia.co.kr/onpiaMailInput.do", szData, 511) > 0);
	  case CFG_HTTP_TIMEOUT_SEC:	return (GetPrivateProfileStringLW("HTTP", "TIMEOUT_SEC", "5", szData, 10) > 0);
	  /* [PROTOCOL] 호환 상수 */
	  case CFG_RECV_CHUNK:			return (GetPrivateProfileStringLW("PROTOCOL", "RECV_CHUNK", "500", szData, 10) > 0);
	  case CFG_RESIDUAL_MAX:		return (GetPrivateProfileStringLW("PROTOCOL", "RESIDUAL_MAX", "1000", szData, 10) > 0);
	  case CFG_ALARM_MSG_MAX:		return (GetPrivateProfileStringLW("PROTOCOL", "ALARM_MSG_MAX", "80", szData, 10) > 0);
	  case CFG_LIC_IDLE_TIMEOUT_SEC:	return (GetPrivateProfileStringLW("PROTOCOL", "LIC_IDLE_TIMEOUT_SEC", "30", szData, 10) > 0);
	  default:
		return FALSE;
	}
}

/*
**	GetPrivateProfileStringUsr 래퍼: 읽은 값을 run 로그에 남긴다 (nSecret 이면 값 마스킹)
*/
long CConfigLoad::GetPrivateProfileStringLW(const char* lpAppName, const char* lpKeyName, const char* lpDefault, char* lpReturnedString, long nSize, int nSecret)
{
	long lResult = FALSE;
	char szLog[MAX_LOG_CHAR+1] = "";

	lResult = GetPrivateProfileStringUsr((char*)lpAppName, (char*)lpKeyName, lpDefault, lpReturnedString, nSize, (char*)m_strCfgFile);

	if(nSecret == TRUE)
		nprintf(szLog, MAX_LOG_CHAR, "[%s] %s = %s", lpAppName, lpKeyName, (strlen_st(lpReturnedString) > 0) ? "****" : "(empty)");
	else
		nprintf(szLog, MAX_LOG_CHAR, "[%s] %s = %s (default:%s)", lpAppName, lpKeyName, lpReturnedString, lpDefault);

	OnlicLog(LOG_SOFT, szLog);
	return lResult;
}
