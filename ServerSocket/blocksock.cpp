// blocksock.cpp (CBlockingSocketException, CBlockingSocket, CHttpBlockingSocket)
#include "stdafx.h"
//#include "mbu.h"
#include "blocksock.h"

// Class CBlockingSocketException
IMPLEMENT_DYNAMIC(CBlockingSocketException, CException)

CBlockingSocketException::CBlockingSocketException(char* pchMessage)
{
	m_strMessage = pchMessage;
	m_nError = WSAGetLastError();
}

BOOL CBlockingSocketException::GetErrorMessage(LPTSTR lpstrError, UINT nMaxError,
		PUINT pnHelpContext /*= NULL*/)
{

	char text[200];
	if(m_nError == 0) {
		wsprintf(text, "%s error", (const char*) m_strMessage);
	}
	else {
		wsprintf(text, "%s error #%d", (const char*) m_strMessage, m_nError);
	}
	strncpy(lpstrError, text, nMaxError - 1);
	return TRUE;
}

// Class CBlockingSocket
IMPLEMENT_DYNAMIC(CBlockingSocket, CObject)

void CBlockingSocket::Cleanup()
{
	// doesn't throw an exception because it's called in a catch block
	if(m_hSocket == NULL) return;
	closesocket(m_hSocket);
	m_hSocket = NULL;
}

void CBlockingSocket::Create(int nType /* = SOCK_STREAM */)
{
	ASSERT(m_hSocket == NULL);
	if((m_hSocket = socket(AF_INET, nType, 0)) == INVALID_SOCKET) {
		throw new CBlockingSocketException("Create");
	}
}

