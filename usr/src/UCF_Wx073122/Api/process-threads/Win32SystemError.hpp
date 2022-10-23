
#ifndef WIN32SYSTEMERROR_HPP
#define WIN32SYSTEMERROR_HPP

#include "windows.h"
#include <IOString.hpp>


class Win32SystemError {
	public:
	static void Reset();
	static unsigned InError();
    static bool InError(sys::IOString &xs);
};

#endif
