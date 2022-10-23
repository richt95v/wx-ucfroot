
#ifndef IOCHANNELDEVICE
#define IOCHANNELDEVICE

#include <memory>

#include <iDbg.hpp>
#include <IOString.hpp>
#include <iIOChannelDevice.hpp>
#include <iStdIOChannelSystem.hpp>

/***************************************************************************************** 
This class defines an interface to a device as required by the UniConsole Framework
    1.	Lower level functionality is implemented by a class the implements the iStreamIO virtual interface

*/ 

class IOChannelDevice:public iIOChannelDevice {

	public:

	IOChannelDevice();
    IOChannelDevice(iStdIOChannelSystem *system, iStreamIO* &connection, bool debug_mode, iDbg *debugger);

	IOChannelDevice(iStdIOChannelSystem *system, unsigned device_index, bool debug_mode, iDbg *debugger);

	bool Configure(iStdIOChannelSystem *system, unsigned device_index, bool debug_mode);

	iIOChannelDevice* SetAlias(const char *alias) override;
	const char* GetAlias() override;

    iIOChannelDevice* SetDescription(const char *desc) override;
    const char* GetDescription() override;
	unsigned GetIndex() override;

	bool Validate() override;
	bool IsValid() override;

	iStdIOChannelSystem* operator->() { return(TheSystem); }

	bool ValidDevice() override;
    iStreamIO* GetStream();
	//iIOChannelDevice* GetIOChannelDevice(int index) override ;

    bool Open() override;
	/*  open a device prior to any write/read calls
		1.	provides a speed increase by not opening the device on the first write/read call
	    */

	char* SetReadBuffer(unsigned new_size) override;
	char* ClearReadBuffer() override;

	char* GetReadBuffer() override { return(ReadBuffer); }
	unsigned GetReadBufferSize() override { return(ReadBufferSize); }

	unsigned Write(char *data, int length=-1,unsigned wait_ms=0) override;
	unsigned Read() override;
	/*  mid-level interfaces
	    1.	Write data to a specified io channel
	    2.	Read data from a specified io channel
	 
	    Devices are automatically opened on first call
	    */

	bool WaitUntilPrompted(char *pattern_to_match, 	unsigned xout_s=10) override;
	bool SendCmdWhenPrompted(char *prompt, char *cmd, unsigned xout_ms=500) override;
	bool GetCmdResponse(char *cmd,unsigned xout_ms=500);
	/*  sequence control ops
	    1.	allow synchronizing with an incoming serial stream
	    */

	void Close() override;
	/*  close the device
	    1.	required after all calls to functions that take a device id
	    2.	can work for streams returned by GetIOChannel(device_index) if the correct index is used
	    */

    void AssignDebugger(iDbg *debugger) { TheDebugger.Configure(debugger); }

    bool IsOpen() override               { return(IsOpenFlag);   }
    unsigned GetBytesWritten() override  { return(BytesWritten); }
    unsigned GetBytesRead() override     { return(BytesRead);    }

	~IOChannelDevice();

	private:
    bool IsOpenFlag {false};
    int BytesRead {0};
    int BytesWritten {0};

	bool ValidConnection {false};
	imx::IOString Alias;
	imx::IOString Description;
	char *ReadBuffer {nullptr};
	unsigned ReadBufferSize {260};
	iStdIOChannelSystem *TheSystem {nullptr};
	unsigned TheDeviceIndex {99};
    std::unique_ptr<iStreamIO> TheDevice {nullptr};

	bool DebugMode {false};
    iDebugger TheDebugger;
};
		
#endif
