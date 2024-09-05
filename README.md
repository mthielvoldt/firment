# Firment Overview
An opinionated firmware framework that uses web interfaces to ease development and testing.

## Guiding Principles
- GUIs are important, firmware customers need something graphical.
- Use a web interface to avoid constantly building and distributing native GUI versions.
- Facilitate end-to-end testing early in the project to exercise and stabilize interfaces.
- Make a reasonable effort to containerize tools.
- Be declarative about messages; generate message-handling code.

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

# Setting Up for Development

Currently we're using [this development board](https://mischianti.org/vcc-gnd-studio-yd-esp32-s3-devkitc-1-clone-high-resolution-pinout-and-specs/)

## Installing Dependencies
- [Docker Desktop](https://www.docker.com/products/docker-desktop/) or Docker CE (untested)
- [Python >=3.7](https://www.python.org/downloads/) and venv \
`sudo apt install python3 python3-venv`
- ESPTool (a pip package) see next section. 
- [Node >=18](https://nodejs.org/) Needed locally to install and run Playwright
- Playwright `npm init playwright@latest`


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
      "svdFile": "",
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
