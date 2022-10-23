

#include <TestExec1uSequencer.hpp>

#include <iTestExecGui-1u.hpp>

#include <thread>
#include <optional>

#include <iUniConsole.hpp>
#include <TokenString.hpp>
#include <StringEx.hpp>


#include <exception>
#include <array>
#include <thread>
#include <chrono>
#include <array>
#include <mutex>
#include <future>
#include <list>

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#include <IOString.hpp>
#include <iTestDb.hpp>
#include <iDbg.hpp>

#include <StripChartTimer.hpp>

using std::tuple;
using std::vector;
using std::shared_ptr;
using CmdRepository::CmdParam;
using TimerFx=Aobj<TestExec1uSequencer,unsigned long>;

using imx::IOString;
using imx::TokenString;

using std::ofstream;
using std::ios;

#ifdef __linux__

	#pragma GCC diagnostic ignored "-Wunused-variable"
#else

#pragma warning (disable:4100)
#endif


/*************************************************************************************************************************** 
  This file implements a template shared library that uses the 'SharedLib' framework
 
*/

TestExec1uSequencer TheModule("UniConsole 8u Test Sequencer");

iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *local_ix, unsigned module_id) {
/*  The module is entered here and a reference to the lib's framework is returned
	1.	register all exported interfaces into the module-local repository 
	2.	register all exported commands into the module-local repository
	3.	'ix' is the interface manager local to this module
    */

	TheModule.Cmds=local_ix->iGet<CmdRepository::iMethodServer>();

    local_ix
		->iSet<iTestExec_1u>(&TheModule)
		->iSet<iTestUtility>(&TheModule);

	return(&TheModule); 
}

const char* TestExec1uSequencer::GetDescription() {
    static IOString desc("This module implements an 8U test sequencer.\nUcf052020.1 Build date = ");
	desc+=__DATE__;

	return(desc);
}

void TestExec1uSequencer::SetWindowTitle(IOString title) {
	
    if (unique_ix<iTestExecGui_1u> gui(TheIxRepository); gui) {

        gui->SetWindowTitle(title);
    }
}

int TestExec1uSequencer::StartTestCmd(CmdParam params) {

	bool stop_on_first_fail {false};

	StdOutConsole.Write("Starting test execution",iStdOut::Text1);

    StartTest(stop_on_first_fail,1);

	return(1);
}

int TestExec1uSequencer::ClearMeasurements(CmdParam params) {

	StdOutConsole.Write("Clearing all measurement data",iStdOut::Text1);
    TheDb->ResetData();

	return(1);
}

int TestExec1uSequencer::ClearLimits(CmdParam params) {

	StdOutConsole.Write("Clearing all test limits",iStdOut::Text1);
    TheDb->ResetLimits();
	return(1);
}

void TestExec1uSequencer::WriteTestOutput(unsigned test_slot, IOString msg, int type) {

	if (StdOut) StdOut->WriteStdOut(msg.c_str(),type,Slots.GetConsoleID(test_slot));
}

