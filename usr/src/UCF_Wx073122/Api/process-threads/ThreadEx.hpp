
#ifndef THREADEX
#define THREADEX

#include <thread>
#include <functional>
#include <future>

#include <signal.h>

namespace Sys {

    template<class T>
    class shared_future {
	/*  this class defines a shared_future that can be queried for status and killed
	    1.	operator()(...)
		2.	IsActive()
		3.	GetData()
		4.	GetSharedFuture()
	    5.	Kill()
	    */

    	public:
    	bool operator()(std::function<T()> fx) {
		/*	start the thread */

    		TheThread=std::async(std::launch::async,&shared_future::Execute,this,fx);
    		return(true);
    	}

		T GetData() { return(TheThread.get()); }

		std::shared_future<T>& GetSharedFuture() { return(TheThread); }

    	bool IsActive(unsigned xout_ms=200) {
    	/*  return true if the startup thread is still active
    	    */

			return(TidIsValid);

			#if 0
    		std::future_status status;
    		if (TheThread.valid()) {

    			status=TheThread.wait_for(std::chrono::milliseconds(xout_ms));
    			if (status==std::future_status::timeout) {

    				return(true);
    			}
    		}
			#endif

    		return(false);
    	}

    	bool Kill() {
    	/*  kill the thread if it is active
    	    1.	if the thread was never started (no modules), accessing it will throw
    	    */

    		bool status{false};
    		try {

				if (TidIsValid) {

					if (TheThread.wait_for(std::chrono::milliseconds(300))==std::future_status::timeout) {

						pthread_kill(Tid,SIGUSR1);
						TidIsValid=false;
						status=true;
					}
				}
    		}

    		catch(...) {}
    		return(status);
    	}

    	static void KillSharedFuture(int sig) {

			throw(1);
    	}

    	private:
    	pthread_t Tid;		
		bool TidIsValid {false};
		/* 	Tid may be an int, struct, implementation dependant 
			1. TidIsValid is the only real way to tell if the thread is active
			*/

    	T Execute(std::function<T()> fx) {

			T ret;

    		Tid=pthread_self();
			TidIsValid=true;

    		signal(SIGUSR1, KillSharedFuture);

    		ret=fx();
			TidIsValid=false;

			return(ret);
    	}

    	std::shared_future<T> TheThread;
    };
}


#endif
