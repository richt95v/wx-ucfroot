
#include <CmdLineProcessorModule.hpp>

#include <iStdOut.hpp>
#include <iCmdServer.hpp>

#include <exception>
#include <thread>
#include <chrono>

#include <iostream>
#include <string>
#include <cstring>

#include <iDbg.hpp>

using std::vector;
using imx::IOString;
using imx::TokenString;
using CmdRepository::CmdParam;


/*************************************************************************************************************************** 
  This file implements a template shared library that uses the 'SharedLib' framework
 
*/

CmdLineProcessorModule 		TheModule("Template Module");

#ifdef __linux__

    #pragma GCC diagnostic ignored "-Wunused-variable"
    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {

#else

    __declspec(dllexport) iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {
		
#endif

    local_ix->iSet<iCmdLineProcessor>(&TheModule);
    local_ix->iSet<CmdRepository::iMethodServer>(TheModule.GetMethodServer());

    TheModule.ModuleId=module_id;

    return(&TheModule); 
}

int CmdLineProcessorModule::Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) {
/*  Initialize the module(s)
	1.	use the interfaces previously registered by the framework and other modules
    2.	the 'ix' interface provided to this function is the global interface
    3.	any interfaces or commands registered here go into the global repository - and will remain after this module is unloaded - dont do it
    */

	using state=iSharedLibFramework<iIXmgr>::ModuleStates;

	try {
       	
		std::thread startup_thread;
		switch(level) {

			case(state::GetInterfaces):
			/* use the interfaces previously registered by the framework and other modules */

                SetBaseInterfaces(global_ix,ModuleId);
			    break;

			case(state::StartModule):
			/* start the module */

			    Start();
			    break;

			case(state::MakeExternalConnections):
			/* make any external connectionsI */
			    break;

			case(state::StartProcesses):
			/* autostart any runtime processes */
			
			    SetStaticConfiguration();
				
			    startup_thread=std::thread(&CmdLineProcessorModule::ThreadedStartup,this,global_ix);
			    startup_thread.detach();
			    break;

			case(state::Activate):
			/* wake up an idle module */
			    break;

			case(state::SetToIdle):
			/* set the module to an idle state */
			    SetToIdle();
			    break;

			case(state::Shutdown):
			/* shutdown the module */

			    Shutdown();
			    break;

			default:
			    break;
		}
	}
	catch(...) {

	    HandleModuleException("Initialization");
	}

	return(1);
}

const char* CmdLineProcessorModule::GetDescription() {

    static IOString desc("This module implements a UniConsole command line processor");

    return(desc);
}

void CmdLineProcessorModule::ThreadedStartup(iIXmgr *ix) {

    try {

        TheAppEngine->IncrementActiveThreadCount();
        /* let the appengine know that a thread is running (aka, keep the progress bar going */
    }
    catch(IOString msg) {

        TheAppEngine->iErrorManager()->SetHardError(msg);
    }
    catch(...) {

        TheAppEngine->iErrorManager()->SetHardError("Unknown exception while initializing the system");
    }

    TheAppEngine->DecrementActiveThreadCount();
}


