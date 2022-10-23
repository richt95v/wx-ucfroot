
#ifndef SERIALCONSOLE
#define SERIALCONSOLE

#include <StringBuffer.hpp>
#include <iIOChannelDevice.hpp>

class SerialConsole {

    public:
    SerialConsole()=default;
    SerialConsole(iIOChannelDevice *io);
    /*	set the connection to the remote console */

    void Configure(iIOChannelDevice *io,unsigned read_buffer_size) {
    /*	assign the communications channel and set the read buffer size */

        Connection=io;
        if (Connection=io) {

            Connection->SetReadBuffer(read_buffer_size);
        }
    }

    bool Connect();
    /*	open the console */

    void Disconnect();
    /* close the console */

    unsigned WriteRead(std::string msg, unsigned post_write_delay_ms=500,bool clear_history=true);
    /*	write a command into the connected, serial console
        1.	wait xx mS
        2.	read back
        */

    unsigned Write(std::string msg, bool clear_history=true);
    /*	write a command into the connected, ssh console */

    unsigned Read();
    /*	read the response to a command
        -	the response is parsed into multiple lines, based on the line terminator
        -	each pased line is saved into a vector<string> repository
        */

    unsigned WaitForInputIdle(unsigned max_delay_seconds=30);
    /*	wait for the target's output to stop transmitting data */

    bool ReadBufferContains(std::string pattern, std::string &line);
    /*	search the read buffer for a specific string pattern */

    SerialConsole* ClearLineBuffer();
    /*	clear the read buffer */

    void SetLineTerminator(char terminator='\n');
    /*	set the terminator used to parse lines */

    imx::StringBuffer& GetLineBuffer() { return(LineBuffer); }
    /*	get a copy of the read buffer */

    ~SerialConsole();

    imx::StringBuffer LineBuffer;

    private:
    char LineTerminator {'\n'};
    iIOChannelDevice *Connection {nullptr};
};


#endif
