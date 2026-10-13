/*
**	onlicDefine.h : onlic (TrsSocketServer Linux 포팅) 공통 정의
**	- onmw ncmwDefine.h 구조를 따름. onmw struct_def.h 는 포함하지 않음 (값 충돌)
**	- 포트/프레임/nType 값은 클라이언트 호환 계약 (docs/프로토콜명세.md) 이므로 변경 금지
*/
#ifndef ___ONLIC_DEFINE_H___
#define ___ONLIC_DEFINE_H___

#ifndef FD_SETSIZE
#define FD_SETSIZE 2048
#endif

#include <ListObject/ListObject.h>
#include <LogObject/LogObject.h>
#include <ShareLib/copyfile.h>
#include <ShareLib/Error.h>
#include <ShareLib/GetPrivateProfile.h>
#include <ShareLib/strfnc_st.h>
#include <ShareLib/strcasecmp.h>
#include <ShareLib/nprintf.h>
#include <ShareLib/UserTime.h>
#include <ShareLib/UtfIConv.h>
#include <ShareLib/md5.h>
#include <ShareLib/WinMalloc.h>
#include <ShareLib/wrappthread.h>
#include <ShareLib/wrapsock.h>
#include <ShareLib/WriteN.h>
#include <UrlEnDe/url_en_de.h>
#ifndef _WIN32
#include <ShareLib/daemon_init.h>
#endif

#ifndef true
#define	true					1
#endif
#ifndef false
#define	false					0
#endif
#ifndef TRUE
#define	TRUE					true
#endif
#ifndef FALSE
#define	FALSE					false
#endif
#ifndef DWORD
#define	DWORD					unsigned long
#endif
#ifndef UINT
#define	UINT					unsigned int
#endif
#ifndef UCHAR
#define	UCHAR					unsigned char
#endif
#ifndef USHORT
#define	USHORT					unsigned short
#endif
#ifndef BOOL
#define	BOOL					int
#endif

/* 버전 */
#define ONLIC_VERSION			"1.0.0.1"
#define ONLIC_LAST_UPDATE		"2026-10-13"
#define ONLIC_WIN_BASE_VERSION	"TrsSocketServer 1.0.0.7"	/* 포팅 원본 */

#ifndef PID_FILE
#define	PID_FILE				"/tmp/onlic.pid"
#endif
#define DEFAULT_CONFIG_FILE		"../cfg/onlic.ini"

#define	LISTENQ					1024	/* listen() backlog */
#define	MAX_PRINT				500
#define MAX_RECV_BUFF			2048	/* 워커 큐 항목 버퍼 */

/* ---------------- 클라이언트 호환 상수 (StdAfx.h / 프로토콜명세 §1~§2) ---------------- */
#define IPCS_PORT				6321	/* TCP 라이선스 체크 */
#define ALARM_PORT				6770	/* TCP 알람 뷰어 */
#define UDP_PORT				5363	/* UDP 장애 이벤트 */

#define PKT_STX					0x02
#define PKT_ETX					0x03
#define PKT_SEP					0xff	/* 필드 구분자 */

#define	MAXLENGTH				500		/* Receive(500) 단위 */
#define RESIDUAL_MAX			(MAXLENGTH*2)	/* 잔여 버퍼 1000, m_Rdata[1001] */
#define ALARM_MSG_MAX			80		/* Hangul_StrCut 80 */
#define ALARM_PUSH_MAX			1000	/* 6770 푸시 메시지 최대 */
#define STR_SPLIT_MAX			50		/* StrSplit 최대 필드 수 */
#define STR_SPLIT_LEN			1024

#define LIC_RESP_RUN			"Run"	/* raw 3바이트, 프레이밍 없음 */
#define LIC_RESP_CLOSE			"Close"	/* raw 5바이트 */

/* 소켓 종류 */
#define	TCP_SOCKET				1
#define	UDP_SOCKET				2
/* 포트 역할 */
#define PORT_TYPE_LIC			1
#define PORT_TYPE_ALARM			2
#define PORT_TYPE_UDP			3

/* 타이머 ID (StdAfx.h 값 유지) */
#define START_TIMER				100
#define LINK_TIMER				101

/* 수신자 선택 (MyDefine.h:482-483) */
#define USE_NORAML				0		/* sms_use == 'Y' 만 */
#define USE_ALL					1		/* sms_use == 'Y' 또는 'T' (긴급) */

/* 알람 푸시 cmd (6770) */
#define ALARM_CMD_NORMAL		'1'
#define ALARM_CMD_PAUSE			'2'
#define ALARM_CMD_HEARTBEAT		'3'		/* 뷰어 → 서버 */
#define ALARM_CMD_HEARTBEAT_ACK	'4'		/* 서버 → 뷰어 */

