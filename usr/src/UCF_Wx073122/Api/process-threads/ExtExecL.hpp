
#ifndef EXTEXEC
#define EXTEXEC

#ifdef _linux_
#else
#include <stdio.h>
#endif

#include <string>


class ExtExec {

	public:
	bool Exec( const std::string&  command, const std::string&  mode = "r");
	/* 	execute a shell command 	

		1.	command = Command to execute.
		2.	output  = Shell output.
		3.	read/write access
 
		return 0 for success, 1 otherwise.
		*/

	int GetExitCode();
	std::string GetOutputMsg();
	private:

	int ExitCode {-99};
	std::string OutputMessage;
};

#endif
