
#include <IOChannelDevice.hpp>

#include <IOString.hpp>
#include <SimpleAlarm.hpp>
#include <string.h>


IOChannelDevice::IOChannelDevice() {

	SetReadBuffer(ReadBufferSize);
}

IOChannelDevice::IOChannelDevice(iStdIOChannelSystem *system, iStreamIO* &connection, bool debug_mode, iDbg *dbg):IOChannelDevice() {

    TheSystem=system;
    TheDeviceIndex=0;
    TheDevice.reset(connection);
    DebugMode=debug_mode;
	TheDebugger.Configure(dbg);
}

IOChannelDevice::IOChannelDevice(iStdIOChannelSystem *system, unsigned device_index, bool debug_mode, iDbg *dbg):IOChannelDevice() {

    Configure(system,device_index,debug_mode);
	TheDebugger.Configure(dbg);
}

bool IOChannelDevice::Configure(iStdIOChannelSystem *system, unsigned device_index, bool debug_mode) {

	TheSystem=system;
	TheDeviceIndex=device_index;
    TheDevice.reset(GetStream());
	DebugMode=debug_mode;
		
	return(TheDevice ? true:false);
}

iIOChannelDevice* IOChannelDevice::IOChannelDevice::SetAlias(const char *alias) {

	Alias=alias;
	return(this);
}

const char* IOChannelDevice::GetAlias() {

	return(Alias);
}


iIOChannelDevice* IOChannelDevice::SetDescription(const char *desc) {
	Description=desc;
	return(this);
}

const char* IOChannelDevice::GetDescription() {

	return(Description.c_str());
}

unsigned IOChannelDevice::GetIndex() {
	
	return(TheDeviceIndex);	
}

bool IOChannelDevice::Validate() { 

	ValidConnection=false;
	if (TheDevice) {

		if (((ValidConnection=TheDevice->ValidateConnection())==false) && !DebugMode) {

			throw(imx::IOString("Unable to validate device (%-.48s)",Description.c_str()));
		}
	}

	return (ValidConnection);
}

bool IOChannelDevice::IsValid() {

	return (ValidConnection);
}

bool IOChannelDevice::ValidDevice() {
	
	return(TheDevice ? true:false);
}

iStreamIO* IOChannelDevice::GetStream() {

    return(TheSystem ? TheSystem->GetStreamIO(TheDeviceIndex):nullptr);
}

char* IOChannelDevice::ClearReadBuffer() {

	memset(ReadBuffer,0,ReadBufferSize);
	return(ReadBuffer);
}

char* IOChannelDevice::SetReadBuffer(unsigned new_size) {

	if (ReadBuffer) delete[] ReadBuffer;
	ReadBuffer=new char[new_size];
	ReadBufferSize=new_size;

	ClearReadBuffer();

	return(ReadBuffer);
}

bool IOChannelDevice::Open() {
/*  open a device prior to any write/read calls
	1.	provides a speed increase by not opening the device on the first write/read call
    */

    IsOpenFlag=TheDevice->Open(!DebugMode);
    TheDebugger.ProcessEvent(new IOEvent(this,IOEvent::Type::SystemOpenDevice));

    return(IsOpenFlag);
}

unsigned IOChannelDevice::Write(char *data, int length,unsigned wait_ms) {

    BytesWritten=0;
	if (Open()) {

		BytesWritten=TheDevice->Write(data, length, wait_ms);
        TheDebugger.ProcessEvent(new IOEvent(this,IOEvent::Type::SystemWriteDevice));
	}

	return(BytesWritten);
}

unsigned IOChannelDevice::Read() {
/*  mid-level interfaces
    1.	Write data to a specified io channel
    2.	Read data from a specified io channel
 
    Devices are automatically opened on first call
    */

    BytesRead=0;
	if (Open()) {

        BytesRead=TheDevice->Read(ReadBuffer, ReadBufferSize);
        TheDebugger.ProcessEvent(new IOEvent(this,IOEvent::Type::SystemReadDevice));
	}

	return(BytesRead);
}

bool IOChannelDevice::WaitUntilPrompted(char *pattern_to_match, 	unsigned xout_s) {

	SimpleAlarm alarm(xout_s);

	while(Read()) {

		if (strstr(pattern_to_match,ReadBuffer)) {

			return(true);
		}

		if (alarm.Timeout==true) break;
	}

	return(false);
}

bool IOChannelDevice::SendCmdWhenPrompted(char *prompt, char *cmd, unsigned xout_ms) {

	if (WaitUntilPrompted(prompt,xout_ms)) {

        Write(cmd,(int)strlen(cmd));
		return(true);
	}

	return(false);
}

bool IOChannelDevice::GetCmdResponse(char *cmd, unsigned xout_ms) {
/*  sequence control ops
    1.	allow synchronizing with an incoming serial stream
    */

    Write(cmd, (int)strlen(cmd),xout_ms);
	if (Read()) {

		return(true);
	}

	return(false);
}

void IOChannelDevice::Close() {
/*  close the device
    1.	required after all calls to functions that take a device id
    2.	can work for streams returned by GetIOChannel(device_index) if the correct index is used
    */

	if (TheDevice) {

		TheDevice->Close();
        IsOpenFlag=false;

        TheDebugger.ProcessEvent(new IOEvent(this,IOEvent::Type::SystemCloseDevice));
	}
}

IOChannelDevice::~IOChannelDevice() {

	//Close();	//	this screws up tmp io channels
    //if (TheDevice) delete (TheDevice);
	if (ReadBuffer) delete[](ReadBuffer);
}


