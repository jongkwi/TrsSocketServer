// TrsSocketServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrsSocketServer.h"
#include "TrsSocketServerDlg.h"
#include "DirectODBC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrsSocketServerDlg dialog

CTrsSocketServerDlg::CTrsSocketServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrsSocketServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrsSocketServerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bTray = FALSE;
}

void CTrsSocketServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrsSocketServerDlg)
	DDX_Control(pDX, IDC_DISPLAY_SCROLL_STOP, m_btDisScrollStop);
	DDX_Control(pDX, IDC_TRS_RECORD_LIST, m_TrsRecList);
	DDX_Control(pDX, IDC_CON_IP_LIST, m_ConIpList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTrsSocketServerDlg, CDialog)
	//{{AFX_MSG_MAP(CTrsSocketServerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_APP_EXIT, OnAppExit)
	ON_WM_SIZE()
	ON_COMMAND(ID_TRAY_CLOSE, OnTrayClose)
	ON_COMMAND(ID_TRAY_OPEN, OnTrayOpen)
	ON_LBN_DBLCLK(IDC_TRS_RECORD_LIST, OnDblclkTrsRecordList)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_TRS_RECORD_LIST, &CTrsSocketServerDlg::OnLbnSelchangeTrsRecordList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrsSocketServerDlg message handlers

BOOL CTrsSocketServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
    m_ConIpList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ConIpList.InsertColumn(0, "BLANK", LVCFMT_LEFT, 0);
	m_ConIpList.InsertColumn(1, "Address", LVCFMT_LEFT, 145);
	m_ConIpList.InsertColumn(2, "Port", LVCFMT_LEFT, 40);

	HMENU XButton;
	XButton = ::GetSystemMenu(m_hWnd, FALSE);
