#include <gtest/gtest.h>

#include "util/UNum.h"
//#include "util/UPerf.h"
#include "util/UCPU.h"

TEST(TestUtil, UNumStats) {
  util::NumStats<long> s;
  for (long i=0;i<1000;i++) {
      s.add(i);
  }
  EXPECT_EQ(s._count, 1000);
  EXPECT_EQ(s._max, 999);
  EXPECT_EQ(s._min, 0);
  EXPECT_EQ(s._sum, 499500);
  EXPECT_EQ(s._sum2, 332833500);
  EXPECT_EQ(s.getAvg(), 499);
  EXPECT_EQ(s.getVariance(), 83832);
}

TEST(TestUtil, UCPU) {
  auto a = util::cpuTick();
  auto b = util::cpuTick();
  EXPECT_GE(b, a);
}