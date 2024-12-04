#include <stdio.h>
#include <csignal>
#include <thread>
#include "test1.h"
#include "testClock.h"
//#include "util/UCPU.h"

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
  std::thread t(test::clock::test);
  //test::clock::test();
  //test1();
  t.join();
  mainEnd();
  return -1;
}
