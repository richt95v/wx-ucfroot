
/*************************************************************************************************************************** 
 
    Overview:	This class implements a simple function object that executes members of a specified object.

    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef IFUNCTIONOBJECT
#define IFUNCTIONOBJECT

class iFx {
	public:
	virtual ~iFx() {}
};

template<class p1>
class iFunctionElement_1p:public iFx {
/*	This class implements a generic interface to a object/function pointer */
	public:
	virtual int Execute(p1 msg)=0;
	virtual ~iFunctionElement_1p() {}
};

template<class T,class p1>
class FunctionElement_1p:public iFunctionElement_1p<p1> {
/*  This class implements a function object
    1.	Generic interface
    2. 	Safe to pass across shared lib boundaries
    */
	public:
	FunctionElement_1p(T fx) { Fx=fx; }

	int Execute(p1 param1) {
		Fx(param1);
		return(1);
	}

	private:
	T Fx;
};

template<class p1,class p2>
class iFunctionElement_2p:public iFx {
/*	This class implements a generic interface to a object/function pointer */
	public:
	virtual int Execute(p1 msg, p2 option)=0;
	virtual ~iFunctionElement_2p() {}
};

template<class T,class p1, class p2>
class FunctionElement_2p:public iFunctionElement_2p<p1,p2> {
/*  This class implements a function object
    1.	Generic interface
    2. 	Safe to pass across shared lib boundaries
    */
	public:
	FunctionElement_2p(T fx) { Fx=fx; }

	int Execute(p1 param1, p2 param2) {
		Fx(param1,param2);
		return(1);
	}

	private:
	T Fx;
};

#endif
