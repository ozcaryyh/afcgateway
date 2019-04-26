// RemModbus.cpp: implementation of the CRemoteModbus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "mbu.h"
#include "RemoteModbus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CRemoteModbus,CModbus)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRemoteModbus::CRemoteModbus()
{
	CSockAddr saEmpty("www.empty.com",502); //Put Invalid Address
	m_saClient=saEmpty;
	m_saServer=saEmpty;
	m_wPort=502;
	m_bReconnectOnEveryMessage=FALSE;

	m_abyRespBuffer.SetSize(INPUT_RESP_BUFFER_SIZE);
	m_abyQueryBuffer.SetSize(OUTPUT_QUERY_BUFFER_SIZE);
	
	//Modbus Comm errors 
	m_byExceptionOffset=DEFAULT_COMM_EXCEPTION_OFFSET;
	m_bComExceptionEnabled=TRUE;
	
	//Default Time-Out 
	Timeout(10000);

}

CRemoteModbus::~CRemoteModbus()
{
	CloseConnection();
}


BOOL CRemoteModbus::openConnection(CString sHost, WORD nPort, DWORD dwTimeOut)
{
    
	//CSockAddr m_saClient;//Address of this object 
//	CBlockingSocket m_sClient;//Client Socket

	if (sHost.IsEmpty()) {
		sHost=m_sHost;
	}

	if (0==nPort) {
		nPort=m_wPort;
	}


	CloseConnection();

	Lock();

	CSockAddr saServer(sHost,nPort); 

	if (0==dwTimeOut) {
		dwTimeOut=m_dwTimeOut;
	}

	m_dwTimeOut=dwTimeOut;
	m_sHost=sHost;
	m_wPort=nPort;


	try {

		m_sClient.Create();
		if (INADDR_NONE==saServer.IPAddr()) {
			saServer=CBlockingSocket::GetHostByName(sHost,nPort);
		}
        m_sClient.Connect(saServer);
        m_sClient.GetSockAddr(m_saClient);
		m_saServer=saServer;
		Unlock();
		return TRUE;
	}
	catch (CBlockingSocketException* e) {
		
		e->Delete();
		m_sClient.Cleanup();
		Unlock();
		return FALSE;
	
	}
}

BOOL CRemoteModbus::CloseConnection(){
	
	
	CSockAddr saEmpty(m_sHost,m_wPort);
	
	Lock();

	if (!IsActive() ) {
		Unlock();
		return TRUE;
	}
	else {
		
		try {
			m_saClient=saEmpty;
			m_saServer=saEmpty;
			m_sClient.Close();
			Unlock();
			return TRUE;
		}
		catch (CBlockingSocketException* e) {
			
			e->Delete();
			m_sClient.Cleanup();
			Unlock();
			return FALSE;
			
		}
		
	}
	
}

BOOL CRemoteModbus::IsActive() {
	return (INADDR_NONE!=m_saServer.IPAddr());
}

WORD CRemoteModbus::TxRxMessage(CByteArray& abyQuery, //modbus query without CRC
				  WORD wLengthQuery, //QueryLength without CRC
				  CByteArray& abyResponse, //modbus Response without CRC
				  WORD wLengthResponse, //wLengthResponse Response without CRC
				  WORD* pwNumOfBytesRead){

	if (m_bReconnectOnEveryMessage) {
		openConnection(m_sHost,m_wPort,m_dwTimeOut);
	}

	return TxRxMBAP(abyQuery,wLengthQuery,abyResponse,wLengthResponse);

}


