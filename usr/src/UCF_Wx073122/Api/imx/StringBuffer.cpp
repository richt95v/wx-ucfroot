
#include <StringBuffer.hpp>
#include <TokenString.hpp>

using namespace imx;

StringBuffer* StringBuffer::ClearBuffer() {

    Lines.clear();
    return(this);
}

StringBuffer* StringBuffer::Reset() {

    Lines.clear();
    return(this);
}

StringBuffer* StringBuffer::ConvertToLines(std::string str, char delimitter,bool remove_blank_lines) {

    std::string token;
    imx::TokenString tokenized_response(str,delimitter);

    int token_count=tokenized_response.GetTokenCount();
    for (int i=0;i<token_count;i++) {

        tokenized_response.GetToken(i,token);
        if (token.size() || (remove_blank_lines==false)) {

            PushBack(token);
        }
    }

    return(this);
}

StringBuffer* StringBuffer::PushBack(std::string str) {

    Lines.push_back(str);
    return(this);
}

bool StringBuffer::GetLine(unsigned index, imx::TokenString &token) {

    if (index<Lines.size()) {

        token=imx::TokenString(Lines[index]);
        return(true);
    }

    return(false);
}

int StringBuffer::ScanForSubString(std::string pattern, std::string &line) {

    int ret {-1};

    for (int i=0;i<Lines.size();i++) {

        if (Lines[i].find(pattern)!=std::string::npos) {

            line=Lines[i];
			ret=i;
            //break;
        }
    }

    return(ret);
}

unsigned StringBuffer::RemoveLinesThatContain(std::string pattern) {

    unsigned count {0};
    std::vector<std::string>::iterator i=Lines.begin();
    while(i!=Lines.end()) {

        if (i->find(pattern)!=std::string::npos) {

            Lines.erase(i);
            i = Lines.begin();
            ++count;
        }
        else {
            ++i;
        }
    }

    return(count);
}

unsigned StringBuffer::RemoveLinesThatDoNotContain(std::string pattern) {

    unsigned count {0};
    std::vector<std::string>::iterator i=Lines.begin();
    while(i!=Lines.end()) {

        if (i->find(pattern)==std::string::npos) {

            Lines.erase(i);
            i = Lines.begin();
            ++count;
        }
        else {
            ++i;
        }
    }

    return(count);
}

unsigned StringBuffer::GetLineCount() {

    return(Lines.size());
}
