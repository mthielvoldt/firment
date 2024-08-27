set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs -Tlib/XMC-for-Arduino/variants/XMC4700/linker_script.ld" CACHE INTERNAL "")

set(CMAKE_C_COMPILER "/home/rileyt/compilers/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard -mcpu=cortex-m4 -mthumb -fdata-sections -ffunction-sections")

add_compile_definitions(XMC_TARGET)

# Path to pass to the compiler in the --sysroot flag.
# FROM https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html
# --sysroot=dir Use dir as the logical root directory for headers and libraries.
# For example, if the compiler normally searches for headers in /usr/include and libraries in /usr/lib,
# it instead searches dir/usr/include and dir/usr/lib. 
# set(CMAKE_SYSROOT "/home/rileyt/compilers/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi")

# Semicolon-separated list of root paths to search on the filesystem.
# This variable is most useful when cross-compiling. 
# CMake uses the paths in this list as alternative roots to find 
# filesystem items with find_package(), find_library() etc.
# set(CMAKE_FIND_ROOT_PATH "/home/rileyt/compilers/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${XMC_FLAGS} ${XMC_LINK_FLAGS}" CACHE STRING "" FORCE)