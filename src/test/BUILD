cc_library (
  name="includes",
  hdrs=glob(["*.h"]),
)

cc_binary (
  name="test",
  srcs=["test.cpp"],
  deps=["includes",
        "//util:util"],
  copts=["-std=c++20",
         "-D_LINUX",
        ],
)