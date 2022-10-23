
#ifndef TESTSLOTS
#define TESTSLOTS


#include <string>
#include <optional>
#include <vector>

#include <iAobj.hpp>
#include <iMethodServer.hpp>

class Slot {
    public:
    unsigned UutIndex {0};
    unsigned UutInstance {0};
    /*	database references */

    std::string SerialNumber;
    unsigned CurrentTestSessionStatus {3};
    unsigned CurrentTestSequenceStatus {3};
    bool StopOnFirstFail {false};

    unsigned ConsoleID {0};

    bool Enable {true};

    void Reset();
    void ResetTestSequenceStatus(bool stop_on_first_fail);
    void SetTestSequenceStatus(unsigned status);
    void SetTestSessionStatus(unsigned status);
};

class SlotManager {

    public:

    void SetConsoleID (unsigned slot_id, unsigned console_id);
	/*	set the console id where information from the spec'd slot will be displayed */

    unsigned GetConsoleID (unsigned slot_id);
	/*	get the console id where information from the spec'd slot is displayed */

    void SetSlotCount(unsigned count);
	/*	configure the number of active slots */

    void SetSerialNumber(unsigned index, std::string sn);
	/*	set the serial numer for the spec'd slot */

    const char* GetSerialNumber(unsigned index);
	/*	get the serial numer for the spec'd slot */

    unsigned GetSlotCount();
	/*	get the number of active slots */

    void Init();
    void InitCurrentTestSessionStatus();
	/*	initialize the current test status for all active slots */

    void InitTestSequenceStatus(bool stop_on_first_fail);
	/*	initialize the current test session status for all active slots */

    bool UpdateTestSessionStatus(unsigned slot_id, double value,double lolimit, double hilimit);
	/*  set the current test status
		1.	return true if the value is within the (<=) limits, else false
		*/

    void UpdateTestSessionStatus(unsigned slot_id, unsigned status);
    /*  set the current test session status */

    void SetTestSequenceStatus(unsigned slot_id, unsigned status);
    /*  set the current test sequence status */

    unsigned GetTestSessionStatus(unsigned slot_id);
	/*	get the current test group status */

    unsigned GetTestSequenceStatus(unsigned slot_id);
	/*	get the current test sequence status */

    void EnableSlot(unsigned slot_id, bool enable=true);

    void EnableAllSlots(bool enable=true);

    bool SlotIsEnabled(unsigned slot_id);

    unsigned GetUutIndex(unsigned slot_id);

    unsigned GetUutInstance(unsigned slot_id);

    void SetUutIndex(unsigned slot_id,std::optional<unsigned> index);

    void SetUutInstance(unsigned slot_id,std::optional<unsigned> instance);

    template<typename T>
    void ExecuteTestForEachSlotIfEnabled(T fx, iAobj<CmdRepository::CmdParam> *test_fx, iIXmgr *ix) {

        InitCurrentTestSessionStatus();
        /*	initialize the test status for the current test group */

        AbortFlag=false;
        for (unsigned i=0;i<SlotCount;i++) {

            if (AbortFlag==true) {

                throw(IOString("Test session asynchronously aborted"));
            }

            if (SlotIsEnabled(i)) {

                fx(true,i);
                test_fx->Execute(CmdRepository::CmdParam(ix,i));
                fx(false,i);
            }
        }
    }


    unsigned GetOverallTestSequenceStatus();

    void SetStopOnFirstFail(bool stop_requested);

    void RequestAbort();

    private:

	bool ValidSlot(unsigned slot_id);
	/*	if the spec'd slot is within the valid range */

    bool AbortFlag {false};

    std::vector<Slot> CurrentSlots;
    bool StopOnFirstFailFlag {false};
    unsigned SlotCount {1};
};


#endif
