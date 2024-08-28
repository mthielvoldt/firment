set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

# Suppresses the check for a working compiler, which will fail for sys calls since there's no sys.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# An alternate way to suppress the compiler check.
# This alternate has the side-effect of producing error messages.
# set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard -mcpu=cortex-m4 -mthumb -fdata-sections -ffunction-sections")

set(XMC_VARIANT "XMC4700")
add_compile_definitions(XMC4700_F144x2048)
# add_compile_definitions(XMC4400_F100x512)

set(XMC_DIR "lib/XMC-for-Arduino/variants/${XMC_VARIANT}")
set(XMC_STARTUP_FILE "${XMC_DIR}/startup_${XMC_VARIANT}.S")
set(XMC_SYSTEM_FILE "${XMC_DIR}/system_${XMC_VARIANT}.c")
set(XMC_LINKER_FILE "${PROJECT_SOURCE_DIR}/lib/XMC-for-Arduino/variants/${XMC_VARIANT}/linker_script.ld")


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