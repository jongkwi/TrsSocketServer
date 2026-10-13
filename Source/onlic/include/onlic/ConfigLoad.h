/*
**	ConfigLoad.h : cfg/onlic.ini 로더 (onmw CConfigLoad 패턴)
*/
#ifndef ___ONLIC_CONFIGLOAD_H___
#define ___ONLIC_CONFIGLOAD_H___

/* 설정 키 (GetConfigProc switch) */
enum ONLIC_CFG_KEY
{
	CFG_MY_IP = 1,
	CFG_LIC_PORT,
	CFG_ALARM_PORT,
	CFG_UDP_PORT,
	CFG_ROLE_LICENSE,
	CFG_ROLE_ALARM,
	CFG_ROLE_LINK_CHECK,
	CFG_LOG_PATH,
	CFG_LOG_AGO_DAY,
	CFG_PID_FILE,
	CFG_LIC_DSN,
	CFG_LIC_ID,
	CFG_LIC_PW,
	CFG_ATALK_DSN,
	CFG_ATALK_ID,
	CFG_ATALK_PW,
	CFG_SMS_DSN,
	CFG_SMS_ID,
	CFG_SMS_PW,
	CFG_STATEMENT_TIMEOUT,
	CFG_CLIENT_ENCODING,
	CFG_LINK_CHECK_PERIOD_SEC,
	CFG_LINK_ERROR_AFTER_MIN,
	CFG_NEW_SITE_SKIP_HOUR,
	CFG_NAS_FAIL_NOTIFY_MAX,
	CFG_NAS_FAIL_COUNT_MAX,
	CFG_ATALK_USE,
	CFG_ATALK_SENDER_KEY,
	CFG_ATALK_CALLBACK,
	CFG_ATALK_SUBJECT,
	CFG_ATALK_TRAN_TYPE,
	CFG_SMS_USE,
	CFG_SMS_USER_ID,
	CFG_SMS_CALLBACK,
	CFG_WEBPUSH_URL,
	CFG_MAIL_URL,
	CFG_HTTP_TIMEOUT_SEC,
	CFG_RECV_CHUNK,
	CFG_RESIDUAL_MAX,
	CFG_ALARM_MSG_MAX,
	CFG_LIC_IDLE_TIMEOUT_SEC
};

class CConfigLoad
{
public:
	CConfigLoad(const char *szConfig);
	virtual ~CConfigLoad();

	int		GetConfigInfoProc(int nCfgType, char *szData);
	int		GetConfigInfoProc(int nCfgType, int &nValue);
	int		GetConfigInfoProc(int nCfgType, long &lValue);
	int		GetConfigProc(int nCfgType, char *szData);
	long	GetPrivateProfileStringLW(const char* lpAppName, const char* lpKeyName, const char* lpDefault, char* lpReturnedString, long nSize, int nSecret = FALSE);

protected:
	char	m_strCfgFile[256];		/* onmw 는 51 (경로 절단 결함) → 256 */
};

#endif /* ConfigLoad.h */
