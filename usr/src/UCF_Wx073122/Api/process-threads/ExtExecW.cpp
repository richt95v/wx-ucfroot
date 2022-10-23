
#include <ExtExecW.hpp>
#include <Win32SystemError.hpp>


/**
 * Execute an external process using call syntax
 * @param program_name The program to execute
 * @param params The command line arguments to pass to the program
 * @param path The path location of the program
 * @param throw_exception Throw an exception when true
 * 
 * @return char
 */
char ExtProcess::operator() (const char *program_name,const char *params,const char *path,const char *cwd,bool throw_exception)	{
	Path=path;
	CurrentWorkingDirectory=cwd ? cwd:Path;
	return(Start(program_name,params,throw_exception));
}

/**
 * Find if the process execution resulted in an error
 * 
 * @return bool True on error.
 */
bool ExtProcess::InError() {
    return(LastError.GetSize() ? true:false);
}

/**
 * Find if the process execution resulted in an error
 * @param error The last error message will be copied into 'error'
 * 
 * @return bool True on error
 */
bool ExtProcess::InError(const char* &error) {
	error=LastError;
    return(LastError.GetSize() ? true:false);
}

/**
 * Kill the external process if active
 * 
 * @return bool True if the process was active, and terminated
 */
bool ExtProcess::Kill() {
	if (IsActive() && TerminateProcess(Pi.hProcess,0)) {
		LastError="Process terminated programitically";
		Clear();
		return(true);
	}
	return(false);
}

bool ExtProcess::StartGUI(sys::IOString options,bool throw_exception) {

	string runtime_options=Options;
	string p1=Path+="\\";
    p1+=ProgramName.c_str();

    if (options.GetSize()) {
		runtime_options+=" ";
        runtime_options+=options.c_str();
	}

	return(Start(p1,runtime_options,throw_exception));
}

bool ExtProcess::StartConsole(sys::IOString options,bool throw_exception) {
    string cmd_line=Path+="\\";
    cmd_line+=ProgramName+=" ";
    cmd_line+=Options+=" ";
    cmd_line+=options.c_str();
	return(Start(string(),cmd_line,throw_exception));
}

bool ExtProcess::StartProcess(sys::IOString options,bool throw_exception) {

	string p1=Path;
	p1+="\\";
    p1+=ProgramName.c_str();

    SHFILEINFO psfi;
    DWORD ftype=(DWORD)SHGetFileInfo(p1.c_str(),0,&psfi,sizeof(SHFILEINFO),SHGFI_EXETYPE);
	DWORD format=LOWORD(ftype);		// PE, NE, MZ(DOS exe com bat) 4550 is PE(little endian)
    DWORD subtype=HIWORD(ftype);	// 0 is console

    if (subtype && (format==0x4550)) {
    /*	a gui executable */

		return(StartGUI(options,throw_exception));
	}
    else  if ((subtype==0) && (format==0x4550)) {

        return(StartConsole(options,throw_exception));
    }

    return(false);
}

/**
 * Start an external process using parameters configured with the 'Set(...)' command
 * @param throw_exception True if an exception is to be thrown on process creation error
 * 
 * @return bool True on success. Throws an Exception<ExtProcess> Obj which contains an iException interface
 */
bool ExtProcess::Start(string p1,string p2,bool throw_exception)	{

    /* 
    determine the file type based on the name from the previous step 
    
        Notes:  1.  this code looks for the windows designation in HIWORD.
                2.  if this is found, the file is assumed to be graphical.
                3.  any other responses are assumed to be a console program
    SHFILEINFO psfi;
    DWORD ftype=SHGetFileInfo(program_name,0,&psfi,sizeof(SHFILEINFO),SHGFI_EXETYPE);
	DWORD format=LOWORD(ftype);		// PE, NE, MZ(DOS exe com bat) 4550 is PE(little endian)
	start_new_process=HIWORD(ftype);// 0 is console (3.0 3.5 4.0 == windows)
									// both 0's is undefined type
    */

    sys::IOString xs;
	ExitCode=0;

	Win32SystemError::Reset();

    //system(p2.c_str());
    //return(false);

    if (
		CreateProcess(
			p1.size() ? p1.c_str():NULL,						//	the gui program name or NULL
			(LPSTR)p2.c_str(),									//	options for a gui, or the complete command line for a console program
			NULL,												//	security attributes, NULL specifies default parameters
			NULL,												//	security attributes, NULL specifies default parameters
            //false,      											//	parent handles inheritted - if FALSE, no redirection possible
            true,      											//	parent handles inheritted - if FALSE, no redirection possible

			//CREATE_NEW_PROCESS_GROUP,
			//CREATE_NO_WINDOW,
            //CREATE_NEW_CONSOLE |
            0,													//	required for a console app with redirected output
            //DETACHED_PROCESS,									//	works for general gui apps

            nullptr,												//	handle to environment block - if NULL, the parents environment is used
            nullptr,
            //CurrentWorkingDirectory,
			&Si,
			&Pi
			) 
		) {

        LastError.clear();
		return(true);
	}

	LastError="Unable to create process [ ";
	LastError+=Path;
	LastError+=" \\ ";
	LastError+=ProgramName;
	LastError+=" ";
	LastError+=p2.c_str();
	LastError+=" ] ";
	LastError+="::";
	Win32SystemError::InError(xs);
	LastError+=xs;

	if (throw_exception) {
        //throw(DxMsg(Exception<ExtProcess>(LastError.Get()).SetObject(*this)));
	}
		
	return(false);
}

/**
 * Write to the StdIn of the external process
 * @param cs The text to write
 * 
 * @return unsigned The number of bytes written
 */
