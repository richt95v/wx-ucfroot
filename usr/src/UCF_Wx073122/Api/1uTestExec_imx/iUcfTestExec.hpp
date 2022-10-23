
#ifndef IUCfTESTEXEC
#define IUCfTESTEXEC

#include "iMethodServer.hpp"
#include <iTestExec-1u.hpp>

using CmdRepository::CmdParam;

class iUcfTestExec {
    public:
    iUcfTestExec(
        CmdParam &msg,
        unsigned long &module_id
        );

    iUcfTestExec* SaveMeasurement( const char *name_space, const char *name, double value, unsigned limit_pct=30);
    /*  save a measurement value and automatically generate default test limits */

    iUcfTestExec* SaveMeasurement(
        const char *name_space,
        const char *name,
        double value,
        double lo_limit,
        double hi_limit,
        bool update_test_spec=true
        );

    iUcfTestExec* GetMfgId(unsigned &id, imx::IOString name, imx::IOString location);

    iUcfTestExec* SetSn(const char *sn);

    const char* GetSn();

    bool WriteTestOutput(imx::IOString msg, int type);

    bool IsValid(); 

    private:
    iTestUtility *TheExec {nullptr};
    unsigned Slot {0};
};

#endif
