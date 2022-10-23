
#include <ProcessL.hpp>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdarg.h>
#include <iostream>
#include <algorithm>

#include <thread>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

ProcessL::ProcessL(std::string cmd,ConnectionStatus status) {
/*  set the program name to execute and the mode
	1.	Attached, execution will block until process completion
	2.	Detached, free-range, ferral process
	*/

    ResetArgs();
    ProcessID=0;
    Connection=status;
    Command=cmd;

    pipe2(ReadPipe,O_DIRECT|O_NONBLOCK);
    pipe2(WritePipe,O_DIRECT|O_NONBLOCK);
    /*  create the pipes */
}

ProcessL::~ProcessL() {

    #if 0
    Kill();
    #endif

    close(ReadPipe[0]);
    close(WritePipe[1]);
}

iProcessL* ProcessL::SetCmd(std::string cmd,iProcessL::ConnectionStatus status) {
/*  set the program name to execute and the mode
	1.	Attached, execution will block until process completion
	2.	Detached, free-range, ferral process
	*/

    ResetArgs();
    ProcessID=0;
    Connection=status;
    Command=cmd;

    return(this);
}

iProcessL*  ProcessL::SetArgs(std::initializer_list<std::string> il) {
/*	set process arguments using initializer list {"xx","yy","zz"} */

    ResetArgs();
    AppendArgs(il);

    return(this);
}

iProcessL* ProcessL::ResetArgs() {
/*	reset the process arguments */

    Arguments.clear();
    return(this);
}

iProcessL* ProcessL::AppendArg(const char *fmt,...) {
/*	append a single process argument ala sprintf */
    
    char buffer[256];
    memset(buffer,0,255);

    va_list args;
    va_start( args, fmt);
    vsprintf(buffer, fmt, args );
    va_end( args );

    Arguments.push_back(buffer);
    return(this);
}

iProcessL* ProcessL::AppendArgs(std::initializer_list<std::string> il) {
/*	append multiple process arguments using initializer list {"xx","yy","zz"} */

	for (std::string cptr:il) { Arguments.push_back(cptr); }

    return(this);
}

bool ProcessL::Start() {
/*  start the process
    */

    int arg_size=Arguments.size();
    if (arg_size>8) arg_size=8;

    memset(Args,0,9*sizeof(char*));
    Args[0]=(char*)Command.c_str();
    /*  reset the args to be passed
        1.  assign Arg[0] to the executable name
        */

    for (unsigned index=0;index<arg_size;index++) {
    /*  fill the remainder of the pointer array with the command line arguments
        */

        Args[index+1]=const_cast<char*>(Arguments[index].c_str());
    }

     if (Connection==Attached)   StartAttached();
     else                        StartDetached();
     /* start per the specified mode */

    return(true);
}

iProcessL* ProcessL::StartAttached() {

    if (ProcessID=fork()) {
    /* parent */

        waitpid(ProcessID,nullptr,0);
        /*  wait for the child process to exit */
    }
    else {
    /* child */

        execv(Args[0],Args);
    }

    return(this);
}

iProcessL* ProcessL::StartDetached() {

    int tmp_pid1 {0};
	int status {0};

	char buffer[256];
	memset(buffer,0,256);

    if (tmp_pid1=fork()) {
    /* 	this is the parent */

        std::this_thread::sleep_for(std::chrono::seconds(1));
        /* need to give the process a chance to start, otherwise the waitpid won't work right */

        status=errno;
    }
    else {
    /*  create a detached process via double fork */

        int tmp_pid2=0;
        if (tmp_pid2=fork()) {
        
            std::this_thread::sleep_for(std::chrono::seconds(2));
            /* 	need to give the process a chance to start, otherwise the waitpid won't work right,
                give the new process time to start / be reaped appropriately
                ?? hasnt made a difference yet, just a guess
                */

            _exit(0);               
            /* 	_exit doesn't do any cleanup. exit will call destructors,
                deep copied from parent, can be a mess
                */
        }
        else {
		/* this is a new process */

            dup2(ReadPipe[1],1);     // stdout (1) writes into ReadPipe[1]
            dup2(WritePipe[0],0);    // stdin (0) reads from WritePipe[0]

            close(ReadPipe[1]);
            close(WritePipe[0]);
            /*  necessary for the pipes to automatically close when the process terminates */

            execv(Args[0],Args);
            /* execute the child program
			   1.	only returns on failure to execute
			   */

            _exit(-1);	// required, otherwise you will end up with multiple/extra processes
        }
    }

    return(this);
}

bool ProcessL::IsActive() {
/*  return true if the detached process is still active
    */

	bool ret {true};
    int child_pid=0,status=0;
    if (ProcessID>0) {

        if ((child_pid=waitpid(ProcessID,&status,WNOHANG))>0) {
            
            if (
                WIFEXITED(status)
                ||
                WIFSIGNALED(status)
                ) {

                ret=false;
            }
        }

        if (child_pid<0) {
            ret=false;
        }
    }

    return(ret);
}

int ProcessL::WriteTo(std::string msg) {
/*  write to the detached process */

    int bytes_written {0};
    bytes_written=write(WritePipe[1],msg.c_str(),msg.size());	// this is necessary for the ssh command to work

    return(bytes_written>0 ? bytes_written:0);
    /* do not return negative error codes */
}

int ProcessL::WriteTo(unsigned const char *msg, unsigned length) {
/*  write to the detached process */

    int bytes_written {0};
    bytes_written=write(WritePipe[1],msg,length);	// this is necessary for the ssh command to work

    return(bytes_written>0 ? bytes_written:0);
    /* do not return negative error codes */
}


int ProcessL::ReadFrom(char *buffer, unsigned buffer_size) {
/*  read from the detached process */

    memset(buffer,0,buffer_size);
    int bytes_read=read(ReadPipe[0],buffer,buffer_size);

    return(bytes_read>0 ? bytes_read:0);
    /* do not return negative error codes */
}

void ProcessL::Kill() {
/*  kill the detached process */

    if ((Connection==Attached) && (ProcessID)) {

        kill(ProcessID,SIGTERM);
        waitpid(ProcessID,nullptr,0);
        ProcessID=0;
    }
}
