
add_executable(testFirment
  testFirment.cpp
  iocTest.cpp
  # gpioTest.cpp
)


target_link_libraries(testFirment 
  CppUTest 
  FirmentFW
  MCUPort   # RTE_Device.h
)
