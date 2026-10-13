// TrsSocketServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TrsSocketServer.h"
#include "TrsSocketServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrsSocketServerApp

BEGIN_MESSAGE_MAP(CTrsSocketServerApp, CWinApp)
	//{{AFX_MSG_MAP(CTrsSocketServerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrsSocketServerApp construction

CTrsSocketServerApp::CTrsSocketServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	int i = 0;
	m_pServerSocket = NULL;
	m_pAlarmSocket = NULL;
	m_pLinkSocket = NULL;
	m_pUdp = NULL;
	m_pSmsSiteUsers = NULL;
	m_dlgSql = NULL;
	m_bInitLinkTimer = FALSE;
#ifdef _ATALK
	m_pAtalkSql = NULL;
#endif
	m_smsSql = NULL;

	
	m_pAlramWeb = NULL;
	m_pSmsExclude = NULL;

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTrsSocketServerApp object

CTrsSocketServerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTrsSocketServerApp initialization

BOOL CTrsSocketServerApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	AlarmWebTh_Create();

	CTrsSocketServerDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.


	return FALSE;
}



BOOL CTrsSocketServerApp::SmsExcludeLoadProc()
{
	if(m_dlgSql != NULL){
		m_pSmsExclude = m_dlgSql->GetSmsExcludeProc();

		if(m_pSmsExclude != NULL){

			int i = 0;
			char szLog[2048] = "";
	
			for(i = 0; strlen(m_pSmsExclude[i].sm_stime) > 0 ; i++ ){


				sprintf(szLog, "SmsExcludeLoad idx:%d, type:%d, msg:%s, time:%s-%s",
					m_pSmsExclude[i].sm_index,
					m_pSmsExclude[i].sm_type,
					m_pSmsExclude[i].sm_msg,
					m_pSmsExclude[i].sm_stime,
					m_pSmsExclude[i].sm_etime);

				SmsExcludeLogging(szLog);
			}
		}
	}
	
	return FALSE;
}




BOOL CTrsSocketServerApp::IsSmsExclude(int nType, char* szMsg)
{
	int Exclude = FALSE, i = 0;
	int nFindIndex = -1;
	if(m_pSmsExclude != NULL){
		for(i = 0; strlen(m_pSmsExclude[i].sm_stime) > 0 ; i++ ){

			if( strlen(m_pSmsExclude[i].sm_stime) == 4 &&
				strlen(m_pSmsExclude[i].sm_etime) == 4){

				SYSTEMTIME st; 				
				char szCurrentTime[11] = "";
				GetLocalTime(&st); 						  
				sprintf(szCurrentTime, "%02d%02d", st.wHour, st.wMinute); 

				nFindIndex = i;
				if(strcmp(m_pSmsExclude[i].sm_stime, szCurrentTime) <= 0 && strcmp(szCurrentTime, m_pSmsExclude[i].sm_etime) <= 0){

					if(m_pSmsExclude[i].sm_type == nType){

						switch(nType){				
						case SEND_TYPE_PROC_ERROR:
						case SEND_TYPE_PROC_SUCC:
						case SEND_TYPE_SOCK_CON_ERROR:
						case SEND_TYPE_SOCK_CON_SUCC:
						case SEND_TYPE_SOCK_CHECK_ERROR:
						case SEND_TYPE_SOCK_CLOSE_ERROR:
						case SEND_TYPE_DATA_ERROR:
							if(strlen(szMsg) > 0 && strcmp(m_pSmsExclude[i].sm_msg, szMsg) == FALSE){
								Exclude = TRUE;
							}
							break;
						default:
							Exclude = TRUE;
							break;
						}
					}
				}
			}
		}
	}
	
	char szLog[2048] = "";
	sprintf(szLog, "IsSmsExclude:%d, type:%d, msg:%s, findidx:%d",  Exclude, nType, szMsg, nFindIndex);
	SmsExcludeLogging(szLog);

	return Exclude;
}









BOOL CTrsSocketServerApp::CreateSocketProc()
{
	// 대기 할 소켓을 생성
	char	buf[100]="";
	int		error = FALSE, systemType = 0;

	m_pServerSocket = new CListenSocket();
	if (m_pServerSocket->Create(IPCS_PORT)){
		if (m_pServerSocket->Listen())
			strcpy(buf, "대기중입니다.");
		else{
			error = TRUE;
			delete m_pServerSocket;
			m_pServerSocket = NULL;
			strcpy(buf, "대기에 실패하였습니다.");
		}
	}
	else{
		error = TRUE;
		delete m_pServerSocket;
		m_pServerSocket = NULL;
		strcpy(buf, "대기에 실패하였습니다.");
	}


	
	m_pAlarmSocket = new CListenSocket();
	if (m_pAlarmSocket->Create(ALARM_PORT)){
		if (m_pAlarmSocket->Listen())
			strcpy(buf, "대기중입니다.");
		else{
			error = TRUE;
			delete m_pAlarmSocket;
			m_pAlarmSocket = NULL;
			strcpy(buf, "대기에 실패하였습니다.");
		}
	}
	else{
		error = TRUE;
		delete m_pAlarmSocket;
		m_pAlarmSocket = NULL;
		strcpy(buf, "대기에 실패하였습니다.");
	}



	m_pUdp = new CUdpSocket();
	if (m_pUdp->Create(UDP_PORT, SOCK_DGRAM)){
		strcpy(buf, "대기중입니다.");
	}
	else{
		error = TRUE;
		delete m_pUdp;
		m_pUdp = NULL;
		strcpy(buf, "대기에 실패하였습니다.");
	}

	SYSTEMTIME st;
	char szDate[21];
	CString strSql, strSql1;
	GetLocalTime(&st);
	sprintf(szDate, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	strSql.Format("UPDATE CallCenter_Link SET cc_update_time='%s' WHERE cc_use='Y'", szDate);
	m_dlgSql->ExecSql(strSql);

	strSql1.Format("UPDATE CallCenter_Link SET cc_update_time='%s' WHERE cc_use='Y'", szDate);
	m_dlgSql->ExecSql(strSql1);


	return error;
}

// 클라이언트로부터 접속 요청이 들어오면 CListenSocket 객체로부터 호출된다.
void CTrsSocketServerApp::ProcessPendingAccept(CListenSocket * m_pAcceptSocket) 
{
	CString rSocketAddress;
	UINT	rSocketPort = 0;
	CString rSocketAddress1;
	UINT	rSocketPort1 = 0;
	CTrsSocketServerDlg		*dlg;
	char	szLog[201] = "";
	dlg = (CTrsSocketServerDlg*)m_pMainWnd;


	// 이 소켓은 연결된 클라이언트와의 통신을 책임진다.
	CClientSocket* pSocket = new CClientSocket();

	// 클라이언트의 접속 요청을 받아들인다.
	// 성공하면 클라이언트 소켓에 대한 초기화 작업을 한다.
	if (m_pAcceptSocket->Accept(*pSocket))
	{
		pSocket->GetPeerName(rSocketAddress, rSocketPort);
		pSocket->GetSockName(rSocketAddress1, rSocketPort1);

		sprintf((char*)szLog, "Socket Connect IP:%s,Port:%d",
			rSocketAddress, rSocketPort);
		dlg->PrintListProc(szLog);

		pSocket->m_bCon = TRUE;

		if(rSocketPort1 == ALARM_PORT){
			m_connectionAlarmList.AddTail(pSocket);
		}else{
			m_connectionList.AddTail(pSocket);
		}

		CString strAddress;
		int nPort;
		char szPort[11] = "";
		pSocket->GetPeerName(strAddress, (unsigned int&)nPort);
		itoa(nPort, (char*)szPort, 10);
		int cnt = dlg->m_ConIpList.GetItemCount();
		dlg->m_ConIpList.InsertItem(cnt, (LPCSTR)strAddress);
		dlg->m_ConIpList.SetItemText(cnt, 1, (LPCSTR)strAddress);
		dlg->m_ConIpList.SetItemText(cnt, 2, (LPCSTR)szPort);

	}
	else
	{
		delete pSocket;
		pSocket = NULL;
	}
}

// 클라이언트 소켓을 닫는다.
// 클라이언트 소켓에 대한 정리 작업을 한다.
void CTrsSocketServerApp::CloseSocket(CClientSocket* pSocket)
{
	CString rSocketAddress;
	CTrsSocketServerDlg		*dlg;
	UINT	rSocketPort = 0;
	char	szIp[21] = "", szLog[201] = "";
	int		cnt = 0, nPort = 0;
	int		j = 0, error = FALSE, chdev = 0;
	CClientSocket* pSock;
	POSITION pos;

	dlg = (CTrsSocketServerDlg*)m_pMainWnd;
	if(pSocket != NULL){
		if(m_pLinkSocket == pSocket){
			m_pLinkSocket->ShutDown();
			if(m_pLinkSocket->m_bCon == TRUE){
				m_pLinkSocket->GetPeerName(rSocketAddress, (unsigned int&)rSocketPort);
				m_pLinkSocket->Close();

				cnt = dlg->m_ConIpList.GetItemCount();
				for(j = 0; j < cnt; j++){
					dlg->m_ConIpList.GetItemText(j, 1, szIp, sizeof(szIp)-1);
					if(strncmp((LPCSTR)szIp, (LPCSTR)rSocketAddress, 20) == FALSE){
						dlg->m_ConIpList.DeleteItem(j);
					}
				}
				sprintf((char*)szLog, "link Server Close Socket");
				dlg->PrintListProc(szLog);
			}
			m_pLinkSocket->m_bCon = FALSE;
			delete m_pLinkSocket;
			m_pLinkSocket = NULL;
			KillTimer(m_pMainWnd->m_hWnd, RECORD_TIMER);
			SetTimer(m_pMainWnd->m_hWnd, RECORD_TIMER, 10000, NULL);
			return;
		}
	}

	for(pos = m_connectionList.GetHeadPosition(); pos != NULL;){
		pSock = (CClientSocket*)m_connectionList.GetAt(pos);
		if (pSock == pSocket){
			dlg->PrintListProc("Atia Close Socket");

			pSocket->m_bCon = FALSE;
			m_connectionList.RemoveAt(pos);
			pSocket->Close();
			delete pSocket;
			pSocket = NULL;
			break;
		}
		m_connectionList.GetNext(pos);
	}

	for(pos = m_connectionAlarmList.GetHeadPosition(); pos != NULL;){
		pSock = (CClientSocket*)m_connectionAlarmList.GetAt(pos);
		if (pSock == pSocket){
			dlg->PrintListProc("Alram Close Socket");



			pSocket->GetPeerName(rSocketAddress, (unsigned int&)rSocketPort);
			cnt = dlg->m_ConIpList.GetItemCount();
			for(j = 0; j < cnt; j++){
				dlg->m_ConIpList.GetItemText(j, 1, szIp, sizeof(szIp)-1);
				if(strncmp((LPCSTR)szIp, (LPCSTR)rSocketAddress, 20) == FALSE){
					dlg->m_ConIpList.DeleteItem(j);
				}				
			}

			
			pSocket->m_bCon = FALSE;
			m_connectionAlarmList.RemoveAt(pos);
			pSocket->Close();
			delete pSocket;
			pSocket = NULL;
			break;
		}
		m_connectionAlarmList.GetNext(pos);
	}

}


int CTrsSocketServerApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	AlarmWebTh_Delete();

	
	if(m_pSmsExclude != NULL){
		free(m_pSmsExclude);
		m_pSmsExclude = NULL;
	}



	int		i = 0;
	CClientSocket* pSocket = NULL;
	POSITION pos = NULL;

	if(m_connectionList.GetCount() > 0){
		for(pos = m_connectionList.GetHeadPosition(); pos != NULL;){
			pSocket = (CClientSocket*)m_connectionList.GetAt(pos);
			if(pSocket == NULL) break;
			pSocket->szMailbox[0] = '\0';
			pSocket->m_Rdata[0] = '\0';
			delete pSocket;
			pSocket = NULL;

			m_connectionList.GetNext(pos);
		}
	}


	if(m_connectionAlarmList.GetCount() > 0){
		for(pos = m_connectionAlarmList.GetHeadPosition(); pos != NULL;){
			pSocket = (CClientSocket*)m_connectionAlarmList.GetAt(pos);
			if(pSocket == NULL) break;
			pSocket->szMailbox[0] = '\0';
			pSocket->m_Rdata[0] = '\0';
			delete pSocket;
			pSocket = NULL;
			
			m_connectionAlarmList.GetNext(pos);
		}
	}


	for(i = 0; i < MAX_REC_SOCKET; i++){
		if(m_pLinkSocket != NULL){
			delete m_pLinkSocket;
			m_pLinkSocket = NULL;
		}
	}

	if(m_pServerSocket != NULL){
		delete m_pServerSocket;
		m_pServerSocket = NULL;
	}

	if(m_pAlarmSocket != NULL){
		delete m_pAlarmSocket;
		m_pAlarmSocket = NULL;
	}


	if(m_pUdp != NULL){
		delete m_pUdp;
		m_pUdp = NULL;
	}
	if(m_pSmsSiteUsers != NULL){
		delete m_pSmsSiteUsers;
		m_pSmsSiteUsers = NULL;
	}

	if(m_dlgSql != NULL){
		m_dlgSql->DisConnectODBC();
		delete m_dlgSql;
		m_dlgSql = NULL;
	}
#ifdef _ATALK
	if (m_pAtalkSql != NULL) {
		m_pAtalkSql->DisConnectODBC();
		delete m_pAtalkSql;
		m_pAtalkSql = NULL;
	}
#endif
	if(m_smsSql != NULL){
		m_smsSql->DisConnectODBC();
		delete m_smsSql;
		m_smsSql = NULL;
	}




	return CWinApp::ExitInstance();
}

