source esptool-venv/bin/activate 
# Find the correct device '/dev/ttyACM[0,1,2...]'

ESP_NAME=$(ls /dev/serial/by-id | grep '^usb-1a86_USB')
ESP_JTAG_NAME=$(ls /dev/serial/by-id | grep '^usb-Espressif_USB_JTAG_serial')

echo $ESP_JTAG_NAME

if [ -L "/dev/serial/by-id/$ESP_NAME" ]; then
  echo "using ftdi usb..."
  esp_rfc2217_server.py -v -p 4000 "/dev/serial/by-id/$ESP_NAME"

elif [ -L "/dev/serial/by-id/$ESP_JTAG_NAME" ]; then
  echo "using jtag usb..."
  esp_rfc2217_server.py -v -p 4000 "/dev/serial/by-id/$ESP_JTAG_NAME"

else
  echo "neither file found."
fi