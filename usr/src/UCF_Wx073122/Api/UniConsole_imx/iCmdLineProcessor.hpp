
#ifndef ICMDLINEPROCESSOR
#define ICMDLINEPROCESSOR


#include <thread>
#include <cstring>
#include <TokenString.hpp>

class iCmdLineProcessor {

	public:

    virtual int ProcessCmdLine(IOString current_cmdline)=0;
    /*  process keyboard input using command line completion */

    virtual IOString SignalTab()=0;

    virtual bool CommandExists(const char *cmd_line) =0;

    virtual void ExecuteTextCommand(const char *cmd_line) =0;
};

#endif
