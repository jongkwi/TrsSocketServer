#if !defined(AFX_UDPSOCKET_H__B224320D_FAB9_45BF_BB7C_CAA77EA4B3E1__INCLUDED_)
#define AFX_UDPSOCKET_H__B224320D_FAB9_45BF_BB7C_CAA77EA4B3E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UdpSocket.h : header file
//



static enum
{
	SEND_TYPE_PROC_ERROR,
	SEND_TYPE_PROC_SUCC,
	SEND_TYPE_CPU_ERROR,
	SEND_TYPE_CPU_SUCC,
	SEND_TYPE_DISK_ERROR,
	SEND_TYPE_DISK_SUCC,
	SEND_TYPE_MEM_ERROR,
	SEND_TYPE_MEM_SUCC,
	SEND_TYPE_SOCK_CON_ERROR,
	SEND_TYPE_SOCK_CON_SUCC,
	SEND_TYPE_SOCK_CHECK_ERROR,
	SEND_TYPE_SOCK_CLOSE_ERROR,
	SEND_TYPE_ICMP_ERROR,
	SEND_TYPE_ICMP_SUCC,
	SEND_TYPE_USER_MSG,
	SEND_TYPE_DATA_ERROR,
	SEND_TYPE_LINK_CHECK = 100,
	SEND_TYPE_IVR_GPS = 200,
	SEND_TYPE_IVR_NORMAL,
	SEND_TYPE_NAS_CON_FAIL=301,	// 녹취 FTP 백업 연결 실패 
	SEND_TYPE_NAS_CON_SUCC,		// 녹취 FTP 백업 연결 성공 
	SEND_TYPE_NAS_UPLOAD_FAIL,  // 녹취 FTP 백업 업로드 실패 
	SEND_TYPE_NAS_UPLOAD_SUCC,	// 녹취 FTP 백업 업로드 성공 
	SEND_TYPE_BACKUP_MESSAGE,	// 백업 기타 메세지 
	SEND_TYPE_BACKUP_DISKFULL,	// 백업 장치 디스크 용량 가득
	SEND_TYPE_FILE_REMOVE,		// 녹취 사용량 초과 삭제 
	SEND_TYPE_DISK_FULL,		// 서버 디스크 용량 가득 
	SEND_TYPE_NET_ERROR	= 401,	// 네트워크 오류 발생시 
	SEND_TYPE_NET_SUCC	= 402,	// 네트워크 연결시 
	SEND_TYPE_ETC
} warning_err_type;

/////////////////////////////////////////////////////////////////////////////
// CUdpSocket command target

class CUdpSocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CUdpSocket();
	virtual ~CUdpSocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUdpSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CUdpSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UDPSOCKET_H__B224320D_FAB9_45BF_BB7C_CAA77EA4B3E1__INCLUDED_)