// 클라이언트 소켓의 버퍼에 데이터가 전송되면 호출된다.
void CTrsSocketServerApp::ReadMsgProc(CClientSocket* pSocket)
{
	int		startPos = -1, endPos = -1, len = 0;
	UCHAR	Rdata[MAXLENGTH+1] = "";
	char	temp[MAXLENGTH*2+1] = "";
	
	TRY
	{
		pSocket->Receive((LPSTR)Rdata, MAXLENGTH);
		
		len = strlen(pSocket->m_Rdata) + strlen((LPCSTR)Rdata);
		if(len < MAXLENGTH*2)
			sprintf(temp, "%s%s", pSocket->m_Rdata, Rdata);
		else
			strcpy(temp, (LPCSTR)Rdata);
		TRACE("LINE 받은데이타:%s\n", temp);
		while(TRUE){
			startPos = endPos = -1;
			//start character , end character를 조사
			SearchSocketSTX_ETX(temp, startPos, endPos);

			//start character , end character를 자름
			if((len = CutSocketSTX_ETX(pSocket, temp, startPos, endPos)) <= 0){
				if(strlen(temp) < MAXLENGTH)
					strcpy(pSocket->m_Rdata, temp);
				else
					strcpy(pSocket->m_Rdata, "");
				break;
			}
		}
	}

	CATCH(CFileException, e)
	{
		e->Delete();
	}
	END_CATCH
}


// 클라이언트 소켓의 버퍼에 데이터가 전송되면 호출된다.
void CTrsSocketServerApp::ReadUdpMsgProc(CUdpSocket* pSocket)
{
	CString rSocketAddress;
	UINT	rSocketPort;
	int		startPos = -1, endPos = -1, len = 0;
	char	Rdata[MAXLENGTH+1] = "";
	char	temp1[MAXLENGTH *2 + 1] ="";
	
	TRY
	{
		pSocket->ReceiveFrom(Rdata, MAXLENGTH, rSocketAddress, rSocketPort);
		
		TRACE("LINE 받은데이타:%s\n", Rdata);

		startPos = endPos = -1;
		//start character , end character를 조사
		SearchSocketSTX_ETX(Rdata, startPos, endPos);

		if(startPos != -1 && endPos != -1){
			len = endPos - startPos+1; //stx, etx 제외

			//클라이언트와의 통신 상태
			strncpy((char*)temp1, (const char*)Rdata+startPos+1, len-2); temp1[len-2] = '\0';

			ServerUdpProc(pSocket, temp1, rSocketAddress, rSocketPort);
		}
	}

	CATCH(CFileException, e)
	{
		e->Delete();
	}
	END_CATCH
}

// 클라이언트 소켓의 버퍼에 데이터가 전송되면 호출된다.
void CTrsSocketServerApp::ReadMsgTestProc(char szRdata[])
{
	int		startPos = -1, endPos = -1, len = 0;
	UCHAR	Rdata[MAXLENGTH+1] = "";
	char	temp[MAXLENGTH*2+1] = "";
	
	TRY
	{
		strcpy((char*)Rdata, (LPCSTR)szRdata);
		
		len = strlen(m_Rdata) + strlen((LPCSTR)Rdata);
		if(len < MAXLENGTH)
			sprintf(temp, "%s%s", m_Rdata, Rdata);
		else
			strcpy(temp, (LPCSTR)Rdata);
		TRACE("LINE 받은데이타:%s\n", temp);
		while(TRUE){
			startPos = endPos = -1;
			//start character , end character를 조사
			SearchSocketSTX_ETX(temp, startPos, endPos);

			//start character , end character를 자름
			if((len = CutSocketSTX_ETX(NULL, temp, startPos, endPos)) <= 0){
				if(strlen(temp) < MAXLENGTH)
					strcpy(m_Rdata, temp);
				else
					strcpy(m_Rdata, "");
				break;
			}
		}
	}

	CATCH(CFileException, e)
	{
		e->Delete();
	}
	END_CATCH
}


void CTrsSocketServerApp::SearchSocketSTX_ETX(char data[], int &startPos, int &endPos)
{
	int		i = 0, j = 0;
	char	stx[2] ="", etx[2] = "";
	
	//start characteristic , end characteristic를 조사
	sprintf(stx, "%c", 0x02); sprintf(etx, "%c", 0x03);
	for(i = 0; data[i] != NULL; i++){
		if(data[i] == stx[0]){
			startPos  = i; 	//start characteristic
			for(j = i+1; data[j] != NULL; j++){
				if(data[j] == stx[0])
					startPos  = j; //start characteristic
				else if(data[j] == etx[0]){
					endPos = j; //end characteristic
					break;
				}
			}
			break;
		}
		else
			continue;
	}

}


int CTrsSocketServerApp::CutSocketSTX_ETX(CClientSocket* pSocket, char data[], int startPos, int endPos)
{
	int		ret = 0, len = 0;
	char	command[5] ="", temp[MAXLENGTH *2 + 1] ="", temp1[MAXLENGTH *2 + 1] ="";

	if(startPos != -1 && endPos != -1){
		len = endPos - startPos+1; //stx, etx 제외

		strncpy((char*)command, (const char*)data+startPos+1, 2); command[2] = '\0';
		ret = atoi((LPCSTR)command);

		//클라이언트와의 통신 상태
		strncpy((char*)temp1, (const char*)data+startPos+1, len-2); temp1[len-2] = '\0';

		ServerProc(pSocket, ret, temp1);
		//나머지 데이터 
		lstrcpy(temp, (const char*)data+endPos+1);
		lstrcpy(data, temp);
	}
	else if(startPos != -1 && endPos == -1){
		//error data인 경우 
		lstrcpy(temp, (const char*)data+startPos);
		lstrcpy((char*)data, temp);
	}
	else
		lstrcpy((LPSTR)data, "");

	return len;
}


