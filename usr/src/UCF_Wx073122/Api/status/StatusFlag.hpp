#ifndef STATUSFLAG
#define STATUSFLAG

class StatusFlag {
/*  provides a 'latch' for return values
    1.	any non-zero value passed to the object, makes the object value false
	2.	return the first failure value
    */
	public:
	bool operator()(int value,bool init=false);
	/*	set the status value, and optionally set the initial  status to true */

	bool operator = (int value);

	void SetStatus(bool flag);

	operator bool();
	operator int();

	private:
	bool CurrentStatusFlag {true};
	int FirstFail {0};
};

#endif
