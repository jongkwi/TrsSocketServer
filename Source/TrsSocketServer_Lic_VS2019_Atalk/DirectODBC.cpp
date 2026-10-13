// DirectODBC.cpp : implementation file
//

#include "stdafx.h"
#include "TrsSocketServer.h"
#include "DirectODBC.h"
 
#include "MyDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

void trim(char *str)
{
    char *sp, *head, *tail;
    if(str == NULL) return;

    sp = str;
    head = tail = NULL;

    while(*sp){
        if(*sp != ' '){
            tail = sp;
            if(head == NULL)
                head = sp;
		}
        ++sp;
	}
    if(tail){
        *(tail + 1) = '\0';
        while(*head) *str++ = *head++;
	}
    *str = '\0';
}


/////////////////////////////////////////////////////////////////////////////
// CDirectODBC

IMPLEMENT_DYNCREATE(CDirectODBC, CCmdTarget)

CDirectODBC::CDirectODBC()
{
	m_hEnv = NULL;
	m_hDbc = NULL;
	m_hStmt = NULL;
	m_nRetcode = 0;
	m_strDSN = "";
	m_strUID = "";
	m_strPWD = "";
}


CDirectODBC::CDirectODBC(UCHAR FAR * szDSN, UCHAR FAR * szUID,UCHAR FAR * szPWD)
{
	m_hEnv = NULL;
	m_hDbc = NULL;
	m_hStmt = NULL;
	m_nRetcode = 0;
	m_strDSN = szDSN;
	m_strUID = szUID;
	m_strPWD = szPWD;
}


CDirectODBC::~CDirectODBC()
{
	DisConnectODBC();
}


BEGIN_MESSAGE_MAP(CDirectODBC, CCmdTarget)
	//{{AFX_MSG_MAP(CDirectODBC)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectODBC message handlers
//서버에 접속
BOOL CDirectODBC::ConnectODBC(UCHAR FAR * szDSN, UCHAR FAR * szUID,UCHAR FAR * szPWD)
{
	BOOL	bError = FALSE;

	DisConnectODBC();

	m_strDSN = szDSN;
	m_strUID = szUID;
	m_strPWD = szPWD;
	
	return ConnectODBC();
}


//서버에 접속
BOOL CDirectODBC::ConnectODBC()
{
	BOOL	bError = FALSE;

	DisConnectODBC();

	m_nRetcode = SQLAllocEnv(&m_hEnv);					/* Environment handle */
	if (m_nRetcode == SQL_SUCCESS) {
		m_nRetcode = SQLAllocConnect(m_hEnv, &m_hDbc);	/* Connection handle */
		if (m_nRetcode == SQL_SUCCESS) {
			/* Set login timeout to 5 seconds. */
			SQLSetConnectOption(m_hDbc, SQL_LOGIN_TIMEOUT, 30);

			/* Connect to data source */
			m_nRetcode = SQLConnect(m_hDbc, (unsigned char*)(LPCSTR)m_strDSN, SQL_NTS, (unsigned char*)(LPCSTR)m_strUID, SQL_NTS, (unsigned char*)(LPCSTR)m_strPWD, SQL_NTS);
			if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO) bError = FALSE;
			else{
				if(m_hDbc != NULL){ 
					SQLFreeConnect(m_hDbc);
					m_hDbc = NULL;
				}
				if(m_hEnv != NULL){
					SQLFreeEnv(m_hEnv);
					m_hEnv = NULL;
				}
				bError = TRUE;
			}
		}
		else{
			if(m_hEnv != NULL){
				SQLFreeEnv(m_hEnv);
				m_hEnv = NULL;
			}
			bError = TRUE;
		}
	}
	else{
		m_hEnv = NULL;
		bError = TRUE;
	}

	return bError;
}


void CDirectODBC::DisConnectODBC()
{
	CloseTable();

	if(m_hDbc != NULL){
		SQLDisconnect(m_hDbc);
		SQLFreeConnect(m_hDbc);
		m_hDbc = NULL;
	}

	if(m_hEnv != NULL){
		SQLFreeEnv(m_hEnv);
		m_hEnv = NULL;
	}

}


