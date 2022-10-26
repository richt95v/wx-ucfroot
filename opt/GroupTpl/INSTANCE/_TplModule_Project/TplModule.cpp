

#include <TplModule.hpp>

#include <iStdOut.hpp>
#include <iStreamIO.hpp>
#include <iTestExec-1u.hpp>
#include <iStdIOChannelSystem.hpp>

#include <exception>
#include <thread>
#include <chrono>

#include <iostream>
#include <string>

#include <iUniConsole.hpp>
#include <iUcfTestExec.hpp>
#include <TokenString.hpp>
#include <SerialConsole.hpp>
#include <GuardedNotification.hpp>

using std::vector;
using imx::IOString;
using imx::TokenString;
using CmdRepository::CmdParam;

iUniConsole::Console *TheShell {nullptr};

/*************************************************************************************************************************** 
  This file implements a template shared library that uses the 'SharedLib' framework
 
*/

TplModule 		TheModule("Template Module");
iTestUtility      	*TheTestExec 	{nullptr};
iStdIOChannelSystem 	*TheSystem 	{nullptr};

#ifdef __linux__

    #pragma GCC diagnostic ignored "-Wunused-variable"
    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {

#else

    __declspec(dllexport) iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {
		
#endif

    TheModule.ModuleId=module_id;
    TheModule.Cmds=local_ix->iGet<CmdRepository::iMethodServer>();

    return(&TheModule); 
}

int TplModule::Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) {
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

			    TheSystem=global_ix->iGet<iStdIOChannelSystem>(ModuleId, "Unable to find System interface");
			    TheTestExec=global_ix->iGet<iTestUtility> (ModuleId);

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
				
			    startup_thread=std::thread(&TplModule::ThreadedStartup,this,global_ix);
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
    catch(IOString msg) {

        StdTraceConsole.Write(msg, iStdOut::Error);
        HandleModuleException("Initialization");
    }

    catch(...) {

        StdTraceConsole.Write("Std System Module loaded with errors", iStdOut::Error);
        HandleModuleException("Initialization");
	}

	return(1);
}

const char* TplModule::GetDescription() {

    static IOString desc("This module implements a generic UniConsole module. This can be generic, a system, a test, etc.");

    return(desc);
}

void TplModule::ThreadedStartup(iIXmgr *ix) {

    try {

        TheAppEngine->IncrementActiveThreadCount();
        /* let the appengine know that a thread is running (aka, keep the progress bar going */

        iUniConsole::GuardedNotification notification(TheShell, "System Initialization","Please wait while the system is initializing");

        if (TheSystem) {

            std::this_thread::sleep_for(std::chrono::seconds(4));
        }
        else {

            TheAppEngine->iErrorManager()->SetHardError("Unable to initialize the system interface");
            //TheConsole->ShowDialog(true,"Unable to initialize system\n\nEnter 'status' and the command line input\nto display errors");
        }
    }
    catch(IOString msg) {

        TheAppEngine->iErrorManager()->SetHardError(msg);
    }
    catch(...) {

        TheAppEngine->iErrorManager()->SetHardError("Unknown exception while initializing the system");
    }

    //TheAppEngine->ShowStatus();
	/*	display the startup status */

    TheAppEngine->DecrementActiveThreadCount();
}