using Ucfx=Aobj<CmdLineProcessorModule,CmdParam>;
int CmdLineProcessorModule::Start() {
/*  this function, called after the entry point, and initialize() is necessary:
    1.	GetEntryPoint() is used to register exported interfaces
    2.	Initialize() is used to get required interfaces
    3.	If a module's Initialize() gets an interface, and tries to use it, it may call a function in a module that hasn't been initialized yet (it will not have recieved its
	    required interfaces yet.
 
    entry point: 	get global interfaces, register exported interfaces, and return an interface to this module
    initialize(): 	get all interfaces required by this module
    start(): 		execute the retrieved interfaces in whatever manner
    */

    StdOutConsole.Configure(StdOut,StdOut->GetRegisteredConsoleHandle("StdOut"));
    StdSysConsole.Configure(StdOut,StdOut->GetRegisteredConsoleHandle("StdSys"));
    StdTraceConsole.Configure(StdOut,StdOut->GetRegisteredConsoleHandle("StdTrace"));

    //CommandRepository.RegisterApplicationCmd("test1",		new Ucfx(&CmdLineProcessorModule::Test1,this),		"(...)\t ... ");
    //CommandRepository.ExecuteInternalCommand("test1", CmdParam("xx"));

    /*
    Do any required initial system configurations here

        SystemCmd.SetCmd("/usr/bin/killall",iProcessL::Detached)
            ->SetArgs({"iperf3"})
            ->Start();


        imx::IOString check_response;
        ExtExec diag_usb_check;
        if (diag_usb_check.Exec( "/usr/bin/lsof /dev/ttyDashUUT")) {

            if (TheAppEngine->GetDebugMode()==false) {

                throw(imx::IOString("The UUT diagnostic port is already in use"));
            }
        }

        check_response=diag_usb_check.GetOutputMsg();
    */

    return(1);
}

void CmdLineProcessorModule::SetStaticConfiguration() {
/*  set the static configuration for the system 
    1.	assign system interfaces to individual modules
	 */
}

#include <QMessageBox>
#include <QtWidgets>
#include <QMainWindow>


int CmdLineProcessorModule::Test1(CmdParam msg) {
/*	Get the serial number from the UUT */

	TokenString str(msg,',');

	bool open {true};
	std::string title, message;

    str.GetToken(1,open);
    str.GetToken(2,title);
    str.GetToken(3,message);

    TheShell->UserNotify(open, false, title.c_str(), message.c_str());


    return(1);
}

int CmdLineProcessorModule::SetToIdle() {
/*  Set the module to an idle condition
    1.  After system initialization
    2.  Between test sequence executions
*/

	return(1); 
}

int CmdLineProcessorModule::Activate() { 
/*	'Wakeup' an idle module prior to executing a test sequence */

	return(1); 
}

void CmdLineProcessorModule::Shutdown() {
/*  Shutdown the lib on program termination
	 1.  Set any instrumentation to an idle/safe state
	 2.  Any de-allocations as necessary
	*/

    /*
    SystemCmd.SetCmd("/usr/bin/killall",iProcessL::Detached)
        ->SetArgs({"iperf3"})
        ->Start();
    */
}

void CmdLineProcessorModule::HandleModuleException(IOString context) {
/*  this method is called from catch handlers when the exception has a system wide impact
    1.  Information must be displayed to the operation
    2.  Information must be written to local log file(s)
    3.  Test operations must be disabled (possibly not when in a 'development' mode
	 4.  This is not called directly from app or system modules where the thrown types may be unknown
    */
	try {

        TheAppEngine->Debugger()->ProcessEvent(new ExceptionEvent(ModuleName,context));
        throw;
	}
	catch(std::exception& e) {

       IOString err("%-.64s exception (%-.128s)",context.c_str(),e.what());
       throw(err);
	}
	catch(IOString msg) {

       IOString err("%-.64s exception (%-.128s)",context.c_str(),msg.c_str());
       throw(err);
	}

	catch(...) {

       IOString err("%-.64s exception (Undefined)",context.c_str());
       throw(err);
	}
}

IOString CmdLineProcessorModule::SignalTab() {
/*  initiate a command line completion operation
    */

    std::vector<std::string> prefix_matching_names;
    if (CurrentCmdLineInput.size()) {

        prefix_matching_names=CommandRepository.GetUniqueCompletionPrefix(CurrentCmdLineInput);

        StdOutConsole.Write("*",iStdOut::Filename1);
        for (std::string &str:prefix_matching_names) {

            StdOutConsole.Write(str.c_str(),iStdOut::Filename1);
        }

        if (prefix_matching_names.size()==1) {

            CurrentCmdLineInput=prefix_matching_names[0];
        }
    }

    return(IOString(CurrentCmdLineInput.c_str()));
}

int CmdLineProcessorModule::ProcessCmdLine(IOString current_cmdline) {

    CurrentCmdLineInput=current_cmdline.c_str();
    return(0);
}

