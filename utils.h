//
// Created by shirley on 2022/8/28.
//

#pragma once

#ifndef JXL_UTILS_H
#define JXL_UTILS_H

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <thread>

bool is_stopped = false;

void nanotask() {
    struct timespec interval;
    interval.tv_sec = 10;
    interval.tv_nsec = 0;
    while (!is_stopped) {
        std::cout << "nanosleeping" << std::endl;
        nanosleep(&interval, NULL);
        std::cout << "wake up" << std::endl;
    }
}

void task() {
    while (!is_stopped) {
        std::cout << "sleeping" << std::endl;
        // sleep 底层实现还是 nanosleep
        sleep(10);
        std::cout << "wake up" << std::endl;
    }
    std::cout << "cleanup" << std::endl;
}

void handler(int sig) {
    std::cout << "got signal:" << sig << std::endl;
    // 虽然设置了is_stopped = true, 但是要等到 sleep 返回, 程序才能结束
    is_stopped = true;
}


// 测试信号量使用
//int main() {
//    // 谨慎使用 signal, 尽量使用 sigaction
//    struct sigaction sa;
//    sa.sa_handler = handler;
//    sigemptyset(&sa.sa_mask);
//    sa.sa_flags = 0;
//    sigaction(SIGTERM, &sa, NULL);
//    sigaction(SIGINT, &sa, NULL);
//
//    std::thread t1(task);
//    std::thread t2(nanotask);
//    t1.join();
//    t2.join();
//    std::cout << "thread exit" << std::endl;
//    return 0;
//}


#endif //JXL_UTILS_H