void CTrsSocketServerApp::ServerProc(CClientSocket* pSocket, int ret, char Rdata[])
{
	SYSTEMTIME	st;
	CTrsSocketServerDlg		*dlg;
	CString rSocketAddress;
	UINT	rSocketPort = 0;
	CString rSocketAddress1;
	UINT	rSocketPort1 = 0;
	CString strData, strIP, strMac, strType;
	CString strSql, strSql1;
	int nFindPos, nLen;
	POSITION		pos = NULL;
	char	command[4] = "", Sdata[1024] = "", szLog[1024] = "", szStatus[100] = "", szRecordIni[51] = "";
	int		error = FALSE, systemType = 0, nChange = TRUE, i = 0, j = 0, nPort = 0;
	int		k = 0, listCnt = 0;
	long	nCnt = -1;

	nPort = GetPrivateProfileInt(_T("LINK_SERVER"), "SERVER_PORT", -1, TRS_INI_PATH);

	GetLocalTime(&st);
	strncpy(command, Rdata, 1); command[1] = '\0';//copy command

	dlg = (CTrsSocketServerDlg*)m_pMainWnd;

	pSocket->GetSockName(rSocketAddress, rSocketPort);
	pSocket->GetPeerName(rSocketAddress1, rSocketPort1);
	sprintf((char*)Sdata, "IP:%s, Port:%d, Data:%s", rSocketAddress1, rSocketPort1, Rdata);
	switch(rSocketPort){
	  case IPCS_PORT:
		TcpLogging(Sdata);
		strData.Format("%s", Rdata);
		nLen = strData.GetLength();
		nFindPos = strData.Find("IP:");
		if(nFindPos > 3){
			strType = strData.Mid(0, 3);
			strMac = strData.Mid(4, nFindPos-4);
			strIP = strData.Mid(nFindPos+3);

			if(strIP.GetLength() > 0){
				strSql.Format("INSERT INTO IPCS_LOG (LOG_DATA, LOG_IP, LOG_TYPE, LOG_REAL_IP) VALUES ('%s', '%s', '%s', '%s')", 
					strMac, strIP, strType, rSocketAddress1);
				m_dlgSql->ExecSql(strSql);
			}

			strSql.Format("select count(*) from lic_access_list where nasa_con_ip='%s' and nasa_use='Y'", rSocketAddress1 );
			nCnt = m_dlgSql->RowCount(strSql);

			if(nCnt <= 0){
				pSocket->Send("Close", 5);
			}
			else{
				pSocket->Send("Run", 3);
			}
		}
		break;
		
	  case ALARM_PORT:
		  strncpy(command, Rdata, 1); command[1] = '\0';//copy command
		  if(atoi(command) == 3){
			  SYSTEMTIME st; 				
			  char szTime[21] = "";
			  GetLocalTime(&st); 			
			  sprintf(szTime, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond); 
			  sprintf(Sdata, "%c%d%c%s%c%s%c", 0x02, 4, 0xff, "", 0xff, szTime, 0x03);
			  pSocket->Send(Sdata, strlen(Sdata));
		  }

		  break;

	  default:
		if(rSocketPort1 == (unsigned int)nPort){
			listCnt = m_connectionList.GetCount();
			pos = m_connectionList.GetHeadPosition();
			for(k = 0; k < listCnt; k++){
				pSocket = (CClientSocket*)m_connectionList.GetAt(pos);
				if(pSocket != NULL){
					pSocket->Send(Sdata, strlen((LPCSTR)Sdata));
					sprintf((char*)szLog, "send agt:%s", Sdata);
				}
				m_connectionList.GetNext(pos);
			}
		}
		break;
	}

	if(dlg != NULL) dlg->PrintListProc((LPSTR)szLog);
}


void CTrsSocketServerApp::Hangul_StrCut(char* szSrc, char* szDst, int nMaxByteCount)
{	
	int i =0, j = 0;
	
	while(szSrc[i] != '\0'){		
		if(szSrc[i] & 0x80){			
			if(j+2 > nMaxByteCount) break;			
			szDst[j++] = szSrc[i++];
			szDst[j++] = szSrc[i];
		}else{
			if(j+1 > nMaxByteCount) break;			
			szDst[j++] = szSrc[i];
		}	
		i++;
	}	
	szDst[j] ='\0';		
}