WORD CRemoteModbus::TxRxMBAP(CByteArray& abyQuery, //modbus query without CRC
								WORD wLengthQuery, //QueryLength without CRC
								CByteArray& abyResponse, //modbus Response without CRC
								WORD wLengthResponse //wLengthResponse Response without CRC
								){ 
	
	WORD wBytesSent;
	WORD wBytesRecived;
	WORD i;
	char* pcRespBuffer=NULL;
	WORD wUnit_Id=abyQuery[0];
	WORD wErrorCode;

	m_PerfCounter.Start();
	//Fill response 
	if (abyResponse.GetSize()<wLengthResponse) {
		abyResponse.SetSize(wLengthResponse);
		::ZeroMemory(abyResponse.GetData(),abyResponse.GetSize());
	}
	
	//lock com device  
	if (!Lock()) {
		TRACE("Lock Failed");
		m_PerfCounter.End();
		return(ERR_LOCK_TIME_OUT);
	}
	
	if (!IsActive()){
		TRACE("Not Connected"); 
		Unlock();
		m_PerfCounter.End();
		return(ERR_SOCK_NOT_CONNECTED);
	}
	
	::ZeroMemory(m_abyQueryBuffer.GetData(),m_abyQueryBuffer.GetSize());
	::ZeroMemory(m_abyRespBuffer.GetData(),m_abyRespBuffer.GetSize());
	
	if ((wLengthQuery>m_abyQueryBuffer.GetSize())||(wLengthResponse>m_abyRespBuffer.GetSize())) {
		
		TRACE("Buffer Overflow"); 
		Unlock();
		m_PerfCounter.End();
		return(ERR_BUFFER_OVERFLOW);
	}
	//Add Server Header Query Length and Response lenght  
	m_abyQueryBuffer[0]=HIBYTE(TRANSAC_ID);
	m_abyQueryBuffer[1]=LOBYTE(TRANSAC_ID);
	m_abyQueryBuffer[2]=HIBYTE(MODBUS_ENCODING);
	m_abyQueryBuffer[3]=LOBYTE(MODBUS_ENCODING);
	m_abyQueryBuffer[4]=HIBYTE(wLengthQuery);
	m_abyQueryBuffer[5]=LOBYTE(wLengthQuery);
	
	//Add Query 
	m_abyQueryBuffer.InsertAt(MBAP_HEADER_LENGHT,&abyQuery);
	
	try{
		
		//write query 
		wBytesSent=m_sClient.Write((char*)m_abyQueryBuffer.GetData(),MBAP_HEADER_LENGHT+wLengthQuery,Timeout()/1000);
		
		//TRACE(" Tx:\n");
		//for (i=0;i<MBAP_HEADER_LENGHT+wLengthQuery;i++){
		//	TRACE(" %d \n",m_abyQueryBuffer[i]);
		//}

		if (wBytesSent!=(wLengthQuery+MBAP_HEADER_LENGHT)){
            Unlock();
			m_PerfCounter.End();
			return ERR_WR_SOCK_ERROR;
		}

		//read response 
		pcRespBuffer=(char*)m_abyRespBuffer.GetData();

		wBytesRecived=m_sClient.Receive((pcRespBuffer+0),MBAP_HEADER_LENGHT,Timeout()/1000);
		
		if (wBytesRecived<MBAP_HEADER_LENGHT) {
			Unlock();
			m_PerfCounter.End();
			return ERR_RD_SOCK_ERROR;
		}

		//TRACE("Rx Header:\n");
		//for (i=0;i<MBAP_HEADER_LENGHT;i++){
		//	TRACE(" %d \n",m_abyRespBuffer[i]);
		//}
		
		if ((m_abyRespBuffer[MBAP_HEADER_LENGHT-1]+MBAP_HEADER_LENGHT)>m_abyRespBuffer.GetSize()){
			return(ERR_BUFFER_OVERFLOW);
		}

		wBytesRecived=m_sClient.Receive((pcRespBuffer+MBAP_HEADER_LENGHT),m_abyRespBuffer[MBAP_HEADER_LENGHT-1],Timeout()/1000);
		
		m_PerfCounter.End();

		if (wBytesRecived<m_abyRespBuffer[MBAP_HEADER_LENGHT-1]) {
			Unlock();
			return ERR_RD_SOCK_ERROR;
		}

	//	TRACE(" Header + Message Rx:\n");
	//	for (i=0;i<MBAP_HEADER_LENGHT+m_abyRespBuffer[MBAP_HEADER_LENGHT-1];i++){
	//		TRACE(" %d \n",m_abyRespBuffer[i]);
	//	}

		if (m_abyRespBuffer[MBAP_HEADER_LENGHT+1]>0x80) {
			
		
			
			if  ((ComExceptionEnabled())&&(m_abyRespBuffer[MBAP_HEADER_LENGHT+2]>=CommExceptionOffset())){
				wErrorCode=BYTE(m_abyRespBuffer[MBAP_HEADER_LENGHT+2])-CommExceptionOffset();		
			}
			else {
				wErrorCode=(ERR_EXCPTION_CODE+m_abyRespBuffer[MBAP_HEADER_LENGHT+2]);
			}

			Unlock();
			return wErrorCode;
		}

		

		for (i=0;i<wLengthResponse;i++){
		    abyResponse[i]=m_abyRespBuffer[MBAP_HEADER_LENGHT+i];
			//TRACE("%d\n",abyResponse[i]);

		}
		
	}
	catch (CBlockingSocketException* e) {
		
		e->Delete();
		m_sClient.Cleanup();
		Unlock();
		m_PerfCounter.End();
		return ERR_SOCKET_ERROR;
		
	}
	
	Unlock();
	return(ERR_OK);
	
}

