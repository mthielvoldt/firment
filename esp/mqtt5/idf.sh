docker run \
  -it \
  --rm \
  --name idf \
  -v $PWD:/project \
  -v $PWD/../../firmware:/project/firmware \
  -v $PWD/../../protocol/nanopb:/project/protocol/nanopb \
  -v $PWD/../../example/build/pcb3/firment:/project/example/build/pcb3/firment \
  -w /project \
  -u $UID \
  -e HOME=/tmp \
  -e ESPPORT=rfc2217://host.docker.internal:4000?ign_set_control \
  --add-host host.docker.internal:host-gateway \
  espressif/idf:v5.2.1