; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTrsSocketServerApp
LastTemplate=CAsyncSocket
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "trssocketserver.h"
LastPage=0

ClassCount=7
Class1=CClientSocket
Class2=CListenSocket
Class3=CSystemTray
Class4=CTrsSocketServerApp
Class5=CAboutDlg
Class6=CTrsSocketServerDlg

ResourceCount=3
Resource1=IDD_TRSSOCKETSERVER_DIALOG
Resource2=IDD_ABOUTBOX
Class7=CUdpSocket
Resource3=IDR_TRAY_MENU

[CLS:CClientSocket]
Type=0
BaseClass=CAsyncSocket
HeaderFile=ClientSocket.h
ImplementationFile=ClientSocket.cpp

[CLS:CListenSocket]
Type=0
BaseClass=CAsyncSocket
HeaderFile=ListenSocket.h
ImplementationFile=ListenSocket.cpp

[CLS:CSystemTray]
Type=0
BaseClass=CWnd
HeaderFile=SystemTray.h
ImplementationFile=SystemTray.cpp

[CLS:CTrsSocketServerApp]
Type=0
BaseClass=CWinApp
HeaderFile=TrsSocketServer.h
ImplementationFile=TrsSocketServer.cpp
LastObject=CTrsSocketServerApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=TrsSocketServerDlg.cpp
ImplementationFile=TrsSocketServerDlg.cpp

[CLS:CTrsSocketServerDlg]
Type=0
BaseClass=CDialog
HeaderFile=TrsSocketServerDlg.h
ImplementationFile=TrsSocketServerDlg.cpp
LastObject=CTrsSocketServerDlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_TRSSOCKETSERVER_DIALOG]
Type=1
Class=CTrsSocketServerDlg
ControlCount=7
Control1=IDC_TRS_RECORD_LIST,listbox,1352728833
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_CON_IP_LIST,SysListView32,1350631425
Control5=IDC_APP_EXIT,button,1342242816
Control6=IDC_TEST,button,1342242816
Control7=IDC_DISPLAY_SCROLL_STOP,button,1342242819

[MNU:IDR_TRAY_MENU]
Type=1
Class=?
Command1=ID_TRAY_OPEN
Command2=ID_TRAY_CLOSE
CommandCount=2

[CLS:CUdpSocket]
Type=0
HeaderFile=UdpSocket.h
ImplementationFile=UdpSocket.cpp
BaseClass=CAsyncSocket
Filter=N
LastObject=CUdpSocket
VirtualFilter=q

