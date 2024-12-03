#include <stdio.h>
#include <csignal>
#include <thread>
#include "test1.h"
#include "testClock.h"
#include "util/UCPU.h"

#include <gtest/gtest.h>

void mySignalHandler(int signal_) {
  printf("/nshow cursor\033[?25h/n");
  std::exit(-1);
}

void registerSignalHandler() {
  std::signal(SIGTERM, mySignalHandler);
  std::signal(SIGINT, mySignalHandler);
}

/*
TEST(TestSuites, Test1) {
  EXPECT_EQ(1, 1);
}
*/

int main() {
  //int* x = new int[0];
  //delete [] x;
  //x[0] = 1;
  uint64_t tick = util::cpuTick();

  printf("test started %ld\n", tick);
  registerSignalHandler();
  printf("hide cursor\033[?25l/n");
  std::thread t(test::clock::test);
  //test::clock::test();
  //test1();
  t.join();
  printf("show cursor\033[?25h/n");
  return -1;
}
