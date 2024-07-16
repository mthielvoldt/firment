source ../protocol/gen-venv/bin/activate
protoc -I./ \
  --plugin=protoc-gen-can=../protocol/pb-plugins/gen-can.py --can_out=firmware/generated \
  --plugin=protoc-gen-widgets=../protocol/pb-plugins/gen-widgets.py --widgets_out=web-ui/src/generated \
  mcu_1.proto 