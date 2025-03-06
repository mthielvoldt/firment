cmake_minimum_required(VERSION 3.25)

project(firment_test CXX C)

include(firmentConfig.cmake)

set(PORT_DIR ${FIRMENT_DIR}/firmware/port/host)

add_subdirectory(cpputest ../cpputest_build)
add_subdirectory(${PORT_DIR} port)
add_subdirectory(${FIRMENT_DIR} firment)
add_subdirectory(../firmware/test ${PROJECT_BINARY_DIR}/test)

