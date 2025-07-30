#!/bin/bash

if [[ "$#" -ne 1  || ! ($1 =~ ^[0-9]+$) ]]; then
  echo "Usage: $0 <pcb-id>"
  exit 1
fi

# If this is the first time configuring, run config for two passes
# so subsequent config doesn't trigger re-building all c files.
# Note: I'm not sure why two passes speeds up the next rebuild.
if [ ! -d "build/pcb$1" ]; then
  cmake --toolchain config/toolchain-arm-gcc.cmake -B build/pcb$1 -S. -DPCB=$1
  echo "First pass configure done.  Toolchain set.
  "
fi

cmake -B build/pcb$1 -S. -DPCB=$1 && cmake --build build/pcb$1
