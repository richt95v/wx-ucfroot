
#ifndef SIMPLEALARM
#define SIMPLEALARM

#include <thread>

class SimpleAlarm {
	public:
	SimpleAlarm(unsigned s) {

		Timeout=false;

		TheThread=
			std::thread(
			   [this](unsigned s) {
				   std::this_thread::sleep_for(std::chrono::seconds(s));
				   Timeout=true;
				},
				s
			);
	}

	~SimpleAlarm() {
		TheThread.detach();
	}

	std::thread TheThread;
	bool Timeout {false};
};

#endif
