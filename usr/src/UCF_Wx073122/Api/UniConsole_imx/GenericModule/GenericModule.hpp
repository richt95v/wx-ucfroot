
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

#include <memory>

using CmdRepository::CmdParam;

extern "C" {

    #ifndef __linux__
    __declspec(dllexport)
    #endif

    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *ix,unsigned module_id);
	/*	return a reference to the shared libs framework */
}

class Template_Module:public iSharedLibFramework<iIXmgr> {
/*	implement the required virtual interfaces required by iSharedLib */

	public:
	Template_Module(std::string name):iSharedLibFramework(name) {}
	~Template_Module() {}

	int Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix);
	int Start();
	int SetToIdle();
	int Activate();
	void Shutdown();
	const char* GetDescription();

	void ThreadedStartup(iIXmgr *ix);
	
	int ShowCurrentIOChannels(CmdParam msg);

	int Test(CmdParam msg);

	unsigned long ModuleId {0};
	
    private:
	
	void SetStaticConfiguration();

	void CleanUpCmdLine(TokenString &str);
	void HandleModuleException(IOString context);
};

#endif
