
#ifndef REDIRECT_INCLUDED
#define REDIRECT_INCLUDED

#include "stdio.h"
#include "windows.h"
#include "string"

#include <IOString.hpp>

//#include ".\DistributedObjMsg\iSubSystem.hpp"
//#include ".\Exceptions\include\exceptions.hpp"

//#include ".\StdProcLib\iProcess.hpp"

using namespace std;


class ExtProcess {
//class ExtProcess:public iProcess {

    public:

    void SetPath(sys::IOString path,sys::IOString cwd="");
    void SetExecutable(sys::IOString executable, sys::IOString options="");
    void AppendOptions(sys::IOString options);
    void GetProcessSpecification(sys::IOString &path, sys::IOString &cwd, sys::IOString &executable, sys::IOString &options);

    void SetDescription(sys::IOString description);
    sys::IOString& GetDescription();

    bool StartProcess(sys::IOString options="",bool throw_exception=false);
    bool StartGUI(sys::IOString options="",bool throw_exception=false);
    bool StartConsole(sys::IOString options="",bool throw_exception=false);

	char operator () (const char *program_name,const char *params,const char *path,const char *cwd=NULL,bool throw_exception=true);

	operator bool();
	bool IsActive();
	bool Sync(unsigned ms,bool throw_exception=false);
	unsigned ReadStdOut(bool (*fptr)(char*));
	unsigned WriteStdIn(const char *data);

	unsigned GetExitCode();

	bool Kill();

	void ExtProcess::Clear();


    public:
    ExtProcess();
    ~ExtProcess();

	bool InError();
	bool InError(const char* &error);

	protected:
	bool Start(string p1, string p2,bool throw_exception);

    private:
    STARTUPINFO Si;								//	 program startup info and standard handles - stdin, stdout, stderr
    PROCESS_INFORMATION Pi;
    SECURITY_ATTRIBUTES Sa;

    sys::IOString ProgramName,Options,Path,CurrentWorkingDirectory,Description;
    sys::IOString LastError;

	int Status;
	DWORD ExitCode;
	int TextDisplayType;
    //HANDLE ReadPipe,WritePipe;
    HANDLE StdOut_rd, StdOut_wr;
    HANDLE StdIn_rd, StdIn_wr;
};

/*
void DisplayExtProg(const char *msg,bool flag);
int ExecutionApi::ExecuteCommandLine(APIPARAMS)   {

	int size;
    int tmp;
	char var_type;
    ExternalProgram Spx;
    static char buffer[1024];
	char *cptr,*cptr1,*cptr2,*cptr3,*cptr4;


    VaParams data(type);
    va_start(data.Vlist,type);

	cptr=(char*)SystemData.GetSystemVariable("ExtProgName",&var_type,&size);

    data.ref(cptr1,"")->ref(cptr2,"")->ref(cptr3,"")->ref(cptr4,"");
    sprintf(buffer,"%-.192s\\%-.64s %s %s %s %s",UtilityPath.c_str(),cptr,cptr1,cptr2,cptr3,cptr4);
    return(Spx.Exec(&tmp,buffer));
}
*/

#endif


