#!/usr/bin/env python3

import sys
from google.protobuf.compiler.plugin_pb2 import CodeGeneratorResponse, CodeGeneratorRequest
from google.protobuf.descriptor_pb2 import FileDescriptorProto, DescriptorProto
from google.protobuf.descriptor import FieldDescriptor


header = """
// Generated File, do not track.
// Implements react modules for each message defined in .proto files.
import styles from "../styles/Home.module.css";
"""

numeric_fields = (
  FieldDescriptor.TYPE_FLOAT,
  FieldDescriptor.TYPE_DOUBLE,
  FieldDescriptor.TYPE_INT32,
  FieldDescriptor.TYPE_INT64,
  FieldDescriptor.TYPE_SINT32,
  FieldDescriptor.TYPE_SINT64,
  FieldDescriptor.TYPE_FIXED32,
  FieldDescriptor.TYPE_FIXED64,
  FieldDescriptor.TYPE_SFIXED32,
  FieldDescriptor.TYPE_SFIXED64
)

def get_message_widget(message: DescriptorProto):
  # spec a div with a name
  message_name = message.name
  field_strings = ""
  # iterate through fields, adding an element for each type.
  for field in message.field:
    if field.type in numeric_fields:
      field_class = message_name + '-' + str(field.number)
      field_strings += f'''
      <div>
        <p>{field.name + " "}<span className={{`${{styles.code}} {field_class}`}}>0</span></p>
      </div>
      '''
  # enum_name = ""
  # enum = next(enum for enum in proto.enum_type if enum.name == enum_name)
  
  return f'''
export function {message_name}({message_name}Props) {{
  return (
    <div className={{styles.card}}>
      <h1>{message_name}</h1>
      {field_strings}
    </div>
  );
}}

'''
  
def digest_proto(proto: FileDescriptorProto):
  ret = ""
  for message in proto.message_type:
    ret += get_message_widget(message)
  return ret

def generate_widgets(request: CodeGeneratorRequest) -> str:
  body = "\n\n"
  
  for file_name in request.file_to_generate:
    proto = next(file for file in request.proto_file if file.name == file_name)
    body += digest_proto(proto)

  # body += request.source_file_descriptors + "\n"
  return header + body

if __name__ == "__main__":
  request = CodeGeneratorRequest.FromString(sys.stdin.buffer.read())
  response = CodeGeneratorResponse()
  widgets_file = response.file.add(name="widgets.pb.tsx")
  widgets_file.content = generate_widgets(request)
  sys.stdout.buffer.write(response.SerializeToString())