int CDirectODBC::OpenTable(CString SqlCmd)
{
	int nReturn = 0;
	if(this == NULL) return -1;
	if(m_hDbc == NULL){
		ConnectODBC();
	}

	CloseTable();

	//process data after successful connection
	m_nRetcode = SQLAllocStmt(m_hDbc, &m_hStmt); /* Statement handle */
	if (m_nRetcode != SQL_SUCCESS) {
		m_hStmt = NULL;
		ConnectODBC();
		nReturn = OpenLastTable(SqlCmd);
		return nReturn;
	}

	m_nRetcode = SQLExecDirect(m_hStmt, (unsigned char *)(LPSTR)(LPCSTR)SqlCmd, SQL_NTS);

	if(m_nRetcode < 0){
		ConnectODBC();
		nReturn = OpenLastTable(SqlCmd);
		return nReturn;
	}
	return( (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO) ? 0 : 1 );
}


int CDirectODBC::OpenLastTable(CString SqlCmd)
{
	int nReturn = 0;
	if(this == NULL) return -1;

	CloseTable();

	//process data after successful connection
	m_nRetcode = SQLAllocStmt(m_hDbc, &m_hStmt); /* Statement handle */
	if (m_nRetcode != SQL_SUCCESS) {
		m_hStmt = NULL;
		return -1;
	}

	m_nRetcode = SQLExecDirect(m_hStmt, (unsigned char *)(LPSTR)(LPCSTR)SqlCmd, SQL_NTS);

	return( (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO) ? 0 : 1 );
}


int CDirectODBC::OpenTable(CString SqlCmd, HENV &m_localEnv, HDBC &m_localDbc, 	HSTMT &m_localHtsmt)
{
	RETCODE m_nRet = 0;

	if(this == NULL) return -1;

	//process data after successful connection
	m_nRet = SQLAllocStmt(m_localDbc, &m_localHtsmt); /* Statement handle */
	if (m_nRet != SQL_SUCCESS) {
		m_localDbc = NULL;
		return -1;
	}

	m_nRet = SQLExecDirect(m_localDbc, (unsigned char *)(LPSTR)(LPCSTR)SqlCmd, SQL_NTS);
	
	return( (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO) ? 0 : 1 );
}


int CDirectODBC::ExecSql(CString SqlCmd)
{
	int nReturn;
	if(this == NULL) return -1;

	CloseTable();

	//process data after successful connection
	m_nRetcode = SQLAllocStmt(m_hDbc, &m_hStmt); /* Statement handle */
	if (m_nRetcode != SQL_SUCCESS){
		m_hStmt = NULL;
		ConnectODBC();
		nReturn = ExecLastSql(SqlCmd);
		return nReturn;
	}

	m_nRetcode = SQLExecDirect(m_hStmt, (unsigned char *)(LPSTR)(LPCSTR)SqlCmd, SQL_NTS);
	if(m_nRetcode != SQL_SUCCESS && m_nRetcode != SQL_SUCCESS_WITH_INFO){
		SQLFreeStmt(m_hStmt, SQL_DROP); 
		m_hStmt = NULL;
		ConnectODBC();
		nReturn = ExecLastSql(SqlCmd);
		return nReturn;
	}

	SQLFreeStmt(m_hStmt, SQL_DROP); 
	m_hStmt = NULL;

	if(strcmp((LPCSTR)SqlCmd, "COMMIT") != FALSE) ExecSql("COMMIT");
	return 0;
}


