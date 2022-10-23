
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's module management system.

    			This class is used extensively in the UniConsole framework, do not modify. Modifications to this file are gauranteed
    			to break the system if modified incorrectly. 
 
    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef ISHAREDLIB
#define ISHAREDLIB

#include <string>
#include <iStdOutServer.hpp>
#include <iUniConsole.hpp>
#include <iAppEngine.hpp>
#include <iMethodServer.hpp>
#include <iTrace.hpp>

#include <functional>

using namespace AppEngineInterfaces;

template<class T>
class iSharedLibFramework {
/*	this class defines an interface to a shared library */

	public:

	iSharedLibFramework(std::string name) { 

		using namespace std::placeholders;
		ModuleName=name; 
	}

	virtual ~iSharedLibFramework() {}
	
	enum class ModuleStates { NullState,GetInterfaces,StartModule,MakeExternalConnections,StartProcesses, Activate, SetToIdle, Shutdown};

	static bool GetNextState(ModuleStates &state) {

		bool ret=true;
		switch(state) {

			case(ModuleStates::NullState): 				
				state=ModuleStates::GetInterfaces;
				break;

		case(ModuleStates::GetInterfaces): 			
				state=ModuleStates::StartModule; break;
				break;

			case(ModuleStates::StartModule): 						
				state=ModuleStates::MakeExternalConnections;
				break;

			case(ModuleStates::MakeExternalConnections): 	
				state=ModuleStates::StartProcesses;
				break;

			case(ModuleStates::StartProcesses): 			
				state=ModuleStates::Activate;
				break;

			case(ModuleStates::Activate): 
				state=ModuleStates::SetToIdle;
				break;

			case(ModuleStates::SetToIdle): 
				ret=false;
				//state=ModuleStates::Shutdown;
				break;

			default: 
				state=ModuleStates::NullState; ret=false;
				break;
		}

		return(ret);
	}

	virtual int Initialize(ModuleStates level,T *ix)=0;
	virtual const char* GetDescription()=0;

    void SetBaseInterfaces(iIXmgr *ix, unsigned long module_id) {

		TheIxRepository=ix;

        if ( TheAppEngine=ix->iGet<iAppEngine>(); TheAppEngine) {
			
            TheShell=ix->iGet<iUniConsole::Console>();
            StdOut=ix->iGet<iStdOut>(module_id);
            StdOut=ix->iGet<iStdOut>();
            StdTrace=StdOut;
		}
	}

	const char* GetName() { return(ModuleName.c_str()); }

	MultiModuleConsoleIO::iStdOutServer* GetStdOutServer() { return(StdOut); }

    iUniConsole::Console 			*TheShell {nullptr};
    CmdRepository::iMethodServer	*Cmds {nullptr};
	// this needs revisitting - raw pointer in an interface, no abstraction, buffering ???

	protected:

	iIXmgr 							*TheIxRepository {nullptr};
	iAppEngine 						*TheAppEngine {nullptr};
	std::string						ModuleName;

    iStdOut							*StdTrace {nullptr};
    iStdOut							*StdOut {nullptr};
};


template<class T>
class iSharedLib {
/*	this is a host interface to a shared library */

	public:
	virtual bool LoadAndConfigure(T *ix,unsigned module_id)=0;
	virtual bool Unload()=0;
	virtual iSharedLibFramework<T>* GetModuleFramework()=0; 
	virtual ~iSharedLib() {}
};

#endif
