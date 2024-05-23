#!/usr/bin/env python3

import sys
from google.protobuf.compiler.plugin_pb2 import CodeGeneratorResponse

def generate_widgets() -> str:
  return f"""
// Generated File, do not track.
// Implements react modules for each message defined in .proto files.
  """

if __name__ == "__main__":
  response = CodeGeneratorResponse()
  widgets_file = response.file.add(name="widgets.pb.tsx")
  widgets_file.content = generate_widgets()
  sys.stdout.buffer.write(response.SerializeToString())