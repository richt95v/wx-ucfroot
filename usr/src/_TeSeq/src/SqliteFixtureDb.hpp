
#ifndef SQLITEFIXTUREDB
#define SQLITEFIXTUREDB

#include <iFixtureDb.hpp>

using std::string;
using std::vector;
using std::map;

class SqliteFixtureDb: public iFixtureDb {

    public:
	SqliteFixtureDb() = default;
	void Configure(std::string path, std::string name);

	bool CreateDatabaseIfNotExist();
	bool IncrementInsertionCount(std::string sn);
	int  GetFixtureInsertionCount(std::string sn);
	bool UpdateInsertionCount(std::string sn, int new_count);
	bool CreateRecord(std::string sn);

	private:
	std::string Path;
	std::string Name;
};

#endif
