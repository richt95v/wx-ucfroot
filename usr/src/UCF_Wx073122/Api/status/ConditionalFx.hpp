
#ifndef CONDITIONALFX
#define CONDITIONALFX

#include <functional>

template<class T,class U>
class ConditionalFx {
	public:
	void Configure(std::function<void(T,U)> true_fx, std::function<void(T,U)> false_fx) {

		TrueFx=true_fx;
		FalseFx=false_fx;
	}

	bool Exec(T param, int option,bool true_state=true) { 

		if (true_state) TrueFx(param,option); 
		else			FalseFx(param,0);

		return(true_state); 
	}

	bool operator()(bool condition,int option, T true_param, T false_param) {
		
		if (condition) 	TrueFx(true_param,option);
		else			FalseFx(false_param,0);

		return(condition);
	}

	bool Execute(bool condition,int option, T true_param, T false_param) {
		
		if (condition) 	TrueFx(true_param,option);
		else			FalseFx(false_param,option);

		return(condition);
	}

	private:
	std::function<void(T,U)> TrueFx;
	std::function<void(T,U)> FalseFx;
};

#endif
