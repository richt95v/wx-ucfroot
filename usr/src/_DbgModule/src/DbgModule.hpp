
#ifndef DEBUMODULE
#define DEBUMODULE

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
#include <vector>

#include <MethodServer.hpp>
#include <iDbg.hpp>


using CmdRepository::CmdParam;

extern "C" {

    #ifndef __linux__
    __declspec(dllexport)
    #endif

    iSharedLibFramework<iIXmgr>* GetEntryPoint(iIXmgr *ix,unsigned module_id);
	/*	return a reference to the shared libs framework */
}

template<class T, class I>
class BreakPointRepo {
/*  provide a vector based repository for breakpoint records
    -   the object has no ownership of the pointer(s) passed to it
    -   each stored record must implement 'bool operator ==()'
    */

    public:
    bool InsertUnique(I *event) {

        if (T *rec=dynamic_cast<T*>(event)) { 

            if (std::find(TheRepo.begin(), TheRepo.end(), *rec) == TheRepo.end()) {

                TheRepo.push_back(*rec);
                return(true);
            }
        }

        return(false);
    }

    bool RecordExists(I *event) {

        if (T *rec=dynamic_cast<T*>(event)) { 

            for (auto &item:TheRepo) {

                if (item==*rec) {

                    return(true);
                }
            }

            //if (std::find(TheRepo.begin(), TheRepo.end(), *rec) != TheRepo.end()) {
                //return(true);
            //}
        }

        return(false);
    }

    bool Delete(I *event) {

        if (T *rec=dynamic_cast<T*>(event)) { 

            for (auto it=TheRepo.begin();it!=TheRepo.end(); it++) {
                if (*it==*rec) {
                    TheRepo.erase(it);
                    break;
                }
            }

            return(true);
        }
        return(false);
    }

    unsigned Size() { return(TheRepo.size()); }

    void Clear() { TheRepo.clear(); }

    private:
    std::vector<T> TheRepo;
};

class DbgModule:public iDbg, public iSharedLibFramework<iIXmgr> {
/*	implement the required virtual interfaces required by iSharedLib */

	public:
	DbgModule(std::string name):iSharedLibFramework(name) {}
	~DbgModule() {}

	int Initialize(iSharedLibFramework<iIXmgr>::ModuleStates level,iIXmgr *global_ix) override;
	int Start();
	int SetToIdle();
	int Activate();
	void Shutdown();
	const char* GetDescription() override;

	unsigned long ModuleId {0};

	void ThreadedStartup(iIXmgr *ix);
	
    void ProcessEvent(iDebugEvent*) override;
    void SetBreakPoint (bool set, iDebugEvent *event) override;

    int SetCmdLineBreakPoint(CmdParam msg);
    int SetTestExecutionEventBreakPoint(CmdParam msg);
    int SetIOEventBreakPoint(CmdParam msg);
    int SetDataCollectionEventBreakPoint(CmdParam msg);
    int SetExceptionEventBreakPoint(CmdParam msg);

    int ClearAllBreakPoints(CmdParam msg);
    int Test1(CmdParam msg);

    private:

    void ProcessEvent(IOEvent *event);
    void ProcessEvent(ExceptionEvent *event);
    void ProcessEvent(DataCollectionEvent *event);
    void ProcessEvent(CmdLineEvent *event);
    void ProcessEvent(TestExecutionEvent *event);

    void SetCmdLineEventBreakPoint (bool break_enable, CmdLineEvent *event);
    void SetTestExecutionEventBreakPoint (bool break_enable, TestExecutionEvent *event);
    void SetDataCollectionEventBreakPoint (bool break_enable, DataCollectionEvent *event);
    void SetExceptionEventBreakPoint (bool break_enable, ExceptionEvent *event);
    void SetIOEventBreakPoint (bool break_enable, IOEvent *event);

    BreakPointRepo<CmdLineEvent,iDebugEvent>           CmdLineBreakPoints;
    BreakPointRepo<TestExecutionEvent,iDebugEvent>     ExecBreakPoints;
    BreakPointRepo<DataCollectionEvent,iDebugEvent>    DataBreakPoints;
    BreakPointRepo<ExceptionEvent,iDebugEvent>         ExceptionBreakPoints;
    BreakPointRepo<IOEvent,iDebugEvent>                IOBreakPoints;

    MultiModuleConsoleIO::StdOutWriter StdOutConsole;
    MultiModuleConsoleIO::StdOutWriter StdSysConsole;
    MultiModuleConsoleIO::StdOutWriter StdTraceConsole;
	
    void SetStaticConfiguration();

	void CleanUpCmdLine(imx::TokenString &str);
	void HandleModuleException(IOString context);
};


#endif
