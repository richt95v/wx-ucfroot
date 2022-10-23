
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's text message output system.

    			This class is used extensively in the UniConsole framework, do not modify. 
 
    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef ISTDOUTSERVER
#define ISTDOUTSERVER

#include "memory"
#include "vector"
#include "functional"

#include "iAobj.hpp"
#include <IOString.hpp>
#include <iTabbedTable.hpp>

#include <iStdOut.hpp>

using imx::IOString;
using std::tuple;
using std::shared_ptr;

namespace MultiModuleConsoleIO {

    class iStdOutServer:public iStdOut {
    /*  this class implements an interface to a signal/slot mechanism for handling text output from a program or module
    	1.	this implementation supports 0-n signal/slot groups
        */

    	public:

    	bool RegisterStdOutClient(iAobj<tuple<int,IOString,int>> *iobj) {
    	/*  register a new client with the server
    	    1.	a reference is stored in the CleanupRepository so that memory will be deallocated when the lib is unloaded
    	    */

    		RegisterStdOut(iobj);
    		StdOutCleanupRepository.push_back(shared_ptr<iAobj<tuple<int,IOString,int>>>(iobj));

    		return(true);
    	}

		bool RegisterStdOutTableClient(iTabbedTable *ptr) {

			return(VRegisterStdOutTableClient(ptr));
		}

    	private:
    	virtual bool RegisterStdOut(iAobj<tuple<int,IOString,int>> *iobj)=0;
		virtual bool VRegisterStdOutTableClient(iTabbedTable *ptr)=0;

    	static std::vector<shared_ptr<iAobj<tuple<int,IOString,int>>>> StdOutCleanupRepository;
    };

	class StdOutWriter {

		public:
		StdOutWriter() = default;
		
		StdOutWriter(iStdOut *ptr, unsigned handle):StdOut(ptr),Handle(handle) {} 

		void Configure(iStdOut *ptr, unsigned handle) {
			StdOut=ptr;
			Handle=handle;
		}

		bool WriteOnTrue(bool condition, IOString msg, int option=iStdOut::Text1) {

			if (condition) Write(msg,option);
			return(condition);
		}

		void Write(IOString msg,int option=iStdOut::Text1) {

			if (StdOut) StdOut->WriteStdOut(msg,option,Handle);
		}

		private:
		iStdOut *StdOut {nullptr};
        unsigned Handle {0};
    };
}

#endif
