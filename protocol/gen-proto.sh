protoc -I./ \
  --plugin=protoc-gen-can=pb-plugins/gen-can.py --can_out=../shared-c \
  --plugin=protoc-gen-widgets=pb-plugins/gen-widgets.py --widgets_out=../server/next-app/src/pages \
  mcu_1.proto 