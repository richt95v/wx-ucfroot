
#include <WinAnonPipe.hpp>

WinAnonPipe::WinAnonPipe(int buffer_size) {
    SaAttr.bInheritHandle = true;   //Pipe handles are inherited by child process.
    SaAttr.lpSecurityDescriptor = nullptr;
    Status=CreatePipe(&ReadPipe, &WritePipe, &SaAttr, buffer_size);
}

WinAnonPipe::operator bool() {
    return(Status);
}

unsigned WinAnonPipe::ReadBlock() {

    BytesRead=0;
    memset(ReadBuffer,0,256);
    ::PeekNamedPipe(ReadPipe, NULL, 0, NULL, &BytesAvailable, NULL);
    if (BytesAvailable)  {

        if (::ReadFile(ReadPipe, ReadBuffer, 255, &BytesRead, NULL)) {

            ReadBuffer[BytesRead]=0;
        }
    }

    return(BytesRead);
}

WinAnonPipe* WinAnonPipe::ReplaceInBuffer(const char target, const char substitution) {

    for (unsigned i=0; i<BytesRead;i++) {

        if (ReadBuffer[i]==target) ReadBuffer[i]=substitution;
    }

    return(this);
}

char* WinAnonPipe::GetReadBuffer() {

    return(ReadBuffer);
}


WinAnonPipe::~WinAnonPipe() {

    if (WritePipe)	CloseHandle(WritePipe);
    if (ReadPipe) 	CloseHandle(ReadPipe);
}


