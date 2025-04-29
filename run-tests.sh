#!/bin/bash
cmake -B test/build -S test -DPCB=0 && \
  cmake --build test/build --target testFirment --target testExample && \
  echo "Running testFirment" && test/build/testFirment && \
  echo "Running testExample" && test/build/example-test/testExample