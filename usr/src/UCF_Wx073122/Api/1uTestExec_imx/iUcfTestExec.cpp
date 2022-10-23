
#include <iUcfTestExec.hpp>
#include <math.h>

iUcfTestExec::iUcfTestExec(
    CmdParam &msg,
    unsigned long &module_id
    ) {
    /*  Get the test exec virt interface */

    if (TheExec=msg.iGet<iTestUtility>(module_id)) {

        Slot=msg.GetOption();
    }
}

iUcfTestExec* iUcfTestExec::SaveMeasurement( const char *name_space, const char *name, double value, unsigned limit_pct) {

    double lo_limit {0};
    double hi_limit {0};
    double offset   {0};

    if ((limit_pct<=0) || (limit_pct>100)) { limit_pct=30; }
    /*  range check limit_pct */

    offset= limit_pct/100.0;
    offset*=value;
    offset=fabs(offset);
    /*  find the offset value that is the spec'ed pct of value */

    hi_limit=value+offset;
    lo_limit=value-offset;

    if (TheExec) {

        TheExec->SaveMeasurement(Slot,name_space, name,value, lo_limit, hi_limit, true );
    }

    return(this);
}

iUcfTestExec* iUcfTestExec::SaveMeasurement(
    const char *name_space,
    const char *name,
    double value,
    double lo_limit,
    double hi_limit,
    bool update_test_spec
    ) {

    if (TheExec) {

        TheExec->SaveMeasurement(Slot,name_space, name,value, lo_limit, hi_limit, update_test_spec);
    }

    return(this);
}

iUcfTestExec* iUcfTestExec::GetMfgId(unsigned &id, imx::IOString name, imx::IOString location) {

    if (TheExec) {

        TheExec->GetMfgID(id,name,location);
    }

    return(this);
}

iUcfTestExec* iUcfTestExec::SetSn(const char *sn) {

    if (TheExec) {

        TheExec->SetSn(sn,Slot);
    }

    return(this);
}

const char* iUcfTestExec::GetSn() {

    if (TheExec) {

        return(TheExec->GetSn(Slot));
    }

    return(nullptr);
}

bool iUcfTestExec::WriteTestOutput(imx::IOString msg, int type) {

    if (TheExec) {

        TheExec->WriteTestOutput(Slot, msg, type);
        return(true);
    }

    return(false);
}

bool iUcfTestExec::IsValid() { return(TheExec ? true:false); }
