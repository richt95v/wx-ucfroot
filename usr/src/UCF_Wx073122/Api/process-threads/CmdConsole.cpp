
#include <CmdConsole.hpp>

#include <thread>
#include <chrono>

CmdConsole::CmdConsole(std::string command):TheConsole(command,ProcessL::Detached) {

    Buffer=new char[BufferSize_+1];

    *(Buffer+BufferSize_)='\0';
    memset(Buffer,0,BufferSize_+1);
}

CmdConsole& CmdConsole::BufferSize(unsigned new_value) {

    if (Buffer) delete [] Buffer;
    BufferSize_=new_value;
    Buffer=new char[BufferSize_+1];

    return(*this);
}

bool CmdConsole::SetOption(std::string target) { TheConsole.AppendArg(target.c_str()); return(true); }

bool CmdConsole::Start() {

    return(TheConsole.Start());
}

void CmdConsole::Disconnect(std::string disconnect_msg) {

    Write(disconnect_msg);
}

int CmdConsole::WriteRead(std::string msg, unsigned delay_s) {

    int ret {0};
    if (Write(msg)>0) {

        std::this_thread::sleep_for(std::chrono::seconds(delay_s));
        ret=Read();
    }

    return(ret);
}

int CmdConsole::Write(std::string msg) {

    msg+=LineTerminator;
    return(TheConsole.WriteTo(msg));
}

int CmdConsole::Read() {

    int ret {0};
    if (ret=TheConsole.ReadFrom(Buffer,BufferSize_)) {

        LineBuffer.ConvertToLines(Buffer);
    }

    return(ret);
}

bool CmdConsole::WaitForInputIdle(unsigned loop_delay, int max_delay_seconds) {

    unsigned read_count {0};
    if (max_delay_seconds<0) max_delay_seconds=0;
    while(Read()) {

        ++read_count;
        std::this_thread::sleep_for(std::chrono::seconds(loop_delay));
        if (--max_delay_seconds<0) break;
    }

    return(read_count);
}

DataStatus::Optional<unsigned> CmdConsole::ScanResponseFor(std::string pattern, std::string &line) {

    return(LineBuffer.ScanForSubString(pattern,line));
}

imx::TokenString CmdConsole::GetLine(unsigned index) {

    imx::TokenString ret;
    LineBuffer.GetLine(index,ret);
    return(ret);
}

imx::StringBuffer& CmdConsole::GetLines() {

    return(LineBuffer);
}

void CmdConsole::SetLineTerminator(char terminator) {

    LineTerminator=terminator;
}

CmdConsole::~CmdConsole() {

    if (Buffer) delete[] Buffer;
}


