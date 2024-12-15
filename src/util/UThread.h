#pragma once
// 2024.11

#include <sched.h>
#include <pthread.h>
#include "UCommon.h"

namespace util {
  bool pinThreadToCore(int coreID_) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreID_, &cpuset);

    pthread_t currentThread = pthread_self();
    int ret = pthread_setaffinity_np(currentThread, sizeof(cpu_set_t), &cpuset);
    if (ret!=0) {
      util::logError("pthread_setaffinity_np");
      return false;
    }
    return true;
  }

  void runThreadAtCore(uint8_t coreID_, std::function<void()> func_) {
    // CPU
    if (!util::pinThreadToCore(coreID_)) {
      util::logError("pinThreadToCore");
      return;
    }
    func_();
  }
}