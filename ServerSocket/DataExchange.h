// DataExchange.h: interface for the DataExchange class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAEXCHANGE_H__7B2EDB59_DAE1_4DF5_8AE9_8759861ED4E7__INCLUDED_)
#define AFX_DATAEXCHANGE_H__7B2EDB59_DAE1_4DF5_8AE9_8759861ED4E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DataExchange  
{
public:
	DataExchange();
	virtual ~DataExchange();

	void Initial();

	void SetCommonFaultValue(WORD);
	WORD GetCommonFaultValue();
	WORD GetCommonFaultAddress();
	
	void SetStationModeValue(WORD);
	WORD GetStationModeValue();
	WORD GetStationModeAddress();

	void SetHeartbeatValue(WORD);
	WORD GetHeartbeatValue();
	WORD GetHeartbeatAddress();

private:
	WORD m_wCommonFault;
	WORD m_wCommonFaultAddress;

	WORD m_wStationMode;
	WORD m_wStationModeAddress;

	WORD m_wHeartbeat;
	WORD m_wHeartbeatAddress;

	WORD m_wPassengerIn;
	WORD m_wPassengerInAddress;

	WORD m_wPassengerOut;
	WORD m_wPassengerOutAddress;

	WORD m_wNumRegisters;

	WORD m_wGateNumber;

};

#endif // !defined(AFX_DATAEXCHANGE_H__7B2EDB59_DAE1_4DF5_8AE9_8759861ED4E7__INCLUDED_)