using Ucfx=Aobj<TplModule,CmdParam>;
int TplModule::Start() {
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

    Cmds=TheAppEngine->iGet<CmdRepository::iMethodServer>(ModuleId);

    if (TheSystem) {

    }

    if (Cmds) {

        Cmds->RegisterApplicationCmd 	("TcpTest",		new Ucfx(&TplModule::TestTcp,this),			"(message)\tWrite 'message' to Dev2 and read back twice");
        Cmds->RegisterApplicationCmd 	("d1-x",		new Ucfx(&TplModule::Debug1,this),			"(message)\tWrite 'message' to Dev0 and read back");
        Cmds->RegisterApplicationCmd 	("app cmd",		new Ucfx(&TplModule::NotifyTest,this),		"()\tThis is a application command function");
        Cmds->RegisterSystemCmd		 	("sys-cmd",		new Ucfx(&TplModule::NotifyTest,this),		"()\tThis is a system command function");
        Cmds->RegisterFrameworkCmd 		("fm-cmd",		new Ucfx(&TplModule::NotifyTest,this),		"()\tThis is a framework function");
        Cmds->RegisterAdminCmd		 	("admin-cmd",	new Ucfx(&TplModule::NotifyTest,this),		"())\tThis is an admin function");
    }

    if (TheTestExec) {

        TheTestExec->RegisterTestMgr(new Aobj<TplModule,TestXtor>(&TplModule::ManageTestSequences,this));

        TheTestExec->RegisterTestFunction("GetSn",	"()\tGet the UUT serial number",            new Ucfx(&TplModule::Test1, this));
        TheTestExec->RegisterTestFunction("Notifications",	"Test notifications",				new Ucfx(&TplModule::NotifyTest, this));
        TheTestExec->RegisterTestFunction("Test 2",	"Failing boolean test",		                new Ucfx(&TplModule::Test2, this));
        TheTestExec->RegisterTestFunction("Test 3",	"Passing test",								new Ucfx(&TplModule::Test3, this));
        TheTestExec->RegisterTestFunction("Test 4",	"Passing test",								new Ucfx(&TplModule::Test4, this));
        TheTestExec->RegisterTestFunction("Test 5",	"Passing test",								new Ucfx(&TplModule::Test5, this));
        TheTestExec->RegisterTestFunction("Test 6",	"Failing boolean test",                     new Ucfx(&TplModule::Test6, this));
    }

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

int TplModule::ManageTestSequences(TestXtor xtor) {
/*  manage sequence and session construction/destruction

    throw(IOString msg) to terminate the sequence
    */

    int ret {1};

    switch(xtor.GetXtorType()) {

        case(TestXtorType::SequenceCtor):
            break;

        case(TestXtorType::SessionCtor):
            break;

        case(TestXtorType::SessionDtor):
            break;

        case(TestXtorType::SequenceDtor):
            break;
    }

    return(ret);
}

void TplModule::SetStaticConfiguration() {
/*  set the static configuration for the system 
    1.	assign system interfaces to individual modules
	 */

    iIOChannelDevice *SerialPort1 { nullptr };

    /***********************************************************************************************************************
    Open connections to instrumentation here
    */

    int size {0};
    char *cptr {nullptr};

    unsigned char negotion1[] {
        0xFF, 0xFC, 0x18,           //  wont send location
        0xFF, 0xFC, 0x20,           //  wont negotiate window size
        0xFF, 0xFC, 0x23,           //  wont negotiate x window location
        0xFF, 0xFC, 0x27,           //  wont create new environment variables
        0xFF, 0xFC, 0x24            //  wont send environment variables
        };

    unsigned char negotion2[] {
        0xFF, 0xFC, 0x03,           //  won't suppress 'go-ahead'
        0xFF, 0xFC, 0x01,           //  wont echo
        0xFF, 0xFC, 0x1f,           //  wont negotiate window size
        0xFF, 0xFC, 0x05,           //  wont show status
        0xFF, 0xFC, 0x21            //  wont use remote flow control
    };

#if 0
    auto tcp=TheSystem->GetTmpIOChannelDevice(2);                               // use this for connections to the uut
    //auto tcp =TheSystem->RegisterIOChannelDevice(2,"A Telnet connection");    // on use this if connecting to a
                                                                                // module that is part of the test system
    if (tcp->Open()) {

//#if 0
        tcp->Write("abcd");
        size=tcp->Read();
        cptr=tcp->GetReadBuffer();
        // 'nc -lp 1299' to get this test case to work
//#endif


//#if 0	//	telnet negotion code
        tcp->ClearReadBuffer();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        //size=tcp->Read();
        tcp->Write((char*)negotion1,15);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        //size=tcp->Read();

        //tcp->ClearReadBuffer();
        tcp->Write((char*)negotion2,15);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        size=tcp->Read();

        tcp->ClearReadBuffer();
        tcp->Write("rich\r");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        tcp->Write("rootxx\r");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        size=tcp->Read();
        cptr=tcp->GetReadBuffer();
//#endif

        tcp->Close();
    }
#endif

    //SerialPort1=TheSystem->RegisterIOChannelDevice(1,"A generic udp channel");
    //SerialPort1 = TheSystem->GetIOChannel("DEV0", "A description", true);
#if 0
    if (SerialPort1->Open()) {

        char buffer[64];

        sprintf(buffer,"status\r");
        SerialPort1->Write(buffer);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        SerialPort1->Read();

        TokenString str(SerialPort1->GetReadBuffer(),',');
    }
    else {

        TheAppEngine->iErrorManager()->SetHardError("Unable to open serial communication channel");
    }
#endif

#if 0
    SerialPort1=TheSystem->RegisterIOChannelDevice(0,"A generic serial channel");
    if (SerialPort1->Open()) {

        char buffer[64];

        sprintf(buffer,"status\r");
        SerialPort1->Write(buffer);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        SerialPort1->Read();

        TokenString str(SerialPort1->GetReadBuffer(),',');
    }
    else {

        TheAppEngine->iErrorManager()->SetHardError("Unable to open serial communication channel");
    }
#endif
    /***********************************************************************************************************************
    Open connections to instrumentation here
    */

}

int TplModule::NotifyTest(CmdParam msg) {
/*	Get the serial number from the UUT */

    iUniConsole::GuardedNotification notification(TheShell, "Generic Notification","Text message ...");
    std::this_thread::sleep_for(std::chrono::seconds(4));

    return(1);
}

int TplModule::Test1(CmdParam msg) {
/*	Get the serial number from the UUT */

    iUcfTestExec exec(msg,ModuleId);
    exec.SetSn("...");

    return(1);
}

int TplModule::Test2(CmdParam msg) {

    bool status {true};
    iUcfTestExec exec(msg,ModuleId);
    bool meas_a {false};
    bool meas_b {false};

	/* 	make measurements
		1. bool meas_a
		2. bool meas_b
        */

    if (exec.IsValid()) {

        exec.SaveMeasurement("Test A","IsValid",meas_a,1,1);
        exec.SaveMeasurement("Test A","IsValid",meas_b,1,1);
    }
    else {

        StdOutConsole.Write("Test A ... Validation", meas_a? iStdOut::Pass: iStdOut::Error);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return(1);
}

int TplModule::Test3(CmdParam msg) {

    bool status {true};
    iUcfTestExec exec(msg,ModuleId);

    if (exec.IsValid()) {

        exec.SaveMeasurement("Test B","IsValid",status,1,1);
    }
    else {

        StdOutConsole.Write("Test B ... Validation", status ? iStdOut::Pass: iStdOut::Error);
    }

    return(1);
}

int TplModule::Test4(CmdParam msg) {

    bool status {true};
    iUcfTestExec exec(msg,ModuleId);

    if (exec.IsValid()) {

        exec.SaveMeasurement("Test C","IsValid",status,1,1);
    }
    else {

    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return(1);
}

int TplModule::Test5(CmdParam msg) {

    bool status {true};
    iUcfTestExec exec(msg,ModuleId);

    if (exec.IsValid()) {

        exec.SaveMeasurement("Test D","IsValid",status,1,1);
    }
    else {

    }

    return(1);
}

int TplModule::Test6(CmdParam msg) {

    bool status {false};
    iUcfTestExec exec(msg,ModuleId);

    if (exec.IsValid()) {

        exec.SaveMeasurement("Test E","IsValid",status,1,1);
    }
    else {

    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return(1);
}

int TplModule::TestTcp(CmdParam msg) {
/*	test an ethernet TCP connection using 'nc -lp 1299' on the host system to provide a temp server
	1.	write a message to the server
    2.	read twice twice from the server to :
		- open a non-existant target
		- test 20s blocking timeout
        - read capability
        - write to a port with nothing on it
        */

    int size {0};
    char* cptr {nullptr};
    TokenString params(msg,',');
    std::string message_to_write {"aabbcc"};

    auto tcp=TheSystem->GetTmpIOChannelDevice(2);                               
                                                                               
    if (params.GetTokenCount() > 1) {
        
        params.GetToken(1, message_to_write);
    }

    try {

        if (tcp->Open()) {

            tcp->Write((char*)message_to_write.c_str());
            
            StdOutConsole.Write("Attempting read #1", iStdOut::Text1);
            size = tcp->Read();
            cptr = tcp->GetReadBuffer();
            if (size == 0) {

                StdOutConsole.Write("Timeout occurred during read #1", iStdOut::Error);
            }
            else {

                StdOutConsole.Write(cptr, iStdOut::Text1);
            }

            StdOutConsole.Write("Attempting read #2 (type something into the server response)", iStdOut::Text1);
            tcp->ClearReadBuffer();
            size = tcp->Read();
            cptr = tcp->GetReadBuffer();
            StdOutConsole.Write(cptr, iStdOut::Text1);

            tcp->Close();
        }
    } 
    catch (IOString msg) {

            StdOutConsole.Write(msg.c_str(), iStdOut::Error);
    }

    return(1);
}

int TplModule::Debug1(CmdParam msg) {

    bool status{ false };
    std::string token_str;
    TokenString params(msg,',');
 
    TheShell->
        UserSync(
            true,
            false,
            "Title",
            "message line1\n"
            "message line2\n"
            "message line3\n"
            "line 4\n"
            "line 5\n"
            "line 6\n"
            "line 7\n"
            "line 8\n"
            "line 9\n"
            "line 10\n"
        );

    while (TheShell->GetButtonSelection() == false) {

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    TheShell->UserSync(false);

#if 0
    imx::StringBuffer buffer;
    buffer.PushBack("line1");
    buffer.PushBack("line2");
    buffer.PushBack("line3");
    buffer.PushBack("line3");
    buffer.PushBack("line4");
    buffer.PushBack("line3");
    buffer.PushBack("line5");
    buffer.PushBack("line3");

    std::string pattern("line3");
    std::string line;

    int index1 = buffer.ScanForSubString(pattern, line);
    index1 = buffer.ScanForSubString("xxyyzz", line);

    int count = buffer.RemoveLinesThatContain(pattern);
    count = buffer.RemoveLinesThatDoNotContain("line4");
    /* test case 1 */

    index1 = buffer.ScanForSubString("line5", line);

#endif

#if 0
    char* buffer { "ABCD\0" };

    auto port= TheSystem->RegisterIOChannelDevice(2,"A generic tcp channel");
    if (port->Open()) {
        port->Write(buffer);
        port->Read();

        port->Close();
    }
#endif

#if 0
    SerialConsole console(TheSystem->GetTmpIOChannelDevice(0));
    if (console.Connect()) {

        std::string message, message_return;
        if (params.GetTokenCount() > 1) {

            params.GetToken(1,message);
            console.Write(message.c_str(), 1000);     // delay is in ms

            console.ClearLineBuffer();
            console.Read();
            imx::stringBuffer &buffer = console.GetLineBuffer();

            TokenString ts;
            for (int index = 0; index < buffer.GetLineCount(); index++) {

                buffer.GetLine(index, ts);
                StdOutConsole.Write(IOString(ts.StdStr()));
            }

        }
        else {

            console.WriteRead("11\n22\n33\n44,55,66\n77", 100);     // delay is in ms

            if (console.ReadBufferContains("44", token_str)) {

                TokenString result(token_str,',');
                result.GetToken(1, token_str);   // returns "55"
            }

            /* alternate scan method */

            int line = console.LineBuffer.ScanForSubString("4", token_str);
            if (line) {

                TokenString result;
                console.LineBuffer.GetLine(line, result);

                result.Tokenize(',');
                result.GetToken(1, token_str);
            }
        }

       console.Disconnect();
    }
#endif

    return(1);
}

int TplModule::SetToIdle() {
/*  Set the module to an idle condition
    1.  After system initialization
    2.  Between test sequence executions
*/

	return(1); 
}

int TplModule::Activate() { 
/*	'Wakeup' an idle module prior to executing a test sequence */

	return(1); 
}

void TplModule::Shutdown() {
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

void TplModule::HandleModuleException(IOString context) {
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

#if 0
iUcfTestExec::iUcfTestExec(
    CmdParam &msg,
    unsigned long &module_id
    ):Msg(msg) {

    if (TheExec=Msg.iGet<iTestUtility>(module_id)) {

        Slot=Msg.GetOption();
    }
}

iUcfTestExec* iUcfTestExec::SaveMeasurement( const char *name_space, const char *name, double value) {

    double lo_limit {value+.5};
    double hi_limit {value-.5};
    /*  default conditions used when value == 0 */

    if (value>0) {

        lo_limit=.7*value;
        hi_limit=value*1.3;
    }
    else {

        hi_limit=.7*value;
        lo_limit=value*1.3;
    }

    if (TheExec) {

        TheExec->SaveMeasurement(Slot,name_space, name,value, lo_limit, hi_limit, true );
    }

    return(this);
}

iUcfTestExec* iUcfTestExec::SaveMeasurement(
    const char *name_space,
    const char *name,
    double value,
    double lo_limit,
    double hi_limit,
    bool update_test_spec
    ) {

    if (TheExec) {

        TheExec->SaveMeasurement(Slot,name_space, name,value, lo_limit, hi_limit, update_test_spec);
    }

    return(this);
}

iUcfTestExec* iUcfTestExec::GetMfgId(unsigned &id, imx::IOString name, imx::IOString location) {

    if (TheExec) {

        TheExec->GetMfgID(id,name,location);
    }

    return(this);
}

iUcfTestExec* iUcfTestExec::SetSn(const char *sn) {

    if (TheExec) {

        TheExec->SetSn(sn,Slot);
    }

    return(this);
}

const char* iUcfTestExec::GetSn() {

    if (TheExec) {

        return(TheExec->GetSn(Slot));
    }

    return(nullptr);
}

bool iUcfTestExec::WriteTestOutput(imx::IOString msg, int type) {

    if (TheExec) {

        TheExec->WriteTestOutput(Slot, msg, type);
        return(true);
    }

    return(false);
}

bool iUcfTestExec::IsValid() { return(TheExec ? true:false); }
#endif


//std::vector<std::shared_ptr<iAobj<CmdParam>>> CmdRepository::iMethodServer::CleanupRepository;
