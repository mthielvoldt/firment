source esptool-venv/bin/activate 
# Find the correct device '/dev/ttyACM[0,1,2...]'

ESP_NAME=$(ls /dev/serial/by-id | grep '^usb-1a86_USB')
esp_rfc2217_server.py -v -p 4000 "/dev/serial/by-id/$ESP_NAME"