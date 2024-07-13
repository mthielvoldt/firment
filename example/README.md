# Purpose

This example shows how proto files, firmware, a web UI are all knit together with a few scripts.

# Example specifics

This example is a combination waveform generator and digital multimeter.  This was selected to showcase a mix of commands and telemetry.

# Setup

## mqtt venv
We need a virtual environment for mocking telemetry data to the broker, which helps in developing when we don't have or don't want to hassle with an ESP to publish real telemetry.
```
python3 -m venv mqtt-venv
source mqtt-venv/bin/activate
pip install -r requirements.txt
```
