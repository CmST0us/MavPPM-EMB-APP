//
// Created by eric3u on 19-3-7.
//

#pragma once

#include<functional>
#include<chrono>
#include<thread>
#include<atomic>
#include<memory>
#include<mutex>
#include<condition_variable>
#include "nocopyable.hpp"

namespace mavppm {

namespace utils {

class Timer final : public NoCopyable {
public:
    Timer() :_expired(true), _tryToExpire(false){

    }

    ~Timer(){
        expire();
        //      std::cout << "timer destructed!" << std::endl;
    }

    void startTimer(int interval, std::function<void()> task){
        if (_expired == false){
            //          std::cout << "timer is currently running, please expire it first..." << std::endl;
            return;
        }
        _expired = false;
        std::thread([this, interval, task](){
            while (!_tryToExpire){
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                task();
            }
            //          std::cout << "stop task..." << std::endl;
            {
                std::lock_guard<std::mutex> locker(_mutex);
                _expired = true;
                _expiredcond.notify_one();
            }
        }).detach();
    }

    void expire(){
        if (_expired){
            return;
        }

        if (_tryToExpire){
            //          std::cout << "timer is trying to expire, please wait..." << std::endl;
            return;
        }
        _tryToExpire = true;
        {
            std::unique_lock<std::mutex> locker(_mutex);
            _expiredcond.wait(locker, [this]{return _expired == true; });
            if (_expired == true){
                //              std::cout << "timer expired!" << std::endl;
                _tryToExpire = false;
            }
        }
    }

    template<typename callable, class... arguments>
    void syncWait(int after, callable&& f, arguments&&... args){

        std::function<typename std::result_of<callable(arguments...)>::type()> task
                (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
        std::this_thread::sleep_for(std::chrono::milliseconds(after));
        task();
    }

    template<typename callable, class... arguments>
    void asyncWait(int after, callable&& f, arguments&&... args){
        std::function<typename std::result_of<callable(arguments...)>::type()> task
                (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        std::thread([after, task](){
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }).detach();
    }

private:
    std::atomic<bool> _expired;
    std::atomic<bool> _tryToExpire;
    std::mutex _mutex;
    std::condition_variable _expiredcond;
};

}

}