unsigned ExtProcess::WriteStdIn(const char *data) {

	DWORD bytes_written=0;
    WriteFile(StdIn_wr,data,strlen(data),&bytes_written,NULL);
	return(bytes_written);
}

/**
 * Read the StdOut text from the external process
 * @param cs The text read from the external process
 * 
 * @return unsigned The number of bytes read from the external process
 */
unsigned ExtProcess::ReadStdOut(bool (*fptr)(char*)) {

	char buffer[255];
	unsigned count=0;
    DWORD bytes_read=0;

    //CloseHandle(WritePipe);		//	this must be done so that the 'broken_pipe' error will be detected
    //WritePipe=NULL;

    memset(buffer,0,255);
	for (;;) {
        PeekNamedPipe(StdOut_rd, 0, 0, 0, &bytes_read, 0);
        if (bytes_read==0) {

            break;
        }

        ReadFile(StdOut_rd,buffer,254,&bytes_read,nullptr);
        //ReadFile(ReadPipe,buffer,10,&bytes_read,NULL);
        //ReadFile(ReadPipe,buffer,254,&bytes_read,NULL);
        *(buffer+bytes_read)='\0';
		if (!bytes_read) break;
		count+=bytes_read;
		if (fptr) {
			if (!fptr(buffer)) break;
		}
	}

	return(count);
}

/**
 * Return true if the external process is active
 */
ExtProcess::operator bool() {
	return(IsActive());
}

/**
 * Wait for the external process to end, with a timeout
 * @param ms The timeout value in mS.
 * 
 * @return bool True if the process ended without a timeout condition
 */
bool ExtProcess::Sync(unsigned ms,bool throw_exception) {

	DWORD status=WaitForSingleObject(Pi.hProcess,ms);
	Win32SystemError::InError(LastError);
	GetExitCodeProcess(Pi.hProcess,&ExitCode);

	if (status==WAIT_TIMEOUT) {

		if (throw_exception) {
			throw(*this);
		}
		return(false);
	}

	return(true);
}

/**
 * Set the description for the external command
 * @param description The description of the external command.
 * 
 */
void ExtProcess::SetDescription(sys::IOString description) {
	Description=description;
}

/**
 * Get the description for the external command
 * 
 */
sys::IOString& ExtProcess::GetDescription() {
	return(Description);
}

/**
 * Determine if the external process is active
 * 
 * @return bool True if the external process is active
 */
bool ExtProcess::IsActive() {
	if (GetExitCodeProcess(Pi.hProcess,&ExitCode)) {
		return (ExitCode==STILL_ACTIVE);
	}
	return(false);
}

unsigned ExtProcess::GetExitCode() {
	return(ExitCode);
}

/**
 * Clear the internal handles
 */
void ExtProcess::Clear() {

	if (Pi.hProcess) {

		CloseHandle(Pi.hProcess);
		Pi.hProcess=NULL;

		CloseHandle(Pi.hThread);
		Pi.hThread=NULL;

		//CloseHandle(ReadPipe);		
	}
}

ExtProcess::ExtProcess()  {

    int status {0};
	TextDisplayType=0;
    //ReadPipe=WritePipe=NULL;

    /* set up the security attirb struct */
    ZeroMemory(&Sa,sizeof(SECURITY_ATTRIBUTES));
    Sa.nLength=sizeof(SECURITY_ATTRIBUTES);
    Sa.lpSecurityDescriptor=NULL;
    Sa.bInheritHandle=TRUE;

    ZeroMemory(&Si,sizeof(STARTUPINFO));
    GetStartupInfo(&Si);								//	getcurrent / default settings
    Si.wShowWindow	=SW_HIDE;							//	or SW_SHOWNORMAL
    //Si.hStdInput	=GetStdHandle(STD_INPUT_HANDLE);
    //Si.hStdOutput	=GetStdHandle(STD_OUTPUT_HANDLE);
    //Si.hStdError	=GetStdHandle(STD_ERROR_HANDLE);

	Si.cb			=sizeof(STARTUPINFO);
    Si.dwFlags		|=STARTF_USESTDHANDLES;

#if 0
    if (CreatePipe(&ReadPipe,&WritePipe,&Sa,0)) {


        status=SetHandleInformation(ReadPipe, HANDLE_FLAG_INHERIT, 0);
        status=SetHandleInformation(WritePipe, HANDLE_FLAG_INHERIT, 0);

        Si.hStdInput	=ReadPipe;
        Si.hStdOutput	=WritePipe;
		Si.hStdError	=WritePipe;
	}
#endif


    CreatePipe(&StdOut_rd,&StdOut_wr,&Sa,0);
    CreatePipe(&StdIn_rd,&StdIn_wr,&Sa,0);

    Si.hStdInput	=StdIn_rd;
    Si.hStdOutput	=StdOut_wr;
    Si.hStdError	=StdOut_wr;
}

ExtProcess::~ExtProcess()    {
}

/**
 * Set the target current working directory for the external process
 * @param path The target current working directory
 */
void ExtProcess::SetPath(sys::IOString path,sys::IOString cwd) {
	Path=path;
    CurrentWorkingDirectory=cwd.GetSize() ? cwd:Path;
}

void ExtProcess::SetExecutable(sys::IOString executable, sys::IOString options) {
	ProgramName=executable;
	Options=options;
}

void ExtProcess::AppendOptions(sys::IOString options) {
	Options+=options;
}

void ExtProcess::GetProcessSpecification(sys::IOString &path, sys::IOString &cwd, sys::IOString &executable, sys::IOString &options) {
	options=Options;
	executable=ProgramName;
	path=Path;
	cwd=CurrentWorkingDirectory;
}

