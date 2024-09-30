# Overview
This document contains guidance for setting up the debugger tools for the various components in firment: 
- ESP32s3 Wifi Bridge
- XMC4700
- React WebUI

# ESP32s3 Wifi Bridge
The ESP wifi bridge code is intended to work as-is without user modification, but if you have some need to debug this code, here is how to do so.  
*Note:* this requires you to install Espressif's fork of OpenOCD on your host (dev) machine, which is additional to the Esp-idf docker container and RFC2217 python script that are required for flashing and monitoring the ESP32S3.  The IDF install includes this, so if you opted to install IDF on your host (not use the container) you already have this.  If, like me, you've been avoiding that, proceed.

## Installing OpenOCD
ESP maintains a fork of the OpenOCD project [here](https://github.com/espressif/openocd-esp32).  Clone this to get the .cfg files you need. 
Download the appropriate pre-build binary from [Releases](https://github.com/espressif/openocd-esp32/releases) for your system and extract it.

### To Launch OpenOCD
1. plug in your ESP32S3 Dev kit using the left USB-C port (with USB conectors at the bottom).
1. `cd` to `path/to/openocd-esp32/tcl` (where you cloned the repo)
2. `path/to/downloaded/release/bin/openocd -f board/esp32s3-builtin.cfg`

You can also put openocd binary somewhere on your PATH. "It working" looks like this: 

    $ ~/Downloads/openocd-esp32-linux-amd64-0.12.0-esp32-20240821/openocd-esp32/bin/openocd -f board/esp32s3-builtin.cfg
    Open On-Chip Debugger v0.12.0-esp32-20240821 (2024-08-21-14:42)
    Licensed under GNU GPL v2
    For bug reports, read
      http://openocd.org/doc/doxygen/bugs.html
    Info : only one transport option; autoselecting 'jtag'
    Info : esp_usb_jtag: VID set to 0x303a and PID to 0x1001
    Info : esp_usb_jtag: capabilities descriptor set to 0x2000
    Info : Listening on port 6666 for tcl connections
    Info : Listening on port 4444 for telnet connections
    Info : esp_usb_jtag: serial (3C:84:27:D8:84:94)
    Info : esp_usb_jtag: Device found. Base speed 40000KHz, div range 1 to 255
    Info : clock speed 40000 kHz
    Info : JTAG tap: esp32s3.tap0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : JTAG tap: esp32s3.tap1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : [esp32s3.cpu0] Examination succeed
    Info : [esp32s3.cpu1] Examination succeed
    Info : [esp32s3.cpu0] starting gdb server on 3333
    Info : Listening on port 3333 for gdb connections
    Info : [esp32s3.cpu0] Target halted, PC=0x4038304F, debug_reason=00000001
    Info : [esp32s3.cpu0] Reset cause (3) - (Software core reset)
    Info : Set GDB target to 'esp32s3.cpu0'
    Info : [esp32s3.cpu1] Target halted, PC=0x4037A886, debug_reason=00000000
    Info : [esp32s3.cpu1] Reset cause (3) - (Software core reset)


## Breadcrumb: Building OpenOCD
Uncommon: If you're interested in building OpenOCD from source (I won't guess why), here's what I did on Linux.  Note: there seems to be more detail in OpenOCD's README, but it's generally consistent. 

### Build Dependencies
Install the "OpenOCD Dependencies" in openOCD's README
- readme items: `sudo apt install make libtool pkgconf`
- `git submodule init update`
- compression utility (not in readme): `sudo apt install zlib1g-dev`

### Building
Run `cmake . -B build-aux` then cd into `build-aux` and run `make`.  

## Debugging in VSCode
launch.json has a config for this in firment.
1. Start docker desktop (if not already started).
2. Build and flash the code as normal. (USB in other bottom-right position)
2. Launch OpenOCD as above on your host. (Switch USB position *I know*)
2. On Debug tab in VSCode, select the esp32s3-contained config
3. Click the green arrow to begin debugging.  