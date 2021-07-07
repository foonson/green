//
//  UCPU.h
//  green
//
//  Created by Steve Cheng on 24/6/2021.
//

#ifndef UCPU_h
#define UCPU_h

#include <sys/sysctl.h>

namespace util::UCPU {

int coreCount() ;
uint64_t cpuTick();
uint64_t calcTickPerMilli();

}
#endif /* UCPU_h */
