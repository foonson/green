# 2024.11
bazel build --config=debug test:main
#bazel build --config=debug test:test
#bazel build --config=debug util:testUtil
bazel test util:testUtil
bazel build --config=debug green:main
bazel build --config=debug tetris:tetris