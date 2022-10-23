
#ifndef ITABBEDTABLE
#define ITABBEDTABLE

#include <vector>
#include <string>
#include <iStdOut.hpp>

#ifdef __linux__
#else
#pragma warning (disable:4100)	// forma parameter no referenced
#pragma warning (disable:4018)	// signed, unsigned mismatch
#endif

class iTabbedTable {

	public:
	//virtual void WriteText(unsigned console_id, std::string msg, int type)=0;

	virtual bool CreateTable(unsigned console_id, unsigned rows, unsigned col_count,int *col_widths)=0;
	virtual bool InsertIntoTableRow(unsigned console_id,unsigned type,unsigned row,char *str) 						{ return(false); }
	virtual bool InsertIntoTableRow(unsigned console_id,unsigned type,unsigned row,std::vector<std::string> str) 	{ return(false); }

	virtual bool InsertIntoTableRows(unsigned console_id,unsigned type,unsigned row,Table *data) 					{ return(false); }
	virtual bool InsertIntoTableCell(unsigned console_id,unsigned type, unsigned row,unsigned col,const char *str) =0;
};

/*
notes 
 
1.	depending on the implementation, not all interfaces will be appropriate. a default implementation is provided 
*/

#endif
