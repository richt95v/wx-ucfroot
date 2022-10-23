
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's interface management system.

    			This class is used extensively in the UniConsole framework, do not modify. Modifications to this file are gauranteed
    			to break the system if modified incorrectly. 
 
    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef IIXMGR
#define IIXMGR

#include <stddef.h>

class iX {
/* this class is the top level interface to an IXelement
   */
    public:
	 virtual ~iX() { }

    virtual bool IsSameTypeAs(iX *ptr)=0;
	/*	compare the object type to a passed in reference type */

    virtual bool IsNotReferenceCounted()=0;
	/*  return the persistant flag
	    1.	non-ref counted items have a persistant lifetime (lifetime of the application)
		2.	non-ref counted items will not prevent a module from being unloaded
	    */
};

template<class T>
class IXelement:public iX {
/*  this class defines the data storage for an interface reference
    */

    public:
    IXelement(T *ptr,bool persistant=false) { 

        ReferenceCounted=persistant;
        TheInterface=ptr; 
    }

	virtual ~IXelement() { }

    bool IsSameTypeAs(iX *ptr) {

        IXelement<T> *element=dynamic_cast<IXelement<T> *>(ptr);
        if (element) {

            if (dynamic_cast<T*>(element->TheInterface)) {
                return(true);
            }
        }
    
        return(false);
    }

	bool IsNotReferenceCounted() { return(ReferenceCounted); }

    T *TheInterface;

    private:
    bool ReferenceCounted {false};
};
    
#endif
