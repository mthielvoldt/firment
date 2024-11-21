if [ -d "../protocol/gen-venv" ]; then
  source ../protocol/gen-venv/bin/activate
fi

../protocol/nanopb/generator/protoc -I./ \
  --nanopb_out=firmware/generated \
  --plugin=protoc-gen-firment=../protocol/pb-plugins/gen-firment.py --firment_out=firmware/generated \
  --plugin=protoc-gen-widgets=../protocol/pb-plugins/gen-widgets.py --widgets_out=web-ui/src/generated \
  mcu_1.proto

web-ui/node_modules/protobufjs-cli/bin/pbjs \
  -p ../protocol/nanopb/generator/proto \
  -t static-module -w es6 -o web-ui/src/generated/mcu_1.es6.js ./mcu_1.proto

web-ui/node_modules/protobufjs-cli/bin/pbts \
  --no-comments \
  -o web-ui/src/generated/mcu_1.es6.d.ts web-ui/src/generated/mcu_1.es6.js