# 2024.12.07

cc_library (
  name="util",
  srcs=["UCPU.cpp"],
  hdrs=glob(["*.h"]),
  visibility=["//test:__pkg__"],
)

cc_test (
  name="testUtil",
  srcs=["gtest/testUtil.cpp"],
  deps=["//util:util"],
  linkopts=["-lgtest", "-lgtest_main"],
)