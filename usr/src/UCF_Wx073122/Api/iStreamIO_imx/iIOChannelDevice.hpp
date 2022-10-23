/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to an IO device used by the UniConsole framework.

			    This class is used extensively in the UniConsole framework, do not modify.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/


#ifndef IIOCHANNELDEVICE
#define IIOCHANNELDEVICE

#include <iStreamIO.hpp>

class iIOChannelDevice {

	public:

	class Guard {
		public:
		Guard(iIOChannelDevice *io) {

            if (IO=io; (IO==nullptr)) {

				SetErrorMsg("Invalid IO channel initialization");
				throw(*this);
			}

			IO->Open();
		}

		iIOChannelDevice* operator->() {

			if (IO==nullptr) {

				SetErrorMsg("Invalid IO channel assignment");
				throw(*this);
			}

			return(IO);
		}

		~Guard() {

			if (IO) IO->Close();
		}

		void SetErrorMsg(const char *msg) {

			sprintf(ErrorMsg,"%-.255s",msg);
		}

		const char* GetErrorMsg() {

			return(ErrorMsg);
		}

		private:
		char ErrorMsg[256];
		iIOChannelDevice *IO {nullptr};
	};
		
	virtual iIOChannelDevice* SetAlias(const char *alias)=0;
	virtual const char* GetAlias()=0;
	/*	set / get a unique name for a device to be used with status type displays */

	virtual iIOChannelDevice* SetDescription(const char *desc)=0;
	virtual const char* GetDescription()=0;
	/*	set / get a unique description for a device to be used with status type displays */

	virtual unsigned GetIndex()=0;

	virtual bool Validate()=0;
	virtual bool IsValid()=0;
	virtual bool ValidDevice()=0;

	virtual bool Open()=0;
	/*  open a device prior to any write/read calls
		1.	provides a speed increase by not opening the device on the first write/read call
	    */

	virtual char* SetReadBuffer(unsigned new_size)=0;
	virtual char* ClearReadBuffer()=0;

	virtual char* GetReadBuffer()=0;
	virtual unsigned GetReadBufferSize() =0;

	virtual unsigned Write(char *data, int length=-1,unsigned wait_ms=0)=0;
	virtual unsigned Read()=0;
	/*  mid-level interfaces
	    1.	Write data to a specified io channel
	    2.	Read data from a specified io channel
	 
	    Devices are automatically opened on first call
	    */

	virtual bool WaitUntilPrompted(char *pattern_to_match, 	unsigned xout_s=10)=0;
	virtual bool SendCmdWhenPrompted(char *prompt, char *cmd, unsigned xout_ms=500)=0;
	virtual bool GetCmdResponse(char *cmd,unsigned xout_ms=500)=0;
	/*  sequence control ops
	    1.	allow synchronizing with an incoming serial stream
	    */

    virtual bool IsOpen() =0;
    virtual unsigned GetBytesWritten() =0;
    virtual unsigned GetBytesRead() =0;

	virtual void Close()=0;
	/*  close the device
	    1.	required after all calls to functions that take a device id
	    2.	can work for streams returned by GetIOChannel(device_index) if the correct index is used
	    */


	virtual ~iIOChannelDevice() {}
};
		
#endif
