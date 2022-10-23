
#ifndef TESTEXEC1USEQUENCERMODULE
#define TESTEXEC1USEQUENCERMODULE

#include "iAppEngine.hpp"
#include "iSharedLib.hpp"
#include "iTabbedConsoleMgr.hpp"

#include "iTestExec-1u.hpp"
#include <iStdIOChannelSystem.hpp>
#include <SqliteTestDb.hpp>
#include <SqliteFixtureDb.hpp>

#include <future>

#include <iTestDb.hpp>
#include <IOString.hpp>
#include <iUniConsole.hpp>

#include <TestSlots.hpp>

#include <StripChartTimer.hpp>

using imx::IOString;
using CmdRepository::CmdParam;

extern "C" {

    #ifndef __linux__
    __declspec(dllexport)
    #endif

    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *ix,unsigned module_id);
    /*	return a reference to the shared libs framework */
}

template<class T>
class FlagGuard{};

class SequenceIndexer {

    public:
    unsigned ResetTestIndex()           { return(TestIndex=1); };
    unsigned GetTestIndex()             { return(TestIndex); };
    unsigned IncrementTestIndex()       { return(++TestIndex); }

    unsigned IncrementSessionLoopCount(){ return(++SessionLoopCount); }
    unsigned GetSessionLoopCount()      {
        return(SessionLoopCount);
    }

    void ResetSession() {
        ResetSequence();
        SessionLoopCount = 0;
        MaxLoopCount=1;
    }

    void ResetSequence()   {
        TestIndex=1;
        SequenceEnableFlag=true;
        SequenceAbortFlag=false;
    }

    void ConfigureSession(unsigned max_loop_count=1) {
        ResetSequence();
        SessionLoopCount = 1;
        MaxLoopCount=max_loop_count;
    }

    bool FirstTest()    {
        return((TestIndex==1) && (SessionLoopCount==1));
    }

    iAobj<CmdParam>* GetSerialNumberTest(IOString &name, IOString &description) {

        iAobj<CmdParam> *test_fx {nullptr};

        if  (
            (SessionLoopCount ==1)
            &&
            (TestList.size())
            ) {

            name=std::get<0>(TestList[0]);
            test_fx=std::get<1>(TestList[0]);
            description=std::get<2>(TestList[0]);

            return(test_fx);
        }

        return(nullptr);
    }

    iAobj<CmdParam>* GetFirstMeasurementTest(IOString &name, IOString &description) {

        iAobj<CmdParam> *test_fx {nullptr};

        TestIndex=1;
        if (TestList.size()>TestIndex) {

            name=std::get<0>(TestList[TestIndex]);
            test_fx=std::get<1>(TestList[TestIndex]);
            description=std::get<2>(TestList[TestIndex]);

            return(test_fx);
        }

        return(nullptr);
    }

    iAobj<CmdParam>* GetNextMeasurementTest(IOString &name, IOString &description) {

        iAobj<CmdParam> *test_fx {nullptr};

        ++TestIndex;
        if (TestList.size()>TestIndex) {

            name=std::get<0>(TestList[TestIndex]);
            test_fx=std::get<1>(TestList[TestIndex]);
            description=std::get<2>(TestList[TestIndex]);

            return(test_fx);
        }

        return(nullptr);
    }

    bool EnableSequence(bool enable=true)   { return(SequenceEnableFlag=enable); }
    bool SequenceIsEnabled()                { return(SequenceEnableFlag && (SessionLoopCount <= MaxLoopCount)); }

    std::vector<std::tuple<IOString,iAobj<CmdParam>*,IOString>> TestList;

    private:
    unsigned TestIndex {1};
    unsigned SessionLoopCount {0};
    unsigned MaxLoopCount {1};

    bool SequenceEnableFlag     {true};
    bool SequenceAbortFlag      {false};
};

class TestExec1uSequencer:public iTestExec_1u, public iTestUtility,public iSharedLibFramework<iIXmgr> {

	public:
	TestExec1uSequencer(std::string name):iSharedLibFramework(name) {}

	void SetWindowTitle(IOString title) override;
	
