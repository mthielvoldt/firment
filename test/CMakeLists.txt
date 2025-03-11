cmake_minimum_required(VERSION 3.25)

project(firment_test CXX C)

add_compile_options("-g")

include(firmentConfig.cmake)

set(PORT_DIR ${FIRMENT_DIR}/firmware/port/host)

add_subdirectory(cpputest ../cpputest_build)
add_subdirectory(${PORT_DIR} port)
add_subdirectory(${FIRMENT_DIR} firment)

add_executable(testFirment
  ../firmware/test/testFirment.cpp 
  ../firmware/test/gpioTest.cpp
  ../firmware/test/iocTestTest.cpp
  ../firmware/test/logTest.cpp
  ../firmware/test/spiTest.cpp
  ../firmware/test/stub_comms.c
  ../firmware/test/updateTest.cpp
  ${PROJECT_CONFIG_DIR}/spi_config.c
)

target_include_directories(testFirment PRIVATE ${PROJECT_CONFIG_DIR})

target_link_libraries(testFirment 
  CppUTest 
  FirmentFW
  MCUPort   # RTE_Device.h
)