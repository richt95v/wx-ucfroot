
/*************************************************************************************************************************** 
 
    Overview:	This file defines a class that defines trace message types, and an class that will automatically send a trace
				message when it goes out of scope.  
 
    				1.	trace messages are fundamental to the UniConsole framework. do not modify enum values.
    				2.	GuardedTraceBlock class can be modified as needed.

    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef ITRACE
#define ITRACE

#include <string>
#include <functional>

#include <IOString.hpp>

class iTrace {
	public:

    /* these are formatting options, not colors */
    enum MsgType {
		Error=0, 
		FrameworkStart=100, FrameworkInfo, FrameworkStop,
		BlockStart=200, BlockInfo, BlockStop,
		DeviceOpen=300, DeviceInfo, DeviceWrite, DeviceRead, DeviceClose, 
		Framework=1000
    };

    //virtual iTrace* WriteStdTrace(const char *msg, int level=0)=0;
};

class GuardedTraceBlock {
/*	provide message outputs for trace level block starts, and stops */

	public:
	GuardedTraceBlock(std::function<void(const char*,int)> fx,std::string msg) {

		Fx=fx;
		Msg=msg;
		msg+=" start";

        Fx(msg.c_str(), iTrace::BlockStart);
	}

	~GuardedTraceBlock() {

		std::string msg=Msg;
		msg+=" stop";

		Fx(msg.c_str(), iTrace::BlockStop);
	}

	std::function<void(const char*,int)> Fx;
	std::string Msg;
};

#endif
