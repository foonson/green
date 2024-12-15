#pragma once
// 2025.12.05

#include <time.h> //timespec / clocl_gettime
#include <functional>

#include "UCPU.h"
#include "assert.h"

namespace util {

  class StopWatch {
  public:
    StopWatch() {
      start();
    }
    void start() {
      _startTick = util::cpuTick();
      _endTick = 0;
    }
    void stop() {
      assert(_startTick > 0);
      _endTick = util::cpuTick();
    }
    uint64_t getElapsedTick() { 
      if (_endTick==0) { stop(); }
      return _endTick - _startTick; 
    }
    uint64_t getElapsedMilli() {
      return getElapsedTick() / util::getTickPerMilli(); 
    }

  private:
    uint64_t _startTick = 0;
    uint64_t _endTick = 0;
  };

  /**
   * @brief To get the nano-second difference of executing given function
   */
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
    StopWatch sw;
    sw.start();
    func_();
    sw.stop();
    return sw.getElapsedTick();
  }

  void benchmark(const std::function<void(size_t)>& func_, size_t total_) {
    util::NumStats stat;
    util::StopWatch swAll;
    swAll.start();
    for (size_t i=0;i<total_;i++) {
      util::StopWatch sw;
      func_(i);
      stat.add(sw.getElapsedTick());
    }
    swAll.stop();
    printf("Total:%ld Elapsed:%ldms Loop:%ld %ld [%ld %ld]\n",
      total_,
      swAll.getElapsedMilli(),
      stat.getAvg(),
      stat.getVariance(),
      stat._min,
      stat._max
    );
  }

}
