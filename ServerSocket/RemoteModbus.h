// RemModbus.h: interface for the CRemoteModbus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMMODBUS_H__0D14D225_BC57_11D1_8FE9_E4CD05C10000__INCLUDED_)
#define AFX_REMMODBUS_H__0D14D225_BC57_11D1_8FE9_E4CD05C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "blocksock.h"	
#include "modbus.h"

#define DEFAULT_PORT 502

class CRemoteModbus : public CModbus  
{
	DECLARE_DYNAMIC(CRemoteModbus)

public:
	
	WORD Port();
	void Port(WORD wPort);
	CString Host();
	void Host(LPCSTR szHost);
	void ReconnectOnEveryMessage(BOOL bReconnect);
	BOOL ReconnectOnEveryMessage();
	void Timeout(DWORD dwTimeout);
	DWORD Timeout();
	CRemoteModbus();
	virtual ~CRemoteModbus();

	//Enbable Comm Exception Detection and offset 
	BOOL ComExceptionEnabled();
	void ComExceptionEnabled(BOOL bEnabled);
	void CommExceptionOffset(BYTE byOffset);
	BYTE CommExceptionOffset();


	BOOL IsActive(); //return true if serial port opened 
	BOOL IsPollingEnabled();
	

	BOOL CloseConnection();

    //open Connection with Modbus Server 
	BOOL openConnection(CString sHost="", //Host Addresr Dotted decimal IP  or name 
						WORD nPort=0, //socket port the same configured when the server started 
						DWORD dwTimeOut=0); //Time-out mili seconds when Tx Rx Modbus messages 
	
//transmit a query to a modbus device and wait for a response
//return ERR_XXX Code see Modbus.cpp file  
 virtual WORD TxRxMessage(CByteArray& abyQuery, //modbus query without CRC
				  WORD wLengthQuery, //QueryLength without CRC
				  CByteArray& abyResponse, //modbus Response without CRC
				  WORD wLengthResponse, //wLengthResponse Response without CRC
				  WORD* pwNumOfBytesRead=NULL);	
	
	//transmit a query to a modbus device and wait for a response
	//return ERR_XXX Code see Modbus.cpp file  
	WORD TxRxMBAP(CByteArray& abyQuery, //modbus query without CRC
				  WORD wLengthQuery, //QueryLength without CRC
				  CByteArray& abyResponse, //modbus Response without CRC
				  WORD wLengthResponse //wLengthResponse Response without CRC
				  );  	
	
	//return string error message 
	//in:wErrorCode constant errro code 
	//out:string with error 
	CString ErrorMessage(WORD wErrorCode); 
	
	//used for seralization the object to file 
	void Serialize( CArchive& archive );

	//Error Constants 
	enum _ERRROR_CODES_{
		ERR_BUFFER_OVERFLOW=1000,
        ERR_SOCKET_ERROR=1001,
		ERR_WR_SOCK_ERROR=1002,
		ERR_RD_SOCK_ERROR=1003,
		ERR_SOCK_NOT_CONNECTED=1004
	};

protected:
	HANDLE      m_hThread;

	
private:
	BOOL m_bReconnectOnEveryMessage;
	CString m_sHost;
	WORD m_wPort;
	DWORD m_dwTimeOut;
	BOOL m_bComExceptionEnabled;
	BYTE m_byExceptionOffset;
	BOOL m_bPollingEnabled;
	

	CSockAddr m_saServer;//Modbus Server 
	CSockAddr m_saClient;//Address of this object 
	CBlockingSocket m_sClient;//Client Socket 
	CByteArray m_abyRespBuffer;
	CByteArray m_abyQueryBuffer;

	enum _Remote_Mobdbus_Constants {
		INPUT_RESP_BUFFER_SIZE=1024,
        OUTPUT_QUERY_BUFFER_SIZE=1024,
		TCP_HEADER_LENGHT=4,
		//MBAP constants 
		MBAP_HEADER_LENGHT=6,
		MODBUS_ENCODING = 0,
		TRANSAC_ID=0,
		DEFAULT_COMM_EXCEPTION_OFFSET=0x80
	};


};

#endif // !defined(AFX_REMMODBUS_H__0D14D225_BC57_11D1_8FE9_E4CD05C10000__INCLUDED_)
