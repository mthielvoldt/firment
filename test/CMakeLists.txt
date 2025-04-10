cmake_minimum_required(VERSION 3.25)

project(firment_test CXX C)

add_compile_options("-g")

include(firmentConfig.cmake)

set(PORT_DIR ${FIRMENT_DIR}/firmware/port/host)
set(CPPUTEST_INC_DIR ${PROJECT_SOURCE_DIR}/cpputest/include)

add_subdirectory(cpputest ../cpputest_build)
add_subdirectory(${PORT_DIR} port)
add_subdirectory(${FIRMENT_DIR} firment)
add_subdirectory(../example/test example-test)

add_library(esp_lib
  ../esp/mqtt5/main/fmt_esp_spi.c
)
target_include_directories(esp_lib
  PRIVATE
    ../esp/mqtt5/main
    ../firmware
  PUBLIC
    ../esp/mqtt5/test/doubles
)

add_executable(testFirment
  ../firmware/test/testFirment.cpp 
  ../firmware/test/gpioTest.cpp
  ../firmware/test/iocSpyTest.cpp
  ../firmware/test/logTest.cpp
  ../firmware/test/queueTest.cpp
  ../firmware/test/spiTest.cpp
  ../firmware/test/stub_comms.c
  ../firmware/test/updateTest.cpp
  ../esp/mqtt5/test/espSpiTest.cpp
)

target_include_directories(testFirment PRIVATE ${PROJECT_CONFIG_DIR})

target_link_libraries(testFirment 
  CppUTest 
  FirmentFW
  MCUPort   # RTE_Device.h
  esp_lib
)