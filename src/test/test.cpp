#include <stdio.h>
#include <csignal>
#include <thread>
#include "test1.h"
#include "testClock.h"

void mySignalHandler(int signal_) {
  printf("/nshow cursor\033[?25h/n");
  std::exit(-1);
}

void registerSignalHandler() {
  std::signal(SIGTERM, mySignalHandler);
  std::signal(SIGINT, mySignalHandler);
}

int main() {
  int* x = new int[0];
  delete [] x;
  x[0] = 1;
  printf("test started\n");
  registerSignalHandler();
  printf("hide cursor\033[?25l/n");
  std::thread t(test::clock::test);
  //test::clock::test();
  //test1();
  t.join();
  printf("show cursor\033[?25h/n");
  return -1;
}
