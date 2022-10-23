
/*************************************************************************************************************************** 
 
    Overview:	This class implements a simple string class that is safe(r) to pass across dll boundaries.
				connection such as telnet, GPIB, etc. 

    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef IOSTRING
#define IOSTRING

#include <string>

namespace imx {

	
    class IOString {

	    public:
		IOString();
        IOString(std::string &ref);
        IOString(const char *fmt,...);

        const char* operator=(const char *value);
        const char* operator=(std::string ref);

        char* Fmt(const char *fmt,...);
		unsigned char* uFmt(const unsigned char *data, unsigned size);
        /*	'set' operations */

        bool operator<(const IOString &ref)  const;
        bool operator>(const IOString &ref)  const;
        /*	operators required for stl container operations */ 

        char* c_str();
        operator char*();
		operator std::string();
		std::string ToString();
		unsigned char* uc_str();

        /*	'get' operations */

        char* Find(const char *pattern);

        char* operator+=(const char value);
        char* operator+=(const char *value);
        /*	'append' operations */


        void clear();
        size_t GetSize();
        size_t GetMaxSize();
        /*	utility operatons
            1.	clear initializes the string to all 0's
            2.	Size returns the length of the string
            3.	GetMaxSize() returns the size of the buffer
            */

        int compare(const char *ref);
        /*	logical operations */

        ~IOString();

        private:
        size_t Size {0};
        size_t MaxSize {1024};
        char Buffer[1024];
     };

}	//	namespace
#endif