/* UDP nType (UdpSocket.h:12-44 값 그대로) */
enum WARNING_ERR_TYPE
{
	SEND_TYPE_PROC_ERROR = 0,
	SEND_TYPE_PROC_SUCC,
	SEND_TYPE_CPU_ERROR,
	SEND_TYPE_CPU_SUCC,
	SEND_TYPE_DISK_ERROR,
	SEND_TYPE_DISK_SUCC,
	SEND_TYPE_MEM_ERROR,
	SEND_TYPE_MEM_SUCC,
	SEND_TYPE_SOCK_CON_ERROR,
	SEND_TYPE_SOCK_CON_SUCC,
	SEND_TYPE_SOCK_CHECK_ERROR,
	SEND_TYPE_SOCK_CLOSE_ERROR,
	SEND_TYPE_ICMP_ERROR,
	SEND_TYPE_ICMP_SUCC,
	SEND_TYPE_USER_MSG,
	SEND_TYPE_DATA_ERROR,
	SEND_TYPE_LINK_CHECK = 100,
	SEND_TYPE_IVR_GPS = 200,
	SEND_TYPE_IVR_NORMAL,
	SEND_TYPE_NAS_CON_FAIL = 301,	/* 녹취 FTP 백업 연결 실패 */
	SEND_TYPE_NAS_CON_SUCC,			/* 녹취 FTP 백업 연결 성공 */
	SEND_TYPE_NAS_UPLOAD_FAIL,		/* 녹취 FTP 백업 업로드 실패 */
	SEND_TYPE_NAS_UPLOAD_SUCC,		/* 녹취 FTP 백업 업로드 성공 */
	SEND_TYPE_BACKUP_MESSAGE,		/* 백업 기타 메세지 */
	SEND_TYPE_BACKUP_DISKFULL,		/* 백업 장치 디스크 용량 가득 */
	SEND_TYPE_FILE_REMOVE,			/* 녹취 사용량 초과 삭제 */
	SEND_TYPE_DISK_FULL,			/* 서버 디스크 용량 가득 */
	SEND_TYPE_NET_ERROR = 401,		/* 네트워크 오류 (이메일만) */
	SEND_TYPE_NET_SUCC = 402,		/* 네트워크 복구 (이메일만) */
	SEND_TYPE_ETC
};

/* 외부 HTTP 알림 종류 (AlramWeb.h) */
#define AR_POPUP				1
#define AR_ALERT				2
#define AR_NEWIE				3
#define AR_EMAIL				9

/* CSystemLoad 오류 코드 (onmw 동일 체계) */
#define	SYSTEMLOAD_SUCCESS					1
#define	SYSTEMLOAD_SYSTEM_INFO_ERROR		51
#define	SYSTEMLOAD_SYSDB_CONNECT_ERROR		53
#define	SYSTEMLOAD_SOCK_ERROR				54
#define SYSTEMLOAD_CONFIG_FILE_ERROR		55
#define SYSTEMLOAD_ROLE_ERROR				58
#define	SYSTEMLOAD_UNKNOWN					99

/*
**	로그 채널 매핑 (Windows 로그 접두어 → CLogObject 타입). 파일명 <type>_<YYYY-MM-DD>_<LIC>.log
**	  SOFT-       → LOG_OBJ_RUN      run_
**	  TcpLogging  → LOG_OBJ_SOCK     sock_
**	  UDP-        → LOG_OBJ_ETC      etc_
**	  Alarm-      → LOG_OBJ_LINK     link_
**	  SqlErr-     → LOG_OBJ_SQL_ERR  sqlerr_
**	  SmsExclude- → LOG_OBJ_EXC      exc_
*/
#define LOG_SOFT				LOG_OBJ_RUN
#define LOG_TCP					LOG_OBJ_SOCK
#define LOG_UDP					LOG_OBJ_ETC
#define LOG_ALARM				LOG_OBJ_LINK
#define LOG_SQLERR				LOG_OBJ_SQL_ERR
#define LOG_SMSEX				LOG_OBJ_EXC

#include "struct_def.h"
#include "ConfigLoad.h"
#include "MwTimer.h"
#include "DirectODBC.h"
#include "DbLic.h"
#include "Framing.h"
#include "SystemLoad.h"
#include "SocketThread.h"
#include "RecvWaitStack.h"
#include "HttpWaitStack.h"
#include "SockProcLic.h"
#include "AlarmProc.h"

/* ---------------- 전역 (정의: ConfigLoad.cpp) ---------------- */
extern ONLIC_CONF	sys_conf;
extern CSystemLoad	sys_load;
extern CLogObject	*lic_log_object;
extern CMwTimer		*licTm;
extern int			onlic_start;
extern long			onlic_pid;
extern int			daemon_start;
extern int			daemon_proc;
extern char			m_gPidFile[1024];
extern ONLIC_SOCKET	m_SockLic, m_SockAlarm, m_SockUdp;
extern pthread_t	lic_pid, alarm_pid, udp_pid;
extern CListObject	list_lic_socket, list_alarm_socket;

/* 로그 헬퍼 (SystemLoad.cpp) */
void OnlicLog(int nLogType, const char* szLog);
void OnlicLogf(int nLogType, const char* fmt, ...);

/* ---------------- 워커/소켓 초기화 (RecvWaitStack.cpp, HttpWaitStack.cpp, SystemLoad.cpp) ---------------- */
int		LicRecvWaitStackInit();		/* 라이선스 워커 */
void	LicRecvWaitStackKill();
int		EventRecvWaitStackInit();	/* UDP 이벤트 워커 */
void	EventRecvWaitStackKill();
int		HttpWaitStackInit();		/* 웹푸시/메일 워커 */
void	HttpWaitStackKill();
int		SmsExcludeLoadProc();		/* SITE_SmsExcludeTime 로드 (SystemLoad.cpp) */

#endif /* onlicDefine.h */