CString CRemoteModbus::ErrorMessage(WORD wErrorCode){

	CString sRetValue("");
    CString sAux;
	if(ERR_BUFFER_OVERFLOW==wErrorCode) {
		sRetValue=MSG_REMOTE_MODBUS_ERR_OVERFLOW;
	}
	else if(ERR_SOCKET_ERROR==wErrorCode){
		sRetValue=MSG_REMOTE_MODBUS_ERR_SOCKET_ERROR;
	}
	else if(ERR_WR_SOCK_ERROR==wErrorCode){
		sRetValue=MSG_REMOTE_MODBUS_ERR_WR_SOCKET_ERROR;

	}
	else if (ERR_RD_SOCK_ERROR==wErrorCode) {
		sRetValue=MSG_REMOTE_MODBUS_ERR_RD_SOCKET_ERROR;
	}
	else if (ERR_SOCK_NOT_CONNECTED==wErrorCode) {
		sRetValue=MSG_REMOTE_MODBUS_ERR_SOCKET_NOT_CONNECTED;
	}
	else {
	   sRetValue=CModbus::ErrorMessage(wErrorCode);
	}

	return sRetValue;
	
}


BYTE CRemoteModbus::CommExceptionOffset()
{
	return m_byExceptionOffset;
}

void CRemoteModbus::CommExceptionOffset(BYTE byOffset)
{
	m_byExceptionOffset=byOffset;
}

void CRemoteModbus::ComExceptionEnabled(BOOL bEnabled)
{
	m_bComExceptionEnabled=bEnabled;
}

BOOL CRemoteModbus::ComExceptionEnabled()
{
	return m_bComExceptionEnabled;
}


DWORD CRemoteModbus::Timeout()
{
	return m_dwTimeOut;
}

void CRemoteModbus::Timeout(DWORD dwTimeout)
{
	m_dwTimeOut=dwTimeout;
}


BOOL CRemoteModbus::ReconnectOnEveryMessage()
{
	return m_bReconnectOnEveryMessage;
}

void CRemoteModbus::ReconnectOnEveryMessage(BOOL bReconnect)
{
	m_bReconnectOnEveryMessage=bReconnect;
}

void CRemoteModbus::Host(LPCSTR szHost)
{
 m_sHost=szHost;
}

CString CRemoteModbus::Host()
{
	return m_sHost;
}

void CRemoteModbus::Port(WORD wPort)
{
	m_wPort =wPort;
}

WORD CRemoteModbus::Port()
{
	return m_wPort;

}

void CRemoteModbus::Serialize(CArchive & archive)
{
	// call base class function first
	  // base class is CObject in this case
	  CModbus::Serialize( archive );
	  

	  // now do the stuff for our specific class
	  if( archive.IsStoring() ){
		  archive	<< m_bReconnectOnEveryMessage 
			  << m_sHost
			  << m_wPort
			  << m_dwTimeOut
			  << m_bComExceptionEnabled
			  << m_byExceptionOffset;
      } 
	  else {
		  archive	>>m_bReconnectOnEveryMessage 
			  >> m_sHost
			  >> m_wPort
			  >> m_dwTimeOut
			  >> m_bComExceptionEnabled
			  >> m_byExceptionOffset;
      }

	  if (!openConnection()) {
		ASSERT(FALSE);	
	  }

}
