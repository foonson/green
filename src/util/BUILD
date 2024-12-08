# 2024.12.07

cc_library (
  name="util",
  srcs=["Tick.cpp","UCPU.cpp","USocket.cpp","UTime.cpp"],
  hdrs=glob(["*.h"]),
  visibility=["//test:__pkg__",
              "//green:__pkg__",
              "//core:__pkg__",
              "//tetris:__pkg__",              
              ],
)

cc_test (
  name="testUtil",
  srcs=["gtest/testUtil.cpp"],
  deps=["//util:util"],
  linkopts=["-lgtest", "-lgtest_main"],
)