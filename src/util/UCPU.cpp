//
//  UCPU.cpp
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#include "UCPU.h"

#ifdef _MACOS
#include <sys/sysctl.h>
#include <mach/mach_time.h>
#endif

#include <thread>
#include "util/UTime.h"

namespace util {


int coreCount() {
  int numCPU = 0;
#ifdef _LINUX
  numCPU = std::thread::hardware_concurrency();
#endif

#ifdef _MACOS
  int mib[4];
  size_t len = sizeof(numCPU);

  /* set the mib for hw.ncpu */
  mib[0] = CTL_HW;
  mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

  /* get the number of CPUs from the system */
  sysctl(mib, 2, &numCPU, &len, NULL, 0);

  if (numCPU < 1)
  {
    mib[1] = HW_NCPU;
    sysctl(mib, 2, &numCPU, &len, NULL, 0);
    if (numCPU < 1) {
      numCPU = 1;
    }
  }
#endif
  
  return numCPU;
}

uint64_t cpuTick() {
  uint64_t tick = 0;
  #ifdef _MACOS
  return mach_absolute_time();
  #endif

  #ifdef _LINUX
  __asm__ volatile ("mrs %0, CNTVCT_EL0"  : "=r"(tick));
  #endif

  return tick;
}

/*
uint64_t calcTickPerMilli() {
  
  auto t1 = util::UTime::now();
  uint64_t tick1 = util::cpuTick();
  uint64_t j=0;
  for (int i=0;i<100000;i++) {
    j++;
  }
  auto t2 = util::UTime::now();
  uint64_t tick2 = util::cpuTick();
  auto x = double(tick2-tick1) / UTime::diffMilli(t1, t2);
  return x;
}
*/


}