bool TestExec1uSequencer::SaveMeasurement(unsigned test_slot, const char *name_space, const char *name, double value,  double lo_limit, double hi_limit,bool update_test_list) {
/*  save a measurement record. 3 types exist
    1.	qualified named param, test limits are checked, and automatically updated
    2.	dynamic named param, name is set by the test, test limits are checked against name_space::*, and automatically updated
    3.	floating param, value is saved like a qualified named param, but test limits aren't checked or updated
    4.	return true on a successful data save
    */

    bool status {false};
    unsigned current_pass_fail_status {2};

	unique_ix<iTestExecGui_1u> gui(TheIxRepository);

    current_pass_fail_status=TheDb->VerifyTestLimits(name_space, name, value, lo_limit, hi_limit, update_test_list);
    /*	find the pass/fail status and set the current slot status accordingly */

    TheAppEngine->Debugger()->ProcessEvent(new DataCollectionEvent(name_space, name, value, lo_limit, hi_limit));

    status=TheDb->SaveMeasurement(
        Slots.GetUutInstance(test_slot),
        SequenceTimer.GetCurrentDelay(),
        TestSessionIndex.GetSessionLoopCount(),
        TestSessionIndex.GetTestIndex(),
        name_space,
        name,
        value,
        current_pass_fail_status
        );

    IOString output_msg;
    switch (current_pass_fail_status) {

        default:
        case(0):
            output_msg.Fmt("%-.48s :: %-.48s [%2.3f, %2.3f, %2.3f] Failed",name_space, name,lo_limit,value,hi_limit);
            WriteTestOutput(test_slot,output_msg,iStdOut::Error);
            WriteLastTestLogFile(output_msg.c_str());
            break;

        case(1):
            output_msg.Fmt("%-.48s :: %-.48s [%2.3f, %2.3f, %2.3f] Warning",name_space, name,lo_limit,value,hi_limit);
            WriteTestOutput(test_slot,output_msg,iStdOut::Warning);
            WriteLastTestLogFile(output_msg.c_str());
            break;

        case(2):
            output_msg.Fmt("%-.48s :: %-.48s [%2.3f, %2.3f, %2.3f] Pass",name_space, name,lo_limit,value,hi_limit);
            WriteTestOutput(test_slot,output_msg,iStdOut::Pass);
            WriteLastTestLogFile(output_msg.c_str());
            break;
    }

    if (TestSequenceIsCurrentlyInProgress()) {

        Slots.UpdateTestSessionStatus(test_slot,current_pass_fail_status);

        if (gui) {
        /*	only upate the gui if
            1.	it is present
            2.	data is being saved from within a test sequene
            */

            gui->UpdateTestStatus(test_slot, TestSessionIndex.GetTestIndex(), Slots.GetTestSequenceStatus(test_slot));
            /*	update the current test group with test status */

            gui->UpdateTestStatus(test_slot, 0, Slots.GetTestSequenceStatus(test_slot));
            /* 	update the serial number field (row 0) */
        }
	}

    return(status);
    /*	return true if data was successfully saved */
}

bool TestExec1uSequencer::SaveStripChartMeasurement(unsigned test_slot, const char *name_space, const char *name, double value) {
/*  save a measurement record. 3 types exist
    1.	qualified named param, test limits are checked, and automatically updated
    2.	dynamic named param, name is set by the test, test limits are checked against name_space::*, and automatically updated
    3.	floating param, value is saved like a qualified named param, but test limits aren't checked or updated
    */

    TheDb->SaveMeasurement(
        Slots.GetUutInstance(test_slot),
        SequenceTimer.GetCurrentDelay(),
        TestSessionIndex.GetSessionLoopCount(),
        TestSessionIndex.GetTestIndex(),
        name_space,
        name,
        value,
        3
        );

    return(true);
}

bool TestExec1uSequencer::SetTestSessionID() {

	bool status {false};
    unsigned mfg_id {0};

    TestProfileName=TheAppEngine->iPaths()->GetProfileName();
	MfgName.clear();
	MfgLocation.clear();

    if (
        TheDb
        &&
        TheDb->GetTestSpecName(TestProfileName,TestSpecName,DutTypeName)
        &&
        TheDb->GetMfgID(mfg_id,MfgName,MfgLocation)
        ) {

		StdOutConsole.Write(IOString("Test Profile = '%-.128s'",TestProfileName.c_str()),iStdOut::Filename1);
		StdOutConsole.Write(IOString("Test Specification = '%-.128s'",TestSpecName.c_str()),iStdOut::Filename1);
		StdOutConsole.Write(IOString("Test Location ID = '%d %-.64s @ %-.64s'",mfg_id,MfgName.c_str(),MfgLocation.c_str()),iStdOut::Filename1);
		StdOutConsole.Write("Test Sequencer 1U module initialized",iStdOut::Heading2);

        TheDb->SetTestSessionIndex(MfgName,MfgLocation);
        status=true;
	}
	else {

		StdOutConsole.Write("Unable to set Test Session ID",iStdOut::Warning);
		StdOutConsole.Write("- The measurement database must be present.",iStdOut::Warning);
        StdOutConsole.Write("- A test specification must be associated with the specified profile.",iStdOut::Warning);
        StdOutConsole.Write("- The manufacturer's ID, name and location must be specified.",iStdOut::Warning);
    }

	return(status);
}

