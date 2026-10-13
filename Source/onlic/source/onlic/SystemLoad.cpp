/*
**	SystemLoad.cpp : 기동 순서 구현 (onmw SystemLoad.cpp 패턴)
*/
#include "onlicDefine.h"
#include <stdarg.h>

CSystemLoad::CSystemLoad()
{
	memset_st(&sys_conf, 0, sizeof(ONLIC_CONF));
	memset_st(m_szConfig, 0, sizeof(m_szConfig));
	memset_st(m_szLogPath, 0, sizeof(m_szLogPath));
	m_nLogView = FALSE;
	m_pLicDB = NULL;
	m_pAtalkDB = NULL;
	m_pSmsDB = NULL;
	m_pSmsExclude = NULL;
	m_nSmsExcludeCnt = 0;
}

CSystemLoad::~CSystemLoad()
{
	KillSystemMemory();
}

/*
**	기동 순서
**	1 로그 → 2 ini → 3 역할 검증 → 4 DB 연결 → 5 제외시간 로드 → 6 타이머 → 7 워커 → 8 리슨 소켓
**	(소켓 스레드 기동은 onlicmain.cpp RunOnlicSystem 에서, onmw 와 동일)
*/
int CSystemLoad::StartSystem(const char *szConfig)
{
	int nRet = FALSE;
	char szErrMsg[201] = "";

	strncpy_st(m_szConfig, szConfig, sizeof(m_szConfig)-1);

	/* 1. 로그 객체 */
	LogObj_Create();

	/* 2. 설정 */
	if((nRet = GetConfigFileFatchProc()) != SYSTEMLOAD_SUCCESS){
		PrintSystemError(nRet, LOG_YN_PRINT, m_szConfig);
		return nRet;
	}
	LogObj_SetDay();
	ConfigLogWrite();
	AlarmProc_Init();			/* [ALARM_MSG] 문구 (기본값 = 원본 리터럴) */

	/* 3. 역할 */
	if((nRet = CheckRoleProc()) != SYSTEMLOAD_SUCCESS){
		PrintSystemError(nRet, LOG_YN_PRINT);
		return nRet;
	}

	/* 4. DB 연결 (3단계 DirectODBC) */
	if((nRet = SystemDBConnectProc()) != SYSTEMLOAD_SUCCESS){
		nprintf(szErrMsg, 200, "odbc:%s / %s / %s", sys_conf.db_lic.szDSN, sys_conf.db_atalk.szDSN, sys_conf.db_sms.szDSN);
		PrintSystemError(nRet, LOG_YN_PRINT, szErrMsg);
		return nRet;
	}

	/* 5. 문자 제외 시간 (Windows SmsExcludeLoadProc, 기동 시 1회) + 기동 시 cc_update_time 초기화 (CreateSocketProc:250-259) */
	if(sys_conf.role_alarm == TRUE){
		char szDate[21] = "";
		SmsExcludeLoadProc();
		DbLic_Now14(szDate);
		DbLic_UpdateTimeAll(m_pLicDB, szDate);
	}

	/* 6. 타이머 (Windows LINK_TIMER 5초) */
	licTm = new CMwTimer();
	licTm->MwTimerInit("LIC");
	if(sys_conf.role_link_check == TRUE){
		licTm->SetUserTimer(LINK_TIMER, (long)sys_conf.link_check_period_sec * 1000);
	}

	/* 7. 워커 (5단계) */
	if(sys_conf.role_license == TRUE)	LicRecvWaitStackInit();
	if(sys_conf.role_alarm == TRUE){
		EventRecvWaitStackInit();
		HttpWaitStackInit();
	}

	/* 8. 리슨 소켓 (5단계 SocketThread) */
	if(LoadSocketProc() <= FALSE){
		nRet = SYSTEMLOAD_SOCK_ERROR;
		PrintSystemError(nRet, LOG_YN_FILE);
		return nRet;
	}

	OnlicLogf(LOG_SOFT, "onlic start. ver:%s/%s base:%s role(lic:%d alarm:%d link:%d) ip:%s",
		ONLIC_VERSION, ONLIC_LAST_UPDATE, ONLIC_WIN_BASE_VERSION,
		sys_conf.role_license, sys_conf.role_alarm, sys_conf.role_link_check,
		(strlen_st(sys_conf.my_ip) > 0) ? sys_conf.my_ip : "ANY");

	return SYSTEMLOAD_SUCCESS;
}

