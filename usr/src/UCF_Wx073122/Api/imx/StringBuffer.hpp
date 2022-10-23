
#ifndef STRINGBUFFER
#define STRINGBUFFER

#include <string>
#include <vector>
#include <TokenString.hpp>

namespace imx {

    class StringBuffer {

        public:
        StringBuffer* ClearBuffer();
        StringBuffer* Reset();

        StringBuffer* PushBack(std::string str);
        StringBuffer* ConvertToLines(std::string str,char delimitter='\n',bool remove_blank_lines=true);

        unsigned GetLineCount();
        /*	get max lines */

        int ScanForSubString(std::string pattern, std::string &line);
        /*	search for a sub-string in all stored lines 
		 	- return the number of the line containing the search pattern
		 	- return the line containing the search pattern in 'line'
            - return -1 if the pattern could not be found
		 	*/

        unsigned RemoveLinesThatContain(std::string pattern);
        unsigned RemoveLinesThatDoNotContain(std::string pattern);

        bool GetLine(unsigned index, imx::TokenString &str);

        private:
        std::vector<std::string> Lines;
    };
}


#endif
