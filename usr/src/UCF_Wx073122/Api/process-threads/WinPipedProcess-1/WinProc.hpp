#ifndef WINPROC
#define WINPROC

#include <thread>

#include <windows.h>

class WinProc {
    public:
    WinProc(HANDLE write_pipe);
    bool Start(const char *cmd, unsigned xout_s);
    bool IsValid();
	bool Kill();
	~WinProc();

    private:
    STARTUPINFO Si 			{ sizeof(STARTUPINFO) };
    PROCESS_INFORMATION Pi  { 0 };
	std::thread WatchDogThread;
	bool WatchDogTimer(unsigned xout_s);
};

#endif

#if 0
/* example code */

bool ExecuteExtProg(const char *cmd,const char *description,std::vector<std::string> &lines,iStdOut *std_out, unsigned xout_s) {

    bool status {false};
    TokenString line_buffer;

    std::string current_line;
    std::string composite_lines;

    WinAnonPipe read_pipe;
    WinProc proc(read_pipe.WritePipe);

    try {

        status=true;

        std_out->WriteStdTrace(IOString("Executing [%-.128s]\t# %-.128s",cmd,description),iStdOut::BlockStart);

        if (read_pipe) {

            proc.Start(cmd,xout_s);

            bool bProcessEnded = false;
            while (!bProcessEnded) {

                bProcessEnded = !proc.IsValid();
                /* Give some timeslice (50ms), so we won't waste 100% cpu.
                    1.	if the loop exits here, because the process ended, any output from a small/fast program will be lost
                    */

                for (;;) {

                    if (read_pipe.ReadBlock()) {

                        read_pipe.ReplaceInBuffer('\0','#') ->ReplaceInBuffer('\r',' ');
                        composite_lines+=read_pipe.GetReadBuffer();

                        line_buffer.Set(read_pipe.GetReadBuffer()) ->Tokenize('\n');
                        /*	clean up the line */

                        current_line=line_buffer.Tokens.back();
                        line_buffer.Tokens.pop_back();
                        /*	save the last token as the start of the next line */

                        for (std::string &str:line_buffer.Tokens) {
                        /*	save and display all other tokens */

                            std_out->WriteStdTrace(str.c_str(),iStdOut::Text1);
                        }
                    }
                    else {
                        if (current_line.size()) {
                            //WriteStdTrace(current_line.c_str(),iStdOut::Text1);
                            current_line="";
                        }
                        break;
                    }
                }		// 	for (;;)
            }			//	while (...)

            TokenString final_lines(composite_lines);
            final_lines.Replace("\r","")->Tokenize('\n');
            for (std::string &str:final_lines.Tokens) {
            /*	parse and save everything here
                1.	this is the simplest and cleanest
                2.	will be used for output searches
                3.	previous code is for display only
                */

                lines.push_back(str);
            }

        }		//	if read_pipe
    }
    catch(...) {

        status=false;
    }

    return(status);
}

#endif