/*
**	ini 전체 로드. MY_IP 가 비어 있으면 INADDR_ANY (Windows 동작)
*/
int CSystemLoad::GetConfigFileFatchProc()
{
	CConfigLoad cfg(m_szConfig);
	char szTemp[256] = "";

	/* 파일 존재 확인: LOG_PATH 키를 읽어 보고 파일 자체가 없으면 오류 */
	if(access(m_szConfig, R_OK) != 0) return SYSTEMLOAD_CONFIG_FILE_ERROR;

	cfg.GetConfigInfoProc(CFG_MY_IP,			sys_conf.my_ip);
	cfg.GetConfigInfoProc(CFG_LIC_PORT,			sys_conf.lic_port);
	cfg.GetConfigInfoProc(CFG_ALARM_PORT,		sys_conf.alarm_port);
	cfg.GetConfigInfoProc(CFG_UDP_PORT,			sys_conf.udp_port);

	cfg.GetConfigInfoProc(CFG_ROLE_LICENSE,		sys_conf.role_license);
	cfg.GetConfigInfoProc(CFG_ROLE_ALARM,		sys_conf.role_alarm);
	cfg.GetConfigInfoProc(CFG_ROLE_LINK_CHECK,	sys_conf.role_link_check);

	cfg.GetConfigInfoProc(CFG_LOG_PATH,			sys_conf.log_path);
	cfg.GetConfigInfoProc(CFG_LOG_AGO_DAY,		sys_conf.log_ago_day);
	cfg.GetConfigInfoProc(CFG_PID_FILE,			sys_conf.pid_file);

	cfg.GetConfigInfoProc(CFG_LIC_DSN,			sys_conf.db_lic.szDSN);
	cfg.GetConfigInfoProc(CFG_LIC_ID,			sys_conf.db_lic.szUID);
	cfg.GetConfigInfoProc(CFG_LIC_PW,			sys_conf.db_lic.szPWD);
	strcpy_st(sys_conf.db_lic.szName, "LIC");
	cfg.GetConfigInfoProc(CFG_ATALK_DSN,		sys_conf.db_atalk.szDSN);
	cfg.GetConfigInfoProc(CFG_ATALK_ID,			sys_conf.db_atalk.szUID);
	cfg.GetConfigInfoProc(CFG_ATALK_PW,			sys_conf.db_atalk.szPWD);
	strcpy_st(sys_conf.db_atalk.szName, "ATALK");
	cfg.GetConfigInfoProc(CFG_SMS_DSN,			sys_conf.db_sms.szDSN);
	cfg.GetConfigInfoProc(CFG_SMS_ID,			sys_conf.db_sms.szUID);
	cfg.GetConfigInfoProc(CFG_SMS_PW,			sys_conf.db_sms.szPWD);
	strcpy_st(sys_conf.db_sms.szName, "SMS");
	cfg.GetConfigInfoProc(CFG_STATEMENT_TIMEOUT, sys_conf.statement_timeout);
	cfg.GetConfigInfoProc(CFG_CLIENT_ENCODING,	sys_conf.client_encoding);
	sys_conf.db_lic.lStatementTimeout = sys_conf.db_atalk.lStatementTimeout = sys_conf.db_sms.lStatementTimeout = sys_conf.statement_timeout;

	cfg.GetConfigInfoProc(CFG_LINK_CHECK_PERIOD_SEC,	sys_conf.link_check_period_sec);
	cfg.GetConfigInfoProc(CFG_LINK_ERROR_AFTER_MIN,		sys_conf.link_error_after_min);
	cfg.GetConfigInfoProc(CFG_NEW_SITE_SKIP_HOUR,		sys_conf.new_site_skip_hour);
	cfg.GetConfigInfoProc(CFG_NAS_FAIL_NOTIFY_MAX,		sys_conf.nas_fail_notify_max);
	cfg.GetConfigInfoProc(CFG_NAS_FAIL_COUNT_MAX,		sys_conf.nas_fail_count_max);

	cfg.GetConfigInfoProc(CFG_ATALK_USE,		sys_conf.atalk_use);
	cfg.GetConfigInfoProc(CFG_ATALK_SENDER_KEY,	sys_conf.atalk_sender_key);
	cfg.GetConfigInfoProc(CFG_ATALK_CALLBACK,	sys_conf.atalk_callback);
	cfg.GetConfigInfoProc(CFG_ATALK_SUBJECT,	sys_conf.atalk_subject);
	cfg.GetConfigInfoProc(CFG_ATALK_TRAN_TYPE,	sys_conf.atalk_tran_type);

	cfg.GetConfigInfoProc(CFG_SMS_USE,			sys_conf.sms_use);
	cfg.GetConfigInfoProc(CFG_SMS_USER_ID,		sys_conf.sms_user_id);
	cfg.GetConfigInfoProc(CFG_SMS_CALLBACK,		sys_conf.sms_callback);

	cfg.GetConfigInfoProc(CFG_WEBPUSH_URL,		sys_conf.webpush_url);
	cfg.GetConfigInfoProc(CFG_MAIL_URL,			sys_conf.mail_url);
	cfg.GetConfigInfoProc(CFG_HTTP_TIMEOUT_SEC,	sys_conf.http_timeout_sec);

	cfg.GetConfigInfoProc(CFG_RECV_CHUNK,		sys_conf.recv_chunk);
	cfg.GetConfigInfoProc(CFG_RESIDUAL_MAX,		sys_conf.residual_max);
	cfg.GetConfigInfoProc(CFG_ALARM_MSG_MAX,	sys_conf.alarm_msg_max);
	cfg.GetConfigInfoProc(CFG_LIC_IDLE_TIMEOUT_SEC, sys_conf.lic_idle_timeout_sec);

	/* 호환 상수 방어: 잘못 바꾸면 클라이언트 재조립이 깨지므로 기본값으로 되돌린다 */
	if(sys_conf.recv_chunk != MAXLENGTH){
		OnlicLogf(LOG_SOFT, "RECV_CHUNK %d is not compatible, forced to %d", sys_conf.recv_chunk, MAXLENGTH);
		sys_conf.recv_chunk = MAXLENGTH;
	}
	if(sys_conf.residual_max != RESIDUAL_MAX){
		OnlicLogf(LOG_SOFT, "RESIDUAL_MAX %d is not compatible, forced to %d", sys_conf.residual_max, RESIDUAL_MAX);
		sys_conf.residual_max = RESIDUAL_MAX;
	}
	if(sys_conf.alarm_msg_max <= 0 || sys_conf.alarm_msg_max > ALARM_PUSH_MAX) sys_conf.alarm_msg_max = ALARM_MSG_MAX;
	if(sys_conf.link_check_period_sec <= 0) sys_conf.link_check_period_sec = 5;
	if(sys_conf.link_error_after_min <= 0) sys_conf.link_error_after_min = 21;
	if(sys_conf.http_timeout_sec <= 0) sys_conf.http_timeout_sec = 5;
	if(sys_conf.lic_port <= 0) sys_conf.lic_port = IPCS_PORT;
	if(sys_conf.alarm_port <= 0) sys_conf.alarm_port = ALARM_PORT;
	if(sys_conf.udp_port <= 0) sys_conf.udp_port = UDP_PORT;

	strcpy_st(m_szLogPath, sys_conf.log_path);
	if(strlen_st(m_gPidFile) <= 0 && strlen_st(sys_conf.pid_file) > 0) strcpy_st(m_gPidFile, sys_conf.pid_file);

	return SYSTEMLOAD_SUCCESS;
}

