#if !defined(ALARM_WEB_H___INCLUDED_)
#define ALARM_WEB_H___INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CAlramWeb.h : header file
//




#include "URLEncode.h"



/////////////////////////////////////////////////////////////////////////////
// CAlramWeb command target

UINT AlarmWorkerThread(LPVOID lpvThreadParm);

#define AR_POPUP				1		// 상담팝업 
#define AR_ALERT				2		// 시스템알림
#define AR_NEWIE				3		// 웹브라우저 새창 팝업
#define AR_EMAIL				9		// 이메일 전송


typedef struct{
	int ao_type;			
	int	 alert_type;		// 알림타입
	char ao_msg[1024];
	char np_tel[101];
	char np_trunk[101];
	char np_ivrmenu[512];
	char np_intime[51];
	char np_url[2048];

	char szInfo1[101];
	char szInfo2[101];
	char szInfo3[101];
	char szInfo4[101];
	char szInfo5[101];

	int bp_attr1;
	int show_time;

	
	char szEMail_to[501];
	char szEMail_from[201];
	char szEMail_title[201];
	char szEMail_Msg[2048];
	char szEMail_Url[2048];


} ALRAM_OBJ;



class CAlramWeb
{
// Attributes
public:

// Operations
public:
	CAlramWeb();
	virtual ~CAlramWeb();


	void	AlarmTh_Start();
	void	AlarmTh_Terminate();
	int		AlertList_Push(char* szMsg, int nAlertType=1);
	int		AlertList_EmailPush(EMAIL_SITE_USERS &emailObj);
	
	int		AlarmList_Get(ALRAM_OBJ &alarmData);
	void	AlarmList_RemoveAll();
	void	AlarmList_Show(ALRAM_OBJ &alram_Obj);
	void	AlarmList_SendEmail(ALRAM_OBJ &alram_Obj);


	BOOL	RequestPost(LPCTSTR lpUrl, CString strSendMsg, CString &strOutMsg);

	CRITICAL_SECTION	m_csEventIndex;
	int					m_nAlarmTh_Stop;
	CWinThread*			m_pAlarmThread;
	CPtrList	m_AlarmList;

	CURLEncode		m_urlEncode;
// Overrides
public:
	
protected:


};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(ALARM_WEB_H___INCLUDED_)