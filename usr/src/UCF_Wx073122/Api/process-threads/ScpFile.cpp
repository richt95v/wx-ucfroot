
#include <ScpFile.hpp>


ScpFile::ScpFile(std::string scp_command) : TheConsole(scp_command, iProcessL::Detached) { }

void ScpFile::SetTargetHost(std::string host) {

    TheHost=host;
}

bool ScpFile::CopyFile(std::string filename, std::string target) {

    TheConsole.ResetArgs();
    std::string cmd_line_args;

    cmd_line_args+=TheHost;
    cmd_line_args+=":";
    cmd_line_args+=target;

    TheConsole.AppendArgs({filename,cmd_line_args});
    TheConsole.Start();

    return(true);
}