void CTrsSocketServerApp::ServerUdpProc(CUdpSocket* pSocket, char Rdata[], CString rSocketAddress1, UINT rSocketPort1)
{
	SYSTEMTIME	st;
	CTrsSocketServerDlg		*dlg;
	CString rSocketAddress;
	UINT rSocketPort;
	char	Sdata[1024], szLog[1024];
	char	szResult[51][1024], szIP[21], szDate[21], szUsers[1024], szRMsg[1024], szDay[21] = "", szNasUpFailDate[21] = "";
	int		i, nIndex, nType, nUserCnt, nUserBasicCnt, nSiteCnt, nCnt, nNasFailCnt = 0;
	char	szMsg[1024] = "", szAlarm[1024] = "";
#ifdef _ATALK
	char szSendData[1025] = "";
#else
	char szSendData[81] = "";
#endif
	char	szRealIP[51];
	char	szTempCode[128] = "";
	CString	strSql, strSql1, strSql2, strSql3, strSql4;

	int nSmsExclude = FALSE;

	GetLocalTime(&st);
	int nIsEmergency = FALSE;

	dlg = (CTrsSocketServerDlg*)m_pMainWnd;

	pSocket->GetSockName(rSocketAddress, rSocketPort);
	sprintf((char*)szLog, "IP:%s, Port:%d, Data:%s", rSocketAddress1, rSocketPort1, Rdata);
	sprintf(szDate, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf(szDay, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);

	StrSplit(Rdata, szResult, (char)0xff);
	nIndex = atoi(szResult[0]);
	nType = atoi(szResult[1]);
	strcpy(szIP, szResult[2]);
	strcpy(szRMsg, szResult[3]);
	rSocketAddress1.TrimLeft(" "); rSocketAddress1.TrimRight(" ");
	strcpy(szRealIP, (LPCSTR)rSocketAddress1);

	char szUseIP[200] = "";
	if(strcmp(szRealIP, szIP) == FALSE){
		strcpy(szUseIP, szRealIP);
	}else{
		sprintf(szUseIP, "%s(%s)", szRealIP,  szIP);
	}



	switch(nType){
	case SEND_TYPE_IVR_GPS:
	case SEND_TYPE_IVR_NORMAL:
		IVRSMSSendProc(szResult, szLog);
		return;
	default:
		break;
	}

	sprintf(Sdata, "%c%s%c", 0x02, Rdata, 0x03);
	strSql.Format("SELECT Count(sms_user_phone) FROM SITE_SMS_USER where (sms_use='Y' OR sms_use='T') AND (sms_site_ip='%s' OR sms_site_ip='ALL') AND (sms_site_real_ip='%s' OR sms_site_real_ip='ALL')",
		szIP, szRealIP);
	nUserCnt = m_dlgSql->RowCount(strSql);


	//DbgOut("main=%d, %s \r\n", nUserCnt, strSql);


	if(nUserCnt > 0){
		strSql.Format("SELECT sms_site_ip,sms_user_name,sms_user_phone, sms_site_name, sms_site_real_ip, sms_use FROM SITE_SMS_USER where (sms_use='Y' OR sms_use='T') AND (sms_site_ip='%s' OR sms_site_ip='ALL') AND (sms_site_real_ip='%s' OR sms_site_real_ip='ALL')",
			szIP, szRealIP);
		m_dlgSql->GetSmsSiteUsersProc(strSql, nUserCnt);

		strSql1.Format("SELECT COUNT(cc_ip) FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", szIP, szRealIP);
		nSiteCnt = m_dlgSql->RowCount(strSql1);
		if(nSiteCnt <= 0){
			strSql.Format("INSERT INTO CallCenter_Link (cc_name, cc_site_name, cc_ip, cc_real_ip) values ('new_site', '신규1', '%s', '%s')", szIP, szRealIP);
			m_dlgSql->ExecSql(strSql);
			
			sprintf(szMsg, "CallCenter_Link 테이블 이름 설정 확인\n시스템IP:%s\n공인IP:%s", szIP, szRealIP);			
			//strncpy(szSendData, szMsg, 79); szSendData[79] = '\0';
			Hangul_StrCut(szMsg, szSendData, 80);

			GetUserString(szUsers, szIP, szRealIP, nUserCnt, USE_NORAML);
			if(strlen(szUsers) > 0){
#ifdef _ATALK
				//알림톡으로 발송
				char smsUsrName[50] = "", smsUsrNum[20] = "";
				BOOL bRet = FALSE;
				strcpy(szTempCode, (char*)"tableCheck001");
				for (int usrIdx = 0; usrIdx < nUserCnt; usrIdx++) {
					bRet = GetUserInfoIdx(usrIdx, smsUsrName, smsUsrNum, nUserCnt, USE_NORAML);
					if (bRet) {
						strSql.Format("INSERT INTO \"MTS_ATALK_MSG\" (TRAN_PR, TRAN_SENDER_KEY, TRAN_TMPL_CD, TRAN_CALLBACK, TRAN_PHONE, TRAN_SUBJECT, TRAN_MSG, TRAN_DATE, TRAN_TYPE, TRAN_STATUS, TRAN_REPLACE_TYPE, TRAN_REPLACE_MSG) VALUES (nextval('mts_atalk_msg_seq'), 'fdd97f2559e5244dd3999e27683086b3d3f3eeba', '%s', '15889310', '%s', 'OnpiaATalk', '%s', now(), 5, '1', 'S', '%s')",
							szTempCode, smsUsrNum, szSendData, szSendData);	//템플릿코드, 번호, 메시지, 메시지
						if (m_pAtalkSql->ExecSql(strSql) != FALSE) SqlErrLogging(strSql.GetBuffer(0));
					}
					bRet = FALSE;
				}
#else

				strSql.Format("INSERT INTO SDK_SMS_SEND (USER_ID, SUBJECT, SMS_MSG, NOW_DATE, SEND_DATE, CALLBACK, DEST_COUNT, DEST_INFO) VALUES ('onpia', '장애발생', '%s', '%s', '%s', '15889310', %d, '%s')", 
					szSendData, szDate, szDate, nUserCnt, szUsers);
				if(m_smsSql->ExecSql(strSql) != FALSE) SqlErrLogging(strSql.GetBuffer(0));
#endif
			}
			return;
		}
		else{
			strSql1.Format("SELECT COUNT(cc_ip) FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", szIP, szRealIP);
			nSiteCnt = m_dlgSql->RowCount(strSql1);
			if(nSiteCnt <= 0){
				sprintf(szLog, "문자메시지 서비스 사용 안하는 아이피 입니다. 실IP:%s, 공인IP:%s, sms_site_name:%s", szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name);
				if(dlg != NULL) dlg->PrintListProc((LPSTR)szLog);
				UdpLogging(szLog);
				return;
			}



			
			// "신규1" 등록된 사이트이면서 3시간 경과된 건은 알람 전송 안함 - 190731
			strSql2.Format("SELECT * FROM CallCenter_Link where cc_use='Y' AND cc_site_name='신규1' AND DATEDIFF(hour, cc_insert_time, getdate()) > 3 AND cc_insert_time is NOT NULL AND cc_ip='%s' AND cc_real_ip='%s'", szIP, szRealIP);
			int nInsertTimeCompCount = m_dlgSql->RowCount(strSql2);
			if(nInsertTimeCompCount > 0){
				sprintf(szLog, "신규1 -- 미변경 사이트 : 실IP:%s, 공인IP:%s, sms_site_name:%s", szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name);
				if(dlg != NULL) dlg->PrintListProc((LPSTR)szLog);
				UdpLogging(szLog);
				return;
			}							
			


			switch(rSocketPort){
			  case UDP_PORT:
				UdpLogging(szLog);
				strcpy(szTempCode, (char*)"");
				
				switch(nType){
				case SEND_TYPE_PROC_ERROR:
					sprintf(szMsg, "%s-%s에서 %s 프로세스 장애가 발생 했습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"procError001");
					AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_PROC_ERROR,  szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name, szRMsg, szMsg); // 이메일전송 - 프로세서 장애  


					break;
				case SEND_TYPE_PROC_SUCC:
					sprintf(szMsg, "%s-%s에서 %s 프로세스 장애가 복구 됬습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"procReturn001");

					AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_PROC_SUCC,  szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name, szRMsg, szMsg); // 이메일전송 - 프로세서 복구 


					break;
				case SEND_TYPE_CPU_ERROR:
					sprintf(szMsg, "%s-%s에서 CPU 장애가 발생 했습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"cpuError001");
					break;
				case SEND_TYPE_CPU_SUCC:
					sprintf(szMsg, "%s-%s에서 CPU 장애가 복구 됬습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"cpuReturn001");
					break;
				case SEND_TYPE_DISK_ERROR:
					sprintf(szMsg, "%s-%s에서 %s DISK 장애가 발생 했습니다. 용량을 확인해 주세요.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"diskError001");
					nIsEmergency = TRUE;
					break;
				case SEND_TYPE_DISK_SUCC:
					sprintf(szMsg, "%s-%s에서 %s DISK 장애가 복구 됬습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"diskReturn001");
					nIsEmergency = TRUE;
					break;
				case SEND_TYPE_MEM_ERROR:
					sprintf(szMsg, "%s-%s에서 MEM 장애가 발생 했습니다. 가상 메모리가 부족 합니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"memError001");
					break;
				case SEND_TYPE_MEM_SUCC:
					sprintf(szMsg, "%s-%s에서 MEM 장애가 복구 됬습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"memReturn001");
					break;
				case SEND_TYPE_SOCK_CON_ERROR:
					sprintf(szMsg, "%s-%s에서 %s SOCKET 연결 장애가 발생 했습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"sockError001");
					break;
				case SEND_TYPE_SOCK_CON_SUCC:
					sprintf(szMsg, "%s-%s에서 %s SOCKET에 연결 되었습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"sockReturn001");
					break;
				case SEND_TYPE_SOCK_CHECK_ERROR:
					sprintf(szMsg, "%s-%s에서 %s SOCKET LINK CHECK에 장애가 발생 했습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"sockLinkError001");
					break;
				case SEND_TYPE_SOCK_CLOSE_ERROR:
					sprintf(szMsg, "%s-%s에서 %s SOCKET Close 장애가 발생 했습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"sockDisconError001");
					break;
				case SEND_TYPE_ICMP_ERROR:
					sprintf(szMsg, "%s-%s에서 PING 전송이 실패 되고 있습니다. 네트워크 검사", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"pingError001");
					break;
				case SEND_TYPE_ICMP_SUCC:
					sprintf(szMsg, "%s-%s에서 PING 전송이 성공 되고 있습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"pingReturn001");
					break;
				case SEND_TYPE_USER_MSG:
					sprintf(szMsg, "%s", szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					break;
				case SEND_TYPE_DATA_ERROR:					
					if(PauseCheckProc(szIP, szRealIP) == FALSE){
						sprintf(szMsg, "%s-%s에서 %s 로그 장애가 발생 했습니다. 해당 프로세스 검사", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
						
					}else{
						sprintf(szAlarm, "%s-%s에서 %s 로그 장애가 발생 했습니다. 해당 프로세스 검사", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
						sprintf(szLog, "%s-%s에서 %s 로그 장애가 발생 했습니다. - 문자전송 일시정지중! ", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
						if(dlg != NULL) dlg->PrintListProc((LPSTR)szLog);						
						UdpLogging(szLog);
					}
					strcpy(szTempCode, (char*)"procLogError001");
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					break;
				case SEND_TYPE_LINK_CHECK:
					strSql.Format("SELECT Count(cc_ip) FROM CallCenter_Link where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y' AND cc_link_error='Y'", szIP, szRealIP);
					nCnt = m_dlgSql->RowCount(strSql);
					if(nCnt > 0) {
						sprintf(szMsg, "%s-%s에서 링크데이터가 수신 되고 있습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
						strcpy(szTempCode, (char*)"linkDataReturn001");

						AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_NET_SUCC,  szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name, szMsg); // 이메일전송 - 네트워크 정상 연결 

					}
					strSql.Format("UPDATE CallCenter_Link SET cc_update_time='%s',cc_link_error='N' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", szDate, szIP, szRealIP);
					m_dlgSql->ExecSql(strSql);


					nIsEmergency = TRUE;


					break;
				case SEND_TYPE_NAS_CON_FAIL:

					strSql3.Format("SELECT cc_nas_failcnt FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", szIP, szRealIP);
					nNasFailCnt = m_dlgSql->GetUnsignedIntValue(strSql3);
					
					if (nNasFailCnt > 0) {	//최초 NAS 연결 장애 건에 대한 메시지는 전달하지 않는다....
						nIsEmergency = TRUE;
					}

					if (nNasFailCnt > 999) nNasFailCnt = 999;	//Max 값 설정...

					strSql4.Format("UPDATE CallCenter_Link SET cc_nas_failcnt = %d WHERE cc_ip='%s' AND cc_real_ip='%s'", ++nNasFailCnt, szIP, szRealIP); //실패 건수에 대한 DB 업데이트
					m_dlgSql->ExecSql(strSql4);

					if (nNasFailCnt <= 10) {		//	실패 횟수가 10회 이하의 건에 대해서만 처리한다. - 240122 JY
						if (strlen(szRMsg) > 0) {
							sprintf(szMsg, "%s-%s에서 NAS 연결 장애가 발생하였습니다.(%s)", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
							strcpy(szTempCode, (char*)"nasConError001");
							AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_NAS_CON_FAIL, szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name, szRMsg, szMsg); // 이메일전송 - NAS 연결 장애  

						}
						else {
							sprintf(szMsg, "%s-%s에서 NAS 연결 장애가 발생 했습니다.(%d회)", m_pSmsSiteUsers[0].sms_site_name, szUseIP, nNasFailCnt);
							strcpy(szTempCode, (char*)"nasConError003");
							AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_NAS_CON_FAIL, szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name, szRMsg, szMsg); // 이메일전송 - NAS 연결 장애 
						}
					}

					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);

					break;
				case SEND_TYPE_NAS_CON_SUCC:
					sprintf(szMsg, "%s-%s에서 NAS 연결 되었습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"nasConReturn001");
					AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_NAS_CON_SUCC,  szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name, szRMsg, szMsg); // 이메일전송 - NAS 연결 성공
					
					strSql3.Format("SELECT cc_nas_failcnt FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", szIP, szRealIP);
					nNasFailCnt = m_dlgSql->GetUnsignedIntValue(strSql3);

					if (nNasFailCnt > 1) {	//최초 NAS 연결 건에 대한 메시지는 전달하지 않는다....
						nIsEmergency = TRUE;
					}

					nNasFailCnt = 0;
					
					strSql4.Format("UPDATE CallCenter_Link SET cc_nas_failcnt = %d WHERE cc_ip='%s' AND cc_real_ip='%s'", nNasFailCnt, szIP, szRealIP); //기존 실패 건수 초기화
					m_dlgSql->ExecSql(strSql4);

					break;
				case SEND_TYPE_NAS_UPLOAD_FAIL:
					sprintf(szMsg, "%s-%s에서 NAS 업로드가 실패 하고 있습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"nasUploadError002");
					AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_NAS_UPLOAD_FAIL,  szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name, szRMsg, szMsg); // 이메일전송 - NAS 업로드 실패 

					strSql3.Format("SELECT cc_nasupload_fail FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", szIP, szRealIP);
					m_dlgSql->GetValue(strSql3, szNasUpFailDate);
					
					//마지막으로 성공 메시지 보낸 날짜와 다를 경우
					if ((strcmp(szDay, szNasUpFailDate) != FALSE)) {
						nIsEmergency = TRUE;
					}

					break;
				case SEND_TYPE_NAS_UPLOAD_SUCC:				
					sprintf(szMsg, "%s-%s에서 NAS 업로드가 성공 되고 있습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"nasUploadReturn001");
					AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_NAS_UPLOAD_SUCC,  szIP, szRealIP, m_pSmsSiteUsers[0].sms_site_name, szRMsg, szMsg); // 이메일전송 - NAS 업로드 성공
					
					strSql3.Format("SELECT cc_nasupload_fail FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", szIP, szRealIP);
					m_dlgSql->GetValue(strSql3, szNasUpFailDate);

					if ((strcmp(szDay, szNasUpFailDate) != FALSE)) {
						//성공되었을 때 날짜업데이트를 하여 이후 더 이상 알림이 오지 않게 한다.
						strSql4.Format("UPDATE CallCenter_Link SET cc_nasupload_fail='%s' WHERE cc_ip='%s' AND cc_real_ip='%s'", szDay, szIP, szRealIP);
						m_dlgSql->ExecSql(strSql4);
						nIsEmergency = TRUE;
					}
					
					break;
				case SEND_TYPE_BACKUP_MESSAGE:
					sprintf(szMsg, "%s-%s에서 %s", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					//메시지 내용이 정확하지 않아 임의의 템플릿 코드로 메시지로 발송하게 변경
					strcpy(szTempCode, (char*)"unknownCmd003");
					break;	
				case SEND_TYPE_BACKUP_DISKFULL:
					sprintf(szMsg, "%s-%s에서 녹취 백업 DISK용량 초과 장애가 발생하였습니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"nasDiskError001");
					//nIsEmergency = TRUE;
					break;	
				case SEND_TYPE_FILE_REMOVE:
					sprintf(szMsg, "%s-%s에서 녹취용량(%s%%) 초과로 녹취파일이 삭제 되었습니다", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);	
					strcpy(szTempCode, (char*)"recDiskDel001");	
					//nIsEmergency = TRUE;	//2025-12-08 긴급 문자 원복
					break;
				case SEND_TYPE_DISK_FULL:
					sprintf(szMsg, "%s-%s에서 녹취용량(%s%%) 초과 장애입니다.", m_pSmsSiteUsers[0].sms_site_name, szUseIP, szRMsg);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);	
					strcpy(szTempCode, (char*)"recDiskThres001");
					//nIsEmergency = TRUE;
					break;
				default:
					sprintf(szLog, "UDP_PORT 알 수 없는 형식 입니다. nType:%d", nType);
					pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
					strcpy(szTempCode, (char*)"unknownCmd001");
					break;
				}
				break;
			  default:
				sprintf(szLog, "rSocketPort 알 수 없는 포트 입니다. port:%d, nType:%d", rSocketPort, nType);
				pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
				strcpy(szTempCode, (char*)"unknownCmd002");
				break;
			}



			if(strlen(szMsg) > 0){
				//strncpy(szSendData, szMsg, 79); szSendData[79] = '\0';
				AlarmUser_Send(szMsg);
				AlarmWebTh_Push(szMsg);

#ifdef _ATALK
				Hangul_StrCut(szMsg, szSendData, 1024);
#else
				Hangul_StrCut(szMsg, szSendData, 80);
#endif				
				// 긴급은 문자도 전송
				if(nIsEmergency == TRUE){
					GetUserString(szUsers, szIP, szRealIP, nUserCnt, USE_ALL);
				}else{
					GetUserString(szUsers, szIP, szRealIP, nUserCnt, USE_NORAML);
				}

				if(strlen(szUsers) > 0){

					nSmsExclude = IsSmsExclude(nType, szRMsg);   // 새벽시간대에 문자 발송 중지 처리 

					if(nSmsExclude == FALSE){
#ifdef _ATALK
						//알림톡으로 발송
						char smsUsrName[50] = "", smsUsrNum[20] = "";
						BOOL bRet = FALSE;
						for (int usrIdx = 0; usrIdx < nUserCnt; usrIdx++) {
							if (nIsEmergency == TRUE) {
								bRet = GetUserInfoIdx(usrIdx, smsUsrName, smsUsrNum, nUserCnt, USE_ALL);
							}
							else {
								bRet = GetUserInfoIdx(usrIdx, smsUsrName, smsUsrNum, nUserCnt, USE_NORAML);
							}
							
							if (bRet) {
								strSql.Format("INSERT INTO \"MTS_ATALK_MSG\" (TRAN_PR, TRAN_SENDER_KEY, TRAN_TMPL_CD, TRAN_CALLBACK, TRAN_PHONE, TRAN_SUBJECT, TRAN_MSG, TRAN_DATE, TRAN_TYPE, TRAN_STATUS, TRAN_REPLACE_TYPE, TRAN_REPLACE_MSG) VALUES (nextval('mts_atalk_msg_seq'), 'fdd97f2559e5244dd3999e27683086b3d3f3eeba', '%s', '15889310', '%s', 'OnpiaATalk', '%s', now(), 5, '1', 'S', '%s')",
									szTempCode, smsUsrNum, szSendData, szSendData);	//템플릿코드, 번호, 메시지, 메시지
								if (m_pAtalkSql->ExecSql(strSql) != FALSE) SqlErrLogging(strSql.GetBuffer(0));
							}
							bRet = FALSE;
						}

#else
						strSql.Format("INSERT INTO SDK_SMS_SEND (USER_ID, SUBJECT, SMS_MSG, NOW_DATE, SEND_DATE, CALLBACK, DEST_COUNT, DEST_INFO) VALUES ('onpia', '장애발생', '%s', '%s', '%s', '15889310', %d, '%s')", 
							szSendData, szDate, szDate, nUserCnt, szUsers);				   
						if(m_smsSql->ExecSql(strSql) != FALSE) SqlErrLogging(strSql.GetBuffer(0));
#endif
					}
				}



			}else if(strlen(szAlarm) > 0){
				AlarmUser_Send(szAlarm, 2);
				//AlarmWebTh_Push(szAlarm, 2);
			}
		}
	}
	else{
	
		
		strSql1.Format("SELECT count(site_sms_index) FROM SITE_SMS_BASIC_USER WHERE site_sms_use='Y' OR site_sms_use='T'");
		nUserBasicCnt = m_dlgSql->RowCount(strSql1);

//		DbgOut("cnt=%d, %s \r\n", nUserBasicCnt, strSql1);
		if(nUserBasicCnt > 0){
			

			strSql1.Format("SELECT COUNT(cc_ip) FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", szIP, szRealIP);
			nSiteCnt = m_dlgSql->RowCount(strSql1);
			if(nSiteCnt <= 0){


				// 신규 등록시에만 처리 하도록 수정 - 190801
				SMS_SITE_BASIC_USER *pBasicUser = new SMS_SITE_BASIC_USER[nUserBasicCnt+1];
				strSql1.Format("SELECT site_sms_index, site_sms_name, site_sms_telnum, site_sms_use FROM SITE_SMS_BASIC_USER WHERE site_sms_use='Y' OR site_sms_use='T'"); 
				m_dlgSql->GetSmsBasicUsersProc(strSql1, pBasicUser);
				for(i = 0; i < nUserBasicCnt; i++){
					strSql.Format("INSERT INTO SITE_SMS_USER (sms_site_ip, sms_site_real_ip, sms_user_name, sms_user_phone, sms_use, sms_site_name) values ('%s', '%s', '%s', '%s', '%c', '신규1')",
						szIP, szRealIP, pBasicUser[i].site_sms_name, pBasicUser[i].site_sms_telnum, pBasicUser[i].site_sms_use[0]);
					m_dlgSql->ExecSql(strSql);
			
				}



				strSql.Format("INSERT INTO CallCenter_Link (cc_name, cc_site_name,cc_ip, cc_real_ip) values ('new_site', '신규1', '%s', '%s')", szIP, szRealIP);
				m_dlgSql->ExecSql(strSql);
				
				sprintf(szMsg, "콜센터 신규 사이트 감지\n시스템IP:%s\n공인IP:%s", szIP, szRealIP);
				//strncpy(szSendData, szMsg, 79); szSendData[79] = '\0';
				AlarmUser_Send(szMsg);
				AlarmWebTh_Push(szMsg);
#ifdef _ATALK
				Hangul_StrCut(szMsg, szSendData, 1024);
#else
				Hangul_StrCut(szMsg, szSendData, 80);
#endif	

				GetUserString(szUsers, szIP, szRealIP, nUserBasicCnt, USE_ALL);
				if(strlen(szUsers) > 0){
#ifdef _ATALK
					//알림톡으로 발송
					strcpy(szTempCode, (char*)"newSite001");
					char smsUsrName[50] = "", smsUsrNum[20] = "";
					BOOL bRet = FALSE;
					for (int usrIdx = 0; usrIdx < nUserBasicCnt; usrIdx++) {
						bRet = GetUserInfoIdx(usrIdx, smsUsrName, smsUsrNum, nUserBasicCnt, USE_ALL);
						if (bRet) {
							strSql.Format("INSERT INTO \"MTS_ATALK_MSG\" (TRAN_PR, TRAN_SENDER_KEY, TRAN_TMPL_CD, TRAN_CALLBACK, TRAN_PHONE, TRAN_SUBJECT, TRAN_MSG, TRAN_DATE, TRAN_TYPE, TRAN_STATUS, TRAN_REPLACE_TYPE, TRAN_REPLACE_MSG) VALUES (nextval('mts_atalk_msg_seq'), 'fdd97f2559e5244dd3999e27683086b3d3f3eeba', '%s', '15889310', '%s', 'OnpiaATalk', '%s', now(), 5, '1', 'S', '%s')",
								szTempCode, smsUsrNum, szSendData, szSendData);	//템플릿코드, 번호, 메시지, 메시지
							if (m_pAtalkSql->ExecSql(strSql) != FALSE) SqlErrLogging(strSql.GetBuffer(0));
						}
						bRet = FALSE;
					}


#else
					strSql.Format("INSERT INTO SDK_SMS_SEND (USER_ID, SUBJECT, SMS_MSG, NOW_DATE, SEND_DATE, CALLBACK, DEST_COUNT, DEST_INFO) VALUES ('onpia', '장애발생', '%s', '%s', '%s', '15889310', %d, '%s')", 
						szSendData, szDate, szDate, nUserBasicCnt, szUsers);				
					if(m_smsSql->ExecSql(strSql) != FALSE) SqlErrLogging(strSql.GetBuffer(0));
#endif
				}
			}
			else{
				sprintf(szLog, "등록 되었지만 알 수 없는 아이피 입니다.");
				if(dlg != NULL) dlg->PrintListProc((LPSTR)szLog);
				sprintf(szLog, "CallCenter_Link, SITE_SMS_USER 테이블 확인 해 주세요. szIP:%s, szRealIP:%s, port:%d, nType:%d", szIP, szRealIP, rSocketPort, nType);
				UdpLogging(szLog);
			}
			pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
		}
		else{
			sprintf(szLog, "szIP 알 수 없는 아이피 입니다. szIP:%s, szRealIP:%s, port:%d, nType:%d", szIP, szRealIP, rSocketPort, nType);
			pSocket->SendTo((const char*)Sdata, strlen(Sdata), rSocketPort1, (const char*)rSocketAddress1);
		}
	}

	if(dlg != NULL) dlg->PrintListProc((LPSTR)szLog);
}



int CTrsSocketServerApp::IVRSMSSendProc(char szResult[50][1024], char* szLogMsg)
{
		
	DWORD	dwIndex = 0;
	int		nType = 0, nDbRet = FALSE;
	char	szIP[21] = "", szDate[21] = "", szFrom[31]="", szTo[31] = "", szMsg[81] = "", szSubject[51] = "", szUser[101] = "";
	char	szLog[1024] = "";
	char	szTemplateCd[128] = "";
	CTrsSocketServerDlg* dlg = (CTrsSocketServerDlg*)m_pMainWnd;
	CString	strSql;
	

	SYSTEMTIME	st;
	GetLocalTime(&st);
	
	dwIndex =	atol(szResult[0]);
	nType =		atoi(szResult[1]);
	strcpy(szIP,	 szResult[2]);
	strcpy(szMsg,	 szResult[3]);
	strcpy(szFrom,	 szResult[4]);
	strcpy(szTo,	 szResult[5]);
	
	sprintf(szDate, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf(szUser, "%s^%s", "Onpia", szTo);

	switch(nType){
	case SEND_TYPE_IVR_GPS:
		strcpy(szSubject, "GPS위치");
		break;
	case SEND_TYPE_IVR_NORMAL:
	default:
		strcpy(szSubject, "IVR메세지");
		break;
	}
	
	if(strlen(szMsg) > 0){
		

		//명확한 메시지 내용이 아니므로 알림톡 제외

		strSql.Format("INSERT INTO SDK_SMS_SEND (USER_ID, SUBJECT, SMS_MSG, NOW_DATE, SEND_DATE, CALLBACK, DEST_COUNT, DEST_INFO) VALUES ('onpia', '%s', '%s', '%s', '%s', '%s', %d, '%s')", 
			szSubject, szMsg, szDate, szDate, szFrom, 1, szUser);
		nDbRet = m_smsSql->ExecSql(strSql);

	}

	sprintf((char*)szLog, "%s, DB:%d, Value('onpia', '%s', '%s', '%s', '%s', '%s', %d, '%s')", szLogMsg, nDbRet, szSubject, szMsg, szDate, szDate, szFrom, 1, szUser);
	if(dlg != NULL) dlg->PrintListProc((LPSTR)szLog);
	return FALSE;
}



void CTrsSocketServerApp::GetUserString(char *szUsers, char *szIP, char *szRealIP, int nUserCnt, int nUseType)
{
	int j;
	CString strSql;
	char szTmpUser[1024] = "";
	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();

	for(j = 0; j < nUserCnt; j++){


//		DbgOut("GetUserString ip:%s, rip:%s, cnt:%d, type:%d ,j:%d, use:%s, name:%s, phone:%s, szTmpUser:%s \r\n", szIP, szRealIP, nUserCnt, nUseType, j, m_pSmsSiteUsers[j].sms_use, pApp->m_pSmsSiteUsers[j].sms_user_name, pApp->m_pSmsSiteUsers[j].sms_user_phone, szTmpUser);


		if(nUseType == USE_ALL){

			if(m_pSmsSiteUsers[j].sms_use[0] == 'Y' || m_pSmsSiteUsers[j].sms_use[0] == 'T'){

				if(strlen(szTmpUser) > 3) strcat(szTmpUser, "|");
				strcat(szTmpUser, pApp->m_pSmsSiteUsers[j].sms_user_name);
				strcat(szTmpUser, "^");
				strcat(szTmpUser, pApp->m_pSmsSiteUsers[j].sms_user_phone);
			}

		}else{ // USE_NORAML

			if(m_pSmsSiteUsers[j].sms_use[0] == 'Y'){

				if(strlen(szTmpUser) > 3) strcat(szTmpUser, "|");
				strcat(szTmpUser, pApp->m_pSmsSiteUsers[j].sms_user_name);
				strcat(szTmpUser, "^");
				strcat(szTmpUser, pApp->m_pSmsSiteUsers[j].sms_user_phone);
			}
		}
	}

	if(strlen(szTmpUser) > 3){
		sprintf(szUsers, "%s", szTmpUser);
	}
	else strcpy(szUsers, "");
}

BOOL CTrsSocketServerApp::GetUserInfoIdx(int usrIdx, char* szName, char* szNumber, int nUserCnt, int nUseType)
{
	BOOL bRet = FALSE;
	
	if (usrIdx >= nUserCnt) return bRet;
		
	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();


	if (nUseType == USE_ALL) {
		if (m_pSmsSiteUsers[usrIdx].sms_use[0] == 'Y' || m_pSmsSiteUsers[usrIdx].sms_use[0] == 'T') {
			if (strlen(pApp->m_pSmsSiteUsers[usrIdx].sms_user_name) > 0 && strlen(pApp->m_pSmsSiteUsers[usrIdx].sms_user_phone) > 0) {
				strcpy(szName, pApp->m_pSmsSiteUsers[usrIdx].sms_user_name);
				strcpy(szNumber, pApp->m_pSmsSiteUsers[usrIdx].sms_user_phone);
				bRet = TRUE;
			}
		}
	}
	else{	// USE_NORAML
		if (m_pSmsSiteUsers[usrIdx].sms_use[0] == 'Y') {
			if (strlen(pApp->m_pSmsSiteUsers[usrIdx].sms_user_name) > 0 && strlen(pApp->m_pSmsSiteUsers[usrIdx].sms_user_phone) > 0) {
				strcpy(szName, pApp->m_pSmsSiteUsers[usrIdx].sms_user_name);
				strcpy(szNumber, pApp->m_pSmsSiteUsers[usrIdx].sms_user_phone);
				bRet = TRUE;
			}
		}
	}

	return bRet;

}

void CTrsSocketServerApp::OnCheckLinkData()
{
	CC_LINKINFO *pCCLink = NULL;
	SYSTEMTIME st;
	char szLocalDay[21];
	CString strInitSql, strSql1, strSql2, strSql3, strSql4, strSql5, strSql6;
	CString strEMailSql;
	int i, nCnt;
	char	szUsers[1024], szMsg[1024], szDate[21];
	char	szTempCode[128] = "";
	int		nUserCnt;

	GetLocalTime(&st);
	sprintf(szDate, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	if (m_bInitLinkTimer == FALSE) {
		strInitSql.Format("UPDATE CallCenter_Link SET cc_update_time='%s' where cc_link_error='N' AND cc_use='Y'", szDate);
		m_dlgSql->ExecSql(strInitSql);
		m_bInitLinkTimer = TRUE;
	}

	GetAgoTimeProc(21, szLocalDay); // 10분 넘은것을 에러로 판단 한다.
	strSql1.Format("SELECT COUNT(cc_ip) FROM CallCenter_Link where cc_update_time<='%s' AND cc_link_error='N' AND cc_use='Y'", szLocalDay);
	nCnt = m_dlgSql->RowCount(strSql1);



//	DbgOut("OnCheckLinkData Cnt:%d, %s \r\n", nCnt, strSql1);

	if(nCnt > 0){
		pCCLink = new CC_LINKINFO[nCnt];
		memset(pCCLink, 0, sizeof(CC_LINKINFO)*nCnt);
		strSql2.Format("SELECT cc_index,cc_name,cc_ip, cc_real_ip, cc_site_name FROM CallCenter_Link where cc_update_time<='%s' AND cc_link_error='N' AND cc_use='Y'", szLocalDay);
		nCnt = m_dlgSql->GetCCLinkInfoProc(strSql2, pCCLink);

		for(i = 0; i < nCnt; i++){
			strSql3.Format("SELECT Count(sms_user_phone) FROM SITE_SMS_USER where (sms_use='Y' OR sms_use='T') AND (sms_site_ip='%s' OR sms_site_ip='ALL') AND (sms_site_real_ip='%s' OR sms_site_real_ip='ALL')",
				pCCLink[i].cc_ip, pCCLink[i].cc_real_ip);
			nUserCnt = m_dlgSql->RowCount(strSql3);





			char szSiteName[500] = "";
			if(strlen(pCCLink[i].cc_site_name) > 0){
				strcpy(szSiteName, pCCLink[i].cc_site_name);
			}else{
				strcpy(szSiteName, pCCLink[i].cc_name);
			}
			
			
			char szUseIP[200] = "";
			if(strcmp(pCCLink[i].cc_ip, pCCLink[i].cc_real_ip) == FALSE){
				strcpy(szUseIP, pCCLink[i].cc_ip);
			}else{
				sprintf(szUseIP, "%s(%s)", pCCLink[i].cc_real_ip,  pCCLink[i].cc_ip);
			}
			
			sprintf(szMsg, "%s-%s에서 링크데이터가 수신되고 있지 않습니다.", szSiteName, szUseIP);
			AlarmUser_Send(szMsg);				
			AlarmWebTh_Push(szMsg);




			if(nUserCnt > 0){
				strSql4.Format("SELECT sms_site_ip,sms_user_name,sms_user_phone, sms_site_name, sms_site_real_ip, sms_use FROM SITE_SMS_USER where (sms_use='Y' OR sms_use='T') AND (sms_site_ip='%s' OR sms_site_ip='ALL') AND (sms_site_real_ip='%s' OR sms_site_real_ip='ALL')",
					pCCLink[i].cc_ip, pCCLink[i].cc_real_ip);
				m_dlgSql->GetSmsSiteUsersProc(strSql4, nUserCnt);

				GetUserString(szUsers, pCCLink[i].cc_ip, pCCLink[i].cc_real_ip, nUserCnt, USE_ALL);
				
				
				
				
#ifdef _ATALK
			//알림톡으로 발송
				strcpy(szTempCode, (char*)"linkDataError001");
				char smsUsrName[50] = "", smsUsrNum[20] = "";
				BOOL bRet = FALSE;
				for (int usrIdx = 0; usrIdx < nUserCnt; usrIdx++) {
					bRet = GetUserInfoIdx(usrIdx, smsUsrName, smsUsrNum, nUserCnt, USE_ALL);

					if (bRet) {
						strSql5.Format("INSERT INTO \"MTS_ATALK_MSG\" (TRAN_PR, TRAN_SENDER_KEY, TRAN_TMPL_CD, TRAN_CALLBACK, TRAN_PHONE, TRAN_SUBJECT, TRAN_MSG, TRAN_DATE, TRAN_TYPE, TRAN_STATUS, TRAN_REPLACE_TYPE, TRAN_REPLACE_MSG) VALUES (nextval('mts_atalk_msg_seq'), 'fdd97f2559e5244dd3999e27683086b3d3f3eeba', '%s', '15889310', '%s', 'OnpiaATalk', '%s', now(), 5, '1', 'S', '%s')",
							szTempCode, smsUsrNum, szMsg, szMsg);	//템플릿코드, 번호, 메시지, 메시지
						if (m_pAtalkSql->ExecSql(strSql5) != FALSE) SqlErrLogging(strSql5.GetBuffer(0));
					}
					bRet = FALSE;
				}

#else
				strSql5.Format("INSERT INTO SDK_SMS_SEND (USER_ID, SUBJECT, SMS_MSG, NOW_DATE, SEND_DATE, CALLBACK, DEST_COUNT, DEST_INFO) VALUES ('onpia', '장애발생', '%s', '%s', '%s', '15889310', %d, '%s')",
						szMsg, szDate, szDate, nUserCnt, szUsers);
				m_smsSql->ExecSql(strSql5);
#endif
			}
			strSql6.Format("UPDATE CallCenter_Link SET cc_link_error='Y' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", pCCLink[i].cc_ip, pCCLink[i].cc_real_ip);
			m_dlgSql->ExecSql(strSql6);

			
			AlarmWebTh_EmailPush(m_dlgSql, SEND_TYPE_NET_ERROR, pCCLink[i].cc_ip, pCCLink[i].cc_real_ip, pCCLink[i].cc_name, "", ""); // 이메일전송 - 네트워크 오류 발생 

		}
		if(pCCLink != NULL){
			delete pCCLink;
			pCCLink = NULL;
		}
	}
}

void CTrsSocketServerApp::GetAgoTimeProc(int nAgoMinute, char *szLocalDay)
{
	time_t	localt;
	struct tm today_tm;
	time(&localt);
	localt -= nAgoMinute * 60; // nAgoMinute 전 날짜를 구한다.
	today_tm = *localtime(&localt);

	sprintf(szLocalDay, _T("%04d%02d%02d%02d%02d%02d"),
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday,
		today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);
}

void CTrsSocketServerApp::TcpLogging(char *format)
{
	FILE* fp;
	SYSTEMTIME st; 
	char szDay[12] = "", szLog[2001] = "", filename[MAX_PATH+1] = "", filenamepath[MAX_PATH+1] = ""; 

	GetLocalTime(&st); 
	sprintf(szLog, "%02d:%02d:%02d:%03d ",
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); 
	strcat(szLog, format); 
	sprintf(szDay, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay); 

	GetCurrentDirectory(sizeof(filenamepath), filenamepath); 
	sprintf((char*)filename, "%s\\log\\SOFT-%s.log", filenamepath, szDay);
	if( (fp = fopen(filename,"a")) < 0 ) return ; 

	if(fp == NULL) return; 

	fwrite(szLog, strlen(szLog), 1,fp); 
	fwrite("\r\n", 2, 1, fp);

	fclose(fp);
}


void CTrsSocketServerApp::AlarmLogging(char *format)
{
	FILE* fp;
	SYSTEMTIME st; 
	char szDay[12] = "", szLog[2001] = "", filename[MAX_PATH+1] = "", filenamepath[MAX_PATH+1] = ""; 
	
	GetLocalTime(&st); 
	sprintf(szLog, "%02d:%02d:%02d:%03d ",
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); 
	strcat(szLog, format); 
	sprintf(szDay, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay); 
	
	GetCurrentDirectory(sizeof(filenamepath), filenamepath); 
	sprintf((char*)filename, "%s\\log\\Alarm-%s.log", filenamepath, szDay);
	if( (fp = fopen(filename,"a")) < 0 ) return ; 
	
	if(fp == NULL) return; 
	
	fwrite(szLog, strlen(szLog), 1,fp); 
	fwrite("\r\n", 2, 1, fp);
	
	fclose(fp);
}




void CTrsSocketServerApp::SmsExcludeLogging(char *format)
{
	FILE* fp;
	SYSTEMTIME st; 
	char szDay[12] = "", szLog[2001] = "", filename[MAX_PATH+1] = "", filenamepath[MAX_PATH+1] = ""; 
	
	GetLocalTime(&st); 
	sprintf(szLog, "%02d:%02d:%02d:%03d ",
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); 
	strcat(szLog, format); 
	sprintf(szDay, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay); 
	
	GetCurrentDirectory(sizeof(filenamepath), filenamepath); 
	sprintf((char*)filename, "%s\\log\\SmsExclude-%s.log", filenamepath, szDay);
	if( (fp = fopen(filename,"a")) < 0 ) return ; 
	
	if(fp == NULL) return; 
	
	fwrite(szLog, strlen(szLog), 1,fp); 
	fwrite("\r\n", 2, 1, fp);
	
	fclose(fp);
}






void CTrsSocketServerApp::UdpLogging(char *format)
{
	FILE* fp;
	SYSTEMTIME st; 
	char szDay[12] = "", szLog[2001] = "", filename[MAX_PATH+1] = "", filenamepath[MAX_PATH+1] = ""; 

	GetLocalTime(&st); 
	sprintf(szLog, "%02d:%02d:%02d:%03d ",
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); 
	strcat(szLog, format); 
	sprintf(szDay, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay); 

	GetCurrentDirectory(sizeof(filenamepath), filenamepath); 
	sprintf((char*)filename, "%s\\log\\UDP-%s.log", filenamepath, szDay); 
	if( (fp = fopen(filename,"a")) < 0 ) return ; 

	if(fp == NULL) return; 

	fwrite(szLog, strlen(szLog), 1,fp); 
	fwrite("\r\n", 2, 1, fp);

	fclose(fp); 
} 



void CTrsSocketServerApp::SqlErrLogging(char *format)
{
	FILE* fp;
	SYSTEMTIME st; 
	char szDay[12] = "", szLog[2001] = "", filename[MAX_PATH+1] = "", filenamepath[MAX_PATH+1] = ""; 
	
	GetLocalTime(&st); 
	sprintf(szLog, "%02d:%02d:%02d:%03d ",
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); 
	strcat(szLog, format); 
	sprintf(szDay, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay); 
	
	GetCurrentDirectory(sizeof(filenamepath), filenamepath); 
	sprintf((char*)filename, "%s\\log\\SqlErr-%s.log", filenamepath, szDay); 
	if( (fp = fopen(filename,"a")) < 0 ) return ; 
	
	if(fp == NULL) return; 
	
	fwrite(szLog, strlen(szLog), 1,fp); 
	fwrite("\r\n", 2, 1, fp);
	
	fclose(fp); 
} 




void CTrsSocketServerApp::SoftLogging(char *format)
{
	FILE* fp;
	SYSTEMTIME st; 
	char szDay[12] = "", szLog[2001] = "", filename[MAX_PATH+1] = "", filenamepath[MAX_PATH+1] = ""; 

	GetLocalTime(&st); 
	sprintf(szLog, "%02d:%02d:%02d:%03d ",
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); 
	strcat(szLog, format); 
	sprintf(szDay, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay); 

	GetCurrentDirectory(sizeof(filenamepath), filenamepath); 
	sprintf((char*)filename, "%s\\log\\SOFT-%s.log", filenamepath, szDay); 
	if( (fp = fopen(filename,"a")) < 0 ) return ; 

	if(fp == NULL) return; 

	fwrite(szLog, strlen(szLog), 1,fp); 
	fwrite("\r\n", 2, 1, fp);

	fclose(fp); 
} 

void CTrsSocketServerApp::StrSplit(char *src, char dsttmp[50][1024], char cmp)
{

	int j, m, n;
	memset(&dsttmp[0][0], 0, 50*1024);
	for(j = 0, m = 0, n = 0; src[j] != 0; j++){
		if(src[j] == cmp){
			dsttmp[m][n] = '\0';
			n = 0;
			m++;
		}else{
			dsttmp[m][n++] = src[j];
		}
	}
	dsttmp[m][n] = '\0';
	/*
	int j, m, n;

	memset(&dsttmp[0][0], 0, 50*1024);
	for(j = 0, m = 0, n = 0; src[j] != 0; j++){
		if(src[j] == cmp){
			n = 0;
			m++;
			while(src[j] == cmp) j++;
		}
		dsttmp[m][n++] = src[j];
	}
	*/

}


int	CTrsSocketServerApp::PauseCheckProc(char* szIP, char* szRealIP)
{
	int nPause = FALSE;
	CString strSql;
	char szValue[21] = "", szToday[21] = "", szLog[1024] = "";
	SYSTEMTIME st; 
	GetLocalTime(&st); 
	sprintf(szToday, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay); 

	strSql.Format("SELECT cc_pause FROM CallCenter_Link where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", szIP, szRealIP);	
	m_dlgSql->GetValue(strSql, szValue);

	int nLen = strlen(szValue);

	
	if(nLen == 8 && (strcmp(szToday, szValue) <= 0)){ 
		
		sprintf(szLog, "PauseCheck: Today:%s, Pause:%s", szToday, szValue);
		UdpLogging(szLog);

		nPause = TRUE;
	}else if(nLen > 0){
		// 날짜가 잘못 설정 되었거나 이미 경과된 경우 삭제 한다 
		strSql.Format("UPDATE CallCenter_Link SET cc_pause='' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", szIP, szRealIP);
		m_dlgSql->ExecSql(strSql);

		sprintf(szLog, "PauseCheckDel Today:%s, Pause:%s", szToday, szValue);
		UdpLogging(szLog);
	}
	
	return nPause;
}


int	CTrsSocketServerApp::AlarmUser_Send(char* szMessage, int nCmd)
{
	char	szLog[1024] = "", Sdata[1024] = "";
	CClientSocket* pSocket = NULL;
	int nUserCount = 0;

	if(strlen(szMessage) > 1000){
		szMessage[1000] = '\0';
	}

	SYSTEMTIME st; 
	char szTime[21] = "";
	
	GetLocalTime(&st); 
	sprintf(szTime, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond); 
	
	sprintf(Sdata, "%c%d%c%s%c%s%c", 0x02, nCmd, 0xff, szMessage, 0xff, szTime, 0x03);

	CString strIP;
	UINT uPort;
	
	int ret = 0, k = 0, listCnt = m_connectionAlarmList.GetCount();
	POSITION pos = m_connectionAlarmList.GetHeadPosition();
	for(k = 0; k < listCnt; k++){
		pSocket = (CClientSocket*)m_connectionAlarmList.GetAt(pos);
		if(pSocket != NULL){
			ret = pSocket->Send(Sdata, strlen((LPCSTR)Sdata));

			pSocket->GetPeerName(strIP, uPort);
			
			sprintf((char*)szLog, "Send(%d) [%s:%d] %s", ret, strIP, uPort, Sdata);
			AlarmLogging(szLog);

			if(ret > 0)
				nUserCount ++;
		}
		m_connectionAlarmList.GetNext(pos);
	}

	if(nUserCount <= 0){
		sprintf((char*)szLog, "SendErr List:%d Msg:%s", listCnt, szMessage);
		AlarmLogging(szLog);
	}

	return nUserCount;

}



int	CTrsSocketServerApp::AlarmWebTh_Create()
{	
	if(m_pAlramWeb == NULL){
		m_pAlramWeb = new CAlramWeb;
		m_pAlramWeb->AlarmTh_Start();
	}
	return FALSE;
}


int	CTrsSocketServerApp::AlarmWebTh_Delete()
{	
	if(m_pAlramWeb == NULL){		
		m_pAlramWeb->AlarmTh_Terminate();
		delete m_pAlramWeb;
		m_pAlramWeb = NULL;
	}
	return FALSE;
}


int	CTrsSocketServerApp::AlarmWebTh_Push(char* szMsg, int nType)
{
	m_pAlramWeb->AlertList_Push(szMsg, nType);
	return FALSE;
}


int	CTrsSocketServerApp::AlarmWebTh_EmailPush(CDirectODBC* pDB, int nEventType, char* szIP, char* szRealIP, const char* szSiteName, const char* szProcName, const char* szContents)
{

	char szLog[2048] = "";

	sprintf(szLog, "AlarmWebTh_EmailPush1 type:%d, ip:%s, rip:%s, name:%s, proc:%s", nEventType, szIP, szRealIP, szSiteName, szProcName);
	AlarmLogging(szLog);

	if(pDB == NULL) return FALSE;

	if(strlen(szIP) ==0 ||  strlen(szRealIP) ==0) return FALSE;
	
	char szQuery[2048] = "", szSendMsg[2048] = "";
	int nEuIdx = 0;

	sprintf(szQuery, "email_site_ip='%s' AND email_site_real_ip='%s' AND  alarm_type=%d", szIP, szRealIP, nEventType);

	EMAIL_SITE_USERS *pEmailUser = m_dlgSql->GetEMailUserProc(szQuery);




	if(pEmailUser != NULL){

		sprintf(szLog, "AlarmWebTh_EmailPush getdata type:%d, ip:%s, rip:%s, name:%s, proc:%s", nEventType, szIP, szRealIP, szSiteName, szProcName);
		AlarmLogging(szLog);

		for(nEuIdx = 0; strlen(pEmailUser[nEuIdx].email_site_real_ip) > 0; nEuIdx ++){
			if( strlen(pEmailUser[nEuIdx].email_to) > 0){


				sprintf(szLog, "AlarmWebTh_EmailPush loop  type:%d, ip:%s, rip:%s, name:%s, proc:%s", nEventType, szIP, szRealIP, szSiteName, szProcName);
				AlarmLogging(szLog);


				int nFind = TRUE;

				if(nEventType == SEND_TYPE_NET_ERROR){   // 링크데이터 수신 되지 않았을 경우 
					if( strlen(pEmailUser[nEuIdx].email_msg1)  <= 0) sprintf(pEmailUser[nEuIdx].email_msg1,  "%s-%s에서 링크 데이터가 수신 되고 있지 않습니다. 인터넷 회선 점검 및 서버 점검 필요!", szSiteName, szIP);	
					if( strlen(pEmailUser[nEuIdx].email_title) <= 0) sprintf(pEmailUser[nEuIdx].email_title, "%s-%s의 서버 및 인터넷 회선 장애", szSiteName, szIP);

				}else if(nEventType == SEND_TYPE_NET_SUCC){ // 링크데이터가 수신 되고 있을경우 
					if( strlen(pEmailUser[nEuIdx].email_msg1)  <= 0) sprintf(pEmailUser[nEuIdx].email_msg1,  "%s-%s에서 링크 데이터가 수신 되고 있습니다", szSiteName, szIP);								
					if( strlen(pEmailUser[nEuIdx].email_title) <= 0) sprintf(pEmailUser[nEuIdx].email_title, "%s-%s의 네트워크 정상 연결", szSiteName, szIP);

				}else if(nEventType == SEND_TYPE_PROC_ERROR){ // 프로세서 장애 

					if(strlen(pEmailUser[nEuIdx].alarm_proc_name) > 0){
						if(strstr(szProcName, pEmailUser[nEuIdx].alarm_proc_name) == NULL){
							nFind = FALSE;
						}
					}
					if( strlen(pEmailUser[nEuIdx].email_msg1)  <= 0) strcpy(pEmailUser[nEuIdx].email_msg1,  szContents);
					if( strlen(pEmailUser[nEuIdx].email_title) <= 0) sprintf(pEmailUser[nEuIdx].email_title, "%s-%s의 %s 프로세스 장애 발생", szSiteName, szIP, szProcName);
					
				}else if(nEventType == SEND_TYPE_PROC_SUCC){  // 프로세서 복구 

					if(strlen(pEmailUser[nEuIdx].alarm_proc_name) > 0){
						if(strstr(szProcName, pEmailUser[nEuIdx].alarm_proc_name) == NULL){
							nFind = FALSE;
						}
					}
					if( strlen(pEmailUser[nEuIdx].email_msg1)  <= 0) strcpy(pEmailUser[nEuIdx].email_msg1,  szContents);
					if( strlen(pEmailUser[nEuIdx].email_title) <= 0) sprintf(pEmailUser[nEuIdx].email_title, "%s-%s의 %s 프로세스 장애 복구", szSiteName, szIP, szProcName);




				}else if(nEventType == SEND_TYPE_NAS_CON_FAIL){  // 녹취 FTP 백업 연결 실패 

					
					if( strlen(pEmailUser[nEuIdx].email_msg1)  <= 0) strcpy(pEmailUser[nEuIdx].email_msg1,  szContents);
					if( strlen(pEmailUser[nEuIdx].email_title) <= 0) sprintf(pEmailUser[nEuIdx].email_title, "%s-%s의 %s NAS 네트워크 장애", szSiteName, szIP, szProcName);

				}else if(nEventType == SEND_TYPE_NAS_CON_SUCC){  // 녹취 FTP 백업 연결 성공 

					
					if( strlen(pEmailUser[nEuIdx].email_msg1)  <= 0) strcpy(pEmailUser[nEuIdx].email_msg1,  szContents);
					if( strlen(pEmailUser[nEuIdx].email_title) <= 0) sprintf(pEmailUser[nEuIdx].email_title, "%s-%s의 %s NAS 네트워크 연결", szSiteName, szIP, szProcName);


				}else if(nEventType == SEND_TYPE_NAS_UPLOAD_FAIL){  // 녹취 FTP 백업 업로드 실패 

					
					if( strlen(pEmailUser[nEuIdx].email_msg1)  <= 0) strcpy(pEmailUser[nEuIdx].email_msg1,  szContents);
					if( strlen(pEmailUser[nEuIdx].email_title) <= 0) sprintf(pEmailUser[nEuIdx].email_title, "%s-%s의 %s  NAS 업로드 실패", szSiteName, szIP, szProcName);


				}else if(nEventType == SEND_TYPE_NAS_UPLOAD_SUCC){  // 녹취 FTP 백업 업로드 성공 

				
					if( strlen(pEmailUser[nEuIdx].email_msg1)  <= 0) strcpy(pEmailUser[nEuIdx].email_msg1,  szContents);
					if( strlen(pEmailUser[nEuIdx].email_title) <= 0) sprintf(pEmailUser[nEuIdx].email_title, "%s-%s의 %s NAS 업로드 성공", szSiteName, szIP, szProcName);
					

				}




				sprintf(pEmailUser[nEuIdx].email_msg, "%s%s%s", pEmailUser[nEuIdx].email_msg1, pEmailUser[nEuIdx].email_msg2, pEmailUser[nEuIdx].email_msg3);

				if(nFind == TRUE){

					sprintf(szLog, "AlarmWebTh_EmailPush push type:%d, ip:%s, rip:%s, name:%s, proc:%s", nEventType, szIP, szRealIP, szSiteName, szProcName);
					AlarmLogging(szLog);

					m_pAlramWeb->AlertList_EmailPush(pEmailUser[nEuIdx]);
				}

			}
		}
		free(pEmailUser);				
		pEmailUser = NULL;

	}else{

		sprintf(szLog, "AlarmWebTh_EmailPush nodata type:%d, ip:%s, rip:%s, name:%s, proc:%s", nEventType, szIP, szRealIP, szSiteName, szProcName);
		AlarmLogging(szLog);
	}
	return FALSE;
}


int	CTrsSocketServerApp::AlarmWebTh_EmailTest()
{

	
	EMAIL_SITE_USERS emailObj;
	memset(&emailObj, 0x00, sizeof(EMAIL_SITE_USERS));



	strcpy(emailObj.email_to, "jongkwi@onpia.co.kr,jk@onpia.co.kr");
	strcpy(emailObj.email_from,"test@onpia.co.kr");
	strcpy(emailObj.email_title,"테스트 제목");
	strcpy(emailObj.email_msg,"메세지");
	strcpy(emailObj.email_url, "ㅇㅇㅇㅇ");



	m_pAlramWeb->AlertList_EmailPush(emailObj);

	return FALSE;
}

