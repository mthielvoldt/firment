# Firment Overview
An opinionated firmware framework that uses web interfaces to ease development and testing.

## Guiding Principles
### GUIs
- Support GUIs for FW stakeholders that advertise the FW's capabilities.
- Use a web GUIs to avoid constantly building and distributing native GUI versions.
### Testing
- Facilitate end-to-end testing early in the project to exercise and stabilize interfaces.
- Let unit testing inform the project's structure.
### Adaptability
- Developers should be free to choose the OS for their dev machines.
- Be able to support a variety of MCUs from different manufacturers.
### Data Interfaces
- Be declarative about messages; generate message-handling code.
- Use DRY comms to keep data volume down; send messages only when things change.
- Receivers should respond quickly to commands.
- It's ok if the protocol has features in all transports that only some need - extra bytes are ok.

## Firment-assisted workflow
1. Start services:
    - Dev webserver (Vite in the example)
    - ESP_rfc2217_server (enables IDF container to flash over USB)
    - idf container (for re-flashing  )
    - mosquitto broker container
2. Define a new message structure in your .proto file(s) to communicate something new with your firmware.
1. Invoke protoc with Firment plugins passed as options to generate code:
    - C code for your edge MCU to send and receive messages on your edge transport (CAN, SPI, UART)
    - C++ code for the ESP32 connected to your edge MCU to relay messages to and from the broker.
    - Front-end Widgets for viewing telemetry and sending commands in the browser.
    - End-to-end test stub to validate proper handling of the data.
1. Re-flash the ESP32.
1. Position the new generated widget in your interface webpage (HTML/CSS).
1. Fill in the test stub with sensible data or command validation.
1. Run playwright to see your new failing tests.
1. Write wnd flash new firmware that makes your tests pass.
1. Re-run playwright to see your tests pass.
1. Repeat step 2 on (and smile, hopefully)

## Required Hardware
Firment's main job is to help you build a web interface to your firmware, which we assume runs on hardware without Wifi/ethernet modules.  To bridge the target <-> web gap, Firment generates code for an [ESP32 dev kit](https://www.amazon.com/dp/B0CNGF6S3F) to shuttle data between your target hardware (SPI, UART, CAN) and MQTT over Wifi.

## Build Structure
Firment provides two cmake targets.
- MCUPort: hardware-specific library
- FirmentFW: hardware-agnostic library

The project that uses Firment must specify, by way of the PORT_DIR cmake variable, which port for FirmentFW to link against.


# Integrating Firment into a project
====================================
This workflow assumes there is a port that covers the MCU family your project uses.
The example in firment/example is a good reference.

## CMakeLists.txt
1. __Set `PROJECT_CONFIG_DIR`__  This is where firmentConfig.cmake, mcuCompileOptions.cmake... live.  Use an absolute path here, as this variable is used in subdirectories. 
2. __Set the project version__  This is necessary because firment generates code for transmitting this version from the firmware.  This must be a major.minor.patch form (1.2.3).  If you do it before the `project()` call you can specify it in that call.  config/version.cmake will read a text file into a variable named VERSION_SEM for you.  
3. __Include the needed config files__ The minimum is `firmentConfig.cmake` and `mcuCompileOptions.cmake`.
4. __`add_subdirectory()` port, firment__ Firment comprises two library targets: a "port" that provides code specific to the MCU family (STM32L4, XMC4000...) and the "FirmentFW" that is hardware-agnostic logic.
5. __Add soruce(s) to executable__: You are free to name your file(s) as desired, but I suggest starting with just `message_handlers.c` as message handler functions is most of what will be in there.
6. __Link project against firment__: Use `target_link_libraries(<project target> FirmentFW MCUPort`.
7. __Define USE_FIRMENT_COMMS__: This is a macro for turning firment on and off in your code.`target_compile_definitions()`.

## Configuring Firment
Firment is configured by the files in PROJECT_CONFIG_DIR that you set above.
1. __firmentConfig.cmake__: 
  - Paths to various directories that live outside firment in your project.  *Note* PORT specifies which MCU family is supported.
  - Optional Firment modules: you can turn off certain firment parts.
  - Sizes of protocol items
