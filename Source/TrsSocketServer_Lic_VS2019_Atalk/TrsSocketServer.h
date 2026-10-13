// TrsSocketServer.h : main header file for the TRSSOCKETSERVER application
//

#if !defined(AFX_TRSSOCKETSERVER_H__AEF95B4A_B088_4257_8736_796702AFDC84__INCLUDED_)
#define AFX_TRSSOCKETSERVER_H__AEF95B4A_B088_4257_8736_796702AFDC84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "ListenSocket.h"		// main symbols
#include "ClientSocket.h"		// main symbols
#include "DirectODBC.h"
#include "UdpSocket.h"
#include "MyDefine.h"

#include "AlramWeb.h"


/////////////////////////////////////////////////////////////////////////////
// CTrsSocketServerApp:
// See TrsSocketServer.cpp for the implementation of this class
//

class CTrsSocketServerApp : public CWinApp
{
public:
	CTrsSocketServerApp();

	BOOL	CreateSocketProc();
	BOOL	SmsExcludeLoadProc();
	BOOL	IsSmsExclude(int nType, char* szMsg);
	void	SmsExcludeLogging(char *format);

	void	ProcessPendingAccept(CListenSocket * m_pAcceptSocket);
	void	CloseSocket(CClientSocket* pSocket);

	void	ReadMsgProc(CClientSocket* pSocket);
	void	ReadUdpMsgProc(CUdpSocket* pSocket);
	void	ReadMsgTestProc(char szRdata[]);
	void	SearchSocketSTX_ETX(char data[], int &startPos, int &endPos);
	int		CutSocketSTX_ETX(CClientSocket* pSocket, char data[], int startPos, int endPos);
	void	ServerProc(CClientSocket* pSocket, int ret, char Rdata[]);
	void	ServerUdpProc(CUdpSocket* pSocket, char Rdata[], CString rSocketAddress1, UINT rSocketPort1);
	void	GetUserString(char *szUsers, char *szIP, char *szRealIP, int nUserCnt, int nUseType = USE_NORAML);
	BOOL	GetUserInfoIdx(int usrIdx, char* szName, char* szNumber, int nUserCnt, int nUseType = USE_NORAML);
	void	OnCheckLinkData();
	void	GetAgoTimeProc(int nAgoMinute, char *szLocalDay);
	void	TcpLogging(char *format);
	void	UdpLogging(char *format);
	void	SqlErrLogging(char *format);
	void	SoftLogging(char *format);
	void	AlarmLogging(char *format);
	void	StrSplit(char *src, char dsttmp[50][1024], char cmp);
	int		IVRSMSSendProc(char szResult[50][1024], char* szLogMsg);

	void	Hangul_StrCut(char* szSrc, char* szDst, int nMaxByteCount);

	int		PauseCheckProc(char* szIP, char* szRealIP);
	int		AlarmUser_Send(char* szMessage, int nCmd = 1);



	CAlramWeb*	m_pAlramWeb;
	int		AlarmWebTh_Create();
	int		AlarmWebTh_Delete();
	int		AlarmWebTh_Push(char* szMsg, int nType = 1);	
	int     AlarmWebTh_EmailPush(CDirectODBC* pDB, int nEventType, char* szIP, char* szRealIP, const char* szSiteName = "", const char* szProcName="",  const char* szContents = "");

	int		AlarmWebTh_EmailTest();

  
  

	SMS_EXCLUDE*		m_pSmsExclude;



	CDirectODBC			*m_dlgSql;

#ifdef _ATALK
	CDirectODBC			*m_pAtalkSql;
#endif
	CDirectODBC			*m_smsSql;

	CListenSocket		*m_pServerSocket;
	CClientSocket		*m_pLinkSocket;
	CPtrList			m_connectionList;

	CListenSocket		*m_pAlarmSocket;
	CPtrList			m_connectionAlarmList;

	char				m_Rdata[MAXLENGTH+1];
	CUdpSocket			*m_pUdp;
	SMS_SITE_USERS		*m_pSmsSiteUsers;
	BOOL				m_bInitLinkTimer;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrsSocketServerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTrsSocketServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRSSOCKETSERVER_H__AEF95B4A_B088_4257_8736_796702AFDC84__INCLUDED_)
