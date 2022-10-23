
#ifndef STRIPCHARTTIMER
#define STRIPCHARTTIMER

#include <iAobj.hpp>

#include <chrono>
#include <thread>
#include <atomic>

/***************************************************************************************************************************
    Implement a strip chart type timer

    1.	events are indexed in seconds from a call to StartTimer()
    2.	the timer runs in its own thread in a loop that executes a callback function for each loop
        a.	the delay between calls to the callback function is configurable

    3.	the current delay in seconds from the call to StartTimer() is returned by GetCurrentDelay()

    next:
        1.	find the current time in StartTimer();
        2.	get the current time in each call to GetCurrentDelay, subtract the start time, and return the result
*/

class StripChartTimer {
    public:
    StripChartTimer() {

        CurrentDelay.store(0,std::memory_order_relaxed);
    }


    void SetCallback(iAobj<unsigned long> *ptr, unsigned delay_s) {

        TheCallback=ptr;

        DelaySeconds=delay_s;
        if (DelaySeconds<1) DelaySeconds=1;
    }

    void StartTimer() {
    /*	start the timing loop */

        StartTime=std::chrono::system_clock::now();
        RequestToStop=false;
        TheThread=std::thread(&StripChartTimer::DelayLoop,this);
    }

    void StopTimer() {
    /* stop the timing loop and wait for the thread to finish */

        RequestToStop=true;
        TheThread.join();
    }

    unsigned long GetCurrentDelay() {
    /* return the current delay */

        return(CurrentDelay.load(std::memory_order_relaxed));
    }

    private:
    void DelayLoop () {

        try {

            std::chrono::time_point<std::chrono::system_clock> current_time;
            std::chrono::duration<double> diff;


            while(1) {

                if (TheCallback) {

                        TheCallback->Execute(CurrentDelay.load());
                }

                std::this_thread::sleep_for(std::chrono::seconds(DelaySeconds));

                current_time=std::chrono::system_clock::now();
                diff = current_time-StartTime;

                CurrentDelay.store(diff.count(),std::memory_order_relaxed);

                if (RequestToStop) break;
            }
        }
        catch(...) {
        /* don't let an exception be thrown past here */

        }

        RequestToStop=false;
    }

    std::thread TheThread;
    iAobj<unsigned long> *TheCallback {nullptr};
    unsigned DelaySeconds {0};
    std::atomic<unsigned long> CurrentDelay;
    bool RequestToStop {false};
    std::chrono::time_point<std::chrono::system_clock> StartTime;
};

#endif
