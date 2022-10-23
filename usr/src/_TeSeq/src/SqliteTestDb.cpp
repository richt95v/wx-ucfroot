
#include <HostNameX.hpp>
#include <SqliteIO.hpp>
#include <SqliteTestDb.hpp>
#include <TextSerializer.hpp>

#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <optional>
#include <mutex>


using std::string;
using imx::IOString;

void SqliteTestDb::Configure(std::string path, std::string static_db, std::string limit_db, std::string measurement_db) {

	Path=path;

	StaticDbName=static_db;
	LimitDbName=limit_db;
	MeasurementDbName=measurement_db;

	CreateDatabaseIfNotExist();
}

bool SqliteTestDb::GetTestSpecName(std::string profile_name, std::string &test_spec_name, std::string &dut_type_name) {
/*  get the following params based on the profile extension
	1.	tests spec name
	2.	dut type name
	3.	record id of the test spec from the test spec index
	*/

	SqliteIO db;

    int spec_id {0};
	char buffer[513];
	std::string tmp;
	bool status{false};

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), LimitDbName.c_str())) {

        sprintf(buffer, "select Specification from TestProfileMap where ProfileName='%-.128s'", profile_name.c_str());
        if (db.Exec(buffer); db.GetResultSet()) {

			db.GetResultSetToken(0, CurrentTestSpecID);
            status = true;
		}
		else {
        /* if the record isn't currently in the database, create a new record in the test profile map */

			CurrentTestSpecID=1;
            db.Exec(
               "insert into TestProfileMap (ProfileName,Specification,Description) values ('%-.128s','...','An auto-generated record')",
               profile_name.c_str()
               );
		}

		sprintf(buffer,"select Name,DutType from TestSpecIndex where ID=%d", CurrentTestSpecID);
        if (db.Exec(buffer); db.GetResultSet()) {
		/*	get the default name, and dut type from the test spec index - if it exists */

			db.GetResultSetToken(0, test_spec_name);
			db.GetResultSetToken(1, dut_type_name);

			status = true;
		}
		else {
		/*  create a default name, and dut type in the test spec index
		1.	this should happen rarely since record 1 is should always be a default dev record
		*/

			db.Exec(
				"insert into TestSpecIndex (Name,Version,DutType,Description) "
				"values ('DevSpec','0.01','ProtoDevice','Development test spec place holder')"
				);
				
			test_spec_name= "DevSpec";
			dut_type_name="ProtoDevice";

			status=true;
		}
	}

    lk.unlock();

    return (status);
}

bool SqliteTestDb::ResetData() {

	SqliteIO db;

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		/* each entry defines a unique test specification */
		db.Exec("delete from UUTIndex");
		db.Exec("delete from UUTInstance");
		db.Exec("delete from MeasurementData");
		db.Exec("delete from TestSessionIndex");
	}

    lk.unlock();

    return(true);
}

bool SqliteTestDb::ResetLimits() {

	SqliteIO db;

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), LimitDbName.c_str())) {

		/* each entry defines a unique test specification */
		db.Exec("delete * from TestLimits");
	}

    lk.unlock();

    return(true);
}

