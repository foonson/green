cc_library (
  name="tetrisHeaders",
  hdrs=glob(["*.h","*.hpp"]),
  deps=["//util:util",
        "//core:core",
       ],
)

cc_binary (
  name="tetris",
  srcs=glob(["*.cpp"]),
  linkopts=["-L/usr/lib/aarch64-linux-gnu"],
  deps=["tetrisHeaders",],
)