int CDirectODBC::ExecLastSql(CString SqlCmd)
{
	if(this == NULL) return -1;

	CloseTable();

	//process data after successful connection
	m_nRetcode = SQLAllocStmt(m_hDbc, &m_hStmt); /* Statement handle */
	if (m_nRetcode != SQL_SUCCESS){
		m_hStmt = NULL;
		return -1;
	}

	m_nRetcode = SQLExecDirect(m_hStmt, (unsigned char *)(LPSTR)(LPCSTR)SqlCmd, SQL_NTS);
	if(m_nRetcode != SQL_SUCCESS && m_nRetcode != SQL_SUCCESS_WITH_INFO){
		SQLFreeStmt(m_hStmt, SQL_DROP); 
		m_hStmt = NULL;
		return TRUE;
	}

	SQLFreeStmt(m_hStmt, SQL_DROP); 
	m_hStmt = NULL;

	if(strcmp((LPCSTR)SqlCmd, "COMMIT") != FALSE) ExecSql("COMMIT");
	return 0;
}


int CDirectODBC::ExecProcedureSql(CString SqlCmd)
{
	int   nRows = 0;
	SQLINTEGER cbOutString = SQL_NTS;

	if(this == NULL) return -1;

	CloseTable();

	//process data after successful connection
	m_nRetcode = SQLAllocStmt(m_hDbc, &m_hStmt); /* Statement handle */
	if (m_nRetcode != SQL_SUCCESS){
		m_hStmt = NULL;
		return -1;
	}

	SQLRETURN ret = SQLBindParameter(m_hStmt, 1, SQL_PARAM_OUTPUT, SQL_C_USHORT, SQL_INTEGER, sizeof(nRows), 0, &nRows, sizeof(nRows), &cbOutString);
  //  SetParamType(SQL_PARAM_OUTPUT); //9999999999991
    //SetBindParam(SQL_C_CHAR,    SQL_CHAR,   &szReturn,  10, 10);

	m_nRetcode = SQLExecDirect(m_hStmt, (unsigned char *)(LPSTR)(LPCSTR)SqlCmd, SQL_NTS);
	if(m_nRetcode != SQL_SUCCESS && m_nRetcode != SQL_SUCCESS_WITH_INFO){
		SQLFreeStmt(m_hStmt, SQL_DROP); 
		m_hStmt = NULL;
		return 1;
	}

	SQLFreeStmt(m_hStmt, SQL_DROP); 
	m_hStmt = NULL;

	return TRUE;
}

void CDirectODBC::CloseTable()
{
	if(this == NULL) return;
	if(m_hDbc <= 0) return;
	if(m_hStmt != NULL){
		SQLFreeStmt(m_hStmt, SQL_DROP);  
		m_hStmt = NULL;
	}
}


void CDirectODBC::CloseTable(HSTMT &m_localHtsmt)
{
	if(this == NULL) return;
	if(m_localHtsmt != NULL){
		SQLFreeStmt(m_localHtsmt, SQL_DROP); 
		m_localHtsmt = NULL;
	}
	
}


long CDirectODBC::RowCount(CString sqlCmd)
{
	RETCODE m_nRetcode;
	SDWORD	nRowsCount = 0;
	UCHAR	szTemp[21] = "";
	DWORD		count = 0;
	int	error = FALSE;

	switch(OpenTable(sqlCmd)){
	  case TRUE:
		count = -1;
		CloseTable();
		ConnectODBC();
		break;

	  case FALSE:
		m_nRetcode = SQLFetch(m_hStmt);
		if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO){
			SQLGetData(m_hStmt, 1, SQL_C_CHAR, szTemp,21, &nRowsCount);
			count = (DWORD)atol((const char*)szTemp);
		}
		else if (m_nRetcode == SQL_NO_DATA_FOUND) count = 0;
		else	count =  -1;
		CloseTable();
		break;

	  default:
		count = -1;
	}

	return count;
}



int CDirectODBC::GetValue(CString sqlCmd, char* szValue)
{
	RETCODE m_nRetcode;
	SDWORD	nRowsCount = 0;
	char	szTemp[21] = "";
	
	int	error = TRUE;
	
	switch(OpenTable(sqlCmd)){
	case TRUE:		
		CloseTable();
		ConnectODBC();
		break;
		
	case FALSE:
		m_nRetcode = SQLFetch(m_hStmt);
		if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO){
			SQLGetData(m_hStmt, 1, SQL_C_CHAR, szTemp, 21, &nRowsCount);
			trim(szTemp); 
			strcpy(szValue, szTemp);
			error = FALSE;
		}
		CloseTable();
		break;		
	default:
		break;
	}
	
	return error;
}

