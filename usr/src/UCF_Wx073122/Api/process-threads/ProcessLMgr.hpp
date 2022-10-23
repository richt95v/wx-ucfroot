
#ifndef PROCESSLMGR
#define PROCESSLMGR

#include "vector"
#include "ProcessL.hpp"


using namespace std;

class ProcessLMgr {

    public:
    iProcessL* RegisterProcess(const char *name) {
        ProcessL proc(name,iProcessL::Attached);
        TheRepository.push_back(proc);
        return(&TheRepository.back());
    }

    iProcessL::iProcessLObj GetProcess(const char *name) {
        iProcessL::iProcessLObj obj(new ProcessL(name,iProcessL::Detached));
        return(obj);
    }

    void KillAll() {

        vector<ProcessL>::iterator proc_index=TheRepository.begin();
        while(proc_index!=TheRepository.end()) {
            (*proc_index).Kill();
            TheRepository.erase(proc_index);
        }
    }
    
    ~ProcessLMgr() {
        KillAll(); //   the process seems to start, it gets killed an undefined run-time instant, cant trace it down, can't find where the destructor is being called
    }

    private:
    vector<ProcessL> TheRepository;
};


#endif
