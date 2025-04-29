
# If this is the first time configuring, run config for two passes
# so subsequent config doesn't trigger re-building all c files.
# Note: I'm not sure why two passes speeds up the next rebuild.
if [ ! -d build/xmc ]; then
  cmake --toolchain config/toolchain-arm-gcc.cmake -B build/xmc -S.
  echo "First pass configure done.  Toolchain set.
  "
fi

cmake -B build/xmc -S. && cmake --build build/xmc