int CDirectODBC::GetUnsignedIntValue(CString sqlCmd)
{

	RETCODE m_nRetcode;
	SDWORD	nRowsCount = 0;
	int	szTemp = 0;

	int	error = -1;

	switch (OpenTable(sqlCmd)) {
	case TRUE:
		CloseTable();
		ConnectODBC();
		break;

	case FALSE:
		m_nRetcode = SQLFetch(m_hStmt);
		if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO) {
			SQLGetData(m_hStmt, 1, SQL_C_LONG, &szTemp, 0, &nRowsCount);
			
			error = szTemp;
		}
		CloseTable();
		break;
	default:
		break;
	}

	return error;
}





int	CDirectODBC::GetSmsSiteUsersProc(CString sqlCmd, int nUserCnt)
{
	int		nResult = TRUE, nIndex  = 0;

	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();
	if(pApp->m_pSmsSiteUsers != NULL){
		delete pApp->m_pSmsSiteUsers;
		pApp->m_pSmsSiteUsers = NULL;
	}
	pApp->m_pSmsSiteUsers = new SMS_SITE_USERS[nUserCnt+1];
	memset(pApp->m_pSmsSiteUsers, 0, sizeof(SMS_SITE_USERS)*(nUserCnt+1));

	switch(OpenTable(sqlCmd)){
	  case TRUE:
		CloseTable();
		break;

	  case FALSE:
		while(TRUE){
			nResult = GetSmsSiteUsersData(nIndex);
			if(nResult == FALSE || nResult == -1){
				if(nIndex > 0){
					nResult = TRUE;
					break;
				}else{
					nResult = FALSE;
					break;
				}
			}
			else nIndex++;
		}
		CloseTable();
		break;

	  default:
		nResult = FALSE;
	}	
	return nResult;
}

int CDirectODBC::GetSmsSiteUsersData(int nIndex)
{
	RETCODE m_nRetcode;
	SDWORD	cbLen=0;
	char sms_site_ip[51], sms_user_name[51], sms_user_phone[21], sms_site_name[51], sms_site_real_ip[51], sms_use[2] = "";
	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();

	m_nRetcode = SQLFetch(m_hStmt);
	if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO){
		SQLGetData(m_hStmt, 1, SQL_C_CHAR, sms_site_ip,			51, &cbLen);
		SQLGetData(m_hStmt, 2, SQL_C_CHAR, sms_user_name,		51, &cbLen);
		SQLGetData(m_hStmt, 3, SQL_C_CHAR, sms_user_phone,		21, &cbLen);
		SQLGetData(m_hStmt, 4, SQL_C_CHAR, sms_site_name,		51, &cbLen);
		SQLGetData(m_hStmt, 5, SQL_C_CHAR, sms_site_real_ip,	51, &cbLen);
		SQLGetData(m_hStmt, 6, SQL_C_CHAR, sms_use,				 2, &cbLen);


		trim(sms_site_ip); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_site_ip, sms_site_ip);
		trim(sms_user_name); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_user_name, sms_user_name);
		trim(sms_user_phone); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_user_phone, sms_user_phone);
		trim(sms_site_name); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_site_name, sms_site_name);
		trim(sms_site_real_ip); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_site_real_ip, sms_site_real_ip);
		trim(sms_use); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_use, sms_use);
		return TRUE;
	}
	else if (m_nRetcode == SQL_NO_DATA_FOUND)	return FALSE;
	else	return -1;
}



