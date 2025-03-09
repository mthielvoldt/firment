
add_executable(testFirment
  testFirment.cpp
  logTest.cpp
  spiTest.cpp
  iocTestTest.cpp
  stub_comms.c
  # gpioTest.cpp
)


target_link_libraries(testFirment 
  CppUTest 
  FirmentFW
  MCUPort   # RTE_Device.h
)
