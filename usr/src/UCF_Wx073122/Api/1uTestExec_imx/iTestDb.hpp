

#ifndef ITESTDB
#define ITESTDB

#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <memory>
#include <list>
#include <map>
#include <string>
#include <limits>
#include <functional>

#include <iXdata.hpp>

#include <IOString.hpp>
#include <optional>

#ifdef _WIN64
#undef min
#undef max
#endif

using std::tuple;

class TestProfile {
	public:
	std::string ProfileName;
	std::string Description; 
	unsigned 	TestSpecIndex {0};
	std::string TestSpecName;
	std::string TestSpecVersion;
	std::string TestSpecDescription;
};

class UUTRecord {
	public:
	int 		ID;
	std::string SerialNumber;
	std::string ExtSerialNumber;
	std::string DutType;
};

class UUTInstance {
	public:
	unsigned ID {0};
	unsigned SessionID {0};
	std::string Context {0};
	std::string ContextDescription;
	std::string TestSpec;
	std::string TestSpecVersion;
	unsigned TestFixture;
	unsigned Status {0};
	std::string TimeStamp;
	std::vector<double> Measurements;
};

class TestLimit {
	public:
	std::string NameSpace;
	std::string Name;
	double LoLimit{std::numeric_limits<double>::min()};
	double HiLimit{std::numeric_limits<double>::max()};
};

class MeasurementValue {

	public:
	MeasurementValue() {
		Reset();
	}

	void Reset() {
		Group.clear();
		Name.clear();
		Value.clear();
		Valid=false;
	}

	void Set(std::string group, std::string name, std::string value) {

		Group=group;
		Name=name;
		Value=value;

		Valid=true;
	}

	bool IsValid() { return(Valid); }

	void SetValid(bool enable=true) { Valid=enable; }

	std::string Group;
	std::string Name;
	std::string Value;

	bool Valid {false};
};


class iTestDb {

    public:

    virtual ~iTestDb() {}

    virtual bool CreateDatabaseIfNotExist() =0;

	virtual bool ResetData()=0;
	virtual bool ResetLimits()=0;

    virtual int SetTestSessionIndex(char *mfg_id, char *location)=0;
    /*	get a unique id for the current test session. A test session is 1-n test runs that have:
        1.	The same configuration
        2.	The same machine in the same location
        */

	virtual bool SaveSessionAttributes(char *name_space, char *attribute)=0;
	/*  save a configuration attribute for the current session
	    1.	program/module names
	    2.	operator name
		3.	configuration attributes
		~~	maybe put in namespace/name scheme
	    */

	virtual unsigned GetUUTIndexEntry(const char *sn,const char *dut_type)=0;
    /* 	get the index of a previously tested uut based on the passed params
        1.	ceate a new entry if it doesn't already exist
        */

	virtual unsigned GetUUTInstance (unsigned uut_index, unsigned context,const char *test_spec,const char *test_fixture, unsigned slot=0)=0;
	/*	get a new record to attach measurement results to */

    virtual bool SaveMeasurement(unsigned uut_id, unsigned long stript_chart_index, unsigned loop_count, unsigned test_id, const char *group, const char *name, double value,int status) =0;
	/*  save a test measurement
        1.	status->0=fail, 1=suspect, 2-pass, 3-no limits
	    */

	/***********************************************************************************************************************
	Utility
	*/

	virtual int GetTestStatusFromInstanceID(int instance_id, std::string &spec, std::string &date)=0;
	/*	Get the status of the last test for an entry in the uut index */

	virtual unsigned GetContextID(std::string name)=0;
	/*  get an id for a selected context
	    1.	insert the name if not current in
	    */

	virtual bool GetTestSpecName(std::string profile_name, std::string &test_spec_name, std::string &dut_type_name)=0;
	/*  get the following params based on the profile extension
	    1.	tests spec name
	    2.	dut type name
	    3.	record id of the test spec from the test spec index
	    */

	virtual bool GetTestSpecName(unsigned testspec_id, imx::IOString name, imx::IOString version, imx::IOString dut_type,imx::IOString description)=0;

    virtual unsigned VerifyTestLimits(const char *group, const char *name, double value, double &lolimit,double &hilimit, bool auto_update=false) =0;
	/*	get the test limits for the specified parameter */

	virtual bool GetSessionAttributes(unsigned session_id, std::vector<std::string> &attributes)=0;

	virtual bool GetContext(unsigned context_id, std::string &name, std::string &description)=0;

    //virtual bool GetMeasurements(unsigned instance_id, std::vector<MeasurementValue> &measurements)=0;
	/* 	std::tie(group,param,value,status)=measurements */

	virtual bool GetMfgID(unsigned &mfg_id, imx::IOString &name, imx::IOString &location) =0;

    virtual bool UpdateUUTStatus(unsigned intance_id,unsigned status) =0;
	/*	set the overall test status for the specified instance */

    virtual int GetUUTStatus(unsigned instance_id) =0;
	/*	get the status of a previously set/configured uut instance */
};

#endif
