
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to an active object used to make cross dll function calls.

			    This class is used extensively in the UniConsole framework, do not modify.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/


#ifndef IAOBJ
#define IAOBJ


template<class Param>
class iAobj {
/*	This class implements a generic interface to a object/function pointer */
	public:
	virtual int Execute(Param *param)=0;
	virtual int Execute(Param param)=0;
	virtual ~iAobj() {}
};

template<class T,class Param>
class Aobj:public iAobj<Param> {
/*  This class implements a function object that is safe to pass across shared lib boundaries
    1.	Generic interface
    2.	T is the type of object that contains the function to call
    3.	Param is the type of parameter that will be passed to the function
	4. 	Example: 
 
    	iAobj<xparam> *ptr= new Aobj<xobj,paramx>(&xobj::Fx, &xx);
		ptr->Exec(paramx *msg);
    */
	public:
	Aobj(int (T::*fx)(Param*),T *object) {
	    Object=object;
	    Fxr=fx;
	}

	Aobj(int (T::*fx)(Param),T *object) {
	    Object=object;
	    Fxv=fx;
	}

	int Execute(Param *param) {
		return((Object->*Fxr)(param));
	}

	int Execute(Param param) {
		return((Object->*Fxv)(param));
	}

	private:
	T *Object=nullptr;
	int (T::*Fxr)(Param *obj)=nullptr;
	int (T::*Fxv)(Param obj)=nullptr;
};

#endif
