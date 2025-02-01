#!/bin/bash

# This file is intended to be executed with the working directory set to the
# location of the file.

# Check if a file name is provided as an argument
if [ $# -ne 1 ]; then
  echo "Usage: $0 <file_name.proto>"
  exit 1
fi

PROTO_FILE=$(pwd)/$1
SCRIPT_DIR=$( cd $(dirname $0) && pwd )
NANOPB_DIR=$SCRIPT_DIR/protocol/nanopb/generator
PLUGIN_DIR=$SCRIPT_DIR/protocol/pb-plugins
FW_OUT_DIR=$SCRIPT_DIR/firmware/generated
UI_OUT_DIR=$SCRIPT_DIR/web-ui/src/generated

# Check if the .proto file exists
if [ ! -e "$PROTO_FILE" ]; then
  echo "Error: File '$PROTO_FILE' does not exist."
  exit 2
fi

if [ -d "$SCRIPT_DIR/protocol/gen-venv" ]; then
  source $SCRIPT_DIR/protocol/gen-venv/bin/activate
fi

mkdir -p $FW_OUT_DIR
mkdir -p $UI_OUT_DIR

echo "Generating nanopb into $FW_OUT_DIR"
$NANOPB_DIR/protoc -I$(pwd) \
  --nanopb_out=$FW_OUT_DIR \
  $PROTO_FILE

echo "Generating firmware into $SCRIPT_DIR/firmware/generated"
$NANOPB_DIR/protoc -I$SCRIPT_DIR \
  --plugin=protoc-gen-firment=$PLUGIN_DIR/gen-firment.py \
  --firment_out=$FW_OUT_DIR \
  $PROTO_FILE

echo "Generating ui widgets into $SCRIPT_DIR/web-ui/src/generated"
$NANOPB_DIR/protoc -I$SCRIPT_DIR \
  --plugin=protoc-gen-widgets=$PLUGIN_DIR/gen-widgets.py \
  --widgets_out=$UI_OUT_DIR \
  $PROTO_FILE

echo "Generating (de)serializing JS into $UI_OUT_DIR/messages.js"
web-ui/node_modules/protobufjs-cli/bin/pbjs \
  -p $NANOPB_DIR/proto \
  -t static-module \
  -w es6 \
  -o $UI_OUT_DIR/messages.js \
  $PROTO_FILE

echo "Generating TS types for above JS into $UI_OUT_DIR/messages.d.ts"
web-ui/node_modules/protobufjs-cli/bin/pbts \
  --no-comments \
  -o $UI_OUT_DIR/messages.d.ts \
  $UI_OUT_DIR/messages.js