
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's host IO management system.

    			Modify at your own risk.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef ISTDSYSTEM
#define ISTDSYSTEM

#include <functional>
#include <iStreamIO.hpp>
#include <iStdOut.hpp>
#include <iIOChannelDevice.hpp>

class iStdIOChannelSystem {

	public:

	virtual bool Initialize()=0;

	virtual bool GetSystemVariable(const char *group, const char *name, int *value,bool throw_exception)=0;
	virtual bool GetSystemVariable(const char *group, const char *name, double *value,bool throw_exception)=0;
	virtual bool GetSystemVariable(const char *group, const char *name, char *value, unsigned val_size, char *desc=nullptr, unsigned desc_size=0)=0;

	virtual bool Shutdown()=0;

    virtual iIOChannelDevice* RegisterIOChannelDevice(unsigned index,char *description)=0;
    /*  get an io channel device on a system startup. open / close states are persistant independant of this interface.
	    1.	add the description to the 'CurrentIOChannel' listing
	    2.	validate the connection
	    */

    //virtual iIOChannelDevice* GetIOChannel(char *name, char *description,bool persistant=true)=0;
    /* 	Get an IOChannel based on the name
        1.	if already registered, that copy is returned
        */

    virtual iIOChannelDevice* GetTmpIOChannelDevice(unsigned index)=0;
    /*  get an io channel device randomly. open / close states are persistant independant of this interface.
	    1.	it is not listed in the 'Current IO Channel' listing
	    2.	it is not validated
	    3.	the returned value must be deleted
	    */

	virtual bool ValidateDevice(iIOChannelDevice* device, bool throw_on_error=true)=0;

    virtual iStreamIO* GetStreamIO(unsigned index)=0;

	virtual bool GetConnectionIDRecord(ConnectionID &rec, unsigned index)=0;
	virtual bool GetConnectionIDRecordFromDb(ConnectionID &record)=0;
	/*  primitives
	    1.	GetConnectionIDRecord() gets addressing / connection information
	    2.	GetIOChannel() gets a low-level iStreamIO interface to the specified channel
	    */

	virtual ~iStdIOChannelSystem() {}
	private:
};

#endif
