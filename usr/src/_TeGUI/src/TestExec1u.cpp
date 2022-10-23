

#include <TestExec1u.hpp>
#include <UniTestExec-1u.hpp>
#include <UniTestExec_1uWindow.hpp>

#include <QtWidgets>

#include <iostream>
#include <exception>
#include <array>
#include <thread>
#include <chrono>
#include <array>
#include <mutex>

#include <iostream>
#include <fstream>
#include <string>

#include "IOString.hpp"

#ifdef __linux__
#else
#pragma warning (disable:4100)	// forma parameter no referenced
#pragma warning (disable:4018)	// signed, unsigned mismatch
#endif

using std::tuple;
using std::vector;
using std::shared_ptr;
using imx::IOString;

using CmdRepository::CmdParam;

/*************************************************************************************************************************** 
  This file implements a template shared library that uses the 'SharedLib' framework
 
*/

TestExec1u TheModule("UniTestExec 8u GUI");

class iTestIx {
	public:
	void Test() {
	}
};

iTestIx IxTest;
iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {
/*  The module is entered here and a reference to the lib's framework is returned
	1.	register all exported interfaces into the module-local repository 
	2.	register all exported commands into the module-local repository
	3.	'local_ix' is the interface manager local to this module
    */

	//TheModule.Window=CreateUI();

    TheModule.ModuleId=module_id;
	local_ix->iSet<iTestIx>(&IxTest);

    TheModule.TheGui=new UniTestExec_1uWindow();

    TheModule.Window=dynamic_cast<QWidget*>(TheModule.TheGui);
	local_ix->iSet<iTestExecGui_1u>(TheModule.TheGui);

    TheModule.Cmds=local_ix->iGet<CmdRepository::iMethodServer>();

	return(&TheModule); 
}

const char* TestExec1u::GetDescription() {

    static IOString desc("This module implements an 8U test executiveGUI interface.\nUcf052020.1 Build date = ");
    desc+=__DATE__;

	return(desc);
}

int TestExec1u::TestCmd(IOString msg) {

	StdOutConsole.Write("Template module test-tpl output",iStdOut::Heading2);
	return(0);
}

int TestExec1u::Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) {
/*  Initialize the module(s)
	1.	use the interfaces previously registered by the framework and other modules
    2.	the 'ix' interface provided to this function is the global interface
    3.	any interfaces or commands registered here go into the global repository - and will remain after this module is unloaded - dont do it
    */

	using state=iSharedLibFramework<iIXmgr>::ModuleStates;

	try {

        IOString title;

		switch(level) {

			case(state::GetInterfaces):
				/* use the interfaces previously registered by the framework and other modules */

                SetBaseInterfaces(global_ix,ModuleId);

                TheGui->SetMgmtIx(TheShell);
                TheGui-> Initialize(global_ix);

                title=TheAppEngine->iPaths()->GetProfileName();
                TheGui->SetWindowTitle(title);

				StdSysConsole.Write("Test Exec 1U module initialized",iStdOut::Heading2);
				break;

			case(state::StartModule):
				/*	start the module */
				Start();

				Window->resize(900,400);
				Window->show();

				break;

			case(state::MakeExternalConnections):
			/*	make any external connections */

				break;

			case(state::StartProcesses):
			/*	autostart any runtime processes */
				break;

			case(state::Activate):
			/*	wake up an idle module */
				break;

			case(state::SetToIdle):
			/*	set the module to an idle state */
				//if (TheGui) {

                    //	TheGui->ShowDialog(false,"Initialization is Complete");
                    // 	if the system underneath this takes a while to initialize, this is confusing
				//}

				break;

			case(state::Shutdown):
			/*	shutdown the module */

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

int TestExec1u::Start() {

    StdOutConsole.Configure(StdOut,		StdOut->GetRegisteredConsoleHandle("StdOut"));
    StdSysConsole.Configure(StdOut,		StdOut->GetRegisteredConsoleHandle("StdSys"));
    StdTraceConsole.Configure(StdOut,	StdOut->GetRegisteredConsoleHandle("StdTrace"));

    if (Cmds) {

        //Cmds->RegisterApplicationCmd("test-tpl", new Aobj<TestExec1u,IOString>(&TestExec1u::TestCmd,&TheModule),"a template command description");
	}

	return(1);
}

int TestExec1u::SetToIdle() { 
/*  Set the module to an idle condition
1.	After system initialization
2.	Between test sequence executions
*/

	return(1); 
}

int TestExec1u::Activate() { 
/*	'Wakeup' an idle module prior to executing a test sequence */

	return(1); 
}

void TestExec1u::Shutdown() {
/*  Shutdown the lib on program termination
1.	Set any instrumentation to an idle/safe state
2.	Any de-allocations as necessary
*/

	Window->hide();		//	this is the only cmd that seems to have an effect
	Window->close();	//	by itself, this doesn't appear to do anything
    //delete(Window);	// this causes a segment fault when executed from within a 'main' gui.
}


void TestExec1u::HandleModuleException(IOString context) {
/*  this method is called from catch handlers when the exception has a system wide impact
    1.	Information must be displayed to the operation
    2.	Information must be written to local log file(s)
    3.	Test operations must be disabled (possibly not when in a 'development' mode
	4.	This is not called directly from app or system modules where the thrown types may be unknown
    */
	try {

        TheAppEngine->Debugger()->ProcessEvent(new ExceptionEvent (ModuleName,context));
		throw;
	}
	catch(std::exception& e) {

		IOString err("%-.48s::%-.64s exception (%-.128s)",GetName(),context.c_str(),e.what());
		throw(err);
	}
	catch(IOString msg) {

		IOString err("%-.48s::%-.64s exception (%-.128s)",GetName(),context.c_str(),msg.c_str());
		throw(err);
	}

	catch(...) {

		IOString err("%-.48s::%-.64s exception (Undefined)",GetName(),context.c_str());
		throw(err);
	}
}

//vector<std::shared_ptr<iAobj<CmdParam>>> CmdRepository::iMethodServer::CleanupRepository;
