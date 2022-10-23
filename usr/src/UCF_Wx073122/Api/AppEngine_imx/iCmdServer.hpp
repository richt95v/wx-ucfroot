
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's command execution engine.

			    Do not modify.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef ICMDSERVER
#define ICMDSERVER

#include "IOString.hpp"
#include "iFunctionObject.hpp"

#include "memory"
#include "vector"
#include <functional>

using imx::IOString;
using CmdFunction=std::function<void(IOString)>;

class iCmdServer {

	public:
	virtual bool Execute(IOString cmd, IOString param)=0;
	/*	execute a command */

	iFx* RegisterCmd(IOString cmd_name, CmdFunction fx) {
	/*  register a new command with the server
	    1.	a reference is stored in the CleanupRepository so that memory will be deallocated when the lib is unloaded
	    */

		iFx *ptr=RegisterCmd (cmd_name, new FunctionElement_1p<CmdFunction,IOString>(fx));
		CleanupRepository.push_back(std::shared_ptr<iFx>(ptr));

		return(ptr);
	}

	private:
	virtual iFx* RegisterCmd(IOString cmd_name, iFunctionElement_1p<IOString> *cmd)=0;

	static std::vector<std::shared_ptr<iFx>> CleanupRepository;
};

#endif
