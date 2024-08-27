set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")
# set(CMAKE_SYSROOT "/home/rileyt/compilers/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi")
set(CMAKE_C_COMPILER "/home/rileyt/compilers/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard -mcpu=cortex-m4 -mthumb -fdata-sections -ffunction-sections")

add_compile_definitions(XMC_TARGET)

# set(GCC_PATH
# /home/rileyt/compilers/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin)
# set(CMAKE_C_COMPILER ${GCC_PATH}/arm-none-eabi-gcc CACHE PATH "" FORCE)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${XMC_FLAGS} ${XMC_LINK_FLAGS}" CACHE STRING "" FORCE)