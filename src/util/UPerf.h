#pragma once
// 2025.12.05

#include <time.h> //timespec / clocl_gettime
#include <functional>

#include "UCPU.h"

namespace util {

  uint64_t elapse(const std::function<void()>& func_) {
    timespec ts1;
    timespec ts2;
    ::clock_gettime(CLOCK_MONOTONIC, &ts1);
    func_();
    ::clock_gettime(CLOCK_MONOTONIC, &ts2);
    if (ts2.tv_sec==ts1.tv_sec) { return ts2.tv_nsec - ts1.tv_nsec; }
    if (ts2.tv_sec==ts1.tv_sec+1) { return ts2.tv_nsec - ts1.tv_nsec + 1000000000; }
    return -1;
  }

  uint64_t elapse2(const std::function<void()>& func_) {
    uint64_t tick1 = util::cpuTick();
    func_();
    uint64_t tick2= util::cpuTick();
    return tick2 - tick1;
  }

}
