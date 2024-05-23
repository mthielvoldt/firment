#!/usr/bin/env python3

import sys
from google.protobuf.compiler.plugin_pb2 import CodeGeneratorResponse

def generate_c() -> str:
  return f"""
/**
* Generated File, do not track!
* Impelements functions to pack and unpack data structures for CAN tx/rx.
*/
  """

if __name__ == "__main__":
  response = CodeGeneratorResponse()
  dot_h_file = response.file.add(name="can.pb.h")
  dot_h_file.content = "/**\nGenerated file!  Do not track.\nInterface to pack and unpack data structures for CAN tx/rx.\n*/"

  dot_c_file = response.file.add(name="can.pb.c")
  dot_c_file.content = generate_c()
  sys.stdout.buffer.write(response.SerializeToString())