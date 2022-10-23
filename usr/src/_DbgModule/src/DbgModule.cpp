
#include <DbgModule.hpp>

#include <iStdOut.hpp>
#include <iCmdServer.hpp>

#include <exception>
#include <thread>
#include <chrono>

#include <iostream>
#include <string>
#include <cstring>

#include <QMessageBox>

using std::vector;
using imx::IOString;
using imx::TokenString;
using CmdRepository::CmdParam;


/***************************************************************************************************************************
  This file implements a template shared library that uses the 'SharedLib' framework

*/

DbgModule TheModule("Debug Module");

#ifdef __linux__

    #pragma GCC diagnostic ignored "-Wunused-variable"
    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {

#else

    __declspec(dllexport) iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix,unsigned module_id) {

#endif

    local_ix->iSet<iDbg>(&TheModule);

    //local_ix->iSet<iCmdLineProcessor>(&TheModule);

    TheModule.ModuleId=module_id;

    return(&TheModule);
}

int DbgModule::Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) {
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

                startup_thread=std::thread(&DbgModule::ThreadedStartup,this,global_ix);
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

const char* DbgModule::GetDescription() {

    static IOString desc("This module implements a UniConsole command line processor");

    return(desc);
}

void DbgModule::ThreadedStartup(iIXmgr *ix) {

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


using Ucfx=Aobj<DbgModule,CmdParam>;
int DbgModule::Start() {
/*  this function, called after the entry point, and initialize() is necessary:
    1.	GetEntryPoint() is used to register exported interfaces
    2.	Initialize() is used to get required interfaces
    3.	If a module's Initialize() gets an interface, and tries to use it, it may call a function in a module that hasn't been initialized yet (it will not have recieved its
        required interfaces yet.

    entry point: 	get global interfaces, register exported interfaces, and return an interface to this module
    initialize(): 	get all interfaces required by this module
    start(): 		execute the retrieved interfaces in whatever manner
    */

    Cmds=TheAppEngine->iGet<CmdRepository::iMethodServer>(ModuleId);

    if (Cmds) {

        Cmds->RegisterApplicationCmd ("set-cmdline-breakpoint",     new Ucfx(&DbgModule::SetCmdLineBreakPoint,this),            "(cmd name, enable)\tSet/reset a breakpoint on the specified command");
        Cmds->RegisterApplicationCmd ("set-testexec-breakpoint",    new Ucfx(&DbgModule::SetTestExecutionEventBreakPoint,this), "(Test Group Name, enable)\tSet/reset a breakpoint after the specified test group");
        Cmds->RegisterApplicationCmd ("set-ioevent-breakpoint",     new Ucfx(&DbgModule::SetIOEventBreakPoint,this),            "(enable)\tEnable/disable system IO tracing");
        Cmds->RegisterApplicationCmd ("set-data-fail-breakpoint",   new Ucfx(&DbgModule::SetDataCollectionEventBreakPoint,this),"(enable)\tSet/reset a breakpoint on test measurement fails");
        Cmds->RegisterApplicationCmd ("set-except-breakpoint",      new Ucfx(&DbgModule::SetExceptionEventBreakPoint,this),     "(enable)\tSet/reset a breakpoint on the program exceptions");
    }

    StdOutConsole.Configure(StdOut,StdOut->GetRegisteredConsoleHandle("StdOut"));
    StdSysConsole.Configure(StdOut,StdOut->GetRegisteredConsoleHandle("StdSys"));
    StdTraceConsole.Configure(StdOut,StdOut->GetRegisteredConsoleHandle("StdTrace"));

    return(1);
}

void DbgModule::SetStaticConfiguration() {
/*  set the static configuration for the system
    1.	assign system interfaces to individual modules
     */
}

int DbgModule::SetCmdLineBreakPoint(CmdParam msg) {

    bool enable {false};
    imx::IOString command_name;
    TokenString str(msg,',');
    if (str.GetTokenCount()>1) {

        command_name=str.GetIOStrToken(1);
        str.GetToken(2,enable,true);

        StdOutConsole.Write(IOString("Setting a breakpoint on '%-.48s",command_name.c_str()));
        TheAppEngine->Debugger()->SetBreakPoint(enable,new CmdLineEvent(command_name,CmdLineEvent::CmdComplete,0));
    }

    return(1);
}

int DbgModule::SetTestExecutionEventBreakPoint(CmdParam msg) {

    bool enable {false};
    imx::IOString command_name;
    TokenString str(msg,',');
    if (str.GetTokenCount()>2) {

        command_name=str.GetIOStrToken(1);
        str.GetToken(2,enable,true);

        TheAppEngine->Debugger()->SetBreakPoint(enable, new TestExecutionEvent(TestExecutionEvent::UutTestGroupEnd,command_name, "", 0, 0, 0));
        // TestExecutionEvent(TestExecutionEvent::Type type,imx::IOString test, imx::IOString descr, unsigned test_id, unsigned loop_count, unsigned slot) {
    }

    return(1);
}

int DbgModule::SetIOEventBreakPoint(CmdParam msg) {

    bool enable {false};
    TokenString str(msg,',');
    if (str.GetTokenCount()>1) {

        str.GetToken(1,enable);

        TheAppEngine->Debugger()->SetBreakPoint(enable, new IOEvent);
    }

    return(1);
}

int DbgModule::SetDataCollectionEventBreakPoint(CmdParam msg) {

    bool enable {false};
    TokenString str(msg,',');
    if (str.GetTokenCount()>1) {

        str.GetToken(1,enable);

        TheAppEngine->Debugger()->SetBreakPoint(enable, new DataCollectionEvent);
    }

    return(1);
}

int DbgModule::SetExceptionEventBreakPoint(CmdParam msg) {

    bool enable {false};
    TokenString str(msg,',');
    if (str.GetTokenCount()>1) {

        str.GetToken(1,enable, true);

        TheAppEngine->Debugger()->SetBreakPoint(enable, new ExceptionEvent);
    }

    return(1);
}

int DbgModule::Test1(CmdParam msg) {
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

void DbgModule::ProcessEvent(iDebugEvent *event) {

    if (event) {

        if (CmdLineEvent *cle=dynamic_cast<CmdLineEvent*>(event)) { ProcessEvent(cle); }

        else if (TestExecutionEvent *test_exec_event=dynamic_cast<TestExecutionEvent*>(event)) { ProcessEvent(test_exec_event); }

        else if (DataCollectionEvent *data_event=dynamic_cast<DataCollectionEvent*>(event)) { ProcessEvent(data_event); }

        else if (ExceptionEvent *except_event=dynamic_cast<ExceptionEvent*>(event)) { ProcessEvent(except_event); }

        else if (IOEvent *io_event=dynamic_cast<IOEvent*>(event)) { ProcessEvent(io_event); }

        delete(event);
    }
}

void DbgModule::ProcessEvent(IOEvent *event) {

    if (IOBreakPoints.Size()) {

    }
}

void DbgModule::ProcessEvent(ExceptionEvent *event) {

    if (ExceptionBreakPoints.Size()) {

    }
}

void DbgModule::ProcessEvent(DataCollectionEvent *event) {

    imx::IOString msg;
    if (DataBreakPoints.Size()) {

        msg.Fmt(
           "Measurement Data Fail Event\nName Space = %-.48s\nName = %-.48s\nLow limit = %2.4f\nValue = %2.4f\nHigh limit = %2.4f",
            event->NameSpace.c_str(), event->Name.c_str(),event->Lolimit, event->Value, event->Hilimit
            );

        TheShell->UserNotify(true,false,"Measurement Data Breakpoint",msg);
        while(!TheShell->GetButtonSelection()) {

            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }

        TheShell->UserSync(false);
    }
}

void DbgModule::ProcessEvent(CmdLineEvent *event) {

    imx::IOString msg;
    //QMessageBox msgBox;
    switch(event->Event) {

        case(CmdLineEvent::Type::CmdInProgress):

            StdOutConsole.Write(IOString("\n%-.128s (%d) Started",event->CommandLine.c_str(),event->CurrentLoopCount), iStdOut::Filename1);
            if (CmdLineBreakPoints.RecordExists(event)) {
                //StdTraceConsole.Write(IOString(">> Command %-.128s(%d) Started",event->CommandLine.c_str(),event->CurrentLoopCount));
            }

            break;

        case(CmdLineEvent::Type::CmdComplete):

            if (CmdLineBreakPoints.RecordExists(event)) {


                msg.Fmt("Event = 'Command Complete'\nCommand Name = %-.128s\nLoop count = %d ",event->CommandLine.c_str(),event->CurrentLoopCount);
                TheShell->UserNotify(true,false,"Command Breakpoint",msg);
                while(!TheShell->GetButtonSelection()) {

                    std::this_thread::sleep_for(std::chrono::milliseconds(400));
                }
                TheShell->UserSync(false);
#if 0
                msgBox.setIcon(QMessageBox::Information);
                msg.Fmt("%-.128s(...) Completed (Count=%d)",event->CommandLine.c_str(),event->CurrentLoopCount);
                msgBox.setWindowTitle("Command Line Breakpoint");
                msgBox.setText(msg.c_str());
                msgBox.exec();
#endif
                StdTraceConsole.Write(IOString("# %-.128s(%d) Completed",event->CommandLine.c_str(),event->CurrentLoopCount));
            }

            StdOutConsole.Write(IOString("# %-.128s(%d) Complete",event->CommandLine.c_str(),event->CurrentLoopCount));
            break;

        case(CmdLineEvent::Type::CmdInError):

            if (CmdLineBreakPoints.RecordExists(event)) {
                StdTraceConsole.Write(IOString("Command %-.128s(%d) Exception: %-.128s",event->CommandLine.c_str(),event->CurrentLoopCount),iStdOut::Error);
            }

            break;
    }
}

void DbgModule::ProcessEvent(TestExecutionEvent *event) {

    /*
    IOString TestName
    unsigned SlotID
    unsigned CurrentLoopCount
    IOString Description
    IOString ErrorMsg
    iDebugEvent::Type::UutStart
    unsigned TestID
    */

    imx::IOString msg;
    switch(event->Event) {

        case(TestExecutionEvent::Type::UutTestBegin):

            if (ExecBreakPoints.RecordExists(event)) {

                StdTraceConsole.Write(IOString("\n+ Test ID =%d, Test name =%-.128s, SlotID = %d, Loop count = %d",event->TestID,event->TestName.c_str(),event->SlotID, event->CurrentLoopCount));
            }

            break;

        case(TestExecutionEvent::Type::UutTestEnd):
            
            if (ExecBreakPoints.RecordExists(event)) {

                StdTraceConsole.Write(IOString("    - Test ID =%d, Test name =%-.128s, SlotID = %d, Loop count = %d",event->TestID,event->TestName.c_str(),event->SlotID, event->CurrentLoopCount));
            }

            break;

        case(TestExecutionEvent::Type::UutTestGroupBegin):

            if (ExecBreakPoints.RecordExists(event)) {
            }

            break;

        case(TestExecutionEvent::Type::UutTestGroupEnd):

            if (ExecBreakPoints.RecordExists(event)) {

                msg.Fmt("Break on Test Group End\nGroup Name = %-.48s\nLoop Count = %d", event->TestName.c_str(), event->CurrentLoopCount);
                TheShell->UserNotify(true,false,"Test Group Breakpoint",msg);
                while(!TheShell->GetButtonSelection()) {

                    std::this_thread::sleep_for(std::chrono::milliseconds(400));
                }

                TheShell->UserSync(false);
            }

            break;
    }
}

void DbgModule::SetBreakPoint (bool set, iDebugEvent *event) {

    if (event) {

        if (set) {
            CmdLineBreakPoints.InsertUnique(event)
            ||
            ExecBreakPoints.InsertUnique(event)
            ||
            DataBreakPoints.InsertUnique(event)
            ||
            ExceptionBreakPoints.InsertUnique(event)
            ||
            IOBreakPoints.InsertUnique(event);
        }
        else {

            CmdLineBreakPoints.Delete(event)
            ||
            ExecBreakPoints.Delete(event)
            ||
            DataBreakPoints.Delete(event)
            ||
            ExceptionBreakPoints.Delete(event)
            ||
            IOBreakPoints.Delete(event);
        }

        delete(event);
    }
}

int DbgModule::SetToIdle() {
/*  Set the module to an idle condition
    1.  After system initialization
    2.  Between test sequence executions
*/

    return(1);
}

int DbgModule::Activate() {
/*	'Wakeup' an idle module prior to executing a test sequence */

    return(1);
}

void DbgModule::Shutdown() {
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


void DbgModule::CleanUpCmdLine(TokenString &str) {

    str.Replace(",,",",")
        ->Replace("  "," ")
        ->Replace(", ",",")
        ->Replace(" ,",",")
        ->Replace("(",",")
        ->Replace(")",",")
        ->Tokenize(',');
}

void DbgModule::HandleModuleException(IOString context) {
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

int DbgModule::ClearAllBreakPoints(CmdParam msg) {

    StdOutConsole.Write("Clearing all breakpoints");

    CmdLineBreakPoints.Clear();
    ExecBreakPoints.Clear();
    DataBreakPoints.Clear();
    ExceptionBreakPoints.Clear();
    IOBreakPoints.Clear();

    return(1);
}

