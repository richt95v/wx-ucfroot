#ifndef WINANONPIPE
#define WINANONPIPE

#include <windows.h>

class WinAnonPipe {

    public:
    WinAnonPipe(int buffer_size=2000);

    operator bool();

    unsigned ReadBlock();

    WinAnonPipe* ReplaceInBuffer(const char target, const char substitution);

	char* GetReadBuffer();

    ~WinAnonPipe();

    HANDLE ReadPipe {nullptr};
    HANDLE WritePipe{nullptr};

    private:
    char ReadBuffer[256];
    DWORD BytesRead= 0;
    DWORD BytesAvailable= 0;

    bool Status {false};
    SECURITY_ATTRIBUTES SaAttr = { sizeof(SECURITY_ATTRIBUTES) };
};

#endif
