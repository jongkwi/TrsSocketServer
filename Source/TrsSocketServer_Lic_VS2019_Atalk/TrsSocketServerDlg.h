// TrsSocketServerDlg.h : header file
//

#if !defined(AFX_TRSSOCKETSERVERDLG_H__3E532BA8_C482_4A86_87CD_1A2EC85A9A95__INCLUDED_)
#define AFX_TRSSOCKETSERVERDLG_H__3E532BA8_C482_4A86_87CD_1A2EC85A9A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SystemTray.h"
/////////////////////////////////////////////////////////////////////////////
// CTrsSocketServerDlg dialog
class CTrsSocketServerDlg : public CDialog
{
// Construction
public: 
	CTrsSocketServerDlg(CWnd* pParent = NULL);	// standard constructor

	void	PrintListProc(char szData[]);
	void	TimeSyncProc();

	CSystemTray m_SystemTray;
	BOOL		m_bTray;
// Dialog Data
	//{{AFX_DATA(CTrsSocketServerDlg)
	enum { IDD = IDD_TRSSOCKETSERVER_DIALOG };
	CButton	m_btDisScrollStop;
	CListBox	m_TrsRecList;
	CListCtrl	m_ConIpList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrsSocketServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTrsSocketServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAppExit();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTrayClose();
	afx_msg void OnTrayOpen();
	afx_msg void OnDblclkTrsRecordList();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnSelchangeTrsRecordList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRSSOCKETSERVERDLG_H__3E532BA8_C482_4A86_87CD_1A2EC85A9A95__INCLUDED_)
