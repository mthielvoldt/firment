#!/bin/bash

# Check if a file name is provided as an argument
if [ $# -ne 1 ]; then
  echo "Usage: $0 <file_name>"
  exit 1
fi

FILE=$(pwd)/$1
SCRIPT_DIR=$( cd $(dirname $0) && pwd )
PROTOC=$SCRIPT_DIR/protocol/nanopb/generator/protoc

# Check if the file exists
if [ ! -e "$FILE" ]; then
  echo "Error: File '$FILE' does not exist."
  exit 2
fi

if [ -d "../protocol/gen-venv" ]; then
  source ../protocol/gen-venv/bin/activate
fi

mkdir -p firmware/generated
mkdir -p web-ui/src/generated

echo "Running $PROTOC..."
$PROTOC -I$SCRIPT_DIR \
  --nanopb_out=firmware/generated \
  --plugin=protoc-gen-firment=$SCRIPT_DIR/protocol/pb-plugins/gen-firment.py --firment_out=firmware/generated \
  --plugin=protoc-gen-widgets=$SCRIPT_DIR/protocol/pb-plugins/gen-widgets.py --widgets_out=web-ui/src/generated \
  $FILE

echo "Running $(pwd)/web-ui/node_modules/protobufjs-cli/bin/pbjs..."
web-ui/node_modules/protobufjs-cli/bin/pbjs \
  -p ../protocol/nanopb/generator/proto \
  -t static-module -w es6 -o web-ui/src/generated/messages.js $FILE

echo "Running $(pwd)/web-ui/node_modules/protobufjs-cli/bin/pbts..."
web-ui/node_modules/protobufjs-cli/bin/pbts \
  --no-comments \
  -o web-ui/src/generated/messages.d.ts web-ui/src/generated/messages.js