2. mcuCompileOptions.cmake: 
3. __priority.h__ This should be a full-project-aware file so NVIC priorities can be understood at a glance in the context of all others.  It is nominally located in PROJECT_CONFIG_DIR/.  Macro `FMT_TRANSPORT_PRIORITY` sets the priority of IRQs that firment handles. 

## Add PCB Details
- __Vendor Hal Configs__ some vendor HALs need headers of specific names to be includable with specific content.  For example, `stm32g4xx_hal_conf.h`.  You can either copy it to PCB_DIR/ or add `target_include_directories()` to add the path to the existing file in your project to MCUPort target.
- __`comm_pcbDetails.h`__ specifies which type of transport {spi, uart} firment will use, and includes the relevant pcbDetails file (see next).
- __Transport Details__ `uart_pcbDetails.h` or `spi_pcbDetails.h` must be provided, and aligned with the transport selected in the previous step.
- __`timer_pcbDetails.h`__ (optional) Add if your project uses firment's periodic lib.
- __`gpio_pcbDetails.h`__  (optional) Add if your project uses firments GPIO lib.

## Modify Project Sources
The following assumes Firment will take over hardware resources that your project had been previously using.  In the examples, Firment is taking over USART2.  This is the compex case - if firment is not stealing HW modules from other code, you only need steps 1,2 in main.c

### main.c (top level file)
1. Include message_handlers.h from APP_FW_CONFIG_DIR (specified in firmentConfig.cmake)
2. Call your comms init function (you name this, it lives in your message_handlers.c).
3. #ifndef-guard global declarations of handles to HW resources firment will use.  For example: 
```
#ifndef USE_FIRMENT_COMMS
UART_HandleTypeDef huart2;
#endif
```
4. #ifndef-guard any initialization code referencing resources firment is taking over.

### IRQHandlers
Firment needs to have its code run when the IRQ from the transport HW happens.  In most cases, you also want to prevent the original IRQHandler code from running.  A targeted way to do this is to re-name the original IRQHandler with a #define.  This will prevent the linker from finding the original function. 
```
// In the .c file where USART2_IRQHandler() is defined.
#ifdef USE_FIRMENT_COMMS
#define USART2_IRQHandler static __attribute__((unused)) overriddenFunction
#endif
```

### Init functions called by Vendor's HAL
Some vendors (STM32 for example) expect you to use their BSP to generate init code, and their HAL calls the generated init functions.  This is a similar situation as with the IRQHandlers: the original init must be prevented from running, or it will likely clobber Firment's HW configuration.  I recommend the same technique as above to unlink the original function.
```
// In the .c file where HAL_UART_MspInit() is defined.
#ifdef USE_FIRMENT_COMMS
#define HAL_UART_MspInit static __attribute__((unused)) overriddenFunction
#endif
```

## Add Messages and Handlers
Messages are defined in PROJECT_CONFIG_DIR/messages.proto.  

### Control Messages 
Messages that end with "Ctl" are built in the web-UI and are received and decoded by the device.  Each control message needs the following to be handled by the firmware:
```
// message_handlers.h
#define USE_ExampleCtl
void handleExampleCtl(ExampleCtl msg);
```
```
// message_handlers.c
void handleExampleCtl(ExampleCtl msg)
{
  // your business logic
}
```
If a certain control message isn't relevant to a specific device, (for example if different types of devices share a bus and therefore a message-set), you simply don't add anything referencing that message.

### Telemetry Messages
Messages ending in "Tlm" are built and sent by device firmware and received, decoded and displayed by the web-ui.  Because the firmware initiates the sending of telemetry, these messages are commonly assembled in a function that is called periodically by your project code.

### Init function
Somewhere in your project you need to call `fmt_initComms()`.  A natural place is in an init function that you write in message_handlers.c.  You write this function because this is also a good place to init other resources you might want associated with comms events.

## Editor Setup
For code-following (intellisense) to work well, you need to add a few items:
- includePaths
  - build/pcb#/firment
  - firment/firmware
  - firment/firmware/port
  - firment/protocol/nanopb
  - pcbDetails dir
