#pragma once

#include <sched.h>
#include <pthread.h>

namespace util {
  void pinThreadToCore(int coreID_) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreID_, &cpuset);

    pthread_t currentThread = pthread_self();
    int ret = pthread_setaffinity_np(currentThread, sizeof(cpu_set_t), &cpuset);
  }
}