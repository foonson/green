//
//  UCPU.h
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#ifndef UCPU_h
#define UCPU_h

#include <cstdint>

#ifdef _MACOS
#include <sys/sysctl.h>
#endif

namespace util {

  int coreCount() ;
  uint64_t cpuTick();
  //uint64_t calcTickPerMilli();

}

#endif /* UCPU_h */
