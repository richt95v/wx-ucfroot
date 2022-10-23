
#ifndef SERIALIZEDTEXTDATA
#define SERIALIZEDTEXTDATA

#include <iXdata.hpp>

class SerializedTextData:public iXdata {
/*  this class is used to receive multiple serialized data streams (text)
    1.	provide sequencial access to each stream (record)
    2.	provide token level access to each element within a stream
    */

	public:
	SerializedTextData() =default;

	struct RecordType {
	/*  this is the record type definition
	    1.	Id is a general purpose id value for the record (sql column id, etc)
	    2.	data is a delimitted char string
	    */

		RecordType(int id, const char *data) { 

			Id=id;
			Data=data;
		}

		int Id {-1};
		std::string Data;
	};

	void Clear() override { Records.clear(); }

	bool AppendRecord(const char *data, int id=0) override {

		if (data) {
			Records.push_back(RecordType(id,data));
			return(true);
		}

		return(false);
	}

	void SetDelimitter(char delimitter) {

		Delimitter=delimitter;
	}

	bool GetRecord(unsigned index) {

		RecordIndex=index;
		return(GetRecord());
	}

	bool GetRecord() {

		if (RecordIndex < Records.size()) {

			CurrentRecord=Records[RecordIndex++].Data;
			CurrentRecord.Replace("  "," ");
			CurrentRecord.Tokenize(Delimitter);

			return(true);
		}

		return(false);
	}

	TokenString CurrentRecord;

	private:
	char Delimitter{','};
	unsigned RecordIndex {0};
	std::vector<RecordType> Records;
};

#endif