/*
**	역할 검증: 둘 다 0 이면 할 일이 없음. LINK_CHECK 는 ALARM 없이는 의미 없음(알람 DB 만 갱신하고 통지 못함)
*/
int CSystemLoad::CheckRoleProc()
{
	if(sys_conf.role_license == FALSE && sys_conf.role_alarm == FALSE) return SYSTEMLOAD_ROLE_ERROR;
	if(sys_conf.role_link_check == TRUE && sys_conf.role_alarm == FALSE){
		OnlicLog(LOG_SOFT, "[ROLE] LINK_CHECK=1 but ALARM=0 : LINK_CHECK forced to 0");
		sys_conf.role_link_check = FALSE;
	}
	if(sys_conf.atalk_use == TRUE && strlen_st(sys_conf.atalk_sender_key) <= 0 && sys_conf.role_alarm == TRUE){
		OnlicLog(LOG_SOFT, "[ATALK] USE=1 but SENDER_KEY empty : atalk insert will fail");
	}
	return SYSTEMLOAD_SUCCESS;
}

/*
**	DB 연결 (Windows TrsSocketServerDlg.cpp:216-224 : DSN 3개, 실패해도 기동은 계속)
**	구현은 3단계 DirectODBC 작성 후 채운다. 역할에 따라 필요한 DSN 만 연결
*/
int CSystemLoad::SystemDBConnectProc()
{
	/* 라이선스/알람 공통 DB (NASA_License). 실패하면 Windows 는 그대로 기동했으나(이후 매 쿼리 실패)
	** Linux 판은 기동 중단 → 운영자가 즉시 인지. (결정 B2 는 '운영 중' 장애 응답에 관한 것) */
	m_pLicDB = new CDirectODBC();
	if(m_pLicDB->DbConnectODBC(sys_conf.db_lic, sys_conf.client_encoding) != TRUE){
		return SYSTEMLOAD_SYSDB_CONNECT_ERROR;
	}

	if(sys_conf.role_alarm == TRUE){
		/* 알림톡 큐 (외부 PostgreSQL). 실패는 경고 - ExecSql 시점에 재접속 (Windows 동일) */
		if(sys_conf.atalk_use == TRUE){
			m_pAtalkDB = new CDirectODBC();
			if(m_pAtalkDB->DbConnectODBC(sys_conf.db_atalk, sys_conf.client_encoding) != TRUE){
				OnlicLogf(LOG_SOFT, "atalk db connect fail (dsn:%s) - will retry on use", sys_conf.db_atalk.szDSN);
			}
		}
		/* SMS 큐 (SDK_SMS_SEND). [SMS] USE=1 일 때만 연결. 0 이면 IVR 200/201 도 DB 없이 로그만 남김 */
		if(sys_conf.sms_use == TRUE){
			m_pSmsDB = new CDirectODBC();
			if(m_pSmsDB->DbConnectODBC(sys_conf.db_sms, sys_conf.client_encoding) != TRUE){
				OnlicLogf(LOG_SOFT, "sms db connect fail (dsn:%s) - will retry on use", sys_conf.db_sms.szDSN);
			}
		}
	}
	return SYSTEMLOAD_SUCCESS;
}

