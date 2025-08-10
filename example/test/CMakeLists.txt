

add_executable(testExample
  testExample.cpp
  initSpiTest.cpp
  ../../firmware/fmt_transport.c
)

target_include_directories(testExample PRIVATE 
  ${CPPUTEST_INC_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../firmware
  ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/config
  ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/config/pcb${PCB}
)

target_link_libraries(testExample
  CppUTest 
  FirmentFW
  MCUPort
)