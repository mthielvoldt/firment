set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")
set(CMAKE_C_COMPILER "/home/rileyt/compilers/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc")

add_compile_definitions(XMC_TARGET)

# Specify any host-specific compiler flags
set(XMC_FLAGS "-mfloat-abi=hard" "-mcpu=cortex-m4" "-mthumb")

# Place all objects in separate linker sections.
set(XMC_LINK_FLAGS
  -fdata-sections
  -ffunction-sections
)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${XMC_FLAGS} ${XMC_LINK_FLAGS}" CACHE STRING "" FORCE)




# set(GCC_PATH
# /home/rileyt/compilers/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin)
# set(CMAKE_C_COMPILER ${GCC_PATH}/arm-none-eabi-gcc CACHE PATH "" FORCE)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${XMC_FLAGS} ${XMC_LINK_FLAGS}" CACHE STRING "" FORCE)