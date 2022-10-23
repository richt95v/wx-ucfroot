
#ifndef STRINGEX
#define STRINGEX

#include <string>

namespace StringEx {

std::string& LTrim(std::string &str) {

    if (str.size()) {

        while(str[0]==' ') {

            str.erase(0,1);
        }
    }

	return(str);
}

std::string& RTrim(std::string &str) {

    if (str.size()) {

		char ch {0};

        unsigned long size=static_cast<unsigned>(str.size()-1);
		while(size && (str[size]==' ')) {

			ch=str[size];
			str.erase(size,1);
			size=static_cast<unsigned>(str.size()-1);
        }
    }

	return(str);
}

std::string& ToUpper(std::string &str) {
	for (char &ch:str) ch=toupper(ch);
	return(str);
}

}	// namespace

#endif
