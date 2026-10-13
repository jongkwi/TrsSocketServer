/*
**	struct_def.h : onlic 구조체 정의
**	- DB 레코드 구조체 5종은 Windows MyDefine.h:410-467 이식 (SQLGetData 버퍼 = 컬럼 수용 길이)
**	  운영 DDL 보다 짧았던 3개 컬럼만 확대: cc_name 51→257, sms_site_name 51→257, (cc_nasupload_fail 은 GetValue 버퍼 21→129)
**	- onmw struct_def.h 와 이름이 겹치지 않도록 접두어 ONLIC_ 사용
*/
#ifndef ___ONLIC_STRUCT_DEF_H___
#define ___ONLIC_STRUCT_DEF_H___

/* ---------------- DB 레코드 (NASA_License) ---------------- */
typedef struct {
	DWORD	idx;
	char	email_site_ip[51];
	char	email_site_real_ip[51];
	char	email_site_name[101];
	int		alarm_type;
	char	alarm_proc_name[51];
	char	email_to[501];
	char	email_from[201];
	char	email_title[201];
	char	email_msg1[501];
	char	email_msg2[501];
	char	email_msg3[501];
	char	email_url[2049];		/* 결함 B 수정: 13번째 컬럼을 실제로 읽는다 */
	char	email_attr1[101];
	char	email_attr2[101];
	char	email_attr3[101];
	char	email_msg[2048];
} EMAIL_SITE_USERS;

typedef struct {
	char sms_site_ip[51];
	char sms_user_name[51];
	char sms_user_phone[21];
	char sms_site_name[257];		/* 운영 varchar(256) */
	char sms_site_real_ip[51];
	char sms_use[2];				/* 'Y' 일반+긴급, 'T' 긴급만, 그 외 미수신 */
} SMS_SITE_USERS;

typedef struct {
	int  site_sms_index;
	char site_sms_name[51];
	char site_sms_telnum[21];
	char site_sms_use[2];
} SMS_SITE_BASIC_USER;

typedef struct {
	int  cc_index;
	char cc_name[257];				/* 운영 varchar(256) */
	char cc_ip[51];
	char cc_real_ip[51];
	char cc_site_name[257];
} CC_LINKINFO;

typedef struct {
	int  sm_index;
	int  sm_type;
	char sm_msg[201];
	char sm_stime[11];				/* HHMM */
	char sm_etime[11];				/* HHMM */
	char sm_use[2];
} SMS_EXCLUDE;

/* ---------------- 설정 ---------------- */
typedef struct {
	char	szDSN[51];
	char	szUID[51];
	char	szPWD[51];
	char	szName[31];				/* 로그 표시용 */
	long	lStatementTimeout;
} DATABASEINFO;

typedef struct {
	/* [IP_CONF] */
	char	my_ip[51];
	int		lic_port;
	int		alarm_port;
	int		udp_port;
	/* [ROLE] */
	int		role_license;
	int		role_alarm;
	int		role_link_check;
	/* [PATH_CONF] */
	char	log_path[256];
	int		log_ago_day;
	char	pid_file[256];
	/* [DATABASE_INFO] */
	DATABASEINFO	db_lic;			/* ipcs_log  : NASA_License (라이선스+알람 테이블) */
	DATABASEINFO	db_atalk;		/* onpia_check : 알림톡 큐 (PostgreSQL 외부) */
	DATABASEINFO	db_sms;			/* onpia_sms : SDK_SMS_SEND (차후 SMS Agent) */
	int		statement_timeout;
	char	client_encoding[31];
	/* [LINK_CHECK] */
	int		link_check_period_sec;	/* 5 */
	int		link_error_after_min;	/* 21 */
	int		new_site_skip_hour;		/* 3 */
	int		nas_fail_notify_max;	/* 10 */
	int		nas_fail_count_max;		/* 999 */
	/* [ATALK] */
	int		atalk_use;
	char	atalk_sender_key[101];
	char	atalk_callback[21];
	char	atalk_subject[51];
	int		atalk_tran_type;
	/* [SMS] */
	int		sms_use;
	char	sms_user_id[21];
	char	sms_callback[21];
	/* [HTTP] */
	char	webpush_url[512];
	char	mail_url[512];
	int		http_timeout_sec;
	/* [PROTOCOL] */
	int		recv_chunk;				/* 500 */
	int		residual_max;			/* 1000 */
	int		alarm_msg_max;			/* 80 */
	int		lic_idle_timeout_sec;	/* 0 = Windows 동작(세션 유지) */
} ONLIC_CONF;

/* ---------------- 소켓 세션 (onmw NCMW_SOCKET 축약) ---------------- */
typedef struct {
	struct sockaddr_in	local_sa;
	socklen_t			salen;
	char				local_ip[51];
	UINT				local_port;
	struct sockaddr_in	remote_sa;
	char				remote_ip[51];
	UINT				remote_port;
	int					handle;
	int					type;			/* TCP_SOCKET / UDP_SOCKET */
	int					port_type;		/* PORT_TYPE_LIC / ALARM / UDP */
	int					fd;
	int					sock_seq;
	int64_t				sock_time;		/* 접속 시각 (ms) */
	int64_t				last_recv_time;	/* 마지막 수신 (ms) - 유휴 타임아웃용 */
	/* Windows CClientSocket::m_Rdata[MAXLENGTH*2+1] : 세션별 잔여 버퍼 (onmw 의 공유 버퍼 결함을 피함) */
	char				rdata[RESIDUAL_MAX+1];
	int					rlen;
} ONLIC_SOCKET;

/* ---------------- 워커 큐 항목 (onmw SOCK_RECV_WAIT) ---------------- */
typedef struct {
	ONLIC_SOCKET*		sock;			/* TCP 세션 (UDP 는 NULL). 워커에서는 역참조하지 말 것 (소켓 스레드가 해제할 수 있음) */
	int					handle;			/* 응답 송신용 fd 사본 */
	int					port_type;
	struct sockaddr_in	remote_sa;		/* UDP echo ACK 용 */
	char				remote_ip[51];
	UINT				remote_port;
	UINT				local_port;
	int					evt_type;		/* RECV_TYPE_DATA / RECV_TYPE_ACT / RECV_TYPE_EXIT */
	int					cmd;			/* STX 뒤 2글자 atoi (원본 ret) */
	int					len;
	char				buff[MAX_RECV_BUFF+1];
} SOCK_RECV_WAIT;

#define RECV_TYPE_DATA			1
#define RECV_TYPE_ACT			2
#define RECV_TYPE_EXIT			9

/* ---------------- 외부 HTTP 알림 큐 항목 (AlramWeb.h ALRAM_OBJ 축약) ---------------- */
typedef struct {
	int		ao_type;				/* AR_ALERT(웹푸시) / AR_EMAIL */
	char	ao_msg[1024];
	char	szEMail_to[501];
	char	szEMail_from[201];
	char	szEMail_title[201];
	char	szEMail_Msg[2048];
	char	szEMail_Url[2049];
} ALARM_OBJ;

#endif /* struct_def.h */
