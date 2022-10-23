
#ifndef IMETHODMGR
#define IMETHODMGR

#include "DistributedMessages.hpp"

class iMethodMgr {

    public:

    #if 0
    class Element {

	     public:
        Element() {
		Mask=0;
        }

        //Element(new AObj<xobj>(&test_obj, &xobj::Fx,0,"A name"));

        //template<class T>
        //Element(T *ptr,unsigned mask=0) {

        Element(iAObj *obj,unsigned mask=0) {
        ////Element(boost::shared_ptr<iAObj> obj,unsigned mask=0) {
		Mask=mask;
		TheInterface.reset(obj);
        }

        boost::shared_ptr<iAObj> Get() { return(TheInterface); }

        operator boost::shared_ptr<iAObj> () { return(TheInterface); }
        boost::shared_ptr<iAObj> TheInterface;

        bool MaskIs(unsigned mask) {
            return(Mask & mask);
        }

        private:
        unsigned Mask;
    };
    #endif

    virtual bool RegisterMethod(unsigned name_space,iAObj *iaobj)=0;
    virtual void Reset()=0;
};

#endif
