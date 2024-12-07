#https://www.tweag.io/blog/2023-11-23-debug-fission/
#g++ -std=c++17 top.cpp -o a.x
#g++ -std=c++17 -g top.cpp -o b.x
#g++ -std=c++17 -g -c top.cpp
#g++ -std=c++17 top.o -o c.x

rm release.x debug.x strip.x toy.o linked.x

#g++ -fuse-ld=gold top.cpp -o release.x
g++ -std=c++17 -g -gsplit-dwarf -c toy.cpp
g++ -fuse-ld=gold -Wl,--build-id,--gdb-index toy.o -o toy.x
#strip debug.x -o strip.x
#strip --only-keep-debug debug.x -o top.debug
#objcopy --add-gnu-debuglink=top.debug strip.x linked.x
