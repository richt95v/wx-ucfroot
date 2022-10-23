
#include <TestSlots.hpp>


void Slot::SetTestSessionStatus(unsigned status) {

    if (status<CurrentTestSessionStatus) 		CurrentTestSessionStatus=status;
    if (status<CurrentTestSequenceStatus) 	CurrentTestSequenceStatus=status;

    if ((CurrentTestSessionStatus==0) && StopOnFirstFail) {

        Enable=false;
    }
}

void Slot::Reset() {

    UutIndex=0;
    UutInstance=0;
    /*	database references */

    SerialNumber.clear();
    CurrentTestSessionStatus=3;
    CurrentTestSequenceStatus=3;
    StopOnFirstFail=true;
}

void Slot::ResetTestSequenceStatus(bool stop_on_first_fail) {
    Enable=true;
    CurrentTestSessionStatus=3;
    CurrentTestSequenceStatus=3;

    StopOnFirstFail=stop_on_first_fail;
}

void Slot::SetTestSequenceStatus(unsigned status) {

    CurrentTestSequenceStatus=status;
}

void SlotManager::SetConsoleID (unsigned slot_id, unsigned console_id) {
/*	set the console id where information from the spec'd slot will be displayed */

    if (ValidSlot(slot_id)) {

        CurrentSlots[slot_id].ConsoleID=console_id;
    }
}

unsigned SlotManager::GetConsoleID (unsigned slot_id) {
/*	get the console id where information from the spec'd slot is displayed */

    return(ValidSlot(slot_id) ? CurrentSlots[slot_id].ConsoleID: 0);
}

void SlotManager::SetSlotCount(unsigned count) {
/*	configure the number of active slots */

    SlotCount=count;
    if (SlotCount>16) SlotCount=16;
    if (SlotCount<1) SlotCount=1;

    CurrentSlots.clear();
    CurrentSlots.assign(count,Slot());
}

void SlotManager::SetSerialNumber(unsigned index, std::string sn) {
/*	set the serial numer for the spec'd slot */

    if (ValidSlot(index)) {

        CurrentSlots[index].SerialNumber=sn;
    }
}

const char* SlotManager::GetSerialNumber(unsigned index) {
/*	get the serial numer for the spec'd slot */

    static char null_str[] {"null_str"};
    if (index<SlotCount) {

        return(CurrentSlots[index].SerialNumber.c_str());
    }

    return(null_str);
}

unsigned SlotManager::GetSlotCount() {return(SlotCount); }
/*	get the number of active slots */

void SlotManager::InitCurrentTestSessionStatus() {
/*	initialize the current test status for all active slots */

    for (Slot& rec:CurrentSlots) {

        rec.CurrentTestSessionStatus=3;
    }
}

void SlotManager::Init() {
/*	initialize the current test session status for all active slots */

    for (Slot& rec:CurrentSlots) {

        rec.Reset();
    }
}

void SlotManager::InitTestSequenceStatus(bool stop_on_first_fail) {
/*	initialize the current test session status for all active slots */

    for (Slot& rec:CurrentSlots) {

        rec.ResetTestSequenceStatus(stop_on_first_fail);
    }
}

void SlotManager::SetTestSequenceStatus(unsigned slot_id, unsigned status) {

    for (Slot& rec:CurrentSlots) {

        rec.SetTestSequenceStatus(status);
    }
}

void SlotManager::UpdateTestSessionStatus(unsigned slot_id, unsigned status) {

    if (ValidSlot(slot_id)) {

        CurrentSlots[slot_id].SetTestSessionStatus(status);
    }
}

unsigned SlotManager::GetTestSessionStatus(unsigned slot_id) {

    return(ValidSlot(slot_id) ? CurrentSlots[slot_id].CurrentTestSessionStatus:0);
}

unsigned SlotManager::GetTestSequenceStatus(unsigned slot_id) {

    return(ValidSlot(slot_id) ? CurrentSlots[slot_id].CurrentTestSequenceStatus:0);
}

void SlotManager::EnableSlot(unsigned slot_id, bool enable) {

    if (ValidSlot(slot_id)) {

        CurrentSlots[slot_id].Enable=enable;
    }
}

void SlotManager::EnableAllSlots(bool enable) {

    for (Slot &rec:CurrentSlots) {

        rec.Enable=enable;
    }
}

bool SlotManager::SlotIsEnabled(unsigned slot_id) {

    return(ValidSlot(slot_id) && CurrentSlots[slot_id].Enable);
}

unsigned SlotManager::GetUutIndex(unsigned slot_id) {

    return(ValidSlot(slot_id) ? CurrentSlots[slot_id].UutIndex :0);
}

unsigned SlotManager::GetUutInstance(unsigned slot_id) {

    return(ValidSlot(slot_id) ? CurrentSlots[slot_id].UutInstance :0);
}

void SlotManager::SetUutIndex(unsigned slot_id,std::optional<unsigned> index) {

    if (ValidSlot(slot_id) && index) {

        CurrentSlots[slot_id].UutIndex=index.value();
    }
}

void SlotManager::SetUutInstance(unsigned slot_id,std::optional<unsigned> instance) {

    if (ValidSlot(slot_id) && instance) {

        CurrentSlots[slot_id].UutInstance=instance.value();
    }
}

void SlotManager::RequestAbort() {
/*  must be called from another thread
    */

    AbortFlag=true;
}

unsigned SlotManager::GetOverallTestSequenceStatus() {
/*	initialize the test status for the overall test sequence */

    unsigned tmp {0};
    unsigned ret {2};
    for (unsigned i=0;i<SlotCount;i++) {

        tmp=GetTestSequenceStatus(i);
        if (tmp<ret) ret=tmp;
    }

    return(ret);
}

void SlotManager::SetStopOnFirstFail(bool stop_requested) {

    StopOnFirstFailFlag=stop_requested;
}

bool SlotManager::ValidSlot(unsigned slot_id) {

    return (slot_id<SlotCount);
}