bool TestExec1uSequencer::GetUutID() {

	return(true);
}

bool TestExec1uSequencer::LaunchStartTest( bool stop_on_first_fail, unsigned loop_count) {

	using namespace std;

    ThreadOfExecution=async(launch::async,&TestExec1uSequencer::StartTest,this,stop_on_first_fail, loop_count);

	return(true);
}

void TestExec1uSequencer::ExecuteXtor(TestXtor xtor, bool flag) {
/*	execute ctors/dtors for sequnces and sessions
    1.	a return value of <0 terminates the sequence in all cases.
    2.	a return value of <0 can be used in a session dtor to terminate the sequence to
        implement sequence with a predefined number of sessions.
    */

    xtor.SetParam(TestSessionIndex.GetSessionLoopCount());
    if (TestMgr) {

        switch(xtor.GetXtorType()) {

            case(TestXtorType::SequenceCtor):
                TestMgr->Execute(xtor);
                break;

            case(TestXtorType::SessionCtor):
                TestMgr->Execute(xtor);
                break;

            case(TestXtorType::SessionDtor):
                StdOutConsole.Write(IOString("End of test session (%d)",xtor.GetParam()),iStdOut::Text1);
                if (TestMgr->Execute(xtor)==0) StopTestSequenceSync();
                break;

            case(TestXtorType::SequenceDtor):
                TestMgr->Execute(xtor);
                break;
        }
    }
}

bool TestExec1uSequencer::ClearExecGui() {

    unique_ix<iTestExecGui_1u> the_gui(TheIxRepository);

    if (the_gui) {

        the_gui->ClearCurrentTestStatus();
        the_gui->SetProgressBarPosition(0);

        //for (unsigned slot=0;slot<SlotCount;slot++) {

            //the_gui->SetUutSn(slot,"-");
        //}
	}

	//std::this_thread::sleep_for(std::chrono::seconds(2));
    /*	give the ui time to update */

	return(true);
}

void TestExec1uSequencer::SetGuiProgressBarPosition() {

    unique_ix<iTestExecGui_1u> the_gui(TheIxRepository);
    unsigned progress_bar_offset=(100/(TestSessionIndex.TestList.size()+1));

    if (the_gui) {

        the_gui->SetProgressBarPosition(TestSessionIndex.GetTestIndex()*progress_bar_offset);
    }
}

bool TestExec1uSequencer::InitTestGroupInExecGui() {

    unique_ix<iTestExecGui_1u> the_gui(TheIxRepository);
    if (the_gui) {

        for (unsigned i=0;i<SlotCount;i++) {

            the_gui->MarkUutAsTested(i, TestSessionIndex.GetTestIndex());
        }
    }

    return(true);
}


void TestExec1uSequencer::ConfigureTestSession(bool stop_on_first_fail, unsigned loop_count) {

    StdOutConsole.Write("Starting test session",iStdOut::Heading2);
    TheAppEngine->IncrementActiveThreadCount();

    SequenceTimer.StartTimer();

    TestSessionIndex.ConfigureSession(loop_count);
    Slots.InitTestSequenceStatus(stop_on_first_fail);

    InitLastTestLogFile();

    TheFixtureDatabase.IncrementInsertionCount(FixtureID.ToString());
    /*	*****************************************************************************
        1.	test #1
            a.	execute test
            b.	optionally set a serial number (based on something learned in the test)
            c.	terminate test sequence on a fail

        2.	prompt for a serial number if not automatically set in test #1
            a.	generate a default value

        3.	get uut index and instance
        4.	execute remaining tests

        */
}

