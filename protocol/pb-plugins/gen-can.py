#!/usr/bin/env python3

import sys
# from file where carrier function is import CarrierFunction
from google.protobuf.compiler.plugin_pb2 import CodeGeneratorResponse, CodeGeneratorRequest
from google.protobuf.descriptor_pb2 import FileDescriptorProto, DescriptorProto
from google.protobuf.descriptor import FieldDescriptor

header = """/**
* Generated File, do not track!
* Impelements functions to pack and unpack data structures for CAN tx/rx.
*/
  """

def generate_c() -> str:
  return header 

def get_message_widget(message: DescriptorProto):
  # spec a div with a name
  message_name = message.name
  field_strings = ""
  # iterate through fields, adding an element for each type.
  for field in message.field:
    if field.type in numeric_fields:
      field_class = message_name + '-' + str(field.number)
      field_strings += f'''
      code here for c files
      '''
  # enum_name = ""
  # enum = next(enum for enum in proto.enum_type if enum.name == enum_name)
  
  return f'''
int CANTransmit{message_name}({message_name}_t) {{
  {field_strings}
  return;
}}

'''

if __name__ == "__main__":
  response = CodeGeneratorResponse()
  dot_h_file = response.file.add(name="can.pb.h")
  dot_c_file = response.file.add(name="can.pb.c")
  
  dot_h_file.content = "/**\nGenerated file!  Do not track.\nInterface to pack and unpack data structures for CAN tx/rx.\n*/"
  dot_c_file.content = generate_c()
  sys.stdout.buffer.write(response.SerializeToString())