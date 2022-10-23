
#ifndef ISTDVARIABLES
#define ISTDVARIABLES

#include <iStreamIO.hpp>

class iStdIOChannelVariables {

	public:

	virtual bool GetSystemVariable(char *group, char *name, char *value, unsigned val_size, char *desc=nullptr, unsigned desc_size=0)=0;
	virtual iStreamIO* GetIOChannel(unsigned index)=0;
	virtual bool GetConnectionIDRecord(ConnectionID &record,bool throw_on_error)=0;

	virtual ~iStdIOChannelVariables() {}
	private:
};

#endif