void CBlockingSocket::Bind(LPCSOCKADDR psa)
{
	ASSERT(m_hSocket != NULL);
	if(bind(m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		throw new CBlockingSocketException("Bind");
	}
}

void CBlockingSocket::Listen()
{
	ASSERT(m_hSocket != NULL);
	if(listen(m_hSocket, 5) == SOCKET_ERROR) {
		throw new CBlockingSocketException("Listen");
	}
}

BOOL CBlockingSocket::Accept(CBlockingSocket& sConnect, LPSOCKADDR psa)
{
	ASSERT(m_hSocket != NULL);
	ASSERT(sConnect.m_hSocket == NULL);
	int nLengthAddr = sizeof(SOCKADDR);
	sConnect.m_hSocket = accept(m_hSocket, psa, &nLengthAddr);
	if(sConnect == INVALID_SOCKET) {
		// no exception if the listen was canceled
		if(WSAGetLastError() != WSAEINTR) {
			throw new CBlockingSocketException("Accept");
		}
		return FALSE;
	}
	return TRUE;
}

void CBlockingSocket::Close()
{
	if (m_hSocket == NULL) {
		TRACE("TRYING to close a null socket\n");
	}
	if(closesocket(m_hSocket) == SOCKET_ERROR) {
		// should be OK to close if closed already
		throw new CBlockingSocketException("Close");
	}
	m_hSocket = NULL;
}

void CBlockingSocket::Connect(LPCSOCKADDR psa)
{
	ASSERT(m_hSocket != NULL);
	// should timeout by itself
	if(connect(m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		throw new CBlockingSocketException("Connect");
	}
}

int CBlockingSocket::Write(const char* pch, const int nSize, const int nSecs)
{
	int nBytesSent = 0;
	int nBytesThisTime;
	const char* pch1 = pch;
	do {
		nBytesThisTime = Send(pch1, nSize - nBytesSent, nSecs);
		nBytesSent += nBytesThisTime;
		pch1 += nBytesThisTime;
	} while(nBytesSent < nSize);
	return nBytesSent;
}

int CBlockingSocket::Send(const char* pch, const int nSize, const int nSecs)
{
	//ASSERT(m_hSocket != NULL);
	// returned value will be less than nSize if client cancels the reading
	FD_SET fd = {1, m_hSocket};
	TIMEVAL tv = {nSecs, 0};
	if(select(0, NULL, &fd, NULL, &tv) == 0) {
		throw new CBlockingSocketException("Send timeout");
	}
	int nBytesSent;
	if((nBytesSent = send(m_hSocket, pch, nSize, 0)) == SOCKET_ERROR) {
		throw new CBlockingSocketException("Send");
	}
	return nBytesSent;
}

int CBlockingSocket::Receive(char* pch, const int nSize, const int nSecs)
{
	//ASSERT(m_hSocket != NULL);
	FD_SET fd = {1, m_hSocket};

	if (nSecs>0){
		TIMEVAL tv = {nSecs, 0};
		if(select(0, &fd, NULL, NULL, &tv) == 0) {
			throw new CBlockingSocketException("Receive timeout");
		}
		
	}
	else {
		//block indefinitely 
		if(select(0, &fd, NULL, NULL, NULL) == 0) {
			throw new CBlockingSocketException("socket error");
		}
		
	}

	int nBytesReceived;
	if((nBytesReceived = recv(m_hSocket, pch, nSize, 0)) == SOCKET_ERROR) {
		throw new CBlockingSocketException("Receive");
	}
	return nBytesReceived;
}

int CBlockingSocket::ReceiveDatagram(char* pch, const int nSize, LPSOCKADDR psa, const int nSecs)
{
	ASSERT(m_hSocket != NULL);
	FD_SET fd = {1, m_hSocket};
	TIMEVAL tv = {nSecs, 0};
	if(select(0, &fd, NULL, NULL, &tv) == 0) {
		throw new CBlockingSocketException("Receive timeout");
	}

	// input buffer should be big enough for the entire datagram
	int nFromSize = sizeof(SOCKADDR);
	int nBytesReceived = recvfrom(m_hSocket, pch, nSize, 0, psa, &nFromSize);
	if(nBytesReceived == SOCKET_ERROR) {
		throw new CBlockingSocketException("ReceiveDatagram");
	}
	return nBytesReceived;
}

int CBlockingSocket::SendDatagram(const char* pch, const int nSize, LPCSOCKADDR psa, const int nSecs)
{
	ASSERT(m_hSocket != NULL);
	FD_SET fd = {1, m_hSocket};
	TIMEVAL tv = {nSecs, 0};
	if(select(0, NULL, &fd, NULL, &tv) == 0) {
		throw new CBlockingSocketException("Send timeout");
	}

	int nBytesSent = sendto(m_hSocket, pch, nSize, 0, psa, sizeof(SOCKADDR));
	if(nBytesSent == SOCKET_ERROR) {
		throw new CBlockingSocketException("SendDatagram");
	}
	return nBytesSent;
}

void CBlockingSocket::GetPeerAddr(LPSOCKADDR psa)
{
	ASSERT(m_hSocket != NULL);
	// gets the address of the socket at the other end
	int nLengthAddr = sizeof(SOCKADDR);
	if(getpeername(m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR) {
		throw new CBlockingSocketException("GetPeerName");
	}
}

void CBlockingSocket::GetSockAddr(LPSOCKADDR psa)
{
	ASSERT(m_hSocket != NULL);
	// gets the address of the socket at this end
	int nLengthAddr = sizeof(SOCKADDR);
	if(getsockname(m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR) {
		throw new CBlockingSocketException("GetSockName");
	}
}

//static
CSockAddr CBlockingSocket::GetHostByName(const char* pchName, const USHORT ushPort /* = 0 */)
{
	hostent* pHostEnt = gethostbyname(pchName);
	if(pHostEnt == NULL) {
		throw new CBlockingSocketException("GetHostByName");
	}
	ULONG* pulAddr = (ULONG*) pHostEnt->h_addr_list[0];
	SOCKADDR_IN sockTemp;
	sockTemp.sin_family = AF_INET;
	sockTemp.sin_port = htons(ushPort);
	sockTemp.sin_addr.s_addr = *pulAddr; // address is already in network byte order
	return sockTemp;
}

//static
const char* CBlockingSocket::GetHostByAddr(LPCSOCKADDR psa)
{
	hostent* pHostEnt = gethostbyaddr((char*) &((LPSOCKADDR_IN) psa)
				->sin_addr.s_addr, 4, PF_INET);
	if(pHostEnt == NULL) {
		throw new CBlockingSocketException("GetHostByAddr");
	}
	return pHostEnt->h_name; // caller shouldn't delete this memory
}

BOOL CBlockingSocket::IsDataWaiting( void )
{
  
   if ( m_hSocket == INVALID_SOCKET )
   {
      return( FALSE );
   }

   int bytes_for_this_socket = 0;
   int socket_status         = 0;

   FD_SET socket_in;

   TIMEVAL time_out;

   /*
   ** Initialize the data structures
   */

   FD_ZERO( (LPFD_SET) &socket_in );

   time_out.tv_sec  = 0;
   time_out.tv_usec = 0;

   /*
   ** Set socket_in to specify that we are looking for data on socket port_id
   */

   FD_SET( m_hSocket, (LPFD_SET) &socket_in );

   /*
   ** See if data is waiting
   */

   socket_status = ::select( 0, &socket_in, NULL, NULL, &time_out );

   if ( socket_status == SOCKET_ERROR )
   {
      throw new CBlockingSocketException("IsDataWaiting");
      return( FALSE );
   }

   if ( socket_status == 0 )
   {
      /*
      ** No Data is waiting on any socket in the OS
      */

      return( FALSE );
   }

   /*
   ** Welp, data is waiting for *A* socket. It may not be *our* socket. A socket in the
   ** operating system has data waiting in it. Let's see if it happens to be *our* socket.
   */

   bytes_for_this_socket = ::FD_ISSET( m_hSocket, &socket_in );
   
   if ( bytes_for_this_socket == 0 )
   {
      /*
      ** There is data for a socket somewhere in the system but not for our socket
      */

      return( FALSE );
   }

   return( TRUE );
}

// Class CHttpBlockingSocket
IMPLEMENT_DYNAMIC(CHttpBlockingSocket, CBlockingSocket)




CHttpBlockingSocket::CHttpBlockingSocket()
{
	m_pReadBuf = new char[nSizeRecv];
	m_nReadBuf = 0;
}

CHttpBlockingSocket::~CHttpBlockingSocket()
{
	delete [] m_pReadBuf;
}

int CHttpBlockingSocket::ReadHttpHeaderLine(char* pch, const int nSize, const int nSecs)
// reads an entire header line through CRLF (or socket close)
// inserts zero string terminator, object maintains a buffer
{
	int nBytesThisTime = m_nReadBuf;
	int nLineLength = 0;
	char* pch1 = m_pReadBuf;
	char* pch2;
	do {
		// look for lf (assume preceded by cr)
		if((pch2 = (char*) memchr(pch1 , '\n', nBytesThisTime)) != NULL) {
			ASSERT((pch2) > m_pReadBuf);
			ASSERT(*(pch2 - 1) == '\r');
			nLineLength = (pch2 - m_pReadBuf) + 1;
			if(nLineLength >= nSize) nLineLength = nSize - 1;
			memcpy(pch, m_pReadBuf, nLineLength); // copy the line to caller
			m_nReadBuf -= nLineLength;
			memmove(m_pReadBuf, pch2 + 1, m_nReadBuf); // shift remaining characters left
			break;
		}
		pch1 += nBytesThisTime;
		nBytesThisTime = Receive(m_pReadBuf + m_nReadBuf, nSizeRecv - m_nReadBuf, nSecs);
		if(nBytesThisTime <= 0) { // sender closed socket or line longer than buffer
			throw new CBlockingSocketException("ReadHeaderLine");
		}
		m_nReadBuf += nBytesThisTime;
	}
	while(TRUE);
	*(pch + nLineLength) = '\0';
	return nLineLength;
}

int CHttpBlockingSocket::ReadHttpResponse(char* pch, const int nSize, const int nSecs)
// reads remainder of a transmission through buffer full or socket close
// (assume headers have been read already)
{
	int nBytesToRead, nBytesThisTime, nBytesRead = 0;
	if(m_nReadBuf > 0) { // copy anything already in the recv buffer
		memcpy(pch, m_pReadBuf, m_nReadBuf);
		pch += m_nReadBuf;
		nBytesRead = m_nReadBuf;
		m_nReadBuf = 0;
	}
	do { // now pass the rest of the data directly to the caller
		nBytesToRead = min(nSizeRecv, nSize - nBytesRead);
		nBytesThisTime = Receive(pch, nBytesToRead, nSecs);
		if(nBytesThisTime <= 0) break; // sender closed the socket
		pch += nBytesThisTime;
		nBytesRead += nBytesThisTime;
	}
	while(nBytesRead <= nSize);
	return nBytesRead;
}

void LogBlockingSocketException(LPVOID pParam, char* pch, CBlockingSocketException* pe)
{	// pParam holds the HWND for the destination window (in another thread)
	CString strGmt = CTime::GetCurrentTime().FormatGmt("%m/%d/%y %H:%M:%S GMT");
	char text1[200], text2[50];
	pe->GetErrorMessage(text2, 49);
	wsprintf(text1, "WINSOCK ERROR--%s %s -- %s\r\n", pch, text2, (const char*) strGmt);
	::SendMessage((HWND) pParam, EM_SETSEL, (WPARAM) 65534, 65535);
	::SendMessage((HWND) pParam, EM_REPLACESEL, (WPARAM) 0, (LPARAM) text1);
}

CString CBlockingSocket::GetHostName()
{
	CString sReturn;

	char* szname=new char[256];

	if (SOCKET_ERROR==::gethostname(szname,256)) {
		sReturn=_T("127.0.0.1");
	}
	else {
		sReturn =szname;
	}

	delete[] szname;

	return sReturn;
 	
}

CString CBlockingSocket::GetHostIP()
{

	CSockAddr sHostAddr = GetHostByName(GetHostName());

	
    return sHostAddr.DottedDecimal();
}
