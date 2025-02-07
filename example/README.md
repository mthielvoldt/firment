# Example: Waveform Generator via SPI

This example shows how to include firment libraries into a project. 

This project is a simple waveform generator with two output channels.  This showcases the ghost-probe (time-series plotting) and telemetry  features of firment.  


# Setup

## Install dependencies
- Node.js
- Python3
- Docker Desktop
- CMake
- C/C++ compiler toolchain for your MCU.

## Edit your CMakeLists.txt
CMake is the captain of the firment ship - it handles the necessary build steps for making cmake functional in your application.  Here are the things you must specify for cmake: 
- Where firment is copied to, relative to your project's root directory. 
- Which MCU port you want to use with this project.  

## Install firment in your 

## Optional Development tools
### mqtt venv
We need a virtual environment for mocking telemetry data to the broker, which helps in developing when we don't have or don't want to hassle with an ESP to publish real telemetry.
```
python3 -m venv mqtt-venv
source mqtt-venv/bin/activate
pip install -r requirements.txt
```
