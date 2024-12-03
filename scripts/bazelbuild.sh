bazel build --copt=-D_LINUX=1 --cxxopt=-fsanitize=address --linkopt=-fsanitize=address test:test
