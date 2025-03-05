add_library(MCUPort
  fmt_ioc_test.c
)

target_include_directories(MCUPort
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
    ../..
)