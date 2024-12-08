// 2024.11

#include <stdio.h>
#include <csignal>
#include <thread>
#include <iostream>
#include <functional>
#include "util/UCommon.h"
#include "util/UThread.h"
#include "util/UConsole.h"
#include "util/UCPU.h"

#include "testClock.h"
#include "testMemAlloc.h"

void mainEnd() {
  uint64_t tick = util::cpuTick();
  printf("main end %ld\n", tick);
  util::showCursor();
}

void mySignalHandler(int signal_) {
  mainEnd();
  std::exit(-1);
}

void registerSignalHandler() {
  std::signal(SIGTERM, mySignalHandler);
  std::signal(SIGINT,  mySignalHandler);
  std::signal(SIGSEGV, mySignalHandler);
}

void runThreadAtCore(uint8_t coreID_, std::function<void()> func_) {
  // CPU
  if (!util::pinThreadToCore(coreID_)) {
    util::logError("pinThreadToCore");
    return;
  }
  func_();
}

int main() {
/*
  int* x = new int[0];
  delete [] x;
  x[0] = 1;
*/

  uint64_t tick = util::cpuTick();
  printf("main started %ld Core:%d\n", tick, util::coreCount());
  registerSignalHandler();
  util::hideCursor();    //util::showCursor();

  for (uint8_t cpu:{0,1,2,3}) {

    printf("cpu:%d\n", cpu);
    std::thread t(runThreadAtCore, cpu, [](){ 
      test::memAlloc::test();
      test::clock::test(); 
    });
    t.join();

  }

  mainEnd();
  return -1;
}
