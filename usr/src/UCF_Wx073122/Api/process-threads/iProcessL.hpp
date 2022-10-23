
#ifndef IPROCESSL
#define IPROCESSL

#include <cstring>
#include <stdio.h>
#include <stdarg.h>

#include <string>

#include <initializer_list>

class iProcessL {

    public:

    enum ConnectionStatus {Attached=0, Detached};
	/* 	flags that determine whether or not the process will block until completion */

    virtual iProcessL*  AppendArg(const char *fmt,...)=0;
	/*	append a single process argument ala sprintf */

	virtual iProcessL*  SetArgs(std::initializer_list<std::string> il)=0;
	/*	initialize process arguments using initializer list {"xx","yy","zz"} */

	virtual iProcessL*  AppendArgs(std::initializer_list<std::string> il)=0;
	/*	append multiple process arguments using initializer list {"xx","yy","zz"} */

	virtual iProcessL*  ResetArgs()=0;
	/*	reset the process arguments */

	virtual bool        Start()=0;
	/*	start the process */

    virtual bool IsActive()=0;
	/*	return true if the process is still active */

	virtual int WriteTo(std::string msg)=0;
	virtual int WriteTo(unsigned const char *msg, unsigned length)=0;
	/*	write to the process ala pipes */

	virtual int ReadFrom(char *buffer, unsigned buffer_size)=0;
	/*	read from the process ala pipes */

    virtual void Kill()=0;
	/*	kill the process */

    virtual ~iProcessL() {}
};

#endif