//	RemoveMenu(XButton, 6, MF_BYPOSITION); 
	RemoveMenu(XButton, SC_CLOSE, MF_BYCOMMAND);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_SystemTray.Create(NULL, WM_MY_NOTIFY, _T("IPCS SOCKET SERVER"), m_hIcon, IDR_TRAY_MENU);	
	m_SystemTray.Enabled();

	
	SetWindowText("IPCS Socket Server");
	SetTimer(START_TIMER, 1000, NULL);
	SetTimer(LINK_TIMER, 5000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrsSocketServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrsSocketServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrsSocketServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTrsSocketServerDlg::OnTimer(UINT nIDEvent) 
{
	SYSTEMTIME	st;
	int nIndex = 0;
	char	szLog[101] = "";
	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();
	GetLocalTime(&st);
	int nRet = FALSE;

	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent){
	  case START_TIMER:
		KillTimer(nIDEvent);
		pApp->m_dlgSql = new CDirectODBC((unsigned char*)"ipcs_log", (unsigned char*)"lic", (unsigned char*)"lic8585");
		nRet = pApp->m_dlgSql->ConnectODBC();
//		DbgOut("dbconnect %d \r\n", nRet);
#ifdef _ATALK
		pApp->m_pAtalkSql = new CDirectODBC((unsigned char*)"onpia_check", (unsigned char*)"nasacti", (unsigned char*)"nasa8585");
		pApp->m_pAtalkSql->ConnectODBC();
#endif
		pApp->m_smsSql = new CDirectODBC((unsigned char*)"onpia_sms", (unsigned char*)"lic", (unsigned char*)"lic8585");
		pApp->m_smsSql->ConnectODBC();



		pApp->SmsExcludeLoadProc();

		pApp->CreateSocketProc();



		break;

	  case LINK_TIMER:
		pApp->OnCheckLinkData();
		SetTimer(LINK_TIMER, 5000, NULL);
		break;

	  case RECORD_TIMER:
		KillTimer(nIDEvent);
		sprintf((char*)szLog, "LINK SERVER CONNECT TRY(%d)", nIndex);
		PrintListProc(szLog);
		break;

	  case TIME_SYNC_TIMER:
		TimeSyncProc();
		break;

	  default:
		KillTimer(nIDEvent);
		break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CTrsSocketServerDlg::OnAppExit() 
{
	// TODO: Add your control notification handler code here
	if(MessageBox("이 프로그램이 종료되면 \n\n서버와 통신을 할 수 없습니다.!! \n\n종료 하시겠습니까?", "종 료", MB_OKCANCEL|MB_ICONEXCLAMATION) != TRUE) return;
	PostMessage(WM_CLOSE, 0L, 0L);
	m_SystemTray.UpdateData(FALSE);
}

void CTrsSocketServerDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if(nType){
		m_bTray = TRUE;
		ShowWindow(SW_HIDE);
	}
	// TODO: Add your message handler code here
}

void CTrsSocketServerDlg::OnTrayClose() 
{
	// TODO: Add your command handler code here
	if(MessageBox("이 프로그램이 종료되면 \n\n서버와 통신을 할 수 없습니다.!! \n\n종료 하시겠습니까?", "종 료", MB_OKCANCEL|MB_ICONEXCLAMATION) != TRUE) return;
	PostMessage(WM_CLOSE, 0L, 0L);
	m_SystemTray.UpdateData(FALSE);
}

void CTrsSocketServerDlg::OnTrayOpen() 
{
	// TODO: Add your command handler code here
	ShowWindow(SW_SHOWNORMAL);
	m_bTray = FALSE;
}

void CTrsSocketServerDlg::PrintListProc(char szData[]) 
{
	SYSTEMTIME st;
	char		szPrintData[1024] = "";

	GetLocalTime(&st);
	if(m_TrsRecList.GetCount() > 50000){
		m_TrsRecList.ResetContent();
	}
	sprintf((char*)szPrintData, "[%04d-%02d-%02d %02d:%02d:%02d] %s",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, szData);
	// TODO: Add your command handler code here

	m_TrsRecList.AddString(szPrintData);
	if(m_btDisScrollStop.GetCheck() == FALSE) m_TrsRecList.SetCurSel(m_TrsRecList.GetCount()-1);
}


void CTrsSocketServerDlg::OnDblclkTrsRecordList() 
{
	// TODO: Add your control notification handler code here
	m_TrsRecList.ResetContent();
}


void CTrsSocketServerDlg::TimeSyncProc() 
{
	CString m_strReceive;
	CString strIP;
    CSocket m_sock;
	char	send_buf[201] = "", recv_buf[201] = "";
	int		nResult;
	DWORD	dwErr;

	strIP = "192.168.0.1";
    m_sock.Create();

	nResult = m_sock.Connect(strIP, 13);

    if (nResult) {
        m_sock.Send(send_buf, strlen(send_buf));
        memset(recv_buf, 0x00, sizeof(recv_buf));
        Sleep(3000);
        m_sock.Receive(recv_buf, sizeof(recv_buf));
        m_strReceive = recv_buf;
    } else {
        dwErr = GetLastError();
		
    }
}



void CTrsSocketServerDlg::OnTest() 
{
	// TODO: Add your control notification handler code here

	CTrsSocketServerApp* pApp = (CTrsSocketServerApp*)AfxGetApp();
	
/*
	char	szUsers[1024] = "", szIP[21], szRealIP[21];
	strcpy(szUsers, "alram -sd-d-sdfsf테스트입니다");
	//pApp->AlarmUser_Send(szUsers);
	pApp->AlarmWebTh_Push(szUsers);

	return;

	strcpy(szIP, "203.239.190.28");
	strcpy(szRealIP, "192.168.1.3");
	pApp->GetUserString(szUsers, szIP, szRealIP, 0);

*/
	
	pApp->AlarmWebTh_EmailTest();

	/* DB 연결 테스트 주석 2022_03_29
	char szQuery[2049];

	sprintf(szQuery, "INSERT INTO onpia_reschk_log (reslog_idx, company_name, server_ip, real_ip, check_type) VALUES(nextval('onpia_reschk_log_idx_seq'), '알림테스트서버', '192.168.21.11', '192.168.21.11', 'TestServ')");
	
	if(pApp->m_pAtalkSql != NULL)
		pApp->m_pAtalkSql->ExecSql(szQuery);
	
	CString strSql;
	char szTempCode[128] = "";
	char szMsg[1024] = "윤준영테스트-192.168.21.11에서 onwatch 프로세스 장애가 발생 했습니다.";

	strcpy(szTempCode, (char*)"procError001");
	strSql.Format("INSERT INTO \"MTS_ATALK_MSG\" (TRAN_PR, TRAN_SENDER_KEY, TRAN_TMPL_CD, TRAN_CALLBACK, TRAN_PHONE, TRAN_SUBJECT, TRAN_MSG, TRAN_DATE, TRAN_TYPE, TRAN_STATUS, TRAN_REPLACE_TYPE, TRAN_REPLACE_MSG) VALUES (nextval('mts_atalk_msg_seq'), 'fdd97f2559e5244dd3999e27683086b3d3f3eeba', '%s', '15889310', '%s', 'OnpiaATalk', '%s', now(), 5, '1', 'S', '%s')",
		szTempCode,
		"01089648124",
		szMsg,
		szMsg);
	pApp->m_pAtalkSql->ExecSql(strSql);
	

	SYSTEMTIME st;
	char	szDate[21];
	char szLocalDay[21];
	CString strSql1, strResult;
	int nCnt = 0;


	GetLocalTime(&st);
	sprintf(szDate, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	pApp->GetAgoTimeProc(21, szLocalDay); // 10분 넘은것을 에러로 판단 한다.

	strSql1.Format("SELECT COUNT(cc_ip) FROM CallCenter_Link where cc_update_time<='%s' AND cc_link_error='N' AND cc_use='Y'", szLocalDay);
	nCnt = pApp->m_dlgSql->RowCount(strSql1);

	strResult.Format("nCnt : %d, szLocalDay : %s", nCnt, szLocalDay);
	MessageBox(strResult, "알림", MB_ICONHAND);
	

	//22-03-31 JY 테스트
	CString strTmp, strSql;
	char smsUsrName[50] = "", smsUsrNum[20] = "";
	BOOL bRet = FALSE;
	int nUserCnt;


	strSql.Format("SELECT Count(sms_user_phone) FROM SITE_SMS_USER where (sms_use='Y' OR sms_use='T') AND (sms_site_ip='211.192.89.43' OR sms_site_ip='ALL') AND (sms_site_real_ip='211.192.89.43' OR sms_site_real_ip='ALL')");
	nUserCnt = pApp->m_dlgSql->RowCount(strSql);

	if(nUserCnt <=0)
		MessageBox("nUserCnt ZeroValues", "테스트", MB_ICONEXCLAMATION);
	else {
		strSql.Format("SELECT sms_site_ip,sms_user_name,sms_user_phone, sms_site_name, sms_site_real_ip, sms_use FROM SITE_SMS_USER where (sms_use='Y' OR sms_use='T') AND (sms_site_ip='211.192.89.43' OR sms_site_ip='ALL') AND (sms_site_real_ip='211.192.89.43' OR sms_site_real_ip='ALL')");
		pApp->m_dlgSql->GetSmsSiteUsersProc(strSql, nUserCnt);
		bRet = pApp->GetUserInfoIdx(0, smsUsrName, smsUsrNum, 1, USE_ALL);

		strTmp.Format("bRet : %d, 이름 : %s, 번호 : %s", bRet, smsUsrName, smsUsrNum);

		//strTmp.Format("이름 : %s, 번호 : %s", smsUsrName, smsUsrNum);
		MessageBox(strTmp, "테스트", MB_ICONEXCLAMATION);
	
	}

	*/
	
	


}



void CTrsSocketServerDlg::OnLbnSelchangeTrsRecordList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
