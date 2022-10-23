
#include <GenericModule.hpp>

#include <iStdOut.hpp>
#include <iStreamIO.hpp>
#include <iStdIOChannelSystem.hpp>

#include <TokenString.hpp>

#include <exception>
#include <array>
#include <thread>
#include <chrono>
#include <array>
#include <mutex>
#include <vector>
#include <functional>

#include <iostream>
#include <fstream>
#include <string>


using std::tuple;
using std::vector;
using std::shared_ptr;
using sys::IOString;
using CmdRepository::CmdParam;


/*************************************************************************************************************************** 
  This file implements a template shared library that uses the 'SharedLib' framework
 
*/

Template_Module TheModule("Template Module");

iStdIOChannelSystem *TheSystem {nullptr};

#ifdef __linux__

	#pragma GCC diagnostic ignored "-Wunused-variable"
    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {

#else

    __declspec(dllexport) iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {
		
#endif

	//local_ix->iSet<iStdIOChannelSystem>(&TheSystem);
    TheModule.ModuleId=module_id;
	TheModule.Cmds=local_ix->iGet<CmdRepository::iMethodServer>();

	return(&TheModule); 
}

int Template_Module::Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) {
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

				WriteStdTrace("Template module read interfaces", iTrace::BlockStart);
				
				SetBaseInterfaces(global_ix);
				
                TheSystem=global_ix->iGet<iStdIOChannelSystem>(ModuleId);

				WriteStdTrace("Template module read interfaces", iTrace::BlockStop);

				break;

			case(state::StartModule):
			/*	start the module */

				Start();
				break;

			case(state::MakeExternalConnections):
			/*	make any external connections */
				break;

			case(state::StartProcesses):
			/*	autostart any runtime processes */
			
                SetStaticConfiguration();
				
				startup_thread=std::thread(&Template_Module::ThreadedStartup,this,global_ix);
				startup_thread.detach();
				break;

			case(state::Activate):
			/*	wake up an idle module */
				break;

			case(state::SetToIdle):
			/*	set the module to an idle state */
				SetToIdle();
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

		WriteStdTrace("Std System Module loaded with errors", iTrace::Error);
		HandleModuleException("Initialization");
	}

	return(1);
}

const char* Template_Module::GetDescription() {
    static IOString desc("This module implements a generic UniConsole module. This can be generic, a system, a test, etc.");

	return(desc);
}

void Template_Module::ThreadedStartup(iIXmgr *ix) {

	try {

        TheAppEngine->IncrementActiveThreadCount();
		/*	let the appengine know that a thread is running (aka, keep the progress bar going */

        WriteComposite("Starting Pls System Initialization\n",iStdOut::Heading1);
		if (TheSystem) {

			//if (ThePlsSystem.Initialize()) {

                //TheUUT.SetExternalTemperature(TheUUT.MeasureExternalTemperature());
				//WriteStdSystem(IOString("External temperature = %2.2fC",TheUUT.GetExternalTemperature()),iStdOut::Text1);

                //ThePlsSystem.EnableDutAcPower(false,true);
                //ThePlsSystem.EnableDutAcLoadOutput(false,true);

			//}
			//else {

                //TheAppEngine->SetHardError(ThePlsSystem.GetLastError().c_str());
                //TheConsole->ShowDialog(true,"Unable to initialize system\n\nEnter 'status' and the command line input\nto display errors");
            //}
		}
	}
	catch(IOString msg) {

		TheAppEngine->SetHardError(msg);
	}
	catch(...) {

		TheAppEngine->SetHardError("Unknown exception while initializing the system");
	}

    TheAppEngine->ShowStatus();
	/*	display the startup status */

    TheAppEngine->DecrementActiveThreadCount();
}


int Template_Module::Start() {
/*  this function, called after the entry point, and initialize() is necessary:
    1.	GetEntryPoint() is used to register exported interfaces
    2.	Initialize() is used to get required interfaces
    3.	If a module's Initialize() gets an interface, and tries to use it, it may call a function in a module that hasn't been initialized yet (it will not have recieved its
	    required interfaces yet.
 
    entry point: 	get global interfaces, register exported interfaces, and return an interface to this module
    initialize(): 	get all interfaces required by this module
    start(): 		execute the retrieved interfaces in whatever manner
    */

	//GuardedTraceBlock([this](const char *msg, int level) { WriteStdTrace(msg,level); },"Std System Module");

	GuardedTraceBlock(WriteStdTrace,"Std System Module");

	if (TheSystem) {

    }

	if (Cmds) {

        Cmds->RegisterApplicationCmd("tmp-test", new Aobj<Template_Module,CmdParam>(&Template_Module::Test,this),			"()\tA test-bed function\ttmp-test");
		//Cmds->RegisterCmd("ping-db",	new Aobj<Template_Module,IOString>(&Template_Module::PingDb,this),					"Connect to the system database");
	}

	return(1);
}

void Template_Module::SetStaticConfiguration() {
/*	set the static configuration for the system 
	1.	assign system interfaces to individual modules
	*/
	
	iIOChannelDevice *test_bed_1=TheSystem->GetTmpIOChannelDevice(1);
	test_bed_1->Open();
	test_bed_1->Write("xxyyzz");
	test_bed_1->SetReadBuffer(1000);
	test_bed_1->Read();
	test_bed_1->ClearReadBuffer();
	
	
	iIOChannelDevice *test_bed_2=TheSystem->RegisterIOChannelDevice(1,"Test-bed interface");
	test_bed_2->Open();
	test_bed_2->Write("xxyyzz");
	test_bed_2->SetReadBuffer(1000);
	test_bed_2->Read();
	test_bed_2->ClearReadBuffer();
	/* 	Get a device interface. Interfaces include
		
		bool Open(bool throw_on_error=true)=0;
		unsigned Write(char *data, int length=-1,unsigned wait_ms=0)=0;
		unsigned Read()=0;
		char* ClearReadBuffer()=0;
		*/  
}

int Template_Module::Test(CmdParam msg) {

	WriteStdOut("Test output from template module",iStdOut::Text1);

	return(1);
}


int Template_Module::SetToIdle() { 
/*  Set the module to an idle condition
1.	After system initialization
2.	Between test sequence executions
*/

	return(1); 
}

int Template_Module::Activate() { 
/*	'Wakeup' an idle module prior to executing a test sequence */

	return(1); 
}

void Template_Module::Shutdown() {
/*  Shutdown the lib on program termination
	1.	Set any instrumentation to an idle/safe state
	2.	Any de-allocations as necessary
	*/

	//TheSystem.Shutdown();
}


void Template_Module::CleanUpCmdLine(TokenString &str) {

	str.Replace(",,",",")
		->Replace("  "," ")
		->Replace(", ",",")
		->Replace(" ,",",")
		//->Replace("(",",")
		//->Replace(")",",")
		->Tokenize(',');
}

void Template_Module::HandleModuleException(IOString context) {
/*  this method is called from catch handlers when the exception has a system wide impact
    1.	Information must be displayed to the operation
    2.	Information must be written to local log file(s)
    3.	Test operations must be disabled (possibly not when in a 'development' mode
	4.	This is not called directly from app or system modules where the thrown types may be unknown
    */
	try {

		WriteStdOut(IOString("Application module exception in %-.48s (%-.255s)",ModuleName.c_str(),context.c_str()),iStdOut::Error);

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


vector<shared_ptr<iFx>> iCmdServer::CleanupRepository;
//std::vector<std::shared_ptr<iAobj<CmdParam>>> CmdRepository::iMethodServer::CleanupRepository;
