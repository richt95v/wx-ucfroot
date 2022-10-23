


#include <IOString.hpp>

#include <cstring>
#include <stdio.h>
#include <stdarg.h>

#include <memory.h>
#include <algorithm>


using imx::IOString;
using std::string;


/**
 * Default constructor
 */
IOString::IOString() { 

	clear();
}

IOString::IOString(std::string &ref) {

	clear();
	Fmt(ref.c_str());
}

IOString::IOString(const char *fmt,...) {

    va_list args;

	clear();

    va_start( args, fmt);
    vsprintf(Buffer, fmt, args);
    va_end( args );

	Size=strlen(Buffer);
}

IOString::~IOString() { 

}

char* IOString::Find(const char *pattern) {

    return(strstr(Buffer,pattern));
}

/**
 * Compare the value to another xString
 * @param ref The external value to compare
 * 
 * @return bool True on value is 'less than' the passed reference
 */
bool IOString::operator<(const IOString &ref)  const {
	if (strcmp(Buffer,ref.Buffer)<0) return(true);
	return(false);
}


/**
 * Compare the value to another xString
 * @param ref The external value to compare
 * 
 * @return bool True on value is 'greater than' the passed reference
 */
bool IOString::operator>(const IOString &ref)  const {
	if (strcmp(Buffer,ref.Buffer)>=0) return(true);
	return(false);
}



/**
 * Append a char value to the internal string
 * @param value The value to append
 * 
 * @return char* The stored string
 */
char* IOString::operator+=(const char value) 	{ 

    size_t size=GetSize();
    size_t max=MaxSize-size;
	if (max>0) {
	/*	if there is room to append */

		Buffer[size]=value;
		Buffer[size+1]='\0';
		Size=strlen(Buffer);
	}

	return(Buffer); 
}

/**
 * Append a c string value to the internal string
 * @param value The value to append
 * 
 * @return char* The stored string
 */
char* IOString::operator+=(const char *value) 	{ 
    size_t max=MaxSize-Size;
	if (max>0) {
	/*	if there is room to append */

		strncat(Buffer,value,max);  
		Size=strlen(Buffer);
	}

	return(Buffer); 
}


/**
 * Return a pointer to the stored string
 */
IOString::operator char*() 	{ return(Buffer); }


/**
 * Get the stored string
 * 
 * @return char* A pointer to the stored string
 */
char* IOString::c_str() 			{ return(Buffer); }
unsigned char* IOString::uc_str()	{ return(reinterpret_cast<unsigned char*>(Buffer)); }

std::string IOString::ToString()	{ return(std::string(Buffer)); }
IOString::operator string()			{ return(std::string(Buffer)); }

/**
 * Clear all elements of the internal buffer
 */
void IOString::clear() 		{ Size=0; memset(Buffer,0,MaxSize); }


/**
 * Assign a value to the stored string
 * @param value The c string to copy
 * 
 * @return char* A pointer to the stored string
 */
char* IOString::Fmt(const char *fmt,...) 		{ 

    va_list args;

    clear(); 

    va_start( args, fmt);
    vsprintf(Buffer, fmt, args );
    va_end( args );

	Size=strlen(Buffer);

    return(Buffer); 
}

unsigned char* IOString::uFmt(const unsigned char *data, unsigned size) {

	if (size<MaxSize) {

		std::copy_n((unsigned char*)Buffer,size,(unsigned char*)data);
		//memcpy(Buffer,data,size);
		Size=size;
	}

	return(reinterpret_cast<unsigned char*>(Buffer));
}


/**
 * Assign a value to the stored string
 * @param value The c string to copy
 * 
 * @return char* A pointer to the stored string
 */
const char* IOString::operator=(const char *value) { 

	Fmt(value); 
	return(Buffer); 
}

/**
 * Assign a value to the stored string
 * @param value The stl string to copy
 * 
 * @return char* A pointer to the stored string
 */
const char* IOString::operator=(std::string ref) { 

	Fmt(ref.c_str());
	return(Buffer);
}


/**
 * Compare the stored string to an external c string
 * @param ref The c string to compare to
 * 
 * @return int <0 if the object is < ref, 0 if object == ref, >0 if object > ref
 * 
 */
int IOString::compare(const char *ref) {

	return(strcmp(Buffer,ref));
}

/**
 * Get the length of the stored string
 * 
 * @return unsigned The length of the stored string
 */
size_t IOString::GetSize() { return(Size); }

/**
 * Return the size of the internal buffer
 * 
 * @return unsigned The internal buffer size
 */
size_t IOString::GetMaxSize() { return(MaxSize); }