bool SqliteTestDb::CreateDatabaseIfNotExist() {

	SqliteIO db;

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), StaticDbName.c_str())) {

		db.Exec(
		   "Create Table if not exists MfgID ("
		   "ID			integer primary key,"
		   "Name     	text,"
		   "Location  	text"
		   ")"
		   );

		db.CloseDb();
	}

	if (db.OpenDb(Path.c_str(), LimitDbName.c_str())) {

		/* each entry defines a unique test specification */
		db.Exec(
		   "Create Table if not exists TestSpecIndex ("
		   "ID				integer primary key,"
		   "Name			text,"
		   "Version		text,"
		   "DutType		text,"
		   "Description	text"
		   ")"
		   );

		/* each entry defines an individual test limit that belongs to a unique test spec in TestSpecIndex */
		db.Exec(
		   "Create Table if not exists TestLimits ("
		   "ID				integer primary key,"
		   "Specification	int references TestSpecIndex(ID),"
		   "GroupName		text,"
		   "Parameter		text,"
		   "Lolimit		real not null,"
		   "Hilimit		real not null,"
		   "Description	text"
		   ")"
		   );

		/*	each entry ties a test spec to a specific test profile name */
		db.Exec(
		   "Create Table if not exists TestProfileMap ("
		   "ID				integer primary key,"
		   "Specification	int references TestSpecIndex(ID),"
		   "ProfileName	text,"
		   "Description	text"
		   ")"
		   );


		db.CloseDb();
	}

	if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec(
		   "Create Table if not exists Context ("
		   "ID			integer primary key,"
		   "Name       	text,"
		   "Description	text"
		   ")"
		   );

		db.Exec(
		   "Create Table if not exists TestSessionIndex ("          //  ?? each slot needs a sequence id per session
		   "SeqID		integer primary key,"                        //	this table is aka as a 'configuration index'
		   "HostID		text not null default '-',"                 //  computer name
		   "MfgID		text not null default '-',"               //  foreign key into a mfg id table
		   "Location	text not null default '-'"                 //  foreign key into a mfg id table
		   ")"
		   );

		db.Exec(
		   "Create Table if not exists TestSessionAttributes ("    // list modules used to create the data
		   "ID			integer primary key,"
		   "SeqID		int references TestSessionIndex(SeqID), "       //  foreign key into TestSessionIndex
		   "NameSpace 	char(254) not null,"            //  attribute name
		   "Attribute	char(254) not null"          //  attribute name
		   ")"
		   );

		/** new **/
		db.Exec(
		   "Create Table if not exists UUTIndex("
		   "ID				integer primary key,"
		   "Sn            	text not null,"
		   "ExSn           	text not null default 0,"
		   "DUTType        	text not null default 'Undefined DUT Type'" //  the type of dut, part number, etc
		   ")"
		   );

		db.Exec(
		   "Create Table if not exists UUTInstance ("
		   "ID				integer primary key,"
		   "SessionID	    integer not null references TestSessionIndex(SeqID),"
		   "DeviceID       	integer not null references UUTIndex(ID),"              // foreign key into uutindex
																					//"InstanceID     integer not null references UUTIndex(InstanceCount),"   //  corresponds to InstanceCount in UUTIndex
		   "Context        	text not null,"               							//  foreign key into the context table
		   "TestSpec       	text not null,"         								//  functional, systems, etc test
		   "TestFixture    	text not null,"
		   "Slot			integer not null default '0',"
           "Status	    	integer not null default -1," 							//	required so that any reporting utility does not have to process all values against limits to determine last status
           "TestTimeStamp  	TIMESTAMP default CURRENT_TIMESTAMP"                    // to enumerate instances, order by timestamp
           ")"
		   );

		db.Exec(
           "Create Table if not exists MeasurementData ("
           "ID				integer primary key,"
           "DataInstanceID	integer not null references uutinstance(ID),"
           "TimeIndex		integer not null default '0',"							//	strip chart index
           "LoopCount		integer not null default '0',"
           "TestID			integer not null default '0',"
           "GroupName	    text not null,"
           "Parameter	    text not null,"
           "Value	    	real not null"
           ")"
           );

        /****	performance things ****/

		//TheDb.Exec("PRAGMA journal_mode = OFF");
		//TheDb.Exec("PRAGMA temp_store = MEMORY");

		db.Exec("CREATE INDEX `MeasurementInstanceID` ON `MeasurementData` ( `DataInstanceID` )");

		db.Exec("insert into Context (ID,Name,Description) values (0,'Development','Context used for development')" );
		db.Exec("insert into Context (ID,Name,Description) values (1,'Evaluation','Context used for experiments that occur during production')" );
		db.Exec("insert into Context (ID,Name,Description) values (2,'Debug','Production debug operations')" );
		db.Exec("insert into Context (ID,Name,Description) values (3,'Production','Normal production operations')" );

	}

    lk.unlock();

    return (true);
}

unsigned SqliteTestDb::GetContextID(string name) {

    int tmp {0};
	SqliteIO db;

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), LimitDbName.c_str())) {

		db.Exec(
		   "select id from context where name='%-.128s'",
		   name.c_str()
		   );

		if (db.GetResultSet()) {

            db.GetResultSetToken(0, tmp);
            return(tmp);
		}
		else {

			db.Exec(
			   "insert into context (Name,Description) values ('%-.128s','...') returning id",
			   name.c_str()
			   );

			if (db.GetResultSet()) {

                db.GetResultSetToken(0, tmp);
                return(tmp);
            }
		}
	}

    lk.unlock();

    return (0);
}

int SqliteTestDb::SetTestSessionIndex(char *mfg_id, char *location) {
/*	get a unique id for the current test session. A test session is 1-n test runs that have:
	1.	The same configuration
	2.	The same machine in the same location
	*/

	SqliteIO db;
	HostNameX host_id;

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec(
		   "insert into testsessionindex (hostid, mfgid, location) values ('%-.64s',%-.64s,'%-.64s')",
		   host_id.GetName(),
		   mfg_id,
		   location
		   );

		db.Exec("SELECT last_insert_rowid()");
		if (db.GetResultSet()) {

			db.GetResultSetToken(0, SessionID);
		}
	}

    lk.unlock();

    return (SessionID);
}

