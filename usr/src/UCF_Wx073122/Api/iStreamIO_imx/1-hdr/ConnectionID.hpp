
#ifndef CONNECTIONID
#define CONNECTIONID

/*************************************************************************************************************************** 
 
    Overview:	This class implements a connection id record, that contains all information necessary to identify and use an external 
				connection such as telnet, GPIB, etc. 

			    This class is used extensively in the UniConsole framework, modify at your own risk.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/


#include <vector>
#include <stdio.h>
#include <string>

using std::string;

class ConnectionID {
    public:
    ConnectionID();

    ConnectionID(const char *name);
    /*  Set the connection name */

    string
	    Name,
		Type,
        AddressLabel, 
        Description,
        Host,
		Subnet,
        UserName, 
        Pswd;

	unsigned
        Port,
        Address,
        Offset,
        Rate,
		ID,
        IOMapID;

	bool
        ConnectionEnable;

	void FormatNameType();
	/*  set the following fields to uppercase. All else must be case sensitive
		    1.	Name
		    2.	Type
		    */

    private:
    void SetDefaults(const char *name);

    private:
    char StatusBuffer[513];
};


#endif
