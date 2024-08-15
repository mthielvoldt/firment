# Firment
An opinionated firmware framework that uses web interfaces to ease development and testing.

## Guiding Principles
- GUIs are important, firmware customers need something graphical.
- Use web interface(s) instead of native GUIs.
- Make a reasonable effort to containerize tools.
- Be declarative about messages, let message-handling code be generated.

## Firment workflow
- Start services:
  - Dev webserver (Vite in the example)
  - ESP_rfc2217_server (enables IDF container to flash over USB)
  - idf container (for re-flashing  )
  - mosquitto broker container
- Structure the data you want going in and out of your target device
  - Restrict yourself to message structures that work for the transport your device uses.  Eg. CAN, SPI, UART.
  - Write the .proto file(s).
- Run protoc.  The plugins generate the following code:
  - C code for your edge MCU to send and receive messages on your edge transport (CAN, SPI, UART)
  - C++ code for the ESP32 connected to your edge MCU to relay messages to and from the broker.
  - Front-end Widgets for viewing telemetry and sending commands in the browser.
  - End-to-end test stub to validate proper handling of the data.
- Re-flash the ESP32.
- Position the new widget on your interface webpage.
- Fill in the test stub with sensible data or command validation.
- Run playwright to see your new failing tests.
- Write wnd flash new firmware that makes your tests pass.
- Re-run playwright to see your tests pass.

# User Guide
## Hardware
Firment's main job is to help you build a web interface to your firmware, which we assume is running target hardware without Wifi/ethernet modules.  To bridge the gap, Firment requires an ESP32 to shuttle data between your target hardware (SPI, UART, CAN) and MQTT over Wifi.

Currently we're using [this development board](https://mischianti.org/vcc-gnd-studio-yd-esp32-s3-devkitc-1-clone-high-resolution-pinout-and-specs/)

## Installing Dependencies
- Docker (Docker Desktop is okay)
- ESPTool (a pip package inside a virtual env)
- Node >=18
- Playwright

### Docker
Install Docker Desktop or Docker-CE for your platform.

### Esptool (in a python virtual env)
If `venv` isn't yet installed, you may first need to do that.

```
sudo apt install python3-venv
```

Install esptool in its own virtual environment.
```
python3 -m venv esptool-venv
source esptool-venv/bin/activate
pip install esptool
```
Plug in your ESP to yor computer by USB, and try running
```
esp_rfc2217_server.py -v -p 4000 /dev/ttyACM0
```
If you encounter an error containing:
>... [Errno 13] Permission denied: '/dev/ttyACM0'

Add your user to the dialout group.
```
sudo usermod -a -G dialout $USER
```
## Building and Flashing
Start the frc2217 server.
```
cd esp/mqtt5 && source usb.sh
```
In another terminal, start the container.
```
source idf.sh
```
Configure the SSID, password and spi-flash size with menuconfig.  Note: this will store the SSID and wifi password in the sdkconfig file, which is not tracked.  This will need to be repeated if sdkconfig gets deleted/moved/overwritten.
```
idf.py menuconfig
```
build, flash and monitor as follows:
- `idf.py build`
- `idf.py --port 'rfc2217://host.docker.internal:4000?ign_set_control' flash`

## Working with the Example UI
This shows how to use the example, which is just illustrative.  The example uses Vite for development tooling, but this isn't prescribed by firment; you can use whatever tooling you want. 

### Start vite dev
```
cd example/web-ui
npm run dev
```
# License
Firment is licensed under LGPL-3.0-or-later.  A copy of the GPL-3.0 and additional permissions of the lesser GPL are distributed under the names LICENSE and COPYING.LESSER respectively.
