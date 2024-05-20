docker run \
  -it \
  --rm \
  --name idf \
  -v $PWD:/project \
  -w /project \
  -u $UID \
  -e HOME=/tmp \
  espressif/idf:v5.2.1