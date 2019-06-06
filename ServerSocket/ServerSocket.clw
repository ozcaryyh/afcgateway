; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGatewayDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "serversocket.h"
LastPage=0

ClassCount=5
Class1=CClientSocketDlg
Class2=CServerSocketApp
Class3=CAboutDlg
Class4=CServerSocketDlg

ResourceCount=4
Resource1=IDD_CLIENTSOCKET_DIALOG
Resource2=IDD_SERVERSOCKET_DIALOG
Resource3=IDD_ABOUTBOX
Class5=CGatewayDlg
Resource4=IDD_GATEWAY_DIALOG

[CLS:CClientSocketDlg]
Type=0
BaseClass=CDialog
HeaderFile=ClientSocketDlg.h
ImplementationFile=ClientSocketDlg.cpp
LastObject=CClientSocketDlg

[CLS:CServerSocketApp]
Type=0
BaseClass=CWinApp
HeaderFile=ServerSocket.h
ImplementationFile=ServerSocket.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=ServerSocketDlg.cpp
ImplementationFile=ServerSocketDlg.cpp
LastObject=CAboutDlg

[CLS:CServerSocketDlg]
Type=0
BaseClass=CDialog
HeaderFile=ServerSocketDlg.h
ImplementationFile=ServerSocketDlg.cpp
LastObject=CServerSocketDlg

[DLG:IDD_CLIENTSOCKET_DIALOG]
Type=1
Class=CClientSocketDlg
ControlCount=17
Control1=IDC_STATIC,static,1342308352
Control2=IDC_SVR_PORT,edit,1350631552
Control3=IDC_SVR_PORTINC,msctls_updown32,1342177458
Control4=IDC_STATIC,static,1342308352
Control5=IDC_IPADDRESS,SysIPAddress32,1342242816
Control6=IDC_STATIC,button,1342177287
Control7=IDC_TCP,button,1342308361
Control8=IDC_UDP,button,1342177289
Control9=IDC_STATIC,static,1342177296
Control10=IDC_TXT_MESSAGE,edit,1342242944
Control11=IDC_BTN_SEND,button,1342242816
Control12=IDC_MESSAGE_LIST,edit,1345388740
Control13=IDC_BTN_START,button,1342242816
Control14=IDC_BTN_STOP,button,1342242816
Control15=IDOK,button,1342242817
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SERVERSOCKET_DIALOG]
Type=1
Class=CServerSocketDlg
ControlCount=15
Control1=IDC_STATIC,static,1342308352
Control2=IDC_SVR_PORT,edit,1350631552
Control3=IDC_SVR_PORTINC,msctls_updown32,1342177458
Control4=IDC_STATIC,button,1342177287
Control5=IDC_TCP,button,1342308361
Control6=IDC_UDP,button,1342177289
Control7=IDC_STATIC,static,1342177296
Control8=IDC_TXT_MESSAGE,edit,1342242944
Control9=IDC_BTN_SEND,button,1342242816
Control10=IDC_MESSAGE_LIST,edit,1345388740
Control11=IDC_BTN_START,button,1342242816
Control12=IDC_BTN_STOP,button,1342242816
Control13=IDOK,button,1342242817
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352

[DLG:IDD_GATEWAY_DIALOG]
Type=1
Class=CGatewayDlg
ControlCount=36
Control1=IDC_STATIC,static,1342308352
Control2=IDC_SVR_PORT,edit,1350631552
Control3=IDC_SVR_PORTINC,msctls_updown32,1342177458
Control4=IDC_STATIC,static,1342177296
Control5=IDC_TXT_SVR_MSG,edit,1342242944
Control6=IDC_SVR_SEND,button,1342242816
Control7=IDC_SVR_MESSAGE,edit,1345388740
Control8=IDC_SVR_START,button,1342242816
Control9=IDC_SVR_STOP,button,1342242816
Control10=IDOK,button,1342242817
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,static,1342308352
Control16=IDC_AFC_PORT,edit,1350631552
Control17=IDC_AFC_PORTINC,msctls_updown32,1342177458
Control18=IDC_STATIC,static,1342308352
Control19=IDC_AFC_IP,SysIPAddress32,1342242816
Control20=IDC_STATIC,static,1342177296
Control21=IDC_TXT_AFC_MSG,edit,1342242944
Control22=IDC_AFC_SEND,button,1342242816
Control23=IDC_AFC_MESSAGE,edit,1345388740
Control24=IDC_AFC_START,button,1342242816
Control25=IDC_AFC_STOP,button,1342242816
Control26=IDC_STATIC,static,1342308352
Control27=IDC_STATIC,static,1342308352
Control28=IDC_STATIC,static,1342308352
Control29=IDC_SVR_ADAPTER,combobox,1344340226
Control30=IDC_SVR_IP,SysIPAddress32,1342242816
Control31=IDC_STATIC,static,1342308352
Control32=IDC_ENABLE_POLL,button,1342242819
Control33=IDC_AFC_DEVID,edit,1350631552
Control34=IDC_AFC_DEVID_INC,msctls_updown32,1342177458
Control35=IDC_BTN_FAULT,button,1342242816
Control36=IDC_CBO_STN_MODE,combobox,1344340226

[CLS:CGatewayDlg]
Type=0
HeaderFile=GatewayDlg.h
ImplementationFile=GatewayDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CBO_STN_MODE
VirtualFilter=dWC

