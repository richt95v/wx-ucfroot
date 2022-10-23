#include <WinProc.hpp>

#include <thread>

WinProc::WinProc(HANDLE write_pipe) {
    Si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    Si.hStdOutput  = write_pipe;
    Si.hStdError   = write_pipe;
    Si.wShowWindow = SW_HIDE;       // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.
}

bool WinProc::Start(const char *cmd, unsigned xout_s) {

    bool success= CreateProcess( NULL, (char*)cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &Si, &Pi);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	if (success) {

        WatchDogThread=std::thread(&WinProc::WatchDogTimer,this,xout_s);
		return(true);
	}

    return(false);
}

bool WinProc::WatchDogTimer(unsigned xout_s) {

	while(xout_s){

		if (IsValid()==false) {

			return(false);
			break;
		}

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		--xout_s;
	}

	Kill();
	return(true);
}

bool WinProc::Kill() {

	bool status {false};
	if (IsValid()) {

		TerminateProcess(Pi.hProcess,0);
		status=true;
	}

	return(status);
}

bool WinProc::IsValid() {
/*	return true if the process is still active */

    return(!WaitForSingleObject( Pi.hProcess, 50) == WAIT_OBJECT_0);
}

WinProc::~WinProc() {

	WatchDogThread.join();
}