- defines
  - "USE_FIRMENT_COMMS"
  - ${MCU_FAMILY} (from mcuCompileOptions.cmake)


# Writing a new Port
Firment provides drivers for Infineon XMC4000 STM32L4 series MCUs.  If your project isn't one of those, a new port will be required.  This port will be located in `firmware/port/`.  It should feature the following components, each discussed in more depth below: 
- CMakeLists.txt
- [Hardware Libraries](#hardware-libraries)
- Port source files (you write these)
- config files (.cmake files in config/)
- [Linker script template](#linker-script-template)

## CMakeLists.txt
This file lives in the root directory of the port and it's job is to create a library called MCUPort, which glues together the HAL and custom source files and provides the public include directories for targets that consume this library.  

## Hardware Libraries
Ideally, these are provided as git submodules by the manufacturer.  There are 3 typically used:
1. Chipmaker's HAL
2. CMSIS Device Headers
3. CMSIS SPI Driver for device family.
See the first three lines of CMakeLists.txt in XMC4 and STM32L4 ports for an example. 

## Custom source files
These provide adaptations between the manufacturer's HAL and firment.
 - ioc: Interrupt-on-Change feature
 - gpio: Initialize, setting, reading, toggling pins. 
 - flash: Program memory
 - spi: Any additional requirements of CMSIS SPI driver and IRQ -> ISR hookup
 - periodic: Timer-driven periodic ISR.
 - sysInit: Clock config and other startup code.
 - crc: Cyclic Redundancy check hardware module

## Config files
To use a port, the PCB details files must point the project to this port, and provide defines and cmake variables the port needs.  See example/config/ 
- Partition addresses
- MCU variant compiler define
- <feat>_pcbDetails.h files.

## Linker Script Template
In order to facilitate using a bootloader, a pair of linker scripts can be generated from a single template using with separate root and app memory offsets. 
Procedure:
- copy the .ld file for the device being used into example/firmware/
- append ".in" to the filename to indicate this is a template.
- replace the FLASH ORIGIN value with @CODE_ADDRESS_DIRECT@
- replace the FLASH LENGTH value with @CODE_SIZE@
- in `config/mcuCompileOptions.cmake`, set LINKER_SCRIPT_TEMPLATE to point to this file.
- in `config/partitions.cmake`, create a set of addresses for your flash geometry.

### CMSIS Drivers
Firment firmware is middleware that expects a communication driver under it that implements a CMSIS interface.  To integrate CMSIS Drivers in your project: 
- Download the [CMSIS pack](https://www.keil.arm.com/packs/) for your processor and add the relevant include folders / c files to CMakeLists.txt.  Often you only need a few of the folders in a pack. See `example/firmware/lib`
- Download [CMSIS Driver headers](https://github.com/ARM-software/CMSIS_6/tree/main/CMSIS/Driver/Include) and add to your include paths.

# Setting Up for Development
Currently we're using [this development board](https://mischianti.org/vcc-gnd-studio-yd-esp32-s3-devkitc-1-clone-high-resolution-pinout-and-specs/)

## Installing Dependencies
- [Docker Desktop](https://www.docker.com/products/docker-desktop/) or Docker CE (untested)
- [Python >=3.7](https://www.python.org/downloads/) and venv \
`sudo apt install python3 python3-venv`
- ESPTool (a pip package) see next section. 
- [Node >=18](https://nodejs.org/) Needed locally to install and run Playwright

## Setup A New Project
Create a directory structure with your firmware, ui, and end-to-end dirs.
- Inside your end-to-end folder, init Playwright `npm init playwright@latest`


### ESPTool install
ESPTool is a pip package that provides the rfc2217 server for connecting to the ESP from inside a docker container.\
We need this because we use the ESP-IDF docker container for ESP work instead of setting up our machine with ESP-IDF.\
This lets us avoid the many pitfalls of setting up and maintaining ESP-IDF globally.\
Unfortunately, accessing USB from inside a docker container is not doable without the rfc2217 server.
This can be installed in a virtual environment where you like as follows:
```
python3 -m venv esptool-venv
source esptool-venv/bin/activate
pip install esptool
```
Connect your ESP32 devKit to your computer by USB, and try running
```
esp_rfc2217_server.py -v -p 4000 /dev/ttyACM0
```

If you encounter an error containing:
>... [Errno 13] Permission denied: '/dev/ttyACM0'

Add your user to the dialout group. 
```
sudo usermod -a -G dialout $USER
```

## Example Firmware Prerequisites
The example project is an Infineon XMC4700 Relax Kit project.  
Installing the dependencies for development on this platform is outlined [here](https://github.com/Infineon/pred-main-xmc4700-kit/blob/master/docs/03_Software/01_SoftwareDevelopment.md)
The steps are as follows:
 - Download appropriate [arm toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
 - Add the compiler's bin dir to the PATH in ~/.profile
 - Install CMake `sudo apt install cmake`
 - Download appropriate Segger [JLink tools](https://www.segger.com/downloads/jlink).  **Note:** Recommend v7.52d.  Experienced issues flashing with v7.98.

 ### Firmware Conveniences
 - Install Cortex-Debug extension if you're using VSCode.
 - Install libncurses5, a dependency of arm-none-eabi-gdb.\
 `sudo apt install libncurses5`
 - Add a configuration to `.vscode/launch.json` such as:
 ```
 {
  "configurations": [
    {
      "name": "XMC",
      "type": "cortex-debug",
      "request": "launch",
      "cwd": "${workspaceFolder}/example/firmware/build/xmc",
      "servertype": "jlink",
      "serverpath": "/usr/bin/JLinkGDBServerExe",
      "executable": "spi_example.elf",
      "interface": "swd",
      "device": "XMC4700_F144x2048",
      "preLaunchTask": "",
      "svdFile": "firmware/port/XMC4/mtb-xmclib-cat3/CMSIS/Infineon/SVD/XMC4700.svd",
      "showDevDebugOutput": "raw"
    },
    [...]
  ]
 }
 ```
 - Add a symlink hack to get gdb back to working. \
 `cd /usr/lib/x86_64-linux-gnu && sudo ln -s libncursesw.so.6 libncursesw.so.5`


# Using Firment
This assumes you are making changes to your message definitions in the .proto files, which requires:
- Regenerating code for Target, ESP, and Web interface
- Re-building and flashing Target firmware
- Re-building and flashing ESP firmware
- Re-loading the web interface to parse and display the updated message data

## Launching programs
1. (no terminal rq'd) Start Docker desktop if not already running.
1. (no terminal rq'd) Start mosquitto broker docker container\
`cd broker && broker.sh`
1. Plug in your ESP32-S3 to your computer's USB.
1. Start rfc2217 server to provide USB serial port access to the idf docker container (next step)\
`cd esp/mqtt5 && . usb.sh`
1. Start the ESP's idf docker container \
`cd esp/mqtt5 && . idf.sh`
    - Once container launches, connect to the ESP32: \
    `idf.py --port 'rfc2217://host.docker.internal:4000?ign_set_control' monitor`
1. Start your local web server \
in the example, we use vite: `cd example/web-ui && npm run dev`
1. Open your web interface in your browser \
The port will depend on your local server.


Configure the SSID, password and spi-flash size with menuconfig.  Note: this will store the SSID and wifi password in the sdkconfig file, which is not tracked.  This will need to be repeated if sdkconfig gets deleted/moved/overwritten.
```
idf.py menuconfig
```
build, flash and monitor as follows:
- `idf.py build`
- `idf.py --port 'rfc2217://host.docker.internal:4000?ign_set_control' flash`

## Working with the Example UI
This shows how to use the example, which is just illustrative.  The example uses Vite for development tooling, but this isn't prescribed by firment; you can use whatever tooling you want. 

# License
Firment is licensed under LGPL-3.0-or-later.  A copy of the GPL-3.0 and additional permissions of the lesser GPL are distributed under the names LICENSE and COPYING.LESSER respectively.
