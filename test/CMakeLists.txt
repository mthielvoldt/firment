cmake_minimum_required(VERSION 3.25)

project(firment_test CXX C)

enable_testing()

# Directory structuring
add_subdirectory(cpputest)
set(PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/port/host)
add_subdirectory(${PORT_DIR} ${PROJECT_BINARY_DIR}/port)
add_subdirectory(../firmware/test ${PROJECT_BINARY_DIR}/test)


