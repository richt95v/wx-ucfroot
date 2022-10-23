
#include "StatusFlag.hpp"

bool StatusFlag::operator()(int value,bool init) {

	if (init) CurrentStatusFlag=true;
    if ((CurrentStatusFlag==true) && (value!=0)) {

		FirstFail=value;
		CurrentStatusFlag=0;
	}

    return(CurrentStatusFlag);
}

bool StatusFlag::operator = (int value) {

	if ((CurrentStatusFlag==true) && (value!=0)) {

		FirstFail=value;
		CurrentStatusFlag=false;
	}

    return(CurrentStatusFlag);
}

void StatusFlag::SetStatus(bool flag) { 

	CurrentStatusFlag=flag;
}

StatusFlag::operator bool() { return(CurrentStatusFlag); }

StatusFlag::operator int() { return(FirstFail); }


