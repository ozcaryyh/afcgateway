// DataExchange.cpp: implementation of the DataExchange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "serversocket.h"
#include "DataExchange.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DataExchange::DataExchange()
{
	m_wHeartbeatAddress = 0;
	m_wCommonFaultAddress = 1;
	m_wNumRegisters = 69;
	
	
}

DataExchange::~DataExchange()
{

}

void DataExchange::Initial()
{
	
}

void DataExchange::SetCommonFaultValue(WORD wValue)
{
	m_wCommonFault = wValue;
}

WORD DataExchange::GetCommonFaultValue()
{
	return m_wCommonFault;
}

WORD DataExchange::GetCommonFaultAddress()
{
	return m_wCommonFaultAddress;
}
	
void DataExchange::SetStationModeValue(WORD wValue)
{
}

WORD DataExchange::GetStationModeValue()
{
	return m_wStationMode;
}

WORD DataExchange::GetStationModeAddress()
{
	return m_wStationModeAddress;
}

void DataExchange::SetHeartbeatValue(WORD wValue)
{
	
}

WORD DataExchange::GetHeartbeatValue()
{
	return m_wHeartbeat;
}

WORD DataExchange::GetHeartbeatAddress()
{
	return m_wHeartbeatAddress;
}