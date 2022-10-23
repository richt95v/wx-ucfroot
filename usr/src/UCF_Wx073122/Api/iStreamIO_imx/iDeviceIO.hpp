
#pragma once

#include <windows.h>
#include <IOString.hpp>
#include <ConnectionID.hpp>

using std::string;


namespace DeviceIO {

	class iDeviceIO;

	class iDeviceIOClient {
		public:
		virtual bool Execute(iDeviceIO *iochannel) { return(true); }
	};

	class iDeviceIO {

		public:
		virtual iDeviceIO* SetCfg(ConnectionID &id)=0;
		virtual iDeviceIO* GetCfg(ConnectionID &id)=0;

		virtual bool Open()=0;
		virtual bool Open(ConnectionID &id)=0;
		/*	open the io channel */

		virtual unsigned GetWriteBufferSize()=0;
		virtual unsigned GetReadBufferSize()=0;
		/*	get the size of the pending read/write buffer - devices are modelled as a file in this case */

		virtual int Write(const char *data, int data_size=-1,int transaction_delay=300)=0;
		/*	write to the io channel */

		virtual int Read(char *buffer,int buffer_size,int transaction_delay=0)=0;
		/*	read from the io channel */

		virtual bool Close()=0; 
		/*	close the io channel */

		virtual bool InError()=0;
		virtual const char* GetErrorMsg()=0;

		template<class T>
		bool iGet(T* &ref) {
			return((ref=dynamic_cast<T*>(this)) ? true:false );
		}

		bool Execute(iDeviceIOClient *client) {
			bool status=false;
			EnterCriticalSection(&Lock);
			status=client->Execute(this);
			LeaveCriticalSection(&Lock);
			return(status);
		}

		virtual ~iDeviceIO() { }

		private:
		CRITICAL_SECTION Lock;

	};
}

