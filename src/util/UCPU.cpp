//
//  UCPU.cpp
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#include <sys/sysctl.h>
#include <mach/mach_time.h>
#include "UTime.h"

namespace util::UCPU {

int coreCount() {
  int mib[4];
  int numCPU;
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
  
  return numCPU;
}

uint64_t cpuTick() {
  return mach_absolute_time();
}

uint64_t calcTickPerMilli() {
  
  auto t1 = util::UTime::now();
  uint64_t tick1 = util::UCPU::cpuTick();
  uint64_t j=0;
  for (int i=0;i<100000;i++) {
    j++;
  }
  auto t2 = util::UTime::now();
  uint64_t tick2 = util::UCPU::cpuTick();
  auto x = double(tick2-tick1) / UTime::diffMilli(t1, t2);
  return x;
}



}