int	CDirectODBC::GetSmsBasicUsersProc(CString sqlCmd, SMS_SITE_BASIC_USER *pBasicUser)
{
	int		nResult = TRUE, nIndex  = 0;

	switch(OpenTable(sqlCmd)){
	  case TRUE:
		CloseTable();
		break;

	  case FALSE:
		while(TRUE){
			nResult = GetSmsBasicUsersData(&pBasicUser[nIndex]);
			if(nResult == FALSE || nResult == -1){
				if(nIndex > 0){
					nResult = TRUE;
					break;
				}else{
					nResult = FALSE;
					break;
				}
			}
			else nIndex++;
		}
		CloseTable();
		break;

	  default:
		nResult = FALSE;
	}	
	return nResult;
}

int CDirectODBC::GetSmsBasicUsersData(SMS_SITE_BASIC_USER *pBasicUser)
{
	RETCODE m_nRetcode;
	SDWORD	cbLen=0;
	int site_sms_index;
	char site_sms_name[51];
	char site_sms_telnum[21];
	char site_sms_use[2];
	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();

	m_nRetcode = SQLFetch(m_hStmt);
	if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO){
		SQLGetData(m_hStmt, 1, SQL_C_LONG, &site_sms_index,	0, &cbLen);
		SQLGetData(m_hStmt, 2, SQL_C_CHAR, site_sms_name,		51, &cbLen);
		SQLGetData(m_hStmt, 3, SQL_C_CHAR, site_sms_telnum,		21, &cbLen);
		SQLGetData(m_hStmt, 4, SQL_C_CHAR, site_sms_use,		2, &cbLen);

		pBasicUser->site_sms_index = site_sms_index;
		trim(site_sms_name); strcpy((char*)pBasicUser->site_sms_name, site_sms_name);
		trim(site_sms_telnum); strcpy((char*)pBasicUser->site_sms_telnum, site_sms_telnum);
		trim(site_sms_use); strcpy((char*)pBasicUser->site_sms_use, site_sms_use);
		return TRUE;
	}
	else if (m_nRetcode == SQL_NO_DATA_FOUND)	return FALSE;
	else	return -1;
}

int	CDirectODBC::GetCCLinkInfoProc(CString sqlCmd, CC_LINKINFO *pCCLink)
{
	int		nResult = TRUE, nIndex  = 0;

	switch(OpenTable(sqlCmd)){
	  case TRUE:
		CloseTable();
		break;

	  case FALSE:
		while(TRUE){
			nResult = GetCCLinkInfoData(nIndex, &pCCLink[nIndex]);
			if(nResult == FALSE || nResult == -1){
				if(nIndex > 0){
					nResult = TRUE;
					break;
				}else{
					nResult = FALSE;
					break;
				}
			}
			else nIndex++;
		}
		CloseTable();
		break;

	  default:
		nResult = FALSE;
	}	
	return nResult;
}

int CDirectODBC::GetCCLinkInfoData(int nIndex, CC_LINKINFO *pCCLink)
{
	RETCODE m_nRetcode;
	SDWORD	cbLen=0;
	int cc_index;
	char cc_name[51];
	char cc_ip[51];
	char cc_real_ip[51];
	char cc_site_name[256];

	m_nRetcode = SQLFetch(m_hStmt);
	if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO){
		SQLGetData(m_hStmt, 1, SQL_C_LONG, &cc_index,	0, &cbLen);
		SQLGetData(m_hStmt, 2, SQL_C_CHAR, cc_name,		51, &cbLen);
		SQLGetData(m_hStmt, 3, SQL_C_CHAR, cc_ip,		51, &cbLen);
		SQLGetData(m_hStmt, 4, SQL_C_CHAR, cc_real_ip,	51, &cbLen);
		SQLGetData(m_hStmt, 5, SQL_C_CHAR, cc_site_name,	256, &cbLen);
		
		pCCLink->cc_index = cc_index;
		trim(cc_name); strcpy((char*)pCCLink->cc_name, cc_name);
		trim(cc_ip); strcpy((char*)pCCLink->cc_ip, cc_ip);
		trim(cc_real_ip); strcpy((char*)pCCLink->cc_real_ip, cc_real_ip);
		trim(cc_site_name); strcpy((char*)pCCLink->cc_site_name, cc_site_name);
		return TRUE;
	}
	else if (m_nRetcode == SQL_NO_DATA_FOUND)	return FALSE;
	else	return -1;
}