void TestExec1uSequencer::VerifySerialNumbers() {

    char buffer[256];
    char prompt [128];
    imx::IOString sn;
    unique_ix<iTestExecGui_1u> the_gui(TheIxRepository);

    for (unsigned slot_id=0;slot_id<SlotCount;slot_id++) {

        sn=GetSn(slot_id);
        if (SerialNumberVerificationRequested()) {

            sprintf(prompt,"Enter or confirm the UUT serial number for Slot %d.\n\nCancel terminates the test and disables the Slot.",slot_id);
            if (the_gui && !(the_gui->GetUserInput(prompt,sn) ) ) {

                Slots.SetTestSequenceStatus(slot_id,0);
                Slots.EnableSlot(slot_id,false);
            }

            SetSn(sn, slot_id);
            sprintf(buffer,"Serial Number = %-.128s", sn.c_str());
            WriteLastTestLogFile(buffer);
        }

        IOString start_msg("Starting test session for serial number = %-.48s in slot #%d",sn.c_str(),slot_id);
        TheAppEngine->WriteLogFile(2,start_msg.c_str());
        if (StdOut) StdOut->WriteStdOut(start_msg.c_str(),iStdOut::Text1,Slots.GetConsoleID(slot_id));

        /*******************************************************************************************************
        get an instance for each uut to be tested
        ********************************************************************************************************/

        UutIndex=TheDb->GetUUTIndexEntry(sn.c_str(),DutTypeName.c_str());
        Slots.SetUutIndex(slot_id,UutIndex);

        Slots.SetUutInstance(slot_id,TheDb->GetUUTInstance (UutIndex, TestSessionIndex.GetSessionLoopCount(),TestSpecID.c_str(),GetFixtureID(0).c_str()));
        /*  get a unique instance record for the current device being tested
            1.	loop is used for context. anything non-zero is debug
            2.	this will be used by all subsequent tests for saving data to the database
            3.	GetFixtureID(slotid) could be used if multiple fixtures are used in a single multi-slot test
            */
    }
}

