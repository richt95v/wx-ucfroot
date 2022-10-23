

#pragma once

#include ".\StdObjLib\include\xString.hpp"

using namespace std;



class iExtProcess {

    public:

	virtual void SetPaths(xString path,xString cwd="")=0;
	virtual void SetExecutable(xString executable, xString options)=0;
	virtual void AppendOptions(xString options)=0;

	virtual void GetProcessSpecification(xString &path, xString &cwd, xString &executable, xString &options)=0;

	virtual void SetDescription(xString description)=0;
	virtual xString& GetDescription()=0;

	virtual bool StartProcess(const char *extended_cmdline_options=NULL)=0;

	virtual bool IsActive()=0;

	virtual unsigned ReadStdOut(bool (*)(const char *buffer,unsigned buffer_size)) { return(0); }
	virtual unsigned WriteStdIn(const char *buffer,unsigned buffer_size) { return(0); }
	virtual unsigned GetExitCode()=0;

	virtual bool Sync(unsigned ms)=0;

	virtual bool Kill(const char *message="Process terminated")=0;

	bool InError(char *error);
};


