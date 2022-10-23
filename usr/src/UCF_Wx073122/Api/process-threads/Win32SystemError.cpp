

#include <Win32SystemError.hpp>
#include <IOString.hpp>
//#include ".\StdDataLib\include\DataConversion.hpp"

//using namespace DataConversion;

void Win32SystemError::Reset() { 
	SetLastError(0); 
}

unsigned Win32SystemError::InError() { 
	return(GetLastError()); 
}

bool Win32SystemError::InError(sys::IOString &xs) {

	unsigned status=GetLastError();

	#ifdef WINCE
	wchar_t msg[255];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,0,status,0,msg,255,NULL);
	ConvertWCharStringToAnsi(msg,xs,255);

	#else 
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,0,status,0,xs,255,NULL);
	#endif

	return(status ? true:false);
}
