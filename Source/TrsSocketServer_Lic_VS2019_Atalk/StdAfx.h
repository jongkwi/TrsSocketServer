// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E411320F_2EAC_43E6_85C0_C3F18AC9989A__INCLUDED_)
#define AFX_STDAFX_H__E411320F_2EAC_43E6_85C0_C3F18AC9989A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#define WM_MY_NOTIFY			WM_USER+100

#define IPCS_PORT			6321
#define UDP_PORT			5363
#define ALARM_PORT			6770

#define MAX_REC_SOCKET	11

#define TRS_INI_PATH			_T(".\\TRSSocket.ini")

#define START_TIMER		100
#define LINK_TIMER		101
#define RECORD_TIMER	200
#define TIME_SYNC_TIMER	500

#define	MAXLENGTH		500 


#define ATIA_REC_CONNECT	1	//≥Ï√Î ø¨∞· (LINKSERVER∑Œ ¡§∫∏ ∫∏≥ø)
#define ATIA_REC_CHANGE		2	//≥Ï√Î ø¨∞· (LINKSERVER∑Œ ¡§∫∏ ∫∏≥ø)
#define ATIA_REC_DISCON		3	//≥Ï√Î ¿¸»≠∏¶ ≤˜æ˙¿ª∞ÊøÏ(LINKSERVER∑Œ ¡§∫∏ ∫∏≥ø)

#include "DbgOut_S.h"

#include <ODBCINST.H>		//MS SQL library
#include <sql.h>			//ODBC library
#include <sqlext.h>			//ODBC library

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E411320F_2EAC_43E6_85C0_C3F18AC9989A__INCLUDED_)