bool TestExec1uSequencer::StartTest(bool stop_on_first_fail, unsigned max_loop_count) {

    IOString name;
    char buffer[256];
    IOString description;
    iAobj<CmdParam> *test_fx {nullptr};
    unsigned delay {0};

	unique_ix<iTestExecGui_1u> the_gui(TheIxRepository);

	try {

        SetStripChartTimerCallback(new TimerFx(&TestExec1uSequencer::TimerCallback,this),1);

        ConfigureTestSession(stop_on_first_fail,max_loop_count);
        ExecuteXtor(TestXtor(TestXtorType::SequenceCtor));
        /*	execute the test sequence constructor and throw on error */

        if (the_gui) {

            the_gui->EnableSessionControls(false);
            the_gui->InitTestStatus();
            delay=SequenceTimer.GetCurrentDelay();
            //std::to_string(SequenceTimer.GetCurrentDelay());

            //the_gui->SetTimerDisplayLabel(IOString("%d", delay));
        }

        do {

            InitializeTestSequence();
            ExecuteXtor(TestXtor(TestXtorType::SessionCtor),stop_on_first_fail);
            /*	execute the test session constructor and throw on error */

            SetGuiProgressBarPosition();

            if (test_fx=TestSessionIndex.GetSerialNumberTest(name, description)) {

                Slots.ExecuteTestForEachSlotIfEnabled(
                        [this, name, description]
                        (bool xtor, unsigned slot) {

                            TheAppEngine->Debugger()
                                ->ProcessEvent(new TestExecutionEvent(
                                    xtor ? TestExecutionEvent::Type::UutTestBegin:TestExecutionEvent::Type::UutTestEnd,
                                    name,
                                    description,
                                    TestSessionIndex.GetTestIndex(),
                                    TestSessionIndex.GetSessionLoopCount(),
                                    slot
                                    ));

                        },
                        test_fx,
                        TheIxRepository
                    );

                    /*	execute first test to initialize the uut and/or get the serial number
                    -	the test function can set the serial number using 'exec->SetSn(...)
                    */

                VerifySerialNumbers();
                /*	operator verification / override of automatically generated serial numbers (from mac id's, etc)
                    1.	a serial number is not necessarily automatically generated
                    */

            }

            test_fx=TestSessionIndex.GetFirstMeasurementTest(name, description);
            while (test_fx) {

                if (the_gui) {
                    delay=SequenceTimer.GetCurrentDelay();
                    //std::to_string(SequenceTimer.GetCurrentDelay());

                    //the_gui->SetTimerDisplayLabel(IOString("%d", delay));
                }

                StdOutConsole.Write(IOString("Executing test group '%-.48s' , %-.128s",name.c_str(),description.c_str()),iStdOut::GrayText);

                Slots.ExecuteTestForEachSlotIfEnabled(

                        [this, name, description]
                        (bool xtor, unsigned slot) {

                            TheAppEngine->Debugger()
                                ->ProcessEvent(new TestExecutionEvent(
                                    xtor ? TestExecutionEvent::Type::UutTestBegin:TestExecutionEvent::Type::UutTestEnd,
                                    name,
                                    description,
                                    TestSessionIndex.GetTestIndex(),
                                    TestSessionIndex.GetSessionLoopCount(),
                                    slot
                                    ));
                        }, 
                        test_fx, TheIxRepository);

                TheAppEngine->Debugger() ->ProcessEvent( 
                    new TestExecutionEvent( 
                        TestExecutionEvent::Type::UutTestGroupEnd,
                        name, 
                        description, 
                        0,  //  for a group end, only the name is used for ID
                        //TestSessionIndex.GetTestIndex(), 
                        TestSessionIndex.GetSessionLoopCount(), 
                        0 
                        ));


                if (the_gui) {

                    for (unsigned i=0;i<SlotCount;i++){
                    /*	only update the gui test status display for non-sn related test steps */

                        the_gui->UpdateTestStatus(i, TestSessionIndex.GetTestIndex(), Slots.GetTestSessionStatus(i));
                    }
                }

                if (AbortTestSessionOnFirstFail(stop_on_first_fail)) {
                /*  break on first fail, for all slots, when enabled */

                    break;
                }

                test_fx=TestSessionIndex.GetNextMeasurementTest(name, description);
            }   //  while

            ExecuteXtor(TestXtor(TestXtorType::SessionDtor));
            /*	execute session destructor */

            TestSessionIndex.IncrementSessionLoopCount();

        } while (TestSessionIndex.SequenceIsEnabled());
        /*  terminate test sequence if
            1.  the gui 'loop enable' checkbox is not selected
            2.  the test session dtor returned 0
            3.  the 'TestExec1uSequencer::StopTestSequenceCmd(CmdParam params)' |  TestExec1uSequencer::StopTestSequence() was executed
            */

        if (the_gui) {
        /*  this will only be shown if the sequence was not asynchronously aborted
            */

            for (unsigned i=0;i<SlotCount;i++){
            /*	update the gui test sequence status */

                the_gui->UpdateTestStatus(i, 0, Slots.GetTestSequenceStatus(i));
            }

            delay=SequenceTimer.GetCurrentDelay();
            //the_gui->SetTimerDisplayLabel(IOString("%d", delay));

            if (Slots.GetOverallTestSequenceStatus()) 	the_gui->ShowDialog(false,"  Test Pass.    Select 'OK' to continue  ");
            else 										the_gui->ShowDialog(false,"  Test Fail.    Select 'OK' to continue  ");
        }
    }
    catch(IOString msg) {
    /*  asynchronous sequence termination
        1.  the 'stop' button on the user interface was selected -> calls AbortTestSequence();
        2.  the AbortTestSequenceCmd(CmdParam params) command was invoked
        3.  the exception is thrown from 'Slots'
        */

        TheAppEngine->Debugger()->ProcessEvent(new ExceptionEvent(ModuleName,msg));

        for (unsigned i=0;i<SlotCount;i++){
        /*	update the gui test sequence status */

            the_gui->UpdateTestStatus(i, 0, Slots.GetTestSequenceStatus(i));
        }
    }
    catch(...) {

		if (the_gui) {

			the_gui->ShowDialog(true,"  Unknown Test Session Exception.    The station must be restarted");
        }

        TheAppEngine->Debugger()->ProcessEvent( new  ExceptionEvent(ModuleName,"Unknown test session exception"));
    }

    if (the_gui) {

        the_gui->ShowDialog(false,"  Remove Units under test");
    }

    ExecuteXtor(TestXtor(TestXtorType::SequenceDtor));
    /*	execute sequence destructor and throw on error
	    1.	allow the station to be powered down 'after' the 'test complete' dialog 
		2.	stop the timer
		3.	etc
		*/

    CloseTestSequence();

    if (the_gui) the_gui->EnableSessionControls(true);

    return(true);
}

