
#ifndef CONDITIONALSTDIO
#define CONDITIONALSTDIO

#include <iStdOut.hpp>

class ConditionalStdIO {

	public:
	ConditionalStdIO(iStdOut *stdout, unsigned console=0) {

		StdOut=stdout;
		Console=console;
		SetOptions(iStdOut::Text1,iStdOut::Error);
	}

	void SetOptions(unsigned pass_option, unsigned fail_option) {

		PassOption=pass_option;
		FailOption=fail_option;
	}

	bool Check(bool condition,const char *true_msg, const char *false_msg) {

		StdOut->WriteStdOut(condition ? true_msg:false_msg,condition ? PassOption:FailOption,Console);
		return(condition);
	}

	private:
	iStdOut *StdOut {nullptr};
	unsigned PassOption {0};
	unsigned FailOption {0};
	unsigned Console {0};

};

#endif