unsigned SqliteTestDb::GetUUTIndexEntry(const char *sn, const char *dut_type) {
/* 	get the index of a previously tested uut based on the passed params
	1.	ceate a new entry if it doesn't already exist
	*/

	SqliteIO db;
    int tmp {0};

    char buffer[513];

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		sprintf(
		   buffer,
		   "select id from uutindex where sn='%-.128s'",
		   sn
		   );

        if (db.Exec(buffer); db.GetResultSet()) {

            db.GetResultSetToken(0, tmp);
            //return(tmp);
        }
		else {

			db.Exec(
			   "insert into uutindex (sn,duttype) values ('%-.128s','%-.128s')",
			   sn,
			   dut_type
			   );

			db.Exec("SELECT last_insert_rowid()");
			if (db.GetResultSet()) {

                db.GetResultSetToken(0, tmp);
                //return(tmp);
            }
		}
	}

    lk.unlock();

    return (tmp);
    ///return (0);
}

unsigned SqliteTestDb::GetUUTInstance(unsigned uut_index, unsigned context, const char *test_spec, const char *test_fixture, unsigned slot) {
/*  create a new uut instance
	1.	represents a single test run for the uut
	2.	all measurements for the test run will reference this record

	foreign keys that must be set
	1.	TestSessionIndex(SeqID)
	2.	UUTIndex(ID)
	3.	Context(ID)
	*/


	SqliteIO db;
    unsigned tmp {0};

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec(
		   "insert into uutinstance (sessionid, deviceid, context, testspec,testfixture,slot) "
		   "values (%d,%d,%d,'%-.64s','%-.64s',%d) ",
		   SessionID,
		   uut_index,
		   context,
		   test_spec,
		   test_fixture,
		   slot
		   );

		db.Exec("SELECT last_insert_rowid()");
		if (db.GetResultSet()) {

            db.GetResultSetToken(0, tmp);
            //return(tmp);
        }
	}

    lk.unlock();

    return (tmp);
    //return (0);
}

bool SqliteTestDb::SaveMeasurement(unsigned uut_instance, unsigned long index, unsigned loop_count, unsigned test_id, const char *group, const char *name, double value,int status) {

	SqliteIO db;
	bool ret{false};

    std::mutex lk;
    lk.lock();

	if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

        ret=db.Exec(
           "insert into MeasurementData(DataInstanceID, TimeIndex, LoopCount, TestID, GroupName, Parameter, Value) "
           "values (%d,%d,%d,%d,'%-.128s','%-.128s',%4.4g) ",
           uut_instance,
           index,
           loop_count,
           test_id,
		   group,
		   name,
		   value
		   );

        if (ret) {
        /* 	dont update uut status if the measurement was not saved
            */

            if ((GetUUTStatus(uut_instance)<0) || (status==0)) {
            /*  update the uut status if
                1.	it has not been set yet
                2.	if the current test is a failure
                */

                UpdateUUTStatus(uut_instance, status);
            }
        }
    }

    lk.unlock();

	return (ret);
}

bool SqliteTestDb::UpdateUUTStatus(unsigned id, unsigned status) {

	SqliteIO db;

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec(
		   "update uutinstance set status=%d where id=%d",
		   status,
		   id
		   );
	}

    lk.unlock();

    return (true);
}

/***************************************************************************************************************************/

unsigned SqliteTestDb::GetUUTIndexFromInstance(int instance_id) {

	SqliteIO db;
	unsigned ret {0};

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec("select id from uutindex where id=%d",instance_id);
		if (db.GetResultSet()) {

			db.GetResultSetToken(0, ret);
		}

	}

    lk.unlock();

    return(ret);
}

bool SqliteTestDb::GetSessionAttributes(unsigned session_id, vector<string>& attributes) {

	SqliteIO db;
	std::string str;
	bool status{false};

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec("select Attribute from TestSessionAttributes where SeqID=%d", session_id);
		if (db.GetResultSet()) {

			for (unsigned row = 0; row < 100; row++) {

				if (db.GetResultSetToken(row, str)) {

					attributes.push_back(str);
					status = true;
				}
				else {
					break;
				}
			}
		}
	}

    lk.unlock();

    return (status);
}

bool SqliteTestDb::GetContext(unsigned context_id, string& name, string& description) {

	SqliteIO db;
	bool status{false};

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), LimitDbName.c_str())) {

		db.Exec("select Name, Description from Context where ID=%d", context_id);
		if (db.GetResultSet()) {

			db.GetResultSetToken(0, name);
			db.GetResultSetToken(1, description);
			status = true;
		}
	}

    lk.unlock();

    return (status);
}