vector<IOString> CmdLineProcessorModule::FindMatchingCommands(const char *prefix) {
/*  scan the registered commands and create a list of commands that have 'prefix'
    */

    std::string std_str;
    std::vector<IOString> names;
    for (IOString &str:names) {

        std_str=str.c_str();
        if (std_str.compare(0,sizeof(prefix),prefix)!=std::string::npos) {

            StdOutConsole.Write(str.c_str(),iStdOut::Text1);
        }
    }

    return(names);
}

bool CmdLineProcessorModule::CommandExists(const char *cmd_line) {

    std::string str=cmd_line;
    int index {0};

    index=str.find("*");
    if (index!=std::string::npos) str.erase(index);

    index=str.find(",");
    if (index!=std::string::npos) str.erase(index);

    return(CommandRepository.CommandExists(str));
}

void CmdLineProcessorModule::FormatCmdLine(const char *cmd_line,imx::IOString &command_string, unsigned &loop_count) {

    std::string alt_cmd;
    imx::TokenString parse_str(cmd_line,'*');

    loop_count=1;
    command_string=cmd_line;
    if (parse_str.GetTokenCount()>1) {
    /* if a multiplier was spec'ed */

        parse_str.GetToken(1,loop_count);
        /*  get the loop count */

        parse_str.GetToken(0,alt_cmd);
        /*  get the command line */

        command_string=alt_cmd.c_str();
        /*  set the command without the loop count */
    }
}

void CmdLineProcessorModule::ExecuteTextCommand(const char *cmd_line) {

    if (strlen(cmd_line)) {

        imx::IOString command_string;
        unsigned loop_count {1};

        FormatCmdLine(cmd_line, command_string,loop_count);

        std::thread(&CmdLineProcessorModule::ExecuteCmdInThread, this, command_string,loop_count).detach();
    }
}

bool CmdLineProcessorModule::ExecuteCmdInThread(imx::IOString cmd_line, unsigned loop_count) {

    std::string function_name;
    imx::TokenString ts;
    unsigned current_loop_count {0};

    auto dbg_ptr=TheAppEngine->Debugger();

    try {

        TheAppEngine->IncrementActiveThreadCount();

        ts=cmd_line.c_str();
        ts.Replace(",,",",")
            ->Replace("  "," ")
            ->Replace(", ",",")
            ->Replace(" ,",",")
            ->Replace("(",",")
            ->Replace(")",",");

        ts.Tokenize(',');
        ts.GetToken(0,function_name);

        while((TheAppEngine->IdleIsRequested()==false) && (current_loop_count++ < loop_count)) {

            dbg_ptr->ProcessEvent(new CmdLineEvent(cmd_line, CmdLineEvent::Type::CmdInProgress, current_loop_count));

            CommandRepository.ExecuteCommand(function_name,CmdRepository::CmdParam(cmd_line.c_str()));

            dbg_ptr->ProcessEvent(new CmdLineEvent(cmd_line, CmdLineEvent::Type::CmdComplete, current_loop_count));

            //StdOutConsole.Write(IOString("# %-.48s (%d) Complete",cmd_line.c_str(),current_loop_count),iStdOut::Filename1);
            //StdOutConsole.Write(IOString("[%-.48s * %d] Complete",function_name.c_str(),current_loop_count),iStdOut::Filename1);
        }
    }
	catch (IOString msg) {

        dbg_ptr->ProcessEvent(new ExceptionEvent(ModuleName, msg));
	}
	catch (std::exception& e) {

        dbg_ptr->ProcessEvent(new ExceptionEvent(ModuleName, e.what()));
	}
	catch(...) {

        dbg_ptr->ProcessEvent(new ExceptionEvent(ModuleName, "Unknown exception"));
	}

    TheAppEngine->DecrementActiveThreadCount();

	return(true);
}

//std::vector<std::shared_ptr<iAobj<CmdParam>>> CmdRepository::iMethodServer::CleanupRepository; /*new*/
