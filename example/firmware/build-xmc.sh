cmake -DCMAKE_TOOLCHAIN_FILE=toolchain-xmc.cmake -B build/xmc -S.
cmake --build build/xmc
rm -r build/xmc
