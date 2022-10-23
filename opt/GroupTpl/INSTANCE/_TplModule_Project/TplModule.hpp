
#ifndef TEMPLATEGENERICMODULE_UNICONSOLE
#define TEMPLATEGENERICMODULE_UNICONSOLE

#ifndef __linux__
#include <windows.h>
#undef max
#undef min
#endif

#include <iAppEngine.hpp>
#include <iSharedLib.hpp>
#include <TokenString.hpp>
#include <iIOChannelDevice.hpp>
#include <iStdOutServer.hpp>
#include <StringBuffer.hpp>

#include <iTestExec-1u.hpp>


using CmdRepository::CmdParam;

extern "C" {

    #ifndef __linux__
    __declspec(dllexport)
    #endif

    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *ix,unsigned module_id);
	/*	return a reference to the shared libs framework */
}

class TplModule:public iSharedLibFramework<iIXmgr> {
/*	implement the required virtual interfaces required by iSharedLib */

	public:
	TplModule(std::string name):iSharedLibFramework(name) {}
	~TplModule() {}

	int Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix);
	int Start();
	int SetToIdle();
	int Activate();
	void Shutdown();
	const char* GetDescription();

	void ThreadedStartup(iIXmgr *ix);
	
	int ShowCurrentIOChannels(CmdParam msg);

    int NotifyTest(CmdParam msg);
    int Test1(CmdParam msg);
    int Test2(CmdParam msg);
    int Test3(CmdParam msg);
    int Test4(CmdParam msg);
    int Test5(CmdParam msg);
    int Test6(CmdParam msg);

    int Debug1(CmdParam msg);
    /*  write/read/process iStdIO stream */

    int TestTcp(CmdParam msg);
    /*	test a tcp connection */

	unsigned long ModuleId {0};
	
    private:

    MultiModuleConsoleIO::StdOutWriter StdOutConsole;
    MultiModuleConsoleIO::StdOutWriter StdSysConsole;
    MultiModuleConsoleIO::StdOutWriter StdTraceConsole;
	
    void SetStaticConfiguration();
    int ManageTestSequences(TestXtor level);

	void HandleModuleException(IOString context);

};

#if 0
class UcfTestOption {
    public:
    UcfTestOption(
        CmdParam &msg,
        unsigned long &module_id
        );

    UcfTestOption* SaveMeasurement( const char *name_space, const char *name, double value);

    UcfTestOption* SaveMeasurement(
        const char *name_space,
        const char *name,
        double value,
        double lo_limit=-1e6,
        double hi_limit=1e6,
        bool update_test_spec=true
        );

    UcfTestOption* GetMfgId(unsigned &id, imx::IOString name, imx::IOString location);

    UcfTestOption* SetSn(const char *sn);

    const char* GetSn();

    bool WriteTestOutput(imx::IOString msg, int type);

    bool IsValid(); 

    private:
    iTestUtility *TheExec {nullptr};
    CmdParam Msg;
    unsigned Slot {0};
};
#endif


#endif
