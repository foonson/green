#pragma once
// 2024.11

#include <time.h>     // clock_gettime
#include <stdlib.h>   // abs
#include "util/UNum.h"
#include "util/UPerf.h"

namespace test::clock {

  void test() {

    util::NumStats<long> av; 
    for (int x=0;x<10000000;x++) {
 
      long e = util::elapse2([](){
        ;
      });
      if (e==0) continue;

      av.add(e);
    };

    printf("%ld avg=%ld, max=%ld min=%ld var=%ld ", 
      av._count, 
      //e, 
      av.getAvg(), 
      av._max, 
      av._min, 
      av.getVariance()
      //avD.getAvg(),
      //avD._max,
      //avD._min
    );

    printf("\n");
  }
}