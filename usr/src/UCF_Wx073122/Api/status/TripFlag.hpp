
#ifndef TRIPFLAG
#define TRIPFLAG

//#include <functional>


class TripFlag {
/*  create a signal flag that implements set/reset functionality
	1. for initial_state==true, operator += 'false' will set the flag to false. setting to true will have no effect
	2. for initial_state==false, operator += 'false' will set the flag to false. setting to true will have no effect
    */

	public:
	TripFlag(bool initial_state=true) {
		InitialState=initial_state;
	}

	bool operator += (bool state) {
		if ((InitialState==true) && (state==false)) CurrentState=false;
		if ((InitialState==false) && (state==true)) CurrentState=false;

		return(state);
	}

	operator bool () { return(CurrentState); }

	private:
	bool InitialState{true};
	bool CurrentState{true};
};

#endif
