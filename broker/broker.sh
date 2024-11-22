docker run \
  -it \
  --rm \
  --name broker \
  -u $UID \
  --mount type=bind,source=./mosquitto.conf,target=/mosquitto/config/mosquitto.conf \
  --mount type=bind,source=./certs,target=/mosquitto/certs \
  -p 1883:1883 -p 8883:8883 \
  eclipse-mosquitto:2.0.18 mosquitto -c /mosquitto/config/mosquitto.conf --verbose