bool SqliteTestDb::GetTestSpecName(unsigned testspec_id, IOString name, IOString version, IOString dut_type, IOString description) {

	SqliteIO db;
	bool status{false};
	std::string tmp;

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), LimitDbName.c_str())) {

		db.Exec("select Name, DutType, Description,Version from TestSpecIndex where ID=%d", testspec_id);
		if (db.GetResultSet()) {

			db.GetResultSetToken(0, tmp);
			name = tmp;

			db.GetResultSetToken(1, tmp);
			dut_type = tmp;

			db.GetResultSetToken(2, tmp);
			version = tmp;

			db.GetResultSetToken(3, tmp);
			description = tmp;

			status = true;
		}
	}

    lk.unlock();

    return (status);
}

bool SqliteTestDb::GetMfgID(unsigned &mfg_id, imx::IOString& name, imx::IOString& location) {

	SqliteIO db;
	bool status{false};

    mfg_id=0;
    name="Unknown Manufacturer";
    location="Unknown Location";

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), StaticDbName.c_str())) {

        db.Exec("select ID, Name, Location from MfgID limit 1", mfg_id);
		if (db.GetResultSet()) {

            db.GetResultSetToken(0, mfg_id);
            db.GetResultSetToken(1, name);
            db.GetResultSetToken(2, location);
			status = true;
		}
	}

    lk.unlock();

    return (status);
}


bool SqliteTestDb::SaveSessionAttributes(char *name_space, char *attribute) {

	SqliteIO db;
	int ret{-1};
	bool status{false};

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec(
		   "insert into testsessionattributes (SeqID, NameSpace, Attribute) values ('%d','%-.128s','%-.128s') returning ID",
		   SessionID,
		   name_space,
		   attribute
		   );

		if (db.GetResultSet()) {

			db.GetResultSetToken(0, ret);
		}
	}

    lk.unlock();

    return (status);
}

unsigned SqliteTestDb::VerifyTestLimits(const char *group, const char *name, double value, double &lolimit ,double &hilimit, bool auto_update) {

	SqliteIO db;
    unsigned status {2};

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), LimitDbName.c_str())) {

		db.Exec(
            "select lolimit, hilimit from TestLimits where specification=%d and groupName='%-.128s' and parameter='%-.128s' ",
            CurrentTestSpecID,
            group,
            name
            );

		if (db.GetResultSet()) {

            db.GetResultSetToken(0, lolimit);
            db.GetResultSetToken(1, hilimit);
		}
		else if (auto_update) {

            db.Exec(
				"insert into TestLimits (Specification,GroupName,Parameter,LoLimit,HiLimit,Description) "
				"values (%d, '%-.48s','%-.48s',%2.4f,%2.4f,'Auto-generated limit')",
				CurrentTestSpecID,
				group,
				name,
                lolimit,
                hilimit
				);
		}

        if ((value<lolimit) || (value>hilimit)) {

            status=0;
        }
	}

    lk.unlock();

    return (status);
}

bool SqliteTestDb::TestSpecNameFromIndex(unsigned spec_id,std::string &name, std::string &version) {

	bool status {false};
	SqliteIO db;

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), LimitDbName.c_str())) {

		db.Exec("select Name, Version from TestSpecIndex where ID=%d",spec_id);
		if (db.GetResultSet()) {

				db.GetResultSetToken(0, name);
				db.GetResultSetToken(1, version);

				status=true;
		}
	}

    lk.unlock();

    return (status);
}

int SqliteTestDb::GetTestStatusFromInstanceID(int instance_id, std::string &spec, std::string &date) {

	SqliteIO db;

	int ret {-2}, spec_id {0};

	spec.clear();
	date.clear();

    std::mutex lk;
    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec(
		   "select status, testtimestamp, Testspec from uutinstance "
		   "where deviceid=%d and testtimestamp > date('now', '-30 days' "
		   "order by testtimestamp desc limit 1",
		   instance_id
		   );

		if (db.GetResultSet()) {

			db.GetResultSetToken(0, ret);
			db.GetResultSetToken(1, date);
			db.GetResultSetToken(2, spec_id);
		}

		db.Exec("select name from testspecindex where ID=%d",spec_id);
		if (db.GetResultSet()) {

			db.GetResultSetToken(0, spec);
		}
	}

    lk.unlock();

    return (ret);
}

int SqliteTestDb::GetUUTStatus(unsigned instance_id) {

	SqliteIO db;

	int ret {-2};
	int tmp{0};

    std::mutex lk;

    lk.lock();

    if (db.OpenDb(Path.c_str(), MeasurementDbName.c_str())) {

		db.Exec("select status from uutinstance where id=%d", instance_id);
		if (db.GetResultSet()) {

			db.GetResultSetToken(0, tmp);
			ret = tmp;
		}
	}

    lk.unlock();

	return (ret);
}
