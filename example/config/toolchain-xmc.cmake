set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_EXTENSIONS OFF)

# Suppresses the check for a working compiler, which will fail for sys calls since there's no sys.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_OBJCOPY arm-none-eabi-objcopy)
