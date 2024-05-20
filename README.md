# Firment
An opinionated firmware framework that uses web interfaces to ease development and testing.

## Guiding Principles
- GUIs are important, firmware customers need something graphical.
- Use web interface(s) instead of native GUIs.
- Make a reasonable effort to containerize tools.
- Be declarative about messages, let message-handling code be generated.

# User Guide
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
source esptool-env/bin/activate && esp_rfc2217_server.py -v -p 4000 /dev/ttyACM0
```
In another terminal, start the container.
```
source idf.sh
```
build, flash and monitor as follows:
- `idf.py build`
- `idf.py --port 'rfc2217://host.docker.internal:4000?ign_set_control' flash`

# License
Firment is licensed under LGPL-3.0-or-later.  A copy of the GPL-3.0 and additional permissions of the lesser GPL are distributed under the names LICENSE and COPYING.LESSER respectively.
