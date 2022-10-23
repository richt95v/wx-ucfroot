
#ifndef ACTIVEOBJECT
#define ACTIVEOBJECT

#include "string"

using namespace std;

class iAobj 



#if 0
template<class T>
class Mx {
    public:
    typedef int (T::*FrameworkMethod)(iMsg*);
    //typedef int (T::*FrameworkMethod)(iMsg*);
};

class iAObj {
/*	 an inteface to a function object
	 1.	used primarily for inter-thread communication
	 2.	must be dynamically allocated
	 */
    public:


    iAObj() { }

    virtual void Delete()=0;								
    virtual int Exec(iMsg*)=0;
    virtual string GetLabel()=0;

    virtual void SetOptions(unsigned value)=0;
    virtual unsigned GetOptions()=0;
    virtual bool OptionsContain(unsigned mask)=0;
    
    virtual iAObj* Clone()=0;

    virtual ~iAObj() {
    }
};

template<class ObjectType>					
class AObj:public iAObj {
/*	 function object that uses object and method pointers 
	 */

    public:

    typename Mx<ObjectType>::FrameworkMethod Method;

    AObj(AObj &ref) { *this=ref; }

    AObj(ObjectType *obj,typename Mx<ObjectType>::FrameworkMethod method,unsigned option=0, const char *label=nullptr) {
        Options=option;
        Label=label ? label:"";
        Object=obj;
        Method=method;	
    }

    int Exec(iMsg *mptr) { 
        return((Object->*Method)(mptr)); 
    }

    string GetLabel() { 
        return(Label);
    }

    void SetOptions(unsigned value) {
        Options=value;
    }

    unsigned GetOptions() { 
        return(Options);
    }

    bool OptionsContain(unsigned mask)  { 
        bool status=false;
        unsigned tmp=(Options & mask);
        status=(tmp==mask);
        return(status);
    }

    void Delete() 		{ delete(this); }
    iAObj* Clone() 		{ return(new AObj(*this));	}

    virtual ~AObj() { }

    private:
    ObjectType *Object;
    string Label;
    unsigned Options;
};

template<class T>
class iMsgObj {
    public:
    iMsgObj(iMsg *msg,bool throw_exception=false) {

        if (
            ((TheObject=dynamic_cast<T*>(msg))==nullptr)
            &&
            throw_exception
            ) {
            throw(*this);
        }
    }

    T* operator->() {
        return(TheObject);
    }

    operator bool() {
        return(TheObject ? true:false);
    }

    private:
    T *TheObject;
};
        
#endif
#endif
