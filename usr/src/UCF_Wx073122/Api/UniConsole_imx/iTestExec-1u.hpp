
#ifndef ITESTEXECFRAMEWORK
#define ITESTEXECFRAMEWORK

#include <iAobj.hpp>
#include <iAppEngine.hpp>
#include <ConnectionID.hpp>
#include <IOString.hpp>
#include <limits>

#include "iMethodServer.hpp"

using CmdRepository::CmdParam;

enum class TestXtorType {SessionCtor, SessionDtor, SequenceCtor, SequenceDtor};

class TestXtor {

    public:
    TestXtor(TestXtorType type, int param=0) {
        Type=type;
        Param=param;
    };

    TestXtorType GetXtorType()  { return(Type); }

    int GetParam()              { return(Param); }
    void SetParam(int param)    { Param=param; }

    private:
    TestXtorType Type {TestXtorType::SequenceCtor};
    int Param {0};
};

class iTestExec_1u {

    public:
	virtual ~iTestExec_1u() {}

    virtual void SetStripChartTimerCallback(iAobj<unsigned long> *ptr, unsigned delay_s)=0;

    virtual bool LaunchStartTest(bool stop_on_first_fail, unsigned loop_count)=0;
    virtual bool StartTest(bool stop_on_first_fail, unsigned loop_count)=0;
    virtual bool TestSequenceIsCurrentlyInProgress()=0;

    virtual void AbortTestSequenceAsync()=0;
    virtual void StopTestSequenceSync()=0;

	virtual unsigned GetSlotCount()=0;
};

class iTestUtility {
	public:

    virtual void AbortTestSequenceAsync()=0;
    virtual void StopTestSequenceSync()=0;

	virtual void EnableTestSlot(unsigned slot_id, bool enable=true)=0;

	virtual void SetSn(const char *sn,unsigned test_slot)=0;
	virtual const char* GetSn(unsigned test_slot)=0;
	virtual void GetMfgID(unsigned &id,IOString &name, IOString &location)=0;
	virtual void GetFixtureID(IOString &id) =0;

	virtual void SetWindowTitle(IOString title)=0;

	virtual void RegisterTestFunction(IOString name, IOString description, iAobj<CmdParam> *ia)=0;

    virtual void RegisterTestMgr(iAobj<TestXtor> *ia)=0;

	virtual bool SaveMeasurement(
		unsigned test_slot,
		const char *name_space,
		const char *name,
		double value,
		double lo_limit=-1e6,
		double hi_limit=1e6,
		bool update_test_spec=true
		)=0;

    virtual bool SaveStripChartMeasurement(unsigned test_slot,const char *name_space, const char *name, double value)=0;

    virtual void WriteTestOutput(unsigned test_slot, imx::IOString msg, int type)=0;

	protected:
};
//
//new Aobj<TemplateModule,iIXmgr*>(&TemplateModule::TestCmd,&TheModule),"a template command description");

#endif
