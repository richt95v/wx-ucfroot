
#include <SshConsole.hpp>

#include <thread>
#include <chrono>

SshConsole::SshConsole(std::string ssh_command,unsigned buffer_size):TheConsole(ssh_command,ProcessL::Detached) {

    Buffer=new char[buffer_size+1];

    *(Buffer+buffer_size)='\0';
    memset(Buffer,0,buffer_size+1);

    BufferSize=buffer_size;
}

bool SshConsole::SetTargetHost(std::string target) { TheConsole.AppendArg(target.c_str()); return(true); }

bool SshConsole::Connect() {

    return(TheConsole.Start());
}

void SshConsole::Disconnect(std::string disconnect_msg) {

    Write(disconnect_msg);
}

int SshConsole::WriteRead(std::string msg, unsigned delay_s) {

    int ret {0};
    if (Write(msg)>0) {

        std::this_thread::sleep_for(std::chrono::seconds(delay_s));
        ret=Read();
    }

    return(ret);
}

int SshConsole::Write(std::string msg) {

    msg+=LineTerminator;
    return(TheConsole.WriteTo(msg));
}

int SshConsole::Read() {

    int ret {0};
    if (ret=TheConsole.ReadFrom(Buffer,BufferSize)) {

        LineBuffer.ConvertToLines(Buffer);
    }

    return(ret);
}

bool SshConsole::WaitForInputIdle(unsigned loop_delay, int max_delay_seconds) {

    unsigned read_count {0};
    if (max_delay_seconds<0) max_delay_seconds=0;
    while(Read()) {

        ++read_count;
        std::this_thread::sleep_for(std::chrono::seconds(loop_delay));
        if (--max_delay_seconds<0) break;
    }

    return(read_count);
}

DataStatus::Optional<unsigned> SshConsole::ScanResponseFor(std::string pattern, std::string &line) {

    return(LineBuffer.ScanForSubString(pattern, line));
}

imx::StringBuffer& SshConsole::GetLines() {

    return(LineBuffer);
}

void SshConsole::SetLineTerminator(char terminator) {

    LineTerminator=terminator;
}

SshConsole::~SshConsole() {

    if (Buffer) delete[] Buffer;
}


