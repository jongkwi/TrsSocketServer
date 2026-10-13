#if !defined(AFX_DIRECTODBC_H__E3B27101_6E51_11D4_AC14_0000B4A29ADE__INCLUDED_)
#define AFX_DIRECTODBC_H__E3B27101_6E51_11D4_AC14_0000B4A29ADE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirectODBC.h : header file
// 

#include "MyDefine.h"

#define MAX_DB			10
#define DB_MDB			1 
#define	DB_SQL			2
#define DB_ORC			3
#define DB_SYS			4
#define DB_DB2			5



/////////////////////////////////////////////////////////////////////////////
// CDirectODBC command target

class CDirectODBC : public CCmdTarget
{
	DECLARE_DYNCREATE(CDirectODBC)

	CDirectODBC();           // protected constructor used by dynamic creation
	CDirectODBC(UCHAR FAR * szDSN, UCHAR FAR * szUID,UCHAR FAR * szPWD);    
	virtual ~CDirectODBC();

// Attributes
public:
 
 
// Operations
public:
	BOOL	ConnectODBC(UCHAR FAR * szDSN, UCHAR FAR * szUID,UCHAR FAR * szPWD);
	int		ConnectODBC();
	void	DisConnectODBC();
	int		OpenTable(CString sqlCmd);
	int		OpenTable(CString SqlCmd, HENV &m_localEnv, HDBC &m_localDbc, 	HSTMT &m_localHtsmt);
	int		OpenLastTable(CString SqlCmd);
	void	CloseTable();
	void	CloseTable(HSTMT &m_localHtsmt);
	int		ExecSql(CString SqlCmd);
	int		ExecLastSql(CString SqlCmd);
	int		ExecProcedureSql(CString SqlCmd);
	long	RowCount(CString sqlCmd);
	int		GetValue(CString sqlCmd, char* szValue);
	int		GetUnsignedIntValue(CString sqlCmd);

	int		GetSmsUsersProc(CString sqlCmd);
	int		GetSmsUsersData(int nIndex);
	int		GetSmsSiteUsersProc(CString sqlCmd, int nUserCnt);
	int		GetSmsSiteUsersData(int nIndex);
	int		GetSmsBasicUsersProc(CString sqlCmd, SMS_SITE_BASIC_USER *pBasicUser);
	int		GetSmsBasicUsersData(SMS_SITE_BASIC_USER *pBasicUser);
	int		GetCCLinkInfoProc(CString sqlCmd, CC_LINKINFO *pCCLink);
	int		GetCCLinkInfoData(int nIndex, CC_LINKINFO *pCCLink);


	EMAIL_SITE_USERS*	GetEMailUserProc(CString strWhereQuery);
	int					GetEMailUserData(EMAIL_SITE_USERS &obj);



	SMS_EXCLUDE*	GetSmsExcludeProc();
	int				GetSmsExcludeData(SMS_EXCLUDE &obj);



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirectODBC)
	//}}AFX_VIRTUAL

// Implementation
protected:
	HENV    m_hEnv;
	HDBC    m_hDbc;
	HSTMT   m_hStmt;
	RETCODE m_nRetcode;
	CString	m_strDSN, m_strUID, m_strPWD;
	
	// Generated message map functions
	//{{AFX_MSG(CDirectODBC)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTODBC_H__E3B27101_6E51_11D4_AC14_0000B4A29ADE__INCLUDED_)
