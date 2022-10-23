
#include <SqliteIO.hpp>
#include <SqliteFixtureDb.hpp>


void SqliteFixtureDb::Configure(std::string path, std::string db_name) {

	Path = path;
	Name = db_name;

	CreateDatabaseIfNotExist();
}

bool SqliteFixtureDb::CreateDatabaseIfNotExist() {

	SqliteIO db;

	if (db.OpenDb(Path.c_str(), Name.c_str())) {

		/* each entry defines a unique test specification */
		db.Exec(
		   "Create Table if not exists TestFixtureIndex ("
		   "SerialNumber	text primary key, "
		   "InsertionCount	integer"
		   ")"
		   );

		db.Exec(
		   "insert into TestFixtureIndex(SerialNumber,InsertionCount) values ('FX000',0)"
		   );

		return (true);
	}

	return (false);
}

bool SqliteFixtureDb::IncrementInsertionCount(std::string sn) {

	bool status {false};
	if (int count=GetFixtureInsertionCount(sn)) {

		status=UpdateInsertionCount(sn,count+1);
	}
	else {

		status=CreateRecord(sn);
	}

	return(status);
}

int SqliteFixtureDb::GetFixtureInsertionCount(std::string sn) {

	int count {0};
	SqliteIO db;

	if (db.OpenDb(Path.c_str(), Name.c_str())) {

		db.Exec(
		   "select InsertionCount from TestFixtureIndex where SerialNumber='%-.128s'",
		   sn.c_str()
		   );

		if (db.GetResultSet()) {

			db.GetResultSetToken(0, count);
		}
	}

	return(count);
}

bool SqliteFixtureDb::UpdateInsertionCount(std::string sn, int new_count) {

	int count {0};
	SqliteIO db;

	if (db.OpenDb(Path.c_str(), Name.c_str())) {

		db.Exec(
		   "update TestFixtureIndex set InsertionCount=%d where SerialNumber='%-.128s'",
		   new_count,
		   sn.c_str()
		   );

		return(true);
	}

	return(false);
}

bool SqliteFixtureDb::CreateRecord(std::string sn) {

	SqliteIO db;

	if (db.OpenDb(Path.c_str(), Name.c_str())) {

		db.Exec( 
		   "insert into TestFixtureIndex(SerialNumber,InsertionCount) "
		   "values ('%-.48s',1)",
		   sn.c_str()
		   );

		return(true);
	}

	return(false);
}

