
#ifndef PROCESSL
#define PROCESSL

#include <iProcessL.hpp>
#include <IOString.hpp>
#include <string>
#include <vector>

class ProcessL:public iProcessL {

    public:

    ProcessL()=default;
    ProcessL(std::string cmd,iProcessL::ConnectionStatus status);
    iProcessL* SetCmd(std::string cmd,iProcessL::ConnectionStatus status);
	/*  set the program name to execute and the mode
		1.	Attached, execution will block until process completion
		2.	Detached, free-range, ferral process
	    */

    iProcessL* AppendArg(const char *fmt,...) override;
	/*	append a single process argument ala sprintf */

    iProcessL*  SetArgs(std::initializer_list<std::string> il) override;
    /*	set process arguments using initializer list {"xx","yy","zz"} */

    iProcessL* AppendArgs(std::initializer_list<std::string> il) override;
	/*	append multiple process arguments using initializer list {"xx","yy","zz"} */

    iProcessL* ResetArgs() override;
	/*	reset the process arguments */

    bool Start() override;
	/*	start the process */

    bool IsActive() override;
	/*	return true if the process is still active */

    int WriteTo(std::string msg) override;
    int WriteTo(unsigned const char *msg, unsigned length) override;
	/*	write to the detached process ala pipes */

    int ReadFrom(char *buffer, unsigned buffer_size) override;
	/*	read from the detached process ala pipes */

    void Kill() override;
	/*	kill the detached process */

    ~ProcessL();

    private:
	
	int ReadPipe[2] 	{0,0};
	int WritePipe[2] 	{0,0};

    int ProcessID;
    std::string Command;
    std::vector<std::string> Arguments;

	char *Args[9] {nullptr};

    iProcessL* StartAttached();
    iProcessL* StartDetached();

    iProcessL::ConnectionStatus Connection;

    void ResetArguments();
};

#endif
