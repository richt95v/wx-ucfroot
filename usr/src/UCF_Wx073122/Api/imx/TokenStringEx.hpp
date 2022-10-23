
#ifndef TOKENSTRINGEX
#define TOKENSTRINGEX

#include <TokenString.hpp>

/***************************************************************************************************************************
This class adds extends the capability of the TokenString class
*/

class TokenStringEx:public TokenString {
	public:
	
	TokenStringEx* SetWithEmbeddedNulls(char *buffer, unsigned buffer_size,char replacement='@') {

		char *start=buffer;
		char *stop=buffer+buffer_size-1;
		while(start<stop) {

			if ((*start)=='\0') *start=replacement;
			++start;
		}

		*stop='\0';
		Str()=buffer;
		
		return(this);
	}

	TokenStringEx* EraseUntil(char *pattern) {
	
		size_t index=Str().find(pattern);
		if (index!=std::string::npos) {
			
			Str().erase(0,index);
		}
	
		return(this);
	}
	
	private:
	std::string Src;
};


#endif
