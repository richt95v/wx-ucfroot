
#include <SerialConsole.hpp>
#include <chrono>
#include <thread>

SerialConsole::SerialConsole(iIOChannelDevice *io) {
/*	assign the serial connection */

    Connection=io;
}


bool SerialConsole::Connect() {
/*	connect the console to the target */

    LineBuffer.Reset();
    return(Connection && Connection->Open());
}

void SerialConsole::Disconnect() {
/*	disconnect the target */

    if (Connection) {

        Connection->Close();
    }
}

unsigned SerialConsole::WriteRead(std::string msg, unsigned post_write_delay_ms,bool clear_history) {
/*	write to the target and read the response */

    unsigned bytes_written=Write(msg,clear_history);
    if (bytes_written) {

        std::this_thread::sleep_for(std::chrono::milliseconds(post_write_delay_ms));
        Read();
    }

    return(bytes_written);
}

unsigned SerialConsole::Write(std::string msg, bool clear_history) {
/*	write to the target */

    if (clear_history) LineBuffer.Reset();

    msg+=LineTerminator;
    return(Connection->Write((char*)msg.c_str()));
}

unsigned SerialConsole::Read() {
/*	read the console input, and convert to lines
    */

    unsigned ret =Connection->Read();
    char *string_read=Connection->GetReadBuffer();
    char *target {nullptr};

    if (ret>0) {
    /*	remove any embedded nulls */

        for (unsigned i=0;i<ret;i++) {

            target=string_read+i;
            if (*target=='\0') {

                *(target)=LineTerminator;
            }
        }

        LineBuffer.ConvertToLines(string_read,LineTerminator);
    }

    return(ret);
}

unsigned SerialConsole::WaitForInputIdle(unsigned max_delay_seconds) {
/*	loop until the console input is idle for 5 consecutive reads, with 1s delay between reads
    */

    unsigned tmp {0};
    unsigned read_count {0};
    unsigned idle_count {0};

    while(max_delay_seconds) {

        if (tmp=Read()) {

            read_count+=tmp;
            idle_count=0;
        }
        else {

            if (++idle_count>4) {
            /* wait for 5 successive read fails to break */

                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        --max_delay_seconds;
    }

    return(read_count);
}

bool SerialConsole::ReadBufferContains(std::string pattern, std::string &line) {
/*	search the read buffer for a specific string pattern */

    return(LineBuffer.ScanForSubString(pattern,line));
}

SerialConsole* SerialConsole::ClearLineBuffer() {
/*	clear the read buffer */

    LineBuffer.Reset();
    return(this);
}

void SerialConsole::SetLineTerminator(char terminator) {
/*	set the terminator that is used to convert the read buffer into lines */

    LineTerminator=terminator;
}

SerialConsole::~SerialConsole() {

}


