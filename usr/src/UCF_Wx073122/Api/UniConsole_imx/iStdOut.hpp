
#ifndef ISTDOUT
#define ISTDOUT

#include <string>
#include <vector>

#ifdef __linux__
#else
#pragma warning (disable:4100)	// forma parameter no referenced
#pragma warning (disable:4018)	// signed, unsigned mismatch
#endif

/***************************************************************************************************************************
 
This file defines an interface to an object that is responsible for displaying messages within an application program. 
 
    1.	WriteStdout messages are generally equivelant to status and informational messages displayed in a console program.

****************************************************************************************************************************/

#include <string>
#include <vector>

class Table;

class iStdOut {
/*  This class provides an interface to the STDOUT subsystem
    */

	public:

    enum ParagraphType {
		Heading1=0, Heading2,Heading3,
		Text1,Text2, Text3, GrayText,ItalicText, 
		Warning, Error, Pass, SystemSoftError, SystemHardError,
		Directory, Filename1, Filename2, Filename3
	};
	/*	WriteStdOut(...) message type definitions  */

	enum TraceLevel {
		TraceError=50, 
		FrameworkStart=60, FrameworkInfo, FrameworkStop,
		BlockStart=70, BlockInfo, BlockStop,
		DeviceOpen=90, DeviceInfo, DeviceWrite, DeviceRead, DeviceClose
	};
	/*	WriteStdTrace(...) message type definitions  */

    virtual iStdOut* WriteStdOut(const char *msg,int option=0,unsigned console_id=0)=0;
	/*  Write a message to the stdout server from the application level or device IO level code

	    1.	char *msg is the message to display
		2.	int option = a 'ParagraphType' to define the nature of the text to be displayed
	    3.	int console_id specifies which console output to write to.  The number of availiable consoles
		    is application dependant
		*/

	//virtual iStdOut* WriteStdTrace(const char *msg, int level=0)=0;
	/*  Write a message to the stdout server from system and device IO level code
	 
	    1.	char *msg is the message to display
		2.	int level = a 'TraceLevel' value to define the type of trace message to be displayed
	    */

	virtual iStdOut* RegisterConsoleHandle(const char *label, unsigned handle)=0;
	virtual unsigned GetRegisteredConsoleHandle(const char *label)=0;

	virtual iStdOut* CreateTable(unsigned console_id,unsigned rows, unsigned col_count,int *col_widths)=0;
	/*	create a table in a console window output */

	virtual iStdOut* WriteTableRows(unsigned console_id,unsigned row, Table *data,unsigned type=iStdOut::Text1)=0;
	/*	Populate the table created in the console window */

	virtual iStdOut* WriteTableRow(unsigned console_id,unsigned row, const char *labls,unsigned type=iStdOut::Text1)=0;
	virtual iStdOut* WriteTableCell(unsigned console_id,unsigned row, unsigned col, const char *label,unsigned type=iStdOut::Text1)=0;
	/*  Populate the table created in the console window
	    - deprecated
	    */
};

class Table {
/*  This class defines a text table displayed in a console window
    1.	Objects are dynamically allocated, and must be deleted in the slot handler that receives
	    the reference.
    */

	public:
	Table()=delete;
	Table(const Table&)=delete;
	Table& operator = (const Table)=delete;
	
	static Table* GetNewTable(iStdOut *ptr,unsigned console_id=0) {
	/*  create a new instance
	    1.	This is the only supported process
	    */
		
		return(new Table(ptr,console_id));
	}
	

	Table* operator()() { return(this); }


	Table* CreateTable(std::string heading,unsigned rows, unsigned col_count,unsigned hdr_type=iStdOut::Directory) {
	/*	Create the table */

		TheInterface->CreateTable(ConsoleID,rows,col_count,Columns.data());
		TheInterface->WriteTableRow(ConsoleID,0,heading.c_str(),hdr_type);

		return(this);
	}

	Table* SelectRow(unsigned row) { CurrentRow=row; return(this); }
	/*  Set the current table row to operate on
	    1.	Used by SetCell(..) calls
	    */

	Table* SetCell(unsigned col, std::string label, unsigned type=iStdOut::Text1) {


		TheInterface->WriteTableCell(ConsoleID,CurrentRow, col, label.c_str(),type);
		return(this);
	}

	Table* SetCell(unsigned col, int value, unsigned type=iStdOut::Text1) {

		TheInterface->WriteTableCell(ConsoleID,CurrentRow, col, std::to_string(value).c_str(),type);
		return(this);
	}

	Table* SetCell(unsigned col, double value, unsigned type=iStdOut::Text1) {

		TheInterface->WriteTableCell(ConsoleID,CurrentRow, col, std::to_string(value).c_str(),type);
		return(this);
	}

	Table* SetColumns(std::vector<int> cols) {
	/*	Configure the number of table columns */

		Columns=cols;
		return(this);
	}

	Table* FillRow(const char *row) {
	/*  Save, but do not display a string
	    1.	The string is tab delimitted to specify columns
	    */

		Data.push_back(row);
		return(this);
	}

	void DisplayTable(std::string heading,unsigned hdr_type=iStdOut::Directory,unsigned data_type=iStdOut::Text1) {
	/*  Display a table
	    1.	Create the table output in a console using a size determined by
	    	a.	A previous call to SetColumns(...)
	    	b.	The number of rows in the output buffer
	    	*/

		TheInterface->CreateTable(ConsoleID,static_cast<int>(Data.size())+1,static_cast<int>(Columns.size()),Columns.data());
		TheInterface->WriteTableRow(ConsoleID,0,heading.c_str(),hdr_type);

		TheInterface->WriteTableRows(ConsoleID,1,this); 
	}

	Table* ClearData() { 
	/*	Clear the buffered table contents */
		Data.clear(); 
		DataIndex=0;
		return(this);
	}

	Table* InsertRow(std::string line) {
	/*	Write a row into the buffer */
		Data.push_back(line);
		return(this);
	}

	bool GetLine(const char* &line) {
	/*  Get a table line from the buffer
	    1.	Used internally
	    2.	DataIndex must be externally incremented
	    */

		if (DataIndex<Data.size()) {

			line=Data[DataIndex++].c_str();
			return(true);
		}

		return(false);
	}

	private:
	Table(iStdOut *ptr,unsigned console_id=0) { TheInterface=ptr; ConsoleID=console_id; }
	
	unsigned DataIndex {0};
	std::vector<std::string> Data;
	/*	The output buffer */

	unsigned ConsoleID {0};
	/*	The console to display the table in */
	
	unsigned CurrentRow {0};
	/*	The row selected by a call to SelectRow(...) */

	std::vector<int> Columns;
	/*	Column width information */

	iStdOut *TheInterface {nullptr};
	/*	The interface to the display subsystem that updates the selected console / table */

};

#endif
