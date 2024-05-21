docker run \
  -it \
  --rm \
  --name broker \
  --mount type=bind,source=./mosquitto.conf,target=/mosquitto/config/mosquitto.conf \
  -p 1883:1883 \
  eclipse-mosquitto:2.0.18