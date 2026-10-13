/*
**	HttpWaitStack.h : 웹푸시·메일 HTTP POST 워커 (Windows AlramWeb.cpp 이식, WinInet → libcurl)
*/
#ifndef ___ONLIC_HTTP_WAIT_STACK_H___
#define ___ONLIC_HTTP_WAIT_STACK_H___

class CHttpWait
{
public:
	CHttpWait();
	virtual ~CHttpWait();

	int		Start();
	void	Kill();
	int		AlertList_Push(const char *szMsg, int nAlertType = 1);		/* 웹푸시 (AlramWeb.cpp:105) */
	int		AlertList_EmailPush(const EMAIL_SITE_USERS &emailObj);		/* 메일 (AlramWeb.cpp:130) */
	int		AlarmList_Get(ALARM_OBJ &obj);								/* 워커 내부 (AlramWeb.cpp:156) */
	void	AlarmList_Show(ALARM_OBJ &obj);								/* 웹푸시 POST (AlramWeb.cpp:208) */
	void	AlarmList_SendEmail(ALARM_OBJ &obj);						/* 메일 POST (AlramWeb.cpp:289) */
	int		RequestPost(const char *szUrl, const char *szBody, char *szOut, int nOutSize);	/* AlramWeb.cpp:323 */

	pthread_t		m_pid;
	int				m_bStop;
	int				m_run;
	CListObject		m_list;
	pthread_mutex_t	m_mutex;
};

/* CP949 → UTF-8 → %XX (Windows CURLEncode::Utf8_UrlEncode). 반환 길이 */
int		Http_Utf8UrlEncode(const char *szSrc, char *szDst, int nDstSize);

void*	RunHttpWaitThread(void *arg);
extern CHttpWait	*__http_wait;

/* Windows CTrsSocketServerApp::AlarmWebTh_Push / AlarmWebTh_EmailPush 대응 */
int		AlarmWebTh_Push(const char *szMsg, int nType = 1);
int		AlarmWebTh_EmailPush(CDirectODBC *pDB, int nEventType, const char *szIP, const char *szRealIP, const char *szSiteName, const char *szProcName, const char *szContents);

#endif /* HttpWaitStack.h */
