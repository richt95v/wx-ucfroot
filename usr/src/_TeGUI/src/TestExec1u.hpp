
#ifndef TEMPLATEMODULE_UNICONSOLE_GUI
#define TEMPLATEMODULE_UNICONSOLE_GUI

#include "iAppEngine.hpp"
#include "iSharedLib.hpp"

#include "iTestExec-1u.hpp"
#include "UniTestExec-1u.hpp"
#include "iTestExecGui-1u.hpp"
#include "iTestExecGuiMgr.hpp"


extern "C" {

    #ifndef __linux__
    __declspec(dllexport)
    #endif

    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *ix,unsigned module_id);
	/*	return a reference to the shared libs framework */
}

class TestExec1u:public iSharedLibFramework<iIXmgr> {
/*	implement the required virtual interfaces required by iSharedLib */

	public:
	TestExec1u(std::string name):iSharedLibFramework(name) {}

	int Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix);
	int Start();
	int SetToIdle();
	int Activate();
	void Shutdown();
	const char* GetDescription();

	int TestCmd(IOString msg);

	QWidget* Window{nullptr};
	iTestExecGui_1u* TheGui {nullptr};

    unsigned long ModuleId {0};

    private:

	void HandleModuleException(IOString context);

	ConnectionID MeasurementDbConnection;
	ConnectionID LogFileConnection;

    MultiModuleConsoleIO::StdOutWriter StdOutConsole;
    MultiModuleConsoleIO::StdOutWriter StdSysConsole;
    MultiModuleConsoleIO::StdOutWriter StdTraceConsole;
};

#endif
