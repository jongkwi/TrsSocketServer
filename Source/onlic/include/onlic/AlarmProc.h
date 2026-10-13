/*
**	AlarmProc.h : UDP 5363 이벤트 처리 + 링크 감시 (Windows ServerUdpProc:783-1295, OnCheckLinkData:1425-1535 외)
*/
#ifndef ___ONLIC_ALARMPROC_H___
#define ___ONLIC_ALARMPROC_H___

class CRecvWait;

/* 알람 문구 인덱스. 기본값은 Windows 소스 리터럴, ini [ALARM_MSG] 로 교체 가능 (결정 B11). %s 순서는 바꾸면 안 됨 */
enum ALARM_TXT
{
	TXT_TABLE_CHECK = 0,	/* (시스템IP, 공인IP) */
	TXT_PROC_ERROR,			/* (사이트, IP, 프로세스) */
	TXT_PROC_SUCC,
	TXT_CPU_ERROR,			/* (사이트, IP) */
	TXT_CPU_SUCC,
	TXT_DISK_ERROR,			/* (사이트, IP, 메시지) */
	TXT_DISK_SUCC,
	TXT_MEM_ERROR,			/* (사이트, IP) */
	TXT_MEM_SUCC,
	TXT_SOCK_CON_ERROR,		/* (사이트, IP, 메시지) */
	TXT_SOCK_CON_SUCC,
	TXT_SOCK_CHECK_ERROR,
	TXT_SOCK_CLOSE_ERROR,
	TXT_ICMP_ERROR,			/* (사이트, IP) */
	TXT_ICMP_SUCC,
	TXT_DATA_ERROR,			/* (사이트, IP, 메시지) */
	TXT_DATA_ERROR_PAUSE,	/* 로그용 (사이트, IP, 메시지) */
	TXT_LINK_RETURN,		/* (사이트, IP) */
	TXT_NAS_CON_FAIL_MSG,	/* (사이트, IP, 메시지) */
	TXT_NAS_CON_FAIL_CNT,	/* (사이트, IP, %d 회) */
	TXT_NAS_CON_SUCC,		/* (사이트, IP) */
	TXT_NAS_UPLOAD_FAIL,
	TXT_NAS_UPLOAD_SUCC,
	TXT_BACKUP_MESSAGE,		/* (사이트, IP, 메시지) */
	TXT_BACKUP_DISKFULL,	/* (사이트, IP) */
	TXT_FILE_REMOVE,		/* (사이트, IP, 퍼센트) */
	TXT_DISK_FULL,			/* (사이트, IP, 퍼센트) */
	TXT_NEW_SITE,			/* (시스템IP, 공인IP) */
	TXT_LINK_ERROR,			/* (사이트, IP) */
	TXT_MAX
};

int		AlarmProc_Init();									/* [ALARM_MSG] 로드 (SystemLoad.StartSystem) */
const char*	AlarmText(int nIdx);

void	EventUdpProc(CRecvWait *pWait, SOCK_RECV_WAIT *pItem);	/* ServerUdpProc */
void	LinkCheck_OnTimer();									/* OnCheckLinkData (타이머 스레드) */

/* 보조 (Windows 동명 함수) */
void	GetUserString(char *szUsers, int nUsersSize, const SMS_SITE_USERS *pUsers, int nUserCnt, int nUseType);
int		GetUserInfoIdx(const SMS_SITE_USERS *pUsers, int usrIdx, char *szName, char *szNumber, int nUserCnt, int nUseType);
int		IsSmsExclude(int nType, const char *szMsg);
int		PauseCheckProc(CDirectODBC *pDB, const char *szIP, const char *szRealIP);
int		IVRSMSSendProc(CDirectODBC *pSmsDB, char szResult[STR_SPLIT_MAX][STR_SPLIT_LEN], const char *szLogMsg);
int		AtalkSendUsers(CDirectODBC *pAtalkDB, const SMS_SITE_USERS *pUsers, int nUserCnt, int nUseType, const char *szTmplCode, const char *szSendData);
int		SmsSendUsers(CDirectODBC *pSmsDB, const char *szSubject, const char *szMsg, int nUserCnt, const char *szUsers);

#endif /* AlarmProc.h */