    int Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) override;
	int Start();
	int SetToIdle();
	int Activate();
	void Shutdown();
	const char* GetDescription() override;

	int ClearMeasurements(CmdParam params);
	int ClearLimits(CmdParam params);

	int TestCmd(CmdParam msg);
    int EnableTestLoop(CmdParam msg);
    int StartTestCmd(CmdParam params);

	int ListDut(CmdParam params);

    unsigned GetSlotCount() override;

    bool LaunchStartTest(bool stop_on_first_fail,unsigned loop_count) override;
    bool StartTest(bool stop_on_first_fail, unsigned loop_count) override;

    /***********************************************************************************************************************
    exceptions
    */

    int StopTestSessionCmd(CmdParam params);

    int AbortTestSequenceCmd(CmdParam params);
    void AbortTestSequenceAsync() override;

    int StopTestSequenceCmd(CmdParam params);
    void StopTestSequenceSync() override;

    void EnableTestSlot(unsigned slot_id, bool enable=true) override;
    int EnableTestSlotCmd(CmdParam params);

    /***********************************************************************************************************************/

    void SetStripChartTimerCallback(iAobj<unsigned long> *ptr, unsigned delay_s) override;

    void SetSn(const char *sn, unsigned test_slot) override;
    const char* GetSn(unsigned test_slot) override;
    void GetMfgID(unsigned &id,IOString &name, IOString &location) override;
	void GetFixtureID(IOString &id) override;

    void RegisterTestMgr(iAobj<TestXtor> *ia) override;
    void RegisterTestFunction(IOString name, IOString description, iAobj<CmdParam> *ia) override;

    bool TestSequenceIsCurrentlyInProgress() override;

    void WriteTestOutput(unsigned test_slot, IOString msg, int type) override;

    bool SaveMeasurement(unsigned test_slot,const char *name_space,const char *name,double value, double lo_limit=std::numeric_limits<double>::min(), double hi_limit= std::numeric_limits<double>::max(),bool update_test_limits=true) override;

    bool SaveStripChartMeasurement(unsigned test_slot, const char *name_space, const char *name, double value) override;

    void InitLastTestLogFile();
    void WriteLastTestLogFile(std::string msg);

    iTabbedConsoleMgr *ConsoleMgr {nullptr};

	/****/

    bool AbortTestSessionOnFirstFail(bool stop_on_fail);

    std::mutex LastTestLogFileMutex;

    unsigned TestRegistrationIndex {0};
    SlotManager Slots;
	IOString FixtureID;
	SqliteFixtureDb TheFixtureDatabase;
	SqliteTestDb TheLocalDatabase;

	~TestExec1uSequencer() { 
	/*  shutdown issues
	    1.	if a global shutdown, the repositories may be gone by the time TheDb/SystemDb's destructorys are called -> exception
	    */

		TheDb.Detach();
		SystemDb.Detach();
	}

	protected:

    SequenceIndexer TestSessionIndex;
    unsigned OverallTestStatus {2};

	unsigned CurrentTestStatus {0};

    //std::vector<std::tuple<IOString,iAobj<CmdParam>*,IOString>> TestList;

    iAobj<TestXtor> 		*TestMgr{nullptr};

	private:

    void ConfigureTestSession(bool stop_on_first_fail, unsigned loop_count);
    void VerifySerialNumbers();
    void CloseTestSequence();

    StripChartTimer SequenceTimer;
    int TimerCallback(unsigned long seconds);

    std::map<unsigned, std::string> SerialNumbers;
	IOString MfgName;
	IOString MfgLocation;

    unsigned SessionID 			{0};
    /* global to all slots / threads */

    unsigned UutIndex 			{0};
    unsigned UutInstance		{0};
	/*  should be local to each slot / thread */

    bool ClearExecGui();
    /*	initialize the gui & status at the start of each test sequence */

    void SetGuiProgressBarPosition();
    bool InitTestGroupInExecGui();
    /*	initialize the gui and status for each individual test step */

    void ExecuteXtor(TestXtor xtor, bool flag=false);
    /*	construct / destruct the test group or sequence
        1.	return false to terminate the next test sequence
        */

	bool SetTestSessionID();
	bool GetUutID();
	/* database related */

    std::string GetFixtureID(int slot_id) { return(string()); }
	/*  get the fixture id number
	    1.	the return value is from the database based on a scan of the actual hardware (scan can be barcode or automated)
	    	a.	scan the fixture id name
	    	b.	based on the name, get an id number from the database (fixture-index)
	    	c.	the fixture index is used to track usage of the fixture(s)
			d. 	slot_id is used to support multi-slot systems
	    */

    bool SerialNumberVerificationRequested();
    /*	Request serial number verification from the operator when true is returned */

    std::shared_future<bool> ThreadOfExecution;
	void HandleModuleException(IOString context);

    bool InitializeTestSequence();

	ConnectionID MeasurementDbConnection;
	ConnectionID LogFileConnection;

    std::string
		TestProfileName {"unknown"},
		TestSpecName	{"null-spec"},
		DutTypeName 	{"unknown"};

    std::string TestSpecID;

    int SlotCount {1};
    bool VerifySnRequestFlag {true};

    unsigned long ModuleId {0};

    iStdIOChannelSystem *TheSystem {nullptr};
    unique_ix<iTestDb> TheDb;
    unique_ix<iStdIOChannelSystem> SystemDb;

    MultiModuleConsoleIO::StdOutWriter StdOutConsole;
    MultiModuleConsoleIO::StdOutWriter StdSysConsole;
    MultiModuleConsoleIO::StdOutWriter StdTraceConsole;
};

#endif
