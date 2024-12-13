// 2024.11

#include <stdio.h>
#include <csignal>
#include <thread>
#include <iostream>
#include <functional>
#include <unistd.h>

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


// ./main.x -c 4 -t 1
int main(int argc_, char* argv_[]) {

  int cpuID = 2;
  int testcaseID = 1;

  int opt = 0;
  while ((opt=getopt(argc_, argv_, "c:t:"))!=-1) {
    switch (opt) {
      case 'c': cpuID = atoi(optarg); break;
      case 't': testcaseID = atoi(optarg); break;
    }
  }

  util::log("cpu=", cpuID, "testcase=", testcaseID);

  uint64_t tick = util::cpuTick();
  printf("main started %ld Core:%d\n", tick, util::coreCount());
  registerSignalHandler();
  util::hideCursor();    //util::showCursor();

  std::thread t(runThreadAtCore, cpuID, [testcaseID](){ 
    if (testcaseID==1) {
      test::memAlloc::test(1);
    }
    if (testcaseID==2) {
      test::memAlloc::test(2);
    }
    if (testcaseID==3) {
      test::clock::test(); 
    }
  });
  t.join();

  mainEnd();
  return -1;
}
