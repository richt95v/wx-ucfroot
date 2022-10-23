
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's method management system.

    			This class is used extensively in the UniConsole framework, do not modify. Modifications to this file are gauranteed
    			to break the system if modified incorrectly. 
 
    Author:		Richard Todd
    Date:		11.1.2019

*/

#ifndef IMETHODSERVER
#define IMETHODSERVER

#include "memory"
#include "vector"

#include "iAobj.hpp"
#include "IOString.hpp"
#include "iStdOut.hpp"
#include <IxMgr.hpp>

namespace CmdRepository {

	using imx::IOString;

	class CmdParam {
	/*  this class defines the parameter passed to all methods that are registered in the command repository
	    it contains:
	    1.	an optional command line string that is passed when the method is invoked from the uniconsole command line
	    2.	an optional iIXmgr* to the appengine's interface repository
	    */

		public:
		CmdParam(const char *msg, int option=0)	{ Cmd=msg; CmdLineFlag=true; Option=option;}
		CmdParam(iIXmgr *ix, int option=0) 		{ InterfaceMgr=ix; IxFlag=true; Option=option;}
		
		operator char* () 			{ return(Cmd.c_str()); }
		const char* c_str() 		{ return(Cmd.c_str()); }
		
		bool ContainsCmdLine() 		{ return(CmdLineFlag); }
		bool ContainsIXmgr()		{ return(IxFlag); }

		void SetOption(int value) 	{ Option=value; 	}
		int GetOption()				{ return(Option); 	}
		
		template<class T>
		T* iGet(unsigned long &module_id) 		{  return(InterfaceMgr ? InterfaceMgr->iGet<T>(module_id):nullptr); }
		
		private:
		bool CmdLineFlag 	{false};
		bool IxFlag 		{false};
		IOString Cmd;
		iIXmgr *InterfaceMgr{nullptr};
		int Option			{0};
	};

    class iMethodServer {
    /*  this class implements an interface to a signal/slot mechanism for handling text output from a program or module
    	1.	this implementation supports 0-n signal/slot groups
        */

    	public:

		enum class CmdType:unsigned {Application=0, System, Framework,Admin};
		
        bool RegisterApplicationCmd	(IOString name,iAobj<CmdParam> *iobj,	IOString description) 	{ return(RegisterTextCmd(name,CmdType::Application,iobj,description)); }
        bool RegisterSystemCmd		(IOString name,iAobj<CmdParam> *iobj, 	IOString description) 	{ return(RegisterTextCmd(name,CmdType::System,iobj,description)); }
        bool RegisterFrameworkCmd	(IOString name,iAobj<CmdParam> *iobj, 	IOString description)	{ return(RegisterTextCmd(name,CmdType::Framework,iobj,description)); }
        bool RegisterAdminCmd		(IOString name,iAobj<CmdParam> *iobj, 	IOString description) 	{ return(RegisterTextCmd(name,CmdType::Admin,iobj,description)); }
		/*	register methods to be called from the uniconsole command line */
		
        virtual bool CommandExists(IOString cmd) { return(false); }
		/*	verify that a command exists */

        virtual bool ExecuteCommand(IOString cmd,CmdParam param) { return(0); }

		virtual bool ContainsCustomCmds() { return(false); }
        /*  return true if the module contains any registered commands */

		virtual bool ShowTextCommandHelp(bool verbose,iStdOut *istdout,CmdType level) { return(false); }
		/*  display registered commands */

		bool RegisterTextCmd(IOString name,CmdType type, iAobj<CmdParam> *iobj, IOString description) {
    	/*  register a new command with the server
    	    1.	a reference is stored in the CleanupRepository so that memory will be deallocated when the lib is unloaded
    	    */

			AddTextCmd(name,iobj,type,description);
    		//CleanupRepository.push_back(std::shared_ptr<iAobj<CmdParam>>(iobj)); /*new*/

    		return(true);
    	}

		void ResetServer(); 

    	private:
        virtual bool AddTextCmd(IOString name, iAobj<CmdParam> *iobj,CmdType level,IOString description) { return(false); }

        //static std::vector<std::shared_ptr<iAobj<CmdParam>>> CleanupRepository; /*new*/
    };

	class Methods {
		public:
		iMethodServer *TheMethodServer {nullptr};
		Methods(iMethodServer *ptr) {

			TheMethodServer=ptr;
		}

		bool RegisterApplicationCmd(IOString name,iAobj<CmdParam> *iobj,	IOString description) 	{

			if (TheMethodServer) {

				return(TheMethodServer->RegisterApplicationCmd(name,iobj,description));
			}

			return(false);
		}

		bool RegisterSystemCmd(IOString name,iAobj<CmdParam> *iobj, 		IOString description) 	{

			if (TheMethodServer) {
				return(TheMethodServer->RegisterSystemCmd(name,iobj,description));
			}
			return(false);
		}

		bool RegisterFrameworkCmd(IOString name,iAobj<CmdParam> *iobj, 		IOString description)	{
			if (TheMethodServer) {
				return(TheMethodServer->RegisterFrameworkCmd(name,iobj,description));
			}
			return(false);
		}

		bool RegisterAdminCmd(IOString name,iAobj<CmdParam> *iobj, 			IOString description)	{

			if (TheMethodServer) {
				return(TheMethodServer->RegisterAdminCmd(name,iobj,description));
			}
			return(false);
		}
		/*	register methods to be called from the uniconsole command line */

		bool CommandExists(IOString cmd) {
		/*	verify that a command exists */

			if (TheMethodServer) {

				return(TheMethodServer->CommandExists(cmd));
			}

			return(false);
		}

		int ExecuteInternalCommand(IOString cmd,CmdParam param) {
		/*	write a message to the stdout server */

			if (TheMethodServer) {

                return(TheMethodServer->ExecuteCommand(cmd,param));
			}

			return(false);
		}

		bool ContainsCustomCmds() {
		/*  return true if the module contains any registered commands */

			if (TheMethodServer) {

				return(TheMethodServer->ContainsCustomCmds());
			}

			return(false);
		}

		bool ShowTextCommandHelp(bool verbose,iStdOut *istdout,iMethodServer::CmdType level) {

			if (TheMethodServer) {

				return(TheMethodServer->ShowTextCommandHelp(verbose,istdout,level));
			}

			return(false);
		}

		private:
	};
}

/*

*/

#endif
