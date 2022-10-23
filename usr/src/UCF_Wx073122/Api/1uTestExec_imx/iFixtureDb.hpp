

#ifndef IFIXTUREDB
#define IFIXTUREDB

#include <string>
#include <vector>
#include <utility>
#include <list>
#include <map>
#include <string>
#include <limits>

#include <IOString.hpp>

#ifdef _WIN64
#undef min
#undef max
#endif


class iFixtureDb{

    public:

    virtual ~iFixtureDb() {}

    virtual bool CreateDatabaseIfNotExist()	=0;
	virtual bool IncrementInsertionCount(std::string sn) 	=0;
	virtual int  GetFixtureInsertionCount(std::string sn) 	=0;
	virtual bool UpdateInsertionCount(std::string sn, int new_count)=0;
	virtual bool CreateRecord(std::string sn)=0;
};

#endif
