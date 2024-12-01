#include <stdio.h>

#include "test1.h"
#include "testClock.h"

int main() {
  printf("test started\n");
  printf("hide cursor\033[?25l\n");
  test::clock::test();
  test1();
  printf("show cursor\033[?25h\n");
  return -1;
}