/*
**	Windows SmsExcludeLoadProc : SITE_SmsExcludeTime 을 기동 시 1회 캐시 (변경 시 재기동 필요 - 원본 동일)
*/
int SmsExcludeLoadProc()
{
	int i = 0;
	if(sys_load.m_pSmsExclude != NULL){ delete [] sys_load.m_pSmsExclude; sys_load.m_pSmsExclude = NULL; }
	sys_load.m_nSmsExcludeCnt = DbLic_GetSmsExclude(sys_load.m_pLicDB, &sys_load.m_pSmsExclude);
	OnlicLogf(LOG_SMSEX, "SmsExcludeLoadProc count:%d", sys_load.m_nSmsExcludeCnt);
	for(i = 0; i < sys_load.m_nSmsExcludeCnt; i++){
		OnlicLogf(LOG_SMSEX, "  [%d] idx:%d type:%d msg:%s time:%s~%s", i,
			sys_load.m_pSmsExclude[i].sm_index, sys_load.m_pSmsExclude[i].sm_type, sys_load.m_pSmsExclude[i].sm_msg,
			sys_load.m_pSmsExclude[i].sm_stime, sys_load.m_pSmsExclude[i].sm_etime);
	}
	return TRUE;
}

/*
**	리슨 소켓 (Windows CreateSocketProc: 6321/6770 Listen, 5363 UDP bind, 모두 INADDR_ANY)
**	구현은 5단계 SocketThread 작성 후 채운다
*/
int CSystemLoad::LoadSocketProc()
{
	Pthread_mutex_init(&lic_socklist_mutex, NULL);
	Pthread_mutex_init(&alarm_socklist_mutex, NULL);

	if(sys_conf.role_license == TRUE){
		if(Tcp_CreateServerSocket(m_SockLic, sys_conf.my_ip, sys_conf.lic_port, PORT_TYPE_LIC) != TRUE) return FALSE;
	}
	if(sys_conf.role_alarm == TRUE){
		if(Tcp_CreateServerSocket(m_SockAlarm, sys_conf.my_ip, sys_conf.alarm_port, PORT_TYPE_ALARM) != TRUE) return FALSE;
		if(Udp_CreateServerSocket(m_SockUdp, sys_conf.my_ip, sys_conf.udp_port) != TRUE) return FALSE;
	}
	return TRUE;
}

