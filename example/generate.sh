#!/bin/bash
# Usage:
#   ./generate.sh            # build all PCBs in PCB_IDS
#   ./generate.sh 2          # build only PCB ID 2

set -euo pipefail

# List of supported PCB IDs
PCB_IDS=(0 1 2 3)

# --- Helper: run cmake for a single PCB ---
build_pcb() {
  local pcb="$1"
  # If this is the first time configuring, run config for two passes
  # so subsequent config doesn't trigger re-building all c files.
  # Note: I'm not sure why two passes speeds up the next rebuild.
  if [ ! -d "build/pcb$1" ]; then
    cmake --toolchain config/toolchain-arm-gcc.cmake -B build/pcb${pcb} -S. -DPCB=${pcb}
    echo "First pass configure done.  Toolchain set.
    "
  fi
  cmake --build build/pcb${pcb}
}

# --- Argument parsing ---
if [[ $# -eq 1 ]]; then
  # Validate argument is a positive integer in PCB_IDS
  if [[ ! $1 =~ ^[0-9]+$ ]]; then
    echo "Error: Argument must be a positive integer." >&2
    exit 1
  fi
  if [[ ! " ${PCB_IDS[*]} " =~ " $1 " ]]; then
    echo "Error: PCB ID $1 not recognized. Known IDs: ${PCB_IDS[*]}" >&2
    exit 1
  fi
  build_pcb "$1"
elif [[ $# -eq 0 ]]; then
  for id in "${PCB_IDS[@]}"; do
    build_pcb "$id"
  done
else
  echo "Usage: $0 [pcb_id]" >&2
  exit 1
fi
