

#pragma once

#include <windows.h>

/***************************************************************************************************************************
This file implements a status interface to rs232 ports

The interface is used when hardware specific status operations are required on a device that contains a more generalized
primary interface such as iDeviceIO

*/

class SioCfg {

	public:

	enum DtrModes { DtrEnable=DTR_CONTROL_ENABLE, DtrDisable=DTR_CONTROL_DISABLE, DtrHandshake=DTR_CONTROL_HANDSHAKE};
	enum StopBits { OneStopBit=0, OneP5StopBits=1, TwoStopBits=2 };
	enum Parity   { NoParity=0, OddParity, EvenParity, MarkParity, SpaceParity };	

	SioCfg() {
		_IOBufferSize=50000;
		//_DTRMode=DTR_CONTROL_HANDSHAKE;
		//_DTRMode=DTR_CONTROL_ENABLE;
		_DTRMode=DTR_CONTROL_DISABLE;
		_ByteSize=8;
		_Parity=0;							// 0==no parity, 1==odd, 2==even
		_StopBit=0;							// 0==1 stop bit, 1==1.5 stop bits, 2=2 stop bits

		#if 0
		#define NOPARITY            0
		#define ODDPARITY           1
		#define EVENPARITY          2
		#define MARKPARITY          3
		#define SPACEPARITY         4
		#endif

		_HWFlowControl=false;

		_RdXoutConstant=0;
		_RdXoutMultiplier=0;
		_RdXoutCharSpacing=MAXDWORD;

		//_RdXoutConstant=2000;
		//_RdXoutMultiplier=1;
		//_RdXoutCharSpacing=200;
	}

	SioCfg& IOBufferSize(unsigned size)		{ _IOBufferSize=size; } 

	SioCfg& ByteParams(unsigned char size,StopBits stop_bit,Parity parity) { 
		_ByteSize=size; 
		_StopBit=stop_bit;
		_Parity=parity;
		return(*this);
	} 

	SioCfg& TimeOuts(unsigned rd_xout_constant, unsigned rd_xout_multiplier, unsigned rd_char_spacing) {
		_RdXoutConstant=rd_xout_constant;
		_RdXoutMultiplier=rd_xout_multiplier;
		_RdXoutCharSpacing=rd_char_spacing;

		return(*this);
	}

	SioCfg& HWFlowControl(bool dtr_enable,bool hw_flow_control_enable) { 
		_HWFlowControl=hw_flow_control_enable; 
		_DTRMode= dtr_enable ? DTR_CONTROL_ENABLE:DTR_CONTROL_DISABLE;  //	checkout the situation on control enable / handshake. whats the difference

		return(*this);
	}

	SioCfg& DtrMode(DtrModes value) {
		_DTRMode=value;
		return(*this);
	}

	unsigned _IOBufferSize;
	unsigned _DTRMode;
	unsigned _ByteSize;
	unsigned _StopBit;							
	unsigned _Parity;							
	bool _HWFlowControl;

	unsigned _RdXoutConstant;
	unsigned _RdXoutMultiplier;
	unsigned _RdXoutCharSpacing;
	/*	set the read timeout values
		1.	read timeout=((number of requested chars) * multiplier) + constant
		2.	char_spacing is the max time between individual characters
		3.	default setup generates an immediate return
		*/
};

class iSerialPortStatus {

	public:

	virtual iSerialPortStatus* SetState(SioCfg &state)=0;

	virtual bool CtsIsTrue()=0;
	virtual bool DsrIsTrue()=0;
	virtual bool RingIsTrue()=0;
	virtual bool RlsdIsTrue()=0;
	virtual void SetDtrRts(bool dtr_state,bool rts_state)=0;

	virtual ~iSerialPortStatus() { }
};
