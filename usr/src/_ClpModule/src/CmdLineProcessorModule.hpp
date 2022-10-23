
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

#include <MethodServer.hpp>
#include <iCmdLineProcessor.hpp>


using CmdRepository::CmdParam;

extern "C" {

    #ifndef __linux__
    __declspec(dllexport)
    #endif

    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *ix,unsigned module_id);
	/*	return a reference to the shared libs framework */
}

class CmdLineProcessorModule:public iCmdLineProcessor, public iSharedLibFramework<iIXmgr> {
/*	implement the required virtual interfaces required by iSharedLib */

	public:
	CmdLineProcessorModule(std::string name):iSharedLibFramework(name) {}
	~CmdLineProcessorModule() {}

	int Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) override;
	int Start();
	int SetToIdle();
	int Activate();
	void Shutdown();
	const char* GetDescription() override;

	unsigned long ModuleId {0};

	void ThreadedStartup(iIXmgr *ix);
	
    CmdRepository::iMethodServer* GetMethodServer() { return(&CommandRepository); }
	
	int Test1(CmdParam msg);

    private:

    vector<IOString> FindMatchingCommands(const char *pattern);

    std::string CurrentCmdLineInput;
    int ProcessCmdLine(IOString current_cmdline) override;
    IOString SignalTab() override;

    void ExecuteTextCommand(const char *cmd_line) override;

    void FormatCmdLine(const char *cmd_line,imx::IOString &command_string, unsigned &loop_count);
    bool CommandExists(const char *cmd_line) override;

    bool ExecuteCmdInThread(imx::IOString cmd_line, unsigned loop_count);

	MethodServer CommandRepository;

    MultiModuleConsoleIO::StdOutWriter StdOutConsole;
    MultiModuleConsoleIO::StdOutWriter StdSysConsole;
    MultiModuleConsoleIO::StdOutWriter StdTraceConsole;
	
    void SetStaticConfiguration();

    void HandleModuleException(IOString context);
};


#endif
