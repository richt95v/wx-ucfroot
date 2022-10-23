
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's internal execution engine.

			    This class is used extensively in the UniConsole framework, do not modify.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef IAPPENGINE
#define IAPPENGINE

#include <IxMgr.hpp>
#include "IOString.hpp"
#include <iDbg.hpp>

#include <vector>
#include <string>

namespace AppEngineInterfaces {

    using namespace imx;

    class iPathManager {

        public:
    	virtual const char* GetStdDataPath()=0;
    	virtual const char* GetStdConfigPath()=0;
    	virtual const char* GetStdLibPath()=0;

    	virtual const char* GetApplicationBasePath()=0;
        virtual const char* GetCurrentProfileName()=0;

        virtual const char* GetProfileName()=0;
        virtual const char* GetPathToSelectedProfile()=0;

        virtual const char* GetProfileBasePath()=0;
        virtual const char* GetPathToExtendedCmds()=0;
        virtual const char* GetPathToEtc()=0;
        virtual const char* GetPathToComonLibs()=0;

    	virtual bool SetProfileDependantPaths()=0;
    };

    class iErrorMgr {
    	public:
    	virtual void LockEngine(bool lock=true) =0;
    	virtual bool IsLocked() =0;

    	virtual void ResetSoftErrors() =0;
    	virtual void SetSoftError(IOString msg) =0;
    	virtual bool GetSoftErrorMsg( std::vector<std::string> &errors) =0;
    	virtual unsigned SoftErrorHasOccurred() =0;

    	virtual void ResetHardErrors() =0;
    	virtual void SetHardError(IOString msg) =0;
    	virtual unsigned HardErrorHasOccurred() =0;

    	virtual bool GetHardErrorMsg( std::vector<std::string> &errors)=0 ;
    	virtual ~iErrorMgr() {}
    };

#if 0
    class UcfDebugger {
        public:
        void Configure(iDbg *ptr) { TheDebugger=ptr; }

        void ProcessEvent(iDebugEvent *event) {

            if (TheDebugger && event) {

                TheDebugger->ProcessEvent(event);
            }
        }

        private:
        iDbg *TheDebugger{nullptr};
    };
#endif


    class iAppEngine: public iIXmgr {

    	public:

        virtual iDebugger* Debugger() =0;
        virtual bool GetDebugMode()=0;

    	/*********************************************************************************************************************** 
    	Module support
    	*/
        virtual unsigned long GetModuleID()=0;

    	virtual bool LoadModule(imx::IOString name)=0;
    	virtual bool UnLoadModules()=0;
    	virtual bool UnloadModule(int index)=0;
    	/*	load / unload individual modules */

    	virtual bool ShowStaticModuleConfiguration()=0;

    	virtual bool LoadStartupLibs()=0;
    	/*	load the startup libs located in the StaticAddin, CommonLib or Profile directories */

        virtual void RegisterExtensions()=0;

    	virtual bool GetLoadedModules(std::vector<std::tuple<int,std::string,std::string>> &modules)=0;
    	/*	get a list of loaded modules */

    	/*********************************************************************************************************************** 
    	Symbolic link support
    	*/
        virtual bool VerifyPathLinkAlias(const char* name,const char* request_description,bool display_cfg=true)=0;
        virtual bool GetPathLinkAlias(const char* name,char* buffer,unsigned buffer_size)=0;
        virtual bool GetPathLinkDescription(const char *name,char *buffer,unsigned buffer_size=0)=0;

    	virtual bool GetPathLinks(bool primary_db, unsigned index, imx::IOString &name, imx::IOString &path,bool &enable, imx::IOString &description) =0;
    	/*	used to get all entries in the pathlink databases */


    	/*********************************************************************************************************************** 
    	External process support
    	*/

    	virtual void RequestIdle(bool value=true) =0;
    	virtual bool IdleIsRequested()=0;
    	/*	 request an idle condition from the appengine */

    	/*********************************************************************************************************************** 
    	Thread monitoring support
    	*/

    	virtual void ShowStatus()=0;

    	virtual void IncrementActiveThreadCount()=0;
    	virtual void DecrementActiveThreadCount()=0;
    	virtual bool FrameworkIsIdle()=0;
    	/*	control a thread counter, and counter status
    		1.	when a new thread is started, increment
    		2.	when a thread terminates, decrement
    		3.	FrameworkIsIdle() returns true if the thread count ==0
    		*/

    	/*********************************************************************************************************************** 
    	Path support
        get locations that may be unique to individual instances
        */
    	 
        virtual iPathManager* iPaths()=0;

    	/*********************************************************************************************************************** 
    	Error handling	
    	*/

        virtual iErrorMgr* iErrorManager()=0;

    	/*********************************************************************************************************************** 
    	Misc	
    	*/

    	virtual void WriteLogFile(int level,imx::IOString msg)=0;
    	/*  write a message to the log file
    	    1. level=0 for critical error
    	    2. level=1 for warning
    	    3. level=2 for info
    	    */

    	virtual ~iAppEngine() {}
    };
}

#endif
