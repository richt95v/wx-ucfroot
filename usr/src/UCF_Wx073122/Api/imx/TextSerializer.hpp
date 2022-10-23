
#ifndef TEXTSERIALIZER
#define TEXTSERIALIZER

#include <string>

class TextSerializer {

	public:

	void Clear() { FormattedData.clear(); }

	template<typename Head, typename ... Tail>
	void SetData(Head hd, Tail ... args) {

		FormattedData+=hd;
		FormattedData+=",";
		SetData(args...);
	}

	void SetData() {
		
        int index=(int)FormattedData.size()-1;
		FormattedData[index]='\n';
	}

	const char* Data() { return(FormattedData.c_str()); }

	void SetDelimitter(char del) { Delimitter=del; }

	private:
	char Delimitter {','};
	std::string FormattedData;
};

#endif