/*
**	로그 객체: 1개 (파일은 타입별로 분리됨 - onlicDefine.h 매핑). LOG_PATH 는 ini 에서 먼저 읽는다
*/
void CSystemLoad::LogObj_Create()
{
	CConfigLoad cfg(m_szConfig);
	char szPath[256] = "";

	/* 로그 객체 생성 전이라 이 호출은 파일에 남지 않음 */
	GetPrivateProfileStringUsr((char*)"PATH_CONF", (char*)"LOG_PATH", "../log", szPath, 255, (char*)m_szConfig);
	strcpy_st(m_szLogPath, szPath);

	lic_log_object = new CLogObject();			/* onmw 는 Malloc (생성자 미실행 결함) */
	lic_log_object->LogObjectStart("LIC", 400, m_szLogPath);
}

void CSystemLoad::LogObj_SetDay()
{
	if(lic_log_object != NULL && sys_conf.log_ago_day > 0)
		lic_log_object->LogSetAgoday((unsigned int)sys_conf.log_ago_day);
}

/*
**	기동 시 설정 요약을 run 로그에 남김 (비밀번호 제외)
*/
void CSystemLoad::ConfigLogWrite()
{
	OnlicLogf(LOG_SOFT, "config:%s ip:%s lic:%d alarm:%d udp:%d role(lic:%d alarm:%d link:%d)",
		m_szConfig, sys_conf.my_ip, sys_conf.lic_port, sys_conf.alarm_port, sys_conf.udp_port,
		sys_conf.role_license, sys_conf.role_alarm, sys_conf.role_link_check);
	OnlicLogf(LOG_SOFT, "db lic:%s/%s atalk:%s/%s sms:%s/%s timeout:%d enc:%s",
		sys_conf.db_lic.szDSN, sys_conf.db_lic.szUID,
		sys_conf.db_atalk.szDSN, sys_conf.db_atalk.szUID,
		sys_conf.db_sms.szDSN, sys_conf.db_sms.szUID,
		sys_conf.statement_timeout, sys_conf.client_encoding);
	OnlicLogf(LOG_SOFT, "link period:%ds error_after:%dmin newsite_skip:%dh nasfail notify<=%d max:%d",
		sys_conf.link_check_period_sec, sys_conf.link_error_after_min, sys_conf.new_site_skip_hour,
		sys_conf.nas_fail_notify_max, sys_conf.nas_fail_count_max);
	OnlicLogf(LOG_SOFT, "atalk use:%d callback:%s subject:%s tran_type:%d / sms use:%d user:%s callback:%s",
		sys_conf.atalk_use, sys_conf.atalk_callback, sys_conf.atalk_subject, sys_conf.atalk_tran_type,
		sys_conf.sms_use, sys_conf.sms_user_id, sys_conf.sms_callback);
	OnlicLogf(LOG_SOFT, "http webpush:%s mail:%s timeout:%ds / protocol chunk:%d residual:%d msgmax:%d idle:%ds",
		sys_conf.webpush_url, sys_conf.mail_url, sys_conf.http_timeout_sec,
		sys_conf.recv_chunk, sys_conf.residual_max, sys_conf.alarm_msg_max, sys_conf.lic_idle_timeout_sec);
}

