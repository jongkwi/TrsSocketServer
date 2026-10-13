// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "TrsSocketServer.h"
#include "ClientSocket.h"
#include "TrsSocketServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket()
{
	memset(&m_Rdata, 0, sizeof(m_Rdata));
	memset(&szMailbox, 0, sizeof(szMailbox));
	m_bCon = FALSE;
}

CClientSocket::~CClientSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions

void CClientSocket::OnConnect(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	SYSTEMTIME	st;
	CTrsSocketServerDlg		*dlg;
	int		cnt = 0, nPort = 0;
	char	szLog[101] = "", szPort[8] = "";
	CString strAddress;
	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();
	if(pApp->m_pMainWnd == NULL) return;
	GetLocalTime(&st);

	dlg = (CTrsSocketServerDlg*)pApp->m_pMainWnd;

	if(pApp->m_pLinkSocket == this){
		KillTimer(pApp->m_pMainWnd->m_hWnd, RECORD_TIMER);
		if(nErrorCode == 0){
			m_bCon = TRUE;
			sprintf((char*)szLog, "LINK Server Connect Success");

			pApp->m_pLinkSocket->GetPeerName(strAddress, (unsigned int&)nPort);
			itoa(nPort, (char*)szPort, 10);
			cnt = dlg->m_ConIpList.GetItemCount();
			dlg->m_ConIpList.InsertItem(cnt, (LPCSTR)strAddress);
			dlg->m_ConIpList.SetItemText(cnt, 1, (LPCSTR)strAddress);
			dlg->m_ConIpList.SetItemText(cnt, 2, (LPCSTR)szPort);
		}
		else{
			sprintf((char*)szLog, "LINK Server Connect Fail");
			SetTimer(pApp->m_pMainWnd->m_hWnd, RECORD_TIMER, 10000, NULL);
		}
		dlg->PrintListProc((LPSTR)szLog);
	}

	CAsyncSocket::OnConnect(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();
	pApp->CloseSocket(this);
	CAsyncSocket::OnClose(nErrorCode);
}

void CClientSocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();
	pApp->ReadMsgProc(this);
	CAsyncSocket::OnReceive(nErrorCode);
}

