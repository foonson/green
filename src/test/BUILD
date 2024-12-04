cc_library (
  name="includes",
  hdrs=glob(["*.h"]),
)

cc_binary (
  name="main",
  srcs=["main.cpp"],
  deps=["includes",
        "//util:util"],
  copts=["-std=c++20",
         "-D_LINUX",
         "-fsanitize=address",
         "-g",
        ],
  linkopts=["-fsanitize=address"],
)

cc_test (
  name="test",
  srcs=["test.cpp"],
  deps=["includes",
        "//util:util"],
  copts=["-std=c++20",],
  linkopts=["-lgtest","-lgtest_main"]
)