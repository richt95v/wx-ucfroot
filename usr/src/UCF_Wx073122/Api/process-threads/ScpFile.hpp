
#ifndef REMOTEBASHSCRIPT
#define REMOTEBASHSCRIPT

#include <ProcessL.hpp>

class ScpFile {

    public:
    ScpFile(std::string scp_command="/usr/bin/scp");

    void SetTargetHost(std::string host);

    bool CopyFile(std::string filename, std::string target);

    private:
    std::string TheHost;
    ProcessL TheConsole;
};

#endif
