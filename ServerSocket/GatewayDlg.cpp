// GatewayDlg.cpp : implementation file
//

#include "stdafx.h"
#include <process.h>  
#include "ServerSocket.h"
#include "GatewayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int SOCK_TCP	= 0;
const int SOCK_UDP  = 1;

/////////////////////////////////////////////////////////////////////////////
// CGatewayDlg dialog


CGatewayDlg::CGatewayDlg(CWnd* pParent /*=NULL*/)

	: CDialog(CGatewayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGatewayDlg)
	m_sAfcPort = _T("502");
	m_sSvrPort = _T("3014");
	m_sAfcDevId = _T("1");
	//}}AFX_DATA_INIT

	m_nSockType = SOCK_TCP;
}


void CGatewayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGatewayDlg)
	DDX_Control(pDX, IDC_ENABLE_POLL, m_ctlEnablePoll);
	DDX_Control(pDX, IDC_BTN_FAULT, m_ctlCommonFault);
	DDX_Control(pDX, IDC_TXT_SVR_MSG, m_ctlSvrMsg);
	DDX_Control(pDX, IDC_TXT_AFC_MSG, m_ctlAfcMsg);
	DDX_Control(pDX, IDC_AFC_DEVID_INC, m_ctlAfcDevId);
	DDX_Control(pDX, IDC_AFC_MESSAGE, m_ctlAfcMsgList);
	DDX_Control(pDX, IDC_SVR_IP, m_ipSvrAddress);
	DDX_Control(pDX, IDC_SVR_MESSAGE, m_ctlSvrMsgList);
	DDX_Control(pDX, IDC_SVR_PORTINC, m_ctlSvrPort);
	DDX_Control(pDX, IDC_AFC_IP, m_ipAfcAddress);
	DDX_Control(pDX, IDC_AFC_PORTINC, m_ctlAfcPort);
	DDX_Text(pDX, IDC_AFC_PORT, m_sAfcPort);
	DDX_Text(pDX, IDC_SVR_PORT, m_sSvrPort);
	DDX_Text(pDX, IDC_AFC_DEVID, m_sAfcDevId);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGatewayDlg, CDialog)
	//{{AFX_MSG_MAP(CGatewayDlg)
	ON_BN_CLICKED(IDC_AFC_START, OnAfcStart)
	ON_BN_CLICKED(IDC_SVR_START, OnSvrStart)
	ON_BN_CLICKED(IDC_SVR_STOP, OnSvrStop)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_AFC_STOP, OnAfcStop)
	ON_BN_CLICKED(IDC_ENABLE_POLL, OnEnablePoll)
	ON_BN_CLICKED(IDC_AFC_SEND, OnAfcSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGatewayDlg message handlers

BOOL CGatewayDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		int nVirtKey = (int) pMsg->wParam;

		if (nVirtKey == VK_ESCAPE)
		{
			return TRUE;
		}

		if (nVirtKey == VK_RETURN && (GetFocus()->m_hWnd  == m_ctlSvrMsg.m_hWnd))
		{
			if (m_pCurServer->IsOpen())
			{
				//OnBtnSend();
			}
			return TRUE;
		}

		if (nVirtKey == VK_RETURN && (GetFocus()->m_hWnd  == m_ctlAfcMsg.m_hWnd))
		{
			if (m_ClientSocketManager.IsOpen())
			{
				OnAfcSend();
			}
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGatewayDlg::IsPollingEnabled()
{
	return m_bPollingEnabled;
}

BOOL CGatewayDlg::OnInitDialog() 
{
	ASSERT( GetDlgItem(IDC_AFC_START) != NULL );
	ASSERT( GetDlgItem(IDC_AFC_STOP) != NULL );
	ASSERT( GetDlgItem(IDC_AFC_SEND) != NULL );
	ASSERT( GetDlgItem(IDC_SVR_START) != NULL );
	ASSERT( GetDlgItem(IDC_SVR_STOP) != NULL );
	ASSERT( GetDlgItem(IDC_SVR_SEND) != NULL );

	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	m_ctlAfcPort.SetRange32(500, 599);
	m_ctlSvrPort.SetRange32(3000, 3099);
	m_ctlAfcDevId.SetRange32(1, 31);

	// Client Side
	GetDlgItem(IDC_AFC_SEND)->EnableWindow( FALSE );
	GetDlgItem(IDC_AFC_STOP)->EnableWindow( FALSE );

	CString strLocal;
	m_ClientSocketManager.GetLocalAddress( strLocal.GetBuffer(256), 256);
	strLocal.ReleaseBuffer();
	m_ipAfcAddress.SetWindowText( strLocal );

	m_ClientSocketManager.SetMessageWindow( &m_ctlAfcMsgList );
	m_ClientSocketManager.SetServerState( false );	// run as client
	m_ClientSocketManager.SetSmartAddressing( false );	// always send to server
	m_ClientSocketManager.SetPolling( false );

	m_pModbus = new CRemoteModbus();

	// Server Side
	GetDlgItem(IDC_SVR_SEND)->EnableWindow( FALSE );
	GetDlgItem(IDC_SVR_STOP)->EnableWindow( FALSE );

	for(int i=0; i<MAX_CONNECTION; i++)
	{
		m_SocketManager[i].SetMessageWindow( &m_ctlSvrMsgList );
		m_SocketManager[i].SetServerState( true );	// run as server
	}

	PickNextAvailable();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////
// StartServer : Start the server
bool CGatewayDlg::StartServer()
{
	bool bSuccess = false;
	CString strServerIp;
	m_ipSvrAddress.GetWindowText( strServerIp );
	if (m_pCurServer != NULL)
	{
		if (m_nSockType == SOCK_TCP)
		{
			// no smart addressing - we use connection oriented
			m_pCurServer->SetSmartAddressing( false );
			//bSuccess = m_pCurServer->CreateSocket( m_sSvrPort, AF_INET, SOCK_STREAM, 0); // TCP
			bSuccess = m_pCurServer->CreateSocketEx( strServerIp, m_sSvrPort, AF_INET, SOCK_STREAM, 0); // TCP
		}
		else
		{
			m_pCurServer->SetSmartAddressing( true );
			//bSuccess = m_pCurServer->CreateSocket( m_sSvrPort, AF_INET, SOCK_DGRAM, SO_BROADCAST); // UDP
			bSuccess = m_pCurServer->CreateSocketEx( strServerIp, m_sSvrPort, AF_INET, SOCK_DGRAM, SO_BROADCAST); // UDP
		}

		if (bSuccess && m_pCurServer->WatchComm())
		{
			GetDlgItem(IDC_SVR_SEND)->EnableWindow( TRUE );
			GetDlgItem(IDC_SVR_STOP)->EnableWindow( TRUE );
			NextDlgCtrl();
			GetDlgItem(IDC_SVR_START)->EnableWindow( FALSE );
			CString strServer, strAddr;
			m_pCurServer->GetLocalName( strServer.GetBuffer(256), 256);
			strServer.ReleaseBuffer();
			m_pCurServer->GetLocalAddress( strAddr.GetBuffer(256), 256);
			strAddr.ReleaseBuffer();
			CString strMsg = _T("Server: ") + strServer;
					strMsg += _T(", @Address: ") + strAddr;
					strMsg += _T(" is running on port ") + m_sSvrPort + CString("\r\n");
			m_pCurServer->AppendMessage( strMsg );
		}
	}

	return bSuccess;
}

void CGatewayDlg::PickNextAvailable()
{
	m_pCurServer = NULL;
	for(int i=0; i<MAX_CONNECTION; i++)
	{
		if (!m_SocketManager[i].IsOpen())
		{
			m_pCurServer = &m_SocketManager[i];
			break;
		}
	}
}

void CGatewayDlg::OnSvrStart() 
{
	UpdateData();
	
	StartServer();
}

void CGatewayDlg::OnSvrStop() 
{
	// Disconnect all clients
	for(int i=0; i<MAX_CONNECTION; i++)
	{
		m_SocketManager[i].StopComm();
	}

	if (!m_pCurServer->IsOpen())
	{
		GetDlgItem(IDC_SVR_START)->EnableWindow( TRUE );
		PrevDlgCtrl();
		GetDlgItem(IDC_SVR_STOP)->EnableWindow( FALSE );
		GetDlgItem(IDC_SVR_SEND)->EnableWindow( FALSE );
	}
}

void CGatewayDlg::OnDestroy() 
{
	m_ClientSocketManager.StopComm();

	for(int i=0; i<MAX_CONNECTION; i++)
	{
		m_SocketManager[i].StopComm();
	}

	CDialog::OnDestroy();	
}

/*void CGatewayDlg::GetAdapters()
{
	CString strTemp;
	PIP_ADAPTER_INFO pAdapterInfo;
	PULONG pAdapterCount;

	GetDlgItem(IDC_SVR_ADAPTER)->ResetContent();

	if (ERROR_SUCCESS == GetAdaptersAddresses(NULL, NULL, pAdapterInfo, pAdapterCount))
	{

	}
}*/

void CGatewayDlg::OnAfcStop() 
{
	CRemoteModbus* pRemoteModbus = (CRemoteModbus*)m_pModbus;
	// Disconnect socket
	//m_ClientSocketManager.StopComm();
	
	//if (!m_ClientSocketManager.IsOpen())
	if (pRemoteModbus->CloseConnection())
	{
		GetDlgItem(IDC_AFC_START)->EnableWindow( TRUE );
		PrevDlgCtrl();
		GetDlgItem(IDC_AFC_SEND)->EnableWindow( FALSE );
		GetDlgItem(IDC_AFC_STOP)->EnableWindow( FALSE );

		CString strMsg;
		strMsg = _T("\r\nConnection Abandonned\r\n");
		m_ClientSocketManager.AppendMessage( strMsg );
	}
}

void CGatewayDlg::OnAfcStart() 
{
	CRemoteModbus* pRemoteModbus = (CRemoteModbus*)m_pModbus;
	CString strMsg;
	CString strServer;

	UpdateData();

	m_ipAfcAddress.GetWindowText( strServer );
	//bool bSuccess;
	if (m_nSockType == SOCK_TCP)
	{
		//bSuccess = m_ClientSocketManager.ConnectTo( strServer, m_sAfcPort, AF_INET, SOCK_STREAM); // TCP
		pRemoteModbus->Timeout( 100 );
		pRemoteModbus->Host( strServer );
		pRemoteModbus->Port( (WORD)atol( m_sAfcPort ) );
		pRemoteModbus->ReconnectOnEveryMessage( TRUE );

		if (!pRemoteModbus->openConnection())
		{
			strMsg = _T("Failed to connect the server ") + strServer;
			strMsg += _T(" on port ") + m_sAfcPort + CString("\r\n");
			m_ClientSocketManager.AppendMessage( strMsg );

			delete m_pModbus;
			m_pModbus = new CRemoteModbus();

			return;
		}
	}
	else
	{
		//bSuccess = m_ClientSocketManager.ConnectTo( strServer, m_sAfcPort, AF_INET, SOCK_DGRAM); // UDP
	}

	//strMsg = _T("ConnectTo return: ") + ((bSuccess == true) ? CString("true") : CString("false")) + CString("\r\n");
	//m_ClientSocketManager.AppendMessage( strMsg );

	//if (bSuccess && m_ClientSocketManager.WatchComm())
	if (pRemoteModbus->IsActive())
	{

		GetDlgItem(IDC_AFC_SEND)->EnableWindow( TRUE );
		GetDlgItem(IDC_AFC_STOP)->EnableWindow( TRUE );
		NextDlgCtrl();
		GetDlgItem(IDC_AFC_START)->EnableWindow( FALSE );

		/*if (m_nSockType == SOCK_TCP)
		{*/
			strMsg = _T("Connection established with server: ") + strServer;
			strMsg += _T(" on port ") + m_sAfcPort + CString("\r\n");
			m_ClientSocketManager.GetPeerName( m_SockPeer );
		/*}
		else
		{
			SockAddrIn addrin;
			m_ClientSocketManager.GetSockName( addrin ); // just to get our current address
			LONG  uAddr = addrin.GetIPAddr();
			BYTE* sAddr = (BYTE*) &uAddr;
			int nPort = ntohs( addrin.GetPort() );
			CString strAddr;
			// Address is stored in network format...
			strAddr.Format(_T("IP: %u.%u.%u.%u, Port: %d"),
						(UINT)(sAddr[0]), (UINT)(sAddr[1]),
						(UINT)(sAddr[2]), (UINT)(sAddr[3]), nPort);
			strMsg = _T("Socket created: ") + strAddr + CString("\r\n");
			m_SockPeer.CreateFrom( strServer, m_sAfcPort, AF_INET);
		}*/

		m_ClientSocketManager.AppendMessage( strMsg );
	}
	/*else
	{
		strMsg = _T("Failed to connect the server ") + strServer;
		strMsg += _T(" on port ") + m_sAfcPort + CString("\r\n");
		m_ClientSocketManager.AppendMessage( strMsg );
	}*/


}

void CGatewayDlg::Run() 
{
	BYTE byDevice=0;
	WORD wValue=0;
	WORD wAddr=0;
	CModbus* pModbus=m_pModbus;
	WORD nError=CModbus::ERR_OK;
	CString strValue;
	CWaitCursor WaitCursor;
	CWordArray warValue;
	CShortArray aValues;
	WORD wQuant=1;
	CString str;

	if (NULL == pModbus) 
	{
		return;
	}

	m_ctlAfcDevId.GetWindowText(strValue);
	byDevice = (BYTE)atoi(strValue); // Slave address

	while (IsPollingEnabled())
	{
		wAddr = 0; // Heartbeat address = 0
		wValue = 10; // Heartbeat value = 10

		warValue.SetSize(1);
		warValue[0] = wValue;

		nError=pModbus->PresetMultipleRegisters(byDevice,wAddr,WORD(warValue.GetSize()),warValue);	
			
		if(nError!=CModbus::ERR_OK)
		{
			WaitCursor.Restore();
			str.Format("Error(1001): %s\r\n", pModbus->ErrorMessage(nError));
			m_ClientSocketManager.AppendMessage( str );
//			AfxMessageBox(pModbus->ErrorMessage(nError));
		}

		wAddr = 1; // Common Fault address = 0

		aValues.SetSize(wQuant);
		nError=pModbus->ReadInputRegisters(byDevice, wAddr, wQuant, aValues);

		if(nError!=CModbus::ERR_OK)
		{
			WaitCursor.Restore();
			str.Format("Error(1001): %s\r\n", pModbus->ErrorMessage(nError));
			m_ClientSocketManager.AppendMessage( str );
//			AfxMessageBox(pModbus->ErrorMessage(nError));
		}
		else 
		{
			str.Format("%d",HIBYTE(aValues[0]));
			m_ClientSocketManager.AppendMessage( str );

			if(HIBYTE(aValues[0]) == 1)
			{
				m_ctlCommonFault.SetCheck(1);
				m_ctlCommonFault.SetState(1);
			}
			else 
			{
				m_ctlCommonFault.SetCheck(0);
				m_ctlCommonFault.SetState(0);
			}

		}
	}
}



void CGatewayDlg::OnEnablePoll() 
{
	
	if (0 != m_ctlEnablePoll.GetCheck()) 
	{
		m_ctlEnablePoll.SetCheck(1);
		m_bPollingEnabled = true;

		HANDLE hThread;
		UINT uiThreadId = 0;
        hThread = (HANDLE)_beginthreadex(NULL,  // Security attributes
                                  0,    // stack
                    PollingThreadProc,   // Thread proc
                                this,   // Thread param
                    CREATE_SUSPENDED,   // creation mode
                        &uiThreadId); 

		if ( NULL != hThread)
        {
            //SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
            ResumeThread( hThread );
            m_hThread = hThread;
         
        }
	}
	else 
	{
		m_ctlEnablePoll.SetCheck(0);
		m_bPollingEnabled = false;
	}
	
}

void CGatewayDlg::OnAfcSend() 
{
	
	
}

///////////////////////////////////////////////////////////////////////////////
// PollingThreadProc
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//     Polling Thread function.  This function is the main thread for polling
//     - Asynchronous mode.
// PARAMETERS:
//     LPVOID pParam : Thread parameter - a CGatewayDlg pointer
// NOTES:
///////////////////////////////////////////////////////////////////////////////
UINT WINAPI CGatewayDlg::PollingThreadProc(LPVOID pParam)
{
    CGatewayDlg* pThis = reinterpret_cast<CGatewayDlg*>( pParam );
    _ASSERTE( pThis != NULL );

    pThis->Run();

    return 1L;
} // end PollingThreadProc
