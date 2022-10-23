
#ifndef IXOBJ
#define IXOBJ

#include "iIXMgr.hpp"

template <class T> 
class iXObj {
    public:
    iXObj() { TheObject=NULL; }
    iXObj(iIXmgr *ix,bool throw_exception=false) {

        operator()(ix,throw_exception);
    }

    T* operator()(iIXmgr *ix,bool throw_exception=false) {

        if (
            ((TheObject=ix->iGet<T>())==NULL)
            &&
            throw_exception
            ) {
            throw(*this);
        }

        return(TheObject);
    }

    T* operator->() {
        return(TheObject);
    }

    operator bool () { return(TheObject ? true:false); }

    private:
    T *TheObject;

};

#endif
