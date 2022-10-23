
#include "ExtExecL.hpp"

bool ExtExec::Exec(const std::string&  command, const std::string&  mode) {

    FILE *in {nullptr};
    char buffer[512];

	OutputMessage.clear();
	//in = xopen(command.c_str(), "w");
	#ifdef __linux__
    in = popen(command.c_str(), mode.c_str());
	#else
    in = _popen(command.c_str(), mode.c_str());
	#endif
    if (in) {

		while(fgets(buffer, sizeof(buffer), in)!=NULL) { 

			OutputMessage+=buffer;
		}

		#ifdef __linux__
        ExitCode = pclose(in);
		#else
        ExitCode = _pclose(in);
		#endif
	}

    return (ExitCode==0);
}

int ExtExec::GetExitCode() {

	return(ExitCode);
}

std::string ExtExec::GetOutputMsg() {

	return(OutputMessage);
}

