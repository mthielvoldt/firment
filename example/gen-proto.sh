source ../protocol/gen-venv/bin/activate
../protocol/nanopb/generator/protoc -I./ \
  --nanopb_out=firmware/generated \
  --plugin=protoc-gen-widgets=../protocol/pb-plugins/gen-widgets.py --widgets_out=web-ui/src/generated \
  mcu_1.proto

web-ui/node_modules/protobufjs-cli/bin/pbjs -t static-module -w es6 -o web-ui/src/generated/mcu_1.es6.tsx ./mcu_1.proto

#  --plugin=protoc-gen-can=../protocol/pb-plugins/gen-can.py --can_out=firmware/generated \