
#ifndef ISERIALIO
#define ISERIALIO

#include <string>

#include <ConnectionID.hpp>

class iSerialIO {

    public:
	virtual void SetName(std::string name)=0;
	virtual std::string GetName()=0;
    virtual void SetBaudRate(int rate, bool hw_flow_control=false,bool sw_flow_control=false)=0;
};

#endif
