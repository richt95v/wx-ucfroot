
#ifndef CMDCONSOLE
#define CMDCONSOLE

#include <StringBuffer.hpp>
#include <ProcessL.hpp>

class CmdConsole {

    public:
    //CmdConsole(std::string command,unsigned buffer_size=1000);
    CmdConsole(std::string command);
    CmdConsole& BufferSize(unsigned size);
    /*	set the ssh program name / location and the size of the read buffer */

    bool SetOption(std::string target);

    bool Start();
    /*	open the ssh session */

    void Disconnect(std::string disconnect_msg="exit");
    /* close the ssh session */

    int WriteRead(std::string msg, unsigned delay_s);
    /*	write a command to the connection, wait <delay_s>, read back the response */

    int Write(std::string msg);
    /*	write a command into the connected, ssh console */

    bool WaitForInputIdle(unsigned loop_delay, int max_delay_seconds);
    /*	wait for the output from the target connection to be idle */

    int Read();
    /*	read the response to a command
        -	the response is parsed into multiple lines, based on the line terminator
        -	each pased line is saved into a vector<string> repository
        */

    imx::StringBuffer& GetLines();
    /*	get the buffer that contains all lines that were read */

    imx::TokenString GetLine(unsigned index);

    DataStatus::Optional<unsigned> ScanResponseFor(std::string pattern, std::string &line);
    /*	scan the repository for a line that contains 'pattern' */

    void SetLineTerminator(char terminator='\n');
    /*	set the terminator used to parse lines */

    ~CmdConsole();

    private:
    char *Buffer {nullptr};
    unsigned BufferSize_ {10000};
    imx::StringBuffer LineBuffer;
    char LineTerminator {'\n'};
    ProcessL TheConsole;
};


#endif
