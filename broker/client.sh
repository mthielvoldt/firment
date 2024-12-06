docker run \
  -it \
  --rm \
  --name mq-client \
  --network host \
  -u $UID \
  --mount type=bind,source=./certs,target=/mosquitto/certs \
  eclipse-mosquitto:2.0.18 mosquitto_sub --port 8883 --cafile /mosquitto/certs/broker.crt -h localhost -t "#" -d --insecure
