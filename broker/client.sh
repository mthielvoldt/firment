docker run \
  -it \
  --rm \
  --name mq-client \
  --network host \
  -u $UID \
  --mount type=bind,source=./certs,target=/mosquitto/certs \
  eclipse-mosquitto:2.0.18 mosquitto_sub --port 1883 --cafile /mosquitto/certs/ca-root.crt -h 10.0.0.18 -t "#"
