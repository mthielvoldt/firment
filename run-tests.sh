#!/bin/bash
cmake -B test/build -S test && \
  cmake --build test/build --target testFirment && test/build/testFirment