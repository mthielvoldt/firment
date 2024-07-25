docker run \
  -it \
  --rm \
  --name mq-client \
  --network host \
  eclipse-mosquitto:2.0.18 mosquitto_sub --port 1883 -t "#"