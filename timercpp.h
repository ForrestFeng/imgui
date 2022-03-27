#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>

#include <Windows.h>


class Timer {
	std::atomic<bool> active{true};
    void* ptr_data;
public:
    Timer(void* ptr_data=NULL) :ptr_data(ptr_data) {}
public:
    void setTimeout(void (*callback)(void*), int delay);
    void setInterval(void (*callback)(void*), int interval);
    void stop(); 
};

void Timer::setTimeout(void (*callback)(void*), int delay) {
    active = true;
    std::thread t([=]() {
        if(!active.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(!active.load()) return;
        callback(ptr_data);
    });
    t.detach();
}

// Set run the callback every interval milliseconds
void Timer::setInterval(void (*callback)(void*), int interval) {
    active = true;
    // May use platform specifc timer. e.g. on Windows 
    // HANDLE waitable_timer = NULL;
    // waitable_timer = CreateWaitableTimer(NULL, FALSE, NULL);
    // SetWaitableTimer(waitable_timer, NULL, interval * 1000, NULL, NULL, FALSE);
    // WaitForSingleObject(waitable_timer, INFINITE);
    std::thread t([=]() {
        while(active.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            
            if(!active.load()) return;
            callback(ptr_data);
            //printf("The value is %d, after interval %d\n", *((int*)(ptr_data)), interval);
        }
    });
    t.detach();
}

void Timer::stop() {
    active = false;
}
