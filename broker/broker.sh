docker run \
  -it \
  --rm \
  --name broker \
  -u $UID \
  --mount type=bind,source=./mosquitto.conf,target=/mosquitto/config/mosquitto.conf \
  -p 1883:1883 -p 8080:8080 \
  eclipse-mosquitto:2.0.18