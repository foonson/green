#pragma once
// 2025.12.07

#include "UCommon.h"

namespace util {

void showCursor() {
  printf("\033[?25h\n");
}

void hideCursor() {
  printf("\033[?25l\n");
}

}