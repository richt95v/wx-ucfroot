
#ifndef SQLITETESTDB
#define SQLITETESTDB

#include <iTestDb.hpp>
#include <iXdata.hpp>

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <list>

using std::string;
using std::vector;
using std::map;

class SqliteTestDb: public iTestDb {

    public:
	SqliteTestDb() = default;
	void Configure(std::string path, std::string static_db, std::string limit_db, std::string measurement_db);

    bool CreateDatabaseIfNotExist();

	bool ResetData();
	bool ResetLimits();

	/******************************************************************************************************** 
	session setup 
	*/

    int SetTestSessionIndex(char *mfg_id, char *location);
    /*	get a unique id for the current test session. A test session is 1-n test runs that have:
        1. The same configuration
        2. The same machine in the same location
		3. The value is saved in 'SessionID'	
        */

    unsigned GetUUTIndexEntry(const char *sn, const char *dut_type) override;
    /* 	get the index of a previously tested uut based on the passed params
        1. ceate a new entry if it doesn't already exist
        */

    unsigned GetContextID(std::string name) override;
    /*  get an id for a selected context
	1. insert the name if not current in
	*/

	bool GetTestLimits(unsigned spec_id,std::vector<TestLimit> &limits);
	/*	get the test limits based on a test spec id */

	bool GetTestSpecName(std::string profile_name, std::string &test_spec_name, std::string &dut_type_name) override;
	/*  get the following params based on the profile extension
	    1.	tests spec name
	    2.	dut type name
	    3.	record id of the test spec from the test spec index
	    */

    bool GetTestSpecName(unsigned testspec_id, imx::IOString name, imx::IOString version, imx::IOString dut_type,imx::IOString description) override;

    bool GetContext(unsigned context_id, string &name, string &description) override;
    bool GetMfgID(unsigned &mfg_id, imx::IOString &name, imx::IOString &location) override;

    bool SaveSessionAttributes(char *name_space, char *attribute) override;
    bool GetSessionAttributes(unsigned session_id, vector<string> &attributes) override;

	bool GetTestParamLabels(int spec_id, std::vector<tuple<std::string, std::string>> &param_labels);
	/*  get a list of the test parameter labels associated with the specified spec_id
		1.	will return 'group name', 'parameter name' for general parameters
		2.	will return 'group name', '*' for dynamic labels
		*/

    unsigned VerifyTestLimits(const char *group, const char *name, double value, double &lolimit,double &hilimit, bool auto_update=false) override;

    bool SaveMeasurement(unsigned uut_id, unsigned long stript_chart_index, unsigned loop_count, unsigned test_id, const char *group, const char *name, double value,int status) override;

    unsigned GetUUTInstance (unsigned uut_index, unsigned context,const char *test_spec,const char *test_fixture,unsigned slot=0) override;

	bool GetContext(unsigned instance_id, string &context);
	bool GetTestFixture(unsigned instance_id, string &test_fixture);

    //bool GetMeasurements(unsigned instance_id, vector<MeasurementValue> &measurements) { return(false); }
	/* 	std::tie(group,param,value,status)=measurements */

    bool UpdateUUTStatus(unsigned id,unsigned status) override;
    int GetUUTStatus(unsigned instance_id) override;

	int GetTestStatusFromInstanceID(int instance_id, std::string &spec, std::string &date) override;

    bool TestSpecNameFromIndex(unsigned spec_id,std::string &name, std::string &version);

	private:
	unsigned GetUUTIndexFromInstance(int instance_id);

	private:
	int SessionID {0};
	int CurrentTestSpecID {0};

	std::string
		Path,
		StaticDbName,
		LimitDbName,
		MeasurementDbName;
};

#endif