void TestExec1uSequencer::CloseTestSequence() {

    TheAppEngine->DecrementActiveThreadCount();

    Slots.Init();
    TestSessionIndex.ResetSession();
    SequenceTimer.StopTimer();
}

bool TestExec1uSequencer::InitializeTestSequence() {

    //UutIndex=0;

    ClearExecGui();
    Slots.InitCurrentTestSessionStatus();

    return(true);
}

/***********************************************************************************************************************
exceptions
*/

int TestExec1uSequencer::StopTestSessionCmd(CmdParam params) {

	return(1);
}

bool TestExec1uSequencer::AbortTestSessionOnFirstFail(bool stop_on_fail) {

    if  (
        (Slots.GetOverallTestSequenceStatus()==0)
        &&
        (stop_on_fail==true)
        ) {
	    
        StdOutConsole.Write("The test session has stopped on first fail");

        return(true);
    }

    return(false);
}

int TestExec1uSequencer::StopTestSequenceCmd(CmdParam params) {

    StopTestSequenceSync();
    return(1);
}

void TestExec1uSequencer::StopTestSequenceSync() {
/*  Request a synchronous test sequence stop */

    TestSessionIndex.EnableSequence(false);
}

int TestExec1uSequencer::AbortTestSequenceCmd(CmdParam params) {
/*  Request an asynchronous test sequence stop */

    AbortTestSequenceAsync();
    return(1);
}
void TestExec1uSequencer::AbortTestSequenceAsync() {

    Slots.RequestAbort();
}

void TestExec1uSequencer::EnableTestSlot(unsigned slot_id, bool enable) {

    Slots.EnableSlot(slot_id,enable);
}

int TestExec1uSequencer::EnableTestSlotCmd(CmdParam params) {

    unsigned slot_id {0};
    bool enable {true};

    TokenString str(params,',');
    str.GetToken(1,slot_id,1000);
    str.GetToken(2,enable,false);

    EnableTestSlot(slot_id, enable);

    return(1);
}

/***********************************************************************************************************************/

void TestExec1uSequencer::RegisterTestMgr(iAobj<TestXtor> *ia) {

	TestMgr=ia; 
}

const char* TestExec1uSequencer::GetSn(unsigned test_slot) {

    return(Slots.GetSerialNumber(test_slot));
}

void TestExec1uSequencer::SetSn(const char *sn, unsigned test_slot) {

    unique_ix<iTestExecGui_1u> the_gui(TheIxRepository);
    if (the_gui) {

		the_gui->SetUutSn(test_slot,sn);
    }

    Slots.SetSerialNumber(test_slot,sn);
}

void TestExec1uSequencer::GetMfgID(unsigned &id,IOString &name, IOString &location) {

    TheDb->GetMfgID(id,name,location);
}

void TestExec1uSequencer::GetFixtureID(IOString &id) {

    id=FixtureID;
}

void TestExec1uSequencer::RegisterTestFunction(IOString name, IOString description, iAobj<CmdParam> *ia) {

	unique_ix<iTestExecGui_1u> the_gui(TheIxRepository);
    IOString buffer("t%d",++TestRegistrationIndex);

    std::string cmdline_descr {description.c_str()};

    char *cptr=description.c_str();
    char *fptr=description.Find("\t");
    if (fptr) {
    /*  description contains cmdline params
        -   extract description for exec gui
        */

        cptr=++fptr;
    }
    else {
    /*  description does not contain cmdline params
        -   insert dummy function params into command line description
        */

        cmdline_descr.insert(0,"()\t");
    }

    Cmds->RegisterApplicationCmd(buffer,ia,cmdline_descr.c_str());
    TestSessionIndex.TestList.push_back(std::make_tuple(name,ia,cptr));

    if (the_gui) {

        the_gui->AddTest(name,cptr);
    }
}

bool TestExec1uSequencer::TestSequenceIsCurrentlyInProgress() {

    return(TestSessionIndex.GetSessionLoopCount());
}

int TestExec1uSequencer::TestCmd(CmdParam msg) {

	StdOutConsole.Write("Template module test-tpl output",iStdOut::Heading2);
	return(0);
}

