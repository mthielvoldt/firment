

add_executable(testExample
  testExample.cpp
  initSpiTest.cpp
  ../firmware/init_spi.c
)

target_include_directories(testExample PRIVATE 
  ${CPPUTEST_INC_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../firmware
  ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/config
)

target_link_libraries(testExample
  CppUTest 
  FirmentFW
  MCUPort
)