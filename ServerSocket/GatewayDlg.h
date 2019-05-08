#if !defined(AFX_GATEWAYDLG_H__55B965DB_97B3_4CE6_AC56_F8A34CD8D82F__INCLUDED_)
#define AFX_GATEWAYDLG_H__55B965DB_97B3_4CE6_AC56_F8A34CD8D82F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GatewayDlg.h : header file
//
//#include <iphlpapi.h>
//#pragma comment(lib, "iphlpapi")

#include "SocketManager.h"
#include "RemoteModbus.h"

#define MAX_CONNECTION		10

/////////////////////////////////////////////////////////////////////////////
// CGatewayDlg dialog

class CGatewayDlg : public CDialog
{
// Construction
public:
	CGatewayDlg(CWnd* pParent = NULL);   // standard constructor
	int		m_nSockType;
	BOOL	m_bPollingEnabled;

// Dialog Data
	//{{AFX_DATA(CGatewayDlg)
	enum { IDD = IDD_GATEWAY_DIALOG };
	CButton	m_ctlEnablePoll;
	CButton	m_ctlCommonFault;
	CEdit	m_ctlSvrMsg;
	CEdit	m_ctlAfcMsg;
	CSpinButtonCtrl	m_ctlAfcDevId;
	CEdit	m_ctlAfcMsgList;
	CIPAddressCtrl	m_ipSvrAddress;
	CEdit	m_ctlSvrMsgList;
	CSpinButtonCtrl	m_ctlSvrPort;
	CIPAddressCtrl	m_ipAfcAddress;
	CSpinButtonCtrl	m_ctlAfcPort;
	CString	m_sAfcPort;
	CString	m_sSvrPort;
	CString	m_sAfcDevId;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGatewayDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HANDLE      m_hThread;      // Thread Comm handle

	CSocketManager m_SocketManager[MAX_CONNECTION];
	CSocketManager* m_pCurServer;
	CSocketManager m_ClientSocketManager;
	SockAddrIn		m_SockPeer;
	CModbus* m_pModbus;

	void PickNextAvailable();
	bool StartServer();
//	void GetAdapters();
	virtual void Run();
	BOOL IsPollingEnabled();

	// Generated message map functions
	//{{AFX_MSG(CGatewayDlg)
	afx_msg void OnAfcStart();
	virtual BOOL OnInitDialog();
	afx_msg void OnSvrStart();
	afx_msg void OnSvrStop();
	afx_msg void OnDestroy();
	afx_msg void OnAfcStop();
	afx_msg void OnEnablePoll();
	afx_msg void OnAfcSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static UINT WINAPI PollingThreadProc(LPVOID pParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATEWAYDLG_H__55B965DB_97B3_4CE6_AC56_F8A34CD8D82F__INCLUDED_)
