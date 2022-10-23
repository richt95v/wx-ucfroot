/*************************************************************************************************************************** 
 
    Overview:	This class provides simple string tokenizing capabilities.

    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef TOKENSTRING
#define TOKENSTRING

#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <initializer_list>

#include <IOString.hpp>

#undef min

namespace imx {

class TokenString {

	public:
	TokenString()= default;

	TokenString(std::string str,const char sep=0) {
	/*  assign a value to the internal std::string and tokenize using 'sep' to seperate tokens
		*/

		Src=str;
		if (sep) Tokenize(sep);
	}

    TokenString(IOString str,const char sep=0) {
    /*  assign a value to the internal std::string and tokenize using 'sep' to seperate tokens
        */

        Src=str.c_str();
        if (sep) Tokenize(sep);
    }

    TokenString(const char *str,const char sep=0) {
	/*  assign a value to the internal std::string and tokenize using 'sep' to seperate tokens
		*/

		Src=str;
		if (sep) Tokenize(sep);
	}

    std::string& operator=(char * str) {
	/*	initialize and assign a value to the internal std::string */

		Tokens.clear();
		Src=str;
		return(Src);
	}

	std::string& operator=(std::string &str) {
	/*	initialize and assign a value to the internal std::string */

		Tokens.clear();
		Src=str;
		return(Src);
	}

    TokenString* Set(std::string &str, char sep=0) {
	/*	initialize and assign a value to the internal std::string */

		Tokens.clear();
		Src=str;
        if (sep) Tokenize(sep);

        return(this);
	}

    TokenString* Set(const char * str, char sep=0) {
	/*	initialize and assign a value to the internal std::string */

		Tokens.clear();
		Src=str;
        if (sep) Tokenize(sep);

        return(this);
	}

    TokenString* Set(const unsigned char * str, char sep=0) {
	/*	initialize and assign a value to the internal std::string */

		Tokens.clear();
        Src=(const char*) str;

        if (sep) Tokenize(sep);

        return(this);
	}

	std::string& Str() { return(Src); }
	const char*	 StdStr() { return(Src.c_str()); }

	TokenString* Strip(std::initializer_list<std::string> il) {
	/*	strip a list of strings from the internal std::string */

		for (auto str:il) {

			Replace(str,"");
		}

		return(this);
	}

	TokenString* Replace(std::string pattern, std::string replacement) {
	/*	replace 'pattern' with 'replacement' in the internal string */

		size_t index=0;
		while(true) {

            if (index=Src.find(pattern);( index==std::string::npos)) break;

			Src.erase(index,pattern.size());
			Src.insert(index,replacement);
		}

		return(this);
	}

	TokenString* Tokenize(char sep) {
	/*  tokenize the internal std::string using 'sep' as a token seperator
	    1.	skip values in quotes
	    2.	store tokens in 'Tokens'
	    */

		bool skip=false;
		for (unsigned index=0;index<Src.size();index++) {

			if (Src[index]=='"') { 

				skip= skip ? false:true;
				Src[index]='\b';
				continue; 
			}

			if (Src[index]==sep) {

				if (!skip) {

					Src[index]='\n';
				}
			}
		}

		Replace("\b", "");

		std::string tmp;
		std::stringstream ss(Src);

		Tokens.clear();
		while(getline(ss,tmp,'\n')) {

			Tokens.push_back(tmp);
		}

		return(this);
	}

	bool GetLastToken(std::string &tmp) {
	/*	get the last token, and return the value  as a string in 'tmp' */

		if (Tokens.size()) {

            tmp=Tokens.back();
			return(true);
		}

		tmp.clear();
		return(false);
	}

    std::string GetStrToken(unsigned index,std::string default_value="") {
    /*	get the specified token, and return the value as an std::string using 'default_value' if the token is not found */

        std::string tmp;
        if (index<Tokens.size()) {

            return(Tokens[index]);
        }

        return(default_value);
    }

    IOString GetIOStrToken(unsigned index,std::string default_value="") {
    /*	get the specified token, and return the value as an std::string using 'default_value' if the token is not found */

        IOString tmp;
        if (index<Tokens.size()) {

            return(Tokens[index].c_str());
        }

        return(default_value.c_str());
    }

    bool GetToken(unsigned index,std::string &value, std::string default_value="") {
	/*	get the specified token, and return the value as an std::string using 'default_value' if the token is not found */

		std::string tmp;
		if (index<Tokens.size()) {

			value=Tokens[index];
			return(true);
		}

		value=default_value;
		return(false);
	}

	bool GetToken(unsigned index,unsigned &value,unsigned default_value=std::numeric_limits<unsigned>::min()) {
	/*	get the specified token, and return the value as an unsigned using 'default_value' if the token is not found */

		if (index<Tokens.size()) {

			value=atoi(Tokens[index].c_str());
            //if (value<0) value=0;
			return(true);
		}

		value=default_value;
		return(false);
	}
	
	bool GetToken(unsigned index,int &value,int default_value=std::numeric_limits<int>::min()) {
	/*	get the specified token, and return the value as an unsigned using 'default_value' if the token is not found */

		if (index<Tokens.size()) {

			value=atoi(Tokens[index].c_str());
			return(true);
		}

		value=default_value;
		return(false);
	}

	bool GetToken(unsigned index,bool &value,bool default_value=false) {
	/*	get the specified token, and return the value as a bool using 'default_value' if the token is not found */

		int tmp {0};
		if (index<Tokens.size()) {

			tmp=atoi(Tokens[index].c_str());
			value= tmp ? true:false;

			return(true);
		}

		value=default_value;
		return(false);
	}

	bool GetToken(unsigned index,double &value,double default_value=std::numeric_limits<double>::min()) {
	/*	get the specified token, and return the value as an unsigned using 'default_value' if the token is not found */

		if (index<Tokens.size()) {

			value=atof(Tokens[index].c_str());
			return(true);
		}

		value=default_value;
		return(false);
	}

	/**/

	unsigned GetTokenCount() {
	/*	return the number of tokens */

		return(static_cast<int>(Tokens.size()));
	}

	const std::vector<std::string>& GetTokens() {

		return(Tokens);
	}

	std::vector<std::string> Tokens;
	private:

	std::string Src;
};

}	// namespace

/*****************************************************************************
Example

int main(int argc, char *argv[]) {

	TokenString ts;

	string input {"This,is , String"};

	ts=input;
	ts.Replace(" ","");
	ts.Tokenize(',');


	return(0);
}

*/

#endif
