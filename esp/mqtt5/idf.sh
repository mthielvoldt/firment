docker run \
  -it \
  --rm \
  --name idf \
  -v $PWD:/project \
  -w /project \
  -u $UID \
  -e HOME=/tmp \
  -e ESPPORT=rfc2217://host.docker.internal:4000?ign_set_control \
  espressif/idf:v5.2.1