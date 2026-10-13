#ifndef ___LOG_OBJECT_H___
#define ___LOG_OBJECT_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

#include <shareheader.h>
#include <ListObject/ListObject.h>
#include <ShareLib/ShareLib.h>
#include <ShareLib/wrappthread.h>

#define MAX_LOG_CHAR	4200
#define MAX_STR			512

#define SIP_ERROR_NOERROR1					100
#define SIP_ERROR_NOERROR2					180
#define SIP_ERROR_NOERROR3					181
#define SIP_ERROR_NOERROR4					182
#define SIP_ERROR_NOERROR5					183
#define SIP_ERROR_SUCCESS					200
#define SIP_ERROR_SUCCESS2					202
#define SIP_ERROR_BADREQUEST				400
#define SIP_ERROR_UNAUTHORIZED				401
#define SIP_ERROR_PAYMENT					402
#define SIP_ERROR_FORBIDDON					403
#define SIP_ERROR_NOT_FOUND					404
#define SIP_ERROR_METHOD_NOT_ALLOWED		405
#define SIP_ERROR_NOT_ACCEPTABLE			406
#define SIP_ERROR_PROXY_AUTHREQUIRE			407
#define SIP_ERROR_REQUEST_TIMEOUT			408
#define SIP_ERROR_GONE						410
#define SIP_ERROR_REQ_ENTITY_LARGE			413
#define SIP_ERROR_REQ_LONG					414
#define SIP_ERROR_UNSUPPORT_MEDIA			415
#define SIP_ERROR_UNSUPPORT_SCHEME			416
#define SIP_ERROR_BAD_EXTENSION				420
#define SIP_ERROR_EXT_REQUIRED				421
#define SIP_ERROR_INTERVAL_BRIEF			423
#define SIP_ERROR_TEMPO_UNAVAILABLE			480
#define SIP_ERROR_CALL_TRANSATION			481
#define SIP_ERROR_LOOP_DETECT				482
#define SIP_ERROR_MANY_HOOP					483
#define SIP_ERROR_ADD_INCOMPLETE			484
#define SIP_ERROR_AMBIGOUS					485
#define SIP_ERROR_BUSY_HERE					486
#define SIP_ERROR_REQ_TERMINATE				487
#define SIP_ERROR_NOT_ACCEPTABLE_HERE		488
#define SIP_ERROR_REQ_PENDING				491
#define SIP_ERROR_MIME_ERR					493
#define SIP_ERROR_SERVER_INTERNAL			500
#define SIP_ERROR_NOT_IMPLEMENTED			501
#define SIP_ERROR_BAD_GATEWAY				502
#define SIP_ERROR_SERVICE_UNAVAILABLE		503
#define SIP_ERROR_SERVER_TIMEOUT			504
#define SIP_ERROR_NOT_SUPPORT				505
#define SIP_ERROR_MSG_TOO_LARGE				513
#define SIP_ERROR_BUSY_EVERY_WHERE			600
#define SIP_ERROR_DECLINE					603
#define SIP_ERROR_DOES_NOT_EXITS			604
#define SIP_ERROR_SERVER_NOT_ACCEPTABLE		606
#define SIP_ERROR_CANCEL					2001
#define SIP_ERROR_SEND_BUFFER				2002
#define SIP_ERROR_RECORD_STATE				2003

static enum
{
	LOG_OBJ_NONE,
	LOG_OBJ_RUN,
	LOG_OBJ_SOCK,
	LOG_OBJ_SOCK_ERR,
	LOG_OBJ_SQL,
	LOG_OBJ_SQL_ERR,
	LOG_OBJ_AGT,
	LOG_OBJ_SSW,
	LOG_OBJ_IVR,
	LOG_OBJ_REC,
	LOG_OBJ_WEB,
	LOG_OBJ_EXC,
	LOG_OBJ_RSV,		//전화예약 알림 서비스 관련로그 
	LOG_OBJ_LINK,
	LOG_OBJ_PDS,
	LOG_OBJ_PDS_ERR,
	LOG_OBJ_ETC
} obj_log_type;

static enum
{
	LOG_YN_NONE,
	LOG_YN_FILE,
	LOG_YN_PRINT,
	LOG_YN_BOTH,
	LOG_YN_EXIT
} obj_log_yn;

typedef struct{
	char	szLog[MAX_LOG_CHAR+1];
	int		log_type;
	int		log_yn;
}LOG_STRUCT;

typedef std::vector<LOG_STRUCT*> LOG_VECTOR;
typedef std::vector<LOG_STRUCT*>::iterator LOG_ITER;

class CLogObject
{
// attr
public:
	CLogObject();
	virtual ~CLogObject();

// Operations
public:

	void LogObjectStart(const char *szName, unsigned short nBufCnt, const char *szLogPath);
	void LogSetAgoday(unsigned int ago_day);
	void ObjectLogEvent();
	void LogObjectPush(char *szLog, int nLogType, int log_yn);
	void LogObjectSIP(char *sipData, int nSend, int nLogType, char *szLocalIP, int nLocalPort, char *szRemoteIP, int nRemotePort);
	void LogObjectGetError(int nErrorValue, char *szObjectError);
	void LogObjectExit();

	inline char * ObjectLogGetName() { return &m_log_name[0]; }
	inline unsigned short ObjectLogGetStart() { return m_start; }
	inline pthread_t ObjectLogGetPID() { return m_pid; }

protected:
	LOG_STRUCT * LogWaitStruct(DWORD nWaitTime);
	LOG_STRUCT * LogWaitGetMsg(int nIndex);
	void ObjectLogTerminate();

	void ObjectLogInit(const char *szName, unsigned short nBufCnt, const char *szLogPath);
	void ObjectLogInitDirectory();
	int ObjectLogCreateFile(int nType);
	int ObjectLogGetFileFD(int nType);
	void ObjectLogGetFilename(char *szFullFilename, int nType);
	void ObjectLogFileDelete(char *filename);
	void dir_delete(char *rm_path);
	void LogRemoveDayProc(char *szLocalDay);
	void LogLocalDayProc(char *szLocalDay);
	void LogLocalTimeProc(char *szLocalTime);

	int ObjectLogOpen(char *szFileName);
	void ObjectLogCreateSubDirectory();
	void ObjectLogDeleteFile();
	void ObjectLogClose();

	int ObjectLogWrite(LOG_STRUCT *logST);

	unsigned short	m_init;
	unsigned short	m_start;
	unsigned short	m_index;
	pthread_t		m_pid;

	LOG_VECTOR		*m_log_event;
	size_t			__ago_day;
	char			m_szCurPath[MAX_STR];
	char			m_log_name[51];

	pthread_cond_t	cond;
	pthread_mutex_t sig_mutex;
	pthread_condattr_t	*cond_attr;
	pthread_mutexattr_t *sig_mutex_attr;
	int					sig_action;
	int					sig_count;
	
	char		m_log_path[100];
	LOG_STRUCT	*m_log_buff;
	int			m_log_pos;
	int			m_buf_cnt;

	int			m_fd_run;	
	int			m_fd_sock;
	int			m_fd_sockerr;	
	int			m_fd_sql;
	int			m_fd_sqlerr;
	int			m_fd_agt;
	int			m_fd_ssw;
	int			m_fd_ivr;
	int			m_fd_rec;
	int			m_fd_web;
	int			m_fd_exc;
	int			m_fd_rsv;
	int			m_fd_link;
	int			m_fd_pds;
	int			m_fd_pds_err;
	int			m_fd_etc;

	char		m_fd_date[11];
};


#endif		//ObjectLog.h


