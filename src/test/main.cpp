#include <stdio.h>
#include <csignal>
#include <thread>
#include <iostream>
#include "util/UThread.h"

//#include "test1.h"
#include "testClock.h"
#include "testMemAlloc.h"

void mainEnd() {
  uint64_t tick = util::cpuTick();
  printf("main end %ld\n", tick);
  printf("show cursor\033[?25h/n");
}

void mySignalHandler(int signal_) {
  mainEnd();
  std::exit(-1);
}

void registerSignalHandler() {
  std::signal(SIGTERM, mySignalHandler);
  std::signal(SIGINT, mySignalHandler);
}

void runTest() {
  // CPU
  util::pinThreadToCore(2);
  //test::clock::test();
  test::memAlloc::test();
}

int main() {
/*
  int* x = new int[0];
  delete [] x;
  x[0] = 1;
*/

  uint64_t tick = util::cpuTick();
  printf("main started %ld\n", tick);
  registerSignalHandler();
  printf("hide cursor\033[?25l/n");
  std::thread t(runTest);
  //test1();
  t.join();
  mainEnd();
  return -1;
}