void CSystemLoad::PrintSystemError(int error, int nLogWrite, const char* szAddMsg)
{
	char szPrint[MAX_PRINT+1] = "";
	char szTime[21] = "";

	GetLocalTimeProc(szTime);
	switch(error){
	  case SYSTEMLOAD_CONFIG_FILE_ERROR:
		nprintf(szPrint, MAX_PRINT, "%s SYSTEMLOAD_CONFIG_FILE_ERROR! (%s)", szTime, szAddMsg);
		break;
	  case SYSTEMLOAD_SYSDB_CONNECT_ERROR:
		nprintf(szPrint, MAX_PRINT, "%s SYSTEMLOAD_SYSDB_CONNECT_ERROR! - %s", szTime, szAddMsg);
		break;
	  case SYSTEMLOAD_SOCK_ERROR:
		nprintf(szPrint, MAX_PRINT, "%s SYSTEMLOAD_SOCK_ERROR! (port in use?)", szTime);
		break;
	  case SYSTEMLOAD_ROLE_ERROR:
		nprintf(szPrint, MAX_PRINT, "%s SYSTEMLOAD_ROLE_ERROR! [ROLE] LICENSE=0 and ALARM=0", szTime);
		break;
	  case SYSTEMLOAD_UNKNOWN:
	  default:
		nprintf(szPrint, MAX_PRINT, "%s SYSTEMLOAD_UNKNOWN! code:%d", szTime, error);
		break;
	}
	if(nLogWrite == LOG_YN_PRINT){
		printf("%s\n", szPrint); fflush(stdout);
	}
	if(lic_log_object != NULL) lic_log_object->LogObjectPush(szPrint, LOG_SOFT, LOG_YN_BOTH);
}

void CSystemLoad::KillSystemMemory()
{
	if(m_pSmsExclude != NULL){
		delete [] m_pSmsExclude;
		m_pSmsExclude = NULL;
	}
	m_nSmsExcludeCnt = 0;
	if(m_pLicDB != NULL){ m_pLicDB->DisConnectODBC(); delete m_pLicDB; m_pLicDB = NULL; }
	if(m_pAtalkDB != NULL){ m_pAtalkDB->DisConnectODBC(); delete m_pAtalkDB; m_pAtalkDB = NULL; }
	if(m_pSmsDB != NULL){ m_pSmsDB->DisConnectODBC(); delete m_pSmsDB; m_pSmsDB = NULL; }
}

/* ---------------- 로그 헬퍼 ---------------- */
void OnlicLog(int nLogType, const char* szLog)
{
	if(lic_log_object != NULL) lic_log_object->LogObjectPush((char*)szLog, nLogType, LOG_YN_FILE);
	else if(sys_load.m_nLogView == TRUE){ printf("%s\n", szLog); fflush(stdout); }
}

void OnlicLogf(int nLogType, const char* fmt, ...)
{
	char szLog[MAX_LOG_CHAR+1] = "";
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(szLog, MAX_LOG_CHAR, fmt, ap);
	va_end(ap);
	OnlicLog(nLogType, szLog);
}
