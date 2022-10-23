
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's test executive plugin's user interface.

			    Do not modify.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/


#ifndef ITESTEXECGUI_1U
#define ITESTEXECGUI_1U

#include "IxMgr.hpp"
#include "IOString.hpp"
#include "QString"

#include <iUniConsole.hpp>

using imx::IOString;

class iTestExecGui_1u {

    public:
    virtual ~iTestExecGui_1u() {}

	virtual void SetFocus()=0;
	virtual void SetWindowTitle(IOString name)=0;

	virtual void AddTest(IOString name,IOString description)=0;
    virtual void Initialize(iIXmgr *ix)=0;

	virtual unsigned AppendRow(unsigned width)=0;
	virtual unsigned AppendColumn(IOString name,unsigned width)=0;

    virtual void InitTestStatus() =0;
    virtual void ClearCurrentTestStatus() =0;

	virtual void UpdateTestStatus(unsigned test_slot, int test_id, int status)=0;
	/*  update the gui with the current test result
	    1.	test_id is the 0 based index into the test list.
	    2.	status is the pass/fail status (0=fail, 1=suspect, 2=pass
	    */

    virtual void SetTimerDisplayLabel(IOString label) =0;
    virtual void EnableSessionControls(bool enable) =0;

    virtual void SetUutSn(unsigned test_slot, IOString msg)=0;
	virtual void MarkUutAsTested(unsigned test_slot, int id)=0;

	virtual void SetProgressBarPosition(unsigned position)=0;

	virtual void ShowDialog(bool error, IOString msg)=0;
	virtual bool GetUutID(IOString &sn, IOString &extended_sn)=0;
	virtual bool GetUserInput(IOString prompt, IOString &result)=0;

	virtual bool GetLoopEnable()=0;

    virtual void SetMgmtIx(iUniConsole::Console *ptr)=0;
};

#endif
