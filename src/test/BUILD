# 2024.11

cc_library (
  name="includes",
  hdrs=glob(["*.h"]),
)

cc_binary (
  name="main",
  srcs=["main.cpp"],
  deps=["includes",
        "//util:util"],
  copts=[#"-std=c++17",
         #"-D_LINUX",
         "-Wall",
         #"-fsanitize=address",
         #"-g",
         #"-gsplit-dwarf",
        ],
  #linkopts=["-fsanitize=address"],
)