EMAIL_SITE_USERS* CDirectODBC::GetEMailUserProc(CString strWhereQuery)
{
	int		nResult = FALSE, nCount = 0, nArrIndex = 0;
	
	char szQuery[2048] = "";
	
	EMAIL_SITE_USERS* pEMailuser = NULL, EmailObj; 
	
	sprintf(szQuery, "SELECT COUNT(idx) FROM SITE_EMAIL_USER WHERE %s", strWhereQuery);
	
	if((nCount = RowCount(szQuery)) > 0){
		pEMailuser = (EMAIL_SITE_USERS*)malloc( sizeof(EMAIL_SITE_USERS) * (nCount + 2));
		memset(pEMailuser, 0x00, sizeof(EMAIL_SITE_USERS) * (nCount + 2));
	}else return NULL;
	
	
	sprintf(szQuery, "SELECT idx,email_site_ip,email_site_real_ip,email_site_name,alarm_type,alarm_proc_name,email_to,email_from,email_title,email_msg1,email_msg2,email_msg3,email_url,email_attr1,email_attr2,email_attr3 FROM SITE_EMAIL_USER WHERE %s", strWhereQuery);
	
	switch(OpenTable(szQuery)){
	case TRUE:
		CloseTable();
		break;
		
	case FALSE:
		while(TRUE){
			
			memset(&EmailObj, 0x00, sizeof(EMAIL_SITE_USERS));
			
			
			if( (nResult = GetEMailUserData(EmailObj)) == TRUE){
				
				memcpy(&pEMailuser[nArrIndex++], &EmailObj, sizeof(EMAIL_SITE_USERS));
				
				nResult = TRUE;
				
				if(nArrIndex >= nCount) break;
				
			}else break;				
		}
		CloseTable();
		break;
		
	default:
		nResult = FALSE;
	}	
	
	
	
	if(nCount <= 0 || nResult == FALSE){
		if(pEMailuser != NULL){
			free(pEMailuser);
			pEMailuser = NULL;
		}
	}
	
	return pEMailuser;
}



int	CDirectODBC::GetEMailUserData(EMAIL_SITE_USERS &obj)
{
	RETCODE m_nRetcode;
	SDWORD	cbLen=0;
	m_nRetcode = SQLFetch(m_hStmt);
	
	if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO){
		
		SQLGetData(m_hStmt, 1, SQL_C_LONG, &obj.idx,				0, &cbLen);		
		SQLGetData(m_hStmt, 2, SQL_C_CHAR, obj.email_site_ip,			51, &cbLen);
		SQLGetData(m_hStmt, 3, SQL_C_CHAR, obj.email_site_real_ip,		51, &cbLen);
		SQLGetData(m_hStmt, 4, SQL_C_CHAR, obj.email_site_name,			101, &cbLen);
		SQLGetData(m_hStmt, 5, SQL_C_LONG, &obj.alarm_type,			0, &cbLen);
		SQLGetData(m_hStmt, 6, SQL_C_CHAR, obj.alarm_proc_name,			51, &cbLen);
		SQLGetData(m_hStmt, 7, SQL_C_CHAR, obj.email_to,			501, &cbLen);
		SQLGetData(m_hStmt, 8, SQL_C_CHAR, obj.email_from,			201, &cbLen);
		SQLGetData(m_hStmt, 9, SQL_C_CHAR, obj.email_title,			201, &cbLen);
		SQLGetData(m_hStmt,10, SQL_C_CHAR, obj.email_msg1,			501, &cbLen);
		SQLGetData(m_hStmt,11, SQL_C_CHAR, obj.email_msg2,			501, &cbLen);
		SQLGetData(m_hStmt,12, SQL_C_CHAR, obj.email_msg3,			501, &cbLen);
		SQLGetData(m_hStmt,13, SQL_C_CHAR, obj.email_attr1,			101, &cbLen);
		SQLGetData(m_hStmt,14, SQL_C_CHAR, obj.email_attr2,			101, &cbLen);
		SQLGetData(m_hStmt,15, SQL_C_CHAR, obj.email_attr3,			101, &cbLen);
		
		trim(obj.email_site_ip); 
		trim(obj.email_site_real_ip); 
		trim(obj.email_site_name); 
		trim(obj.alarm_proc_name); 
		trim(obj.email_to); 
		trim(obj.email_from); 
		trim(obj.email_title); 
		trim(obj.email_msg1); 
		trim(obj.email_msg2); 
		trim(obj.email_msg3); 
		trim(obj.email_attr1); 
		trim(obj.email_attr2); 
		trim(obj.email_attr3); 
		return TRUE;
	}
	else if (m_nRetcode == SQL_NO_DATA_FOUND)	return FALSE;
	else	return -1;
}



SMS_EXCLUDE* CDirectODBC::GetSmsExcludeProc()
{
	int		nResult = FALSE, nCount = 0, nArrIndex = 0;

	char szQuery[2048] = "";

	SMS_EXCLUDE* pSmsExclude = NULL, SmsExcludeObj; 

	sprintf(szQuery, "SELECT COUNT(sm_index) FROM SITE_SmsExcludeTime WHERE sm_use='Y'");

	if((nCount = RowCount(szQuery)) > 0){
		pSmsExclude = (SMS_EXCLUDE*)malloc( sizeof(SMS_EXCLUDE) * (nCount + 2));
		memset(pSmsExclude, 0x00, sizeof(SMS_EXCLUDE) * (nCount + 2));
	}else return NULL;


	sprintf(szQuery, "SELECT sm_index, sm_type, sm_msg, sm_stime, sm_etime FROM SITE_SmsExcludeTime WHERE sm_use='Y'");

	switch(OpenTable(szQuery)){
	  case TRUE:
		CloseTable();
		break;

	  case FALSE:
		while(TRUE){

			memset(&SmsExcludeObj, 0x00, sizeof(SMS_EXCLUDE));


			if( (nResult = GetSmsExcludeData(SmsExcludeObj)) == TRUE){

				memcpy(&pSmsExclude[nArrIndex++], &SmsExcludeObj, sizeof(SMS_EXCLUDE));

				nResult = TRUE;

				 if(nArrIndex >= nCount) break;

			}else break;				
		}
		CloseTable();
		break;

	  default:
		nResult = FALSE;
	}	



	if(nCount <= 0 || nResult == FALSE){
		if(pSmsExclude != NULL){
			free(pSmsExclude);
			pSmsExclude = NULL;
		}
	}

	return pSmsExclude;
}



int	CDirectODBC::GetSmsExcludeData(SMS_EXCLUDE &obj)
{
	RETCODE m_nRetcode;
	SDWORD	cbLen=0;
	m_nRetcode = SQLFetch(m_hStmt);

	if (m_nRetcode == SQL_SUCCESS || m_nRetcode == SQL_SUCCESS_WITH_INFO){

		SQLGetData(m_hStmt, 1, SQL_C_LONG, &obj.sm_index,			0, &cbLen);		
		SQLGetData(m_hStmt, 2, SQL_C_LONG, &obj.sm_type,			0, &cbLen);				
		SQLGetData(m_hStmt, 3, SQL_C_CHAR, obj.sm_msg,			201, &cbLen);
		SQLGetData(m_hStmt, 4, SQL_C_CHAR, obj.sm_stime,		11, &cbLen);		
		SQLGetData(m_hStmt, 5, SQL_C_CHAR, obj.sm_etime,		11, &cbLen);
				
		trim(obj.sm_msg); 
		trim(obj.sm_stime); 
		trim(obj.sm_etime); 
		return TRUE;
	}
	else if (m_nRetcode == SQL_NO_DATA_FOUND)	return FALSE;
	else	return -1;
}