int TestExec1uSequencer::Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) {
/*  Initialize the module(s)
	1.	use the interfaces previously registered by the framework and other modules
    2.	the 'ix' interface provided to this function is the global interface
    3.	any interfaces or commands registered here go into the global repository - and will remain after this module is unloaded - dont do it
    */

	using state=iSharedLibFramework<iIXmgr>::ModuleStates;

	try {

		switch(level) {

			case(state::GetInterfaces):
			/* use the interfaces previously registered by the framework and other modules */

                SetBaseInterfaces(global_ix,ModuleId);
                TheLocalDatabase.Configure(TheAppEngine->iPaths()->GetStdDataPath(),"Static.sdb", "TestLimits.sdb","MeasurementDb.sdb");
                TheFixtureDatabase.Configure(TheAppEngine->iPaths()->GetStdDataPath(),"TestFixtureIndex.sdb");

                Cmds=global_ix->iGet<CmdRepository::iMethodServer>(ModuleId);

                StdOut=global_ix->iGet<iStdOut>();

				TheDb.Assign(global_ix,nullptr,&TheLocalDatabase);
            	TheDb.DisableCleanupOnDestruction();
				if (!TheDb) {

					StdOutConsole.Write("An external measurement database interface has not been specified",iStdOut::Warning);
					StdOutConsole.Write("The local database will be used",iStdOut::Text1);
				}

            	SystemDb.Assign(global_ix,nullptr);
            	TheDb.DisableCleanupOnDestruction();
				if (!SystemDb) {

					StdOutConsole.Write("A system configuration database interface has not been specified",iStdOut::Warning);
				}

                //TheConsole=global_ix->iGet<iUniConsoleShell>();
                TheSystem=global_ix->iGet<iStdIOChannelSystem>(ModuleId);
                break;

			case(state::StartModule):
            /*	start the module */

                TestSessionIndex.ResetSession();
                /* explicitly set the starting configuration, independant of ctors */

                Start();
				break;

			case(state::MakeExternalConnections):
			/*	make any external connections */

                break;

			case(state::StartProcesses):
			/*	autostart any runtime processes */

                SetTestSessionID();
				break;

			case(state::Activate):
			/*	wake up an idle module */
				break;

			case(state::SetToIdle):
			/*	set the module to an idle state */
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

int TestExec1uSequencer::Start() {

    unique_ix<iTestExecGui_1u> 	gui(TheIxRepository);

    Cmds->RegisterFrameworkCmd("abort-session", new Aobj<TestExec1uSequencer,CmdParam >(&TestExec1uSequencer::AbortTestSequenceCmd,this),	    "()\tAsynchrounously abort the test session / loop mode");
    Cmds->RegisterFrameworkCmd("stop-session", 	new Aobj<TestExec1uSequencer,CmdParam >(&TestExec1uSequencer::StopTestSequenceCmd,this),	    "()\tStop thetest session loop");
    Cmds->RegisterFrameworkCmd("start-test", 	new Aobj<TestExec1uSequencer,CmdParam >(&TestExec1uSequencer::StartTestCmd,this),				"()\tStart a single test sequence");
    Cmds->RegisterFrameworkCmd("enable-slot", 	new Aobj<TestExec1uSequencer,CmdParam >(&TestExec1uSequencer::EnableTestSlotCmd,this),			"()\tEnable or disable a test slot");
    Cmds->RegisterFrameworkCmd("reset-data", 	new Aobj<TestExec1uSequencer,CmdParam >(&TestExec1uSequencer::ClearMeasurements,this),			"()\tClear the data in the database");
    Cmds->RegisterFrameworkCmd("reset-limits", 	new Aobj<TestExec1uSequencer,CmdParam >(&TestExec1uSequencer::ClearLimits,this),				"()\tClear the test limits in the database");
	
	FixtureID="FCT<xx>";
	if (gui) {
		gui->GetUserInput("Enter Test Fixture ID", FixtureID);
    }

    StdOutConsole.Configure(StdOut,		StdOut->GetRegisteredConsoleHandle("StdOut"));
    StdSysConsole.Configure(StdOut,		StdOut->GetRegisteredConsoleHandle("StdSys"));
    StdTraceConsole.Configure(StdOut,	StdOut->GetRegisteredConsoleHandle("StdTrace"));

    TheAppEngine->WriteLogFile(3,IOString("Test fixture = %-.32s",FixtureID.c_str()));

	char buffer[256];
	int count=GetSlotCount();
    Slots.SetSlotCount(count);
    /*	read number of test slots */

	for (unsigned index=0;index<count;index++) {

		sprintf(buffer,"Test Slot %d",index);
        Slots.SetConsoleID(index,TheShell->RegisterConsole(buffer,95));
	}


    return(1);
}

int TestExec1uSequencer::SetToIdle() { 
/*  Set the module to an idle condition
1.	After system initialization
2.	Between test sequence executions
*/

	return(1); 
}

int TestExec1uSequencer::Activate() { 
/*	'Wakeup' an idle module prior to executing a test sequence */

	return(1); 
}

void TestExec1uSequencer::Shutdown() {
/*  Shutdown the lib on program termination
1.	Set any instrumentation to an idle/safe state
2.	Any de-allocations as necessary
*/

}


void TestExec1uSequencer::HandleModuleException(IOString context) {
/*  this method is called from catch handlers when the exception has a system wide impact
    1.	Information must be displayed to the operation
    2.	Information must be written to local log file(s)
    3.	Test operations must be disabled (possibly not when in a 'development' mode
	4.	This is not called directly from app or system modules where the thrown types may be unknown
    */
	try {

        TheAppEngine->Debugger()->ProcessEvent(new ExceptionEvent(ModuleName,context));
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

unsigned TestExec1uSequencer::GetSlotCount() {

    SlotCount=1;
    if (TheSystem) {

        TheSystem->GetSystemVariable("Exec","SlotCount",&SlotCount,false);
        if (SlotCount<1) SlotCount=1;
        if (SlotCount>16) SlotCount=16;
    }

    return(SlotCount);
}

bool TestExec1uSequencer::SerialNumberVerificationRequested() {

    int tmp {1};
    if (TheSystem) {

        TheSystem->GetSystemVariable("Exec","VerifySn",&tmp,false);
    }

    return(VerifySnRequestFlag=tmp);
}

void TestExec1uSequencer::InitLastTestLogFile() {
/* write info to a log file
   1.	this code may be called within a catch handler - will not throw
   */

    try {

        Cmds->ExecuteCommand("InitLogFiles","Top");
    }
    catch(...) {
    }
}

void TestExec1uSequencer::WriteLastTestLogFile(std::string msg) {
/* write info to a log file
   1.	this code may be called within a catch handler - will not throw
   */

    try {

        std::unique_lock<std::mutex> lk(LastTestLogFileMutex);
        string output_string;

        auto current_time= std::chrono::system_clock::now();

        time_t tt;
        tt = std::chrono::system_clock::to_time_t (current_time);
        string str(ctime(&tt));

        size_t x=str.find("\n");
        if (x!=string::npos) {  str.erase(x); }
        str+="\t";
        /* get rid of the newline, and add a tab */

        str+=msg.c_str();

        Cmds->ExecuteCommand("UpdateLogFiles",output_string.c_str());
    }
    catch(...) {

        TheAppEngine->Debugger()->ProcessEvent(new ExceptionEvent (ModuleName,"Unknown exception while updating the log file"));
    }
}

int TestExec1uSequencer::TimerCallback(unsigned long seconds) {

    unsigned hrs,minutes,sec1,sec2;

    unique_ix<iTestExecGui_1u> the_gui(TheIxRepository);
    if (the_gui) {

        hrs =seconds / 3600;
        sec1=seconds % 3600;
        minutes=sec1 / 60;
        sec2=   sec1 % 60;

        the_gui->SetTimerDisplayLabel(IOString("%dhr %dm %ds", hrs, minutes, sec2));
    }

    return(1);
}

void TestExec1uSequencer::SetStripChartTimerCallback(iAobj<unsigned long> *ptr, unsigned delay_s) {

//    ptr=new TimerFx(&TestExec1uSequencer::TimerCallback, this);
    SequenceTimer.SetCallback(ptr, delay_s);
}

//vector<std::shared_ptr<iAobj<CmdParam>>> CmdRepository::iMethodServer::CleanupRepository; /*new*/
