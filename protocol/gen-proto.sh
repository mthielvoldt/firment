protoc -I./ \
  --plugin=protoc-gen-can=pb-plugins/gen-can.py --can_out=../shared-c \
  --plugin=protoc-gen-widgets=pb-plugins/gen-widgets.py --widgets_out=../example-web-ui/src/generated \
  mcu_1.proto 