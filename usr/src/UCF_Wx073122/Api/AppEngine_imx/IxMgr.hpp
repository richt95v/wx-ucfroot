
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's interface management system.

    			This class is used extensively in the UniConsole framework, do not modify. Modifications to this file are gauranteed
    			to break the system if modified incorrectly. 
 
    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef IXMGR
#define IXMGR

#include <vector>
#include <typeinfo>
#include <string>

#include <iIXMgr.hpp>
#include <IOString.hpp>

#include <map>
#include <string>

#include <assert.h>

#ifdef __linux__
#else
#pragma warning (disable:4100)	// forma parameter no referenced
#pragma warning (disable:4018)	// signed, unsigned mismatch
#endif


using std::vector;
using imx::IOString;

class iIXmgr {
/*  this class is an interface to an IXmgr object which stores object interfaces

    1.	an interface is a raw pointer to a persistant object
	2.	this interface, or the IXmgr object that it points to, does not own the pointers (a weak_ptr in the future?)
	2.	Examples:
 
			xobj xx;
			xobj *xptr;
	    	iIXmgr ix*=new IXMgr;

			ix->iSet<xobj>(&xx);
			xobj *xptr = ix->iGet<xx>();
    */

    public:

	template<class T>
    iIXmgr* iSet(T *object_interface,bool ref_counted=false) {
	/*  register an interface */

        Set(new (std::nothrow) IXelement<T>(object_interface,ref_counted));
        return(this);
    }

    template<class T>
	T* iGet(unsigned long &module_id, const char *err_msg=nullptr) {
	/*  get an interface from an object that searched multiple modules
	    1.	each module contains a unique module_id
	    */

        T* ret{nullptr};

        iX *ref=new (std::nothrow) IXelement<T>(nullptr);

        iX *tmp{nullptr};
        Get(ref,tmp,module_id);

        if (IXelement<T> *xref=dynamic_cast<IXelement<T> *> (tmp)) {

            ret=xref->TheInterface;
        }

		if ((err_msg) && (ret==nullptr)) {

			throw(IOString(err_msg));
		}

        delete(ref);
        return(ret);
    }

    template<class T>
	T* iGet() {
	/*  get an interface from an object that does not contain a module_id */

        T* ret{nullptr};

        iX *ref=new (std::nothrow) IXelement<T>(nullptr);

        iX *tmp{nullptr};
        Get(ref,tmp);

        if (IXelement<T> *xref=dynamic_cast<IXelement<T> *> (tmp)) {

            ret=xref->TheInterface;
        }

        delete(ref);
        return(ret);
    }

    virtual void iRelease(unsigned long module_id) =0;

	template<class T>
	iIXmgr* iDelete(T *object_interface) {

		IXelement<T> tmp(object_interface);
		Delete(&tmp);
		return(this);
	}

    virtual void Set(iX*)=0;
	virtual void Delete(iX*)=0;

    virtual bool Get(iX *ref,iX* &ix)=0;
    virtual bool Get(iX *ref,iX* &ix,unsigned long &module_id)=0;
	
	virtual ~iIXmgr() {}
};

template<class T>
class unique_ix final {

	public:
	unique_ix() = default;
    unique_ix(iIXmgr *ix,char *err_msg=nullptr):TheRepository(ix) {
		
        TheInterface=TheRepository->iGet<T>(ModuleID,err_msg);
	}

    void Assign(iIXmgr *ix,char *err_msg=nullptr,T *ptr=nullptr) {
		
		TheRepository=ix;
        if 	(
            TheInterface=TheRepository->iGet<T>(ModuleID,err_msg);
            (TheInterface==nullptr)
            ) {

			TheInterface=ptr;
		}
	}

	void DisableCleanupOnDestruction() {

		CleanupOnDestruction=false;
	}

	operator bool () {

		if (TheInterface==nullptr) {

			return(false);
		}
		
		return(true);
	}

	T* operator->() { 

		if (TheInterface==nullptr) {
			
			throw(this);
		}

		return(TheInterface);
	}

	void Release() {

		TheInterface=nullptr;
		if (TheRepository) {

			TheRepository->iRelease(ModuleID);
			TheRepository=nullptr;
		}

		ModuleID=1000;
	}

	void Detach() {
	/*	for a global shutdown, the repository may be gone by the time the local destructors are called
		1.	detach all 'global' objects in the containing class's destructor
		*/
		TheInterface=nullptr;
		TheRepository=nullptr;
	}

	~unique_ix() {

		if (CleanupOnDestruction && TheRepository) {

			TheRepository->iRelease(ModuleID);
		}
	}


	private:
	unsigned long ModuleID {1000};
	T *TheInterface {nullptr};
	iIXmgr *TheRepository {nullptr};
	bool CleanupOnDestruction {true};
};



class IXmgr:public iIXmgr {
/*  this class implements that management and stores 1-n registered interfaces
    1.	an interface is a raw pointer to a persistant object
    */

    public:
	iIXmgr* GetAbstractInterface() { return(this); }

	void iRelease(unsigned long module_id) override { }

    void Set (iX *ref) override {
	/*	register an interface */

        bool set{false};

		for (auto &ptr:iXRepository) {
		/*	test each element in the repository to see if its the same type as the passed ref */

            if (ref->IsSameTypeAs(ptr)) {
			/* 	true if the 'Interface' contained by ptr matches the type of ref->Interface */

                ptr=ref;
                set=true;
            }
        }

        if (!set) {
            iXRepository.push_back(ref);
        }
    }

    bool Get(iX *ref,iX* &ix) override {
	/*	get an interface */

		for (auto &ptr:iXRepository) {

            if (ref->IsSameTypeAs(ptr)) {
			/* true if ref-Interface is the same type as ptr->Interface */

                ix=ptr;
                return(true);
            }
        }

        return(false);
    }

	bool Get(iX *ref,iX* &ix,unsigned long &module_id) override { return(false); }
#if 0
	bool Get(iX *ref,iX* &ix,unsigned long &module_id) override {
		ix=nullptr;
		module_id = 0; 	

		iIXmgr *ix_module=nullptr;
		for (auto &lib:LoadedLibs) {
			/*      search the loaded modules for the requested interface */

			if (lib) {

				ix_module=lib->GetIxMgr();
				if (ix_module->Get(ref,ix)) {

					module_id=lib->GetModuleID();
					if (ix->IsNotReferenceCounted()==false) {

						lib->IncrementUseCount();
					}

					return(true);
				}
			}
		}

		module_id=1000;
		for (auto &ptr:iXRepository) {

			if (ref->IsSameTypeAs(ptr)) {

				ix=ptr;
				return(true);
			}
		}


		return(false);
	}

#endif

	void Delete(iX *ref) override {

		vector<iX*>::iterator index=iXRepository.begin();
		while (index!=iXRepository.end()) { 

            if (ref->IsSameTypeAs(*index)) {

				iXRepository.erase(index);
			}

            ++index;
        }
	}

    void Reset() {
	/* reset the repository
	   1.	preserve persistant interfaces
	   */

        vector<iX*>::iterator index=iXRepository.begin();
        while(index!=iXRepository.end()) {

            if ((*index)->IsNotReferenceCounted()) {
                index++;
            }
            else {
                delete(*index);
                index=iXRepository.erase(index);
            }
        }
    }

	protected:
    vector<iX*> iXRepository;
};

#endif
