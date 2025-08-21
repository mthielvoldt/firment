#!/usr/bin/env python3

import sys
from typing import Dict
from google.protobuf.compiler.plugin_pb2 import CodeGeneratorResponse, CodeGeneratorRequest
from google.protobuf.descriptor_pb2 import FileDescriptorProto, DescriptorProto, EnumDescriptorProto, FieldDescriptorProto
from google.protobuf.descriptor import FieldDescriptor


header = """\
// Generated File, do not track.
// Implements react modules for each message defined in .proto files.
import { useState, useEffect } from "react";
import { setMessageHandler, sendMessage } from "../mqclient";"""

float_fields = (
  FieldDescriptor.TYPE_FLOAT,
  FieldDescriptor.TYPE_DOUBLE
)
integer_fields = (
  FieldDescriptor.TYPE_INT32,
  FieldDescriptor.TYPE_INT64,
  FieldDescriptor.TYPE_SINT32,
  FieldDescriptor.TYPE_SINT64,
  FieldDescriptor.TYPE_FIXED32,
  FieldDescriptor.TYPE_FIXED64,
  FieldDescriptor.TYPE_SFIXED32,
  FieldDescriptor.TYPE_SFIXED64
)

enums = {}

def get_message_widget(message: DescriptorProto, enums: Dict[str, EnumDescriptorProto]):
  if message.name.endswith("Ctl"):
    return get_ctl_widget(message, enums)
  elif message.name.endswith("Tlm"):
    return get_tlm_widget(message, enums)
  else:
    return "" # Perhaps an error?

def get_ctl_widget(message: DescriptorProto, enums: Dict[str, EnumDescriptorProto]):
  field_strings = ""
  initial_state = {}

  for field in message.field:
    if field.type in integer_fields:
      initial_state[field.name] = 0
      field_strings += f'''
      <label>
        <input className="field" type="number" size={{5}} step="1" 
          value={{state.{field.name}}} name="{field.name}"
          onChange={{e => setState({{...state, {field.name}:Number(e.target.value)}})}}/>
        {field.name}
      </label>
      <br/>'''
    if field.type in float_fields:
      initial_state[field.name] = 0
      field_strings += f'''
      <label>
        <input className="field" type="number" size={{5}} step="0.01" 
          value={{state.{field.name}}} name="{field.name}"
          onChange={{e => setState({{...state, {field.name}:Number(e.target.value)}})}}/>
        {field.name}
      </label>
      <br/>'''
    if field.type == FieldDescriptor.TYPE_BOOL:
      initial_state[field.name] = False
      field_strings += f'''
      <label>
        <input className="field" type="checkbox" checked={{state.{field.name}}}
          onChange={{e => setState({{...state, {field.name}:e.target.checked}})}} />
        {field.name}
      </label>
      <br/>'''
    if field.type == FieldDescriptor.TYPE_ENUM:
      initial_state[field.name] = 0
      options = get_options_from_enum(enums, field)
        
      field_strings += f'''
      <label>
        <select name="{field.name}" value={{state.{field.name}}}
          onChange={{e => setState({{
            ...state, {field.name}:Number(e.target.value)}})}}>{options}
        </select>
        {field.name}
      </label>
      <br/>'''

  initial_state_str = str(initial_state).replace("True", "true").replace("False","false")
  return f'''
export function {message.name}({{}}) {{
  const [state, setState] = useState({initial_state_str});

  function handleSubmit(e: React.FormEvent) {{
    e.preventDefault();
    sendMessage("{message.name}", state);
  }}

  return (
    <form className="widget" aria-label="{message.name}" onSubmit={{handleSubmit}}>
      <p className="widget-head">
        <span >{message.name} </span>
        <button type="submit">Send</button>
      </p>
      {field_strings}
    </form>
  );
}}
'''

def get_tlm_widget(message: DescriptorProto, enums: Dict[str, EnumDescriptorProto]):
  # spec a div with a name
  message_name = message.name
  field_strings = ""
  initial_state = {}
  setState = f"set{message_name}State"

  # iterate through fields, adding an element for each type.
  for field in message.field:
    if field.type in integer_fields:
      initial_state[field.name] = 0  
      field_strings += f'''
      <div className="field">
        <p>{field.name + " "}<span>{{{message_name}State.{field.name}}}</span></p>
      </div>
      '''
    if field.type in float_fields:
      initial_state[field.name] = 0.0  
      field_strings += f'''
      <div className="field">
        <p>{field.name + " "}<span>{{{message_name}State.{field.name}.toPrecision(4)}}</span></p>
      </div>
      '''
    if field.type == FieldDescriptor.TYPE_BOOL:
      initial_state[field.name] = False
      field_strings += f'''
      {{{message_name}State.{field.name} && <p>{field.name}</p>}}
      '''
    if field.type == FieldDescriptor.TYPE_ENUM:
      initial_state[field.name] = 0   
      field_strings += f'''
      <div className="field">
        <p>{field.name + " "}<span>{{{message_name}State.{field.name}}}</span></p>
      </div>
      '''
  # enum_name = ""
  # enum = next(enum for enum in proto.enum_type if enum.name == enum_name)
  initial_state_str = str(initial_state).replace("True", "true").replace("False","false")
  
  return f'''
export function {message_name}({{}}) {{
  const [{message_name}State, {setState}] = useState({initial_state_str});
  useEffect( () => {{
    setMessageHandler("{message_name}", {setState});
  }}, []);

  return (
    <div className="widget">
      <h4>{message_name}</h4>
      {field_strings}
    </div>
  );
}}
'''

def get_options_from_enum(enums, field: FieldDescriptorProto):
  options = ""
  for value in enums[field.type_name[1:]].value:
    options += f'''
      <option value="{value.number}">{value.name}</option>'''
  return options
  
def digest_proto(proto: FileDescriptorProto):
  ret = ""

  for message in proto.message_type:
    widget_str = get_message_widget(message, enums)
    ret += widget_str

  if False:  # Control appending contents of parsed data as a comment
    ret += "/*"
    ret += str(enums)
    # ret += str(type(enums["WaveShape"]))
    ret += str(proto)
    # ret += str(type(proto))
    # ret += str(type(proto.enum_type))
    ret += "*/"

  return ret

def generate_widgets(request: CodeGeneratorRequest) -> str:
  body = "\n\n"

  # Find all the enums in all files (whether generated or not)
  for file in request.proto_file:
    enums.update({ enum.name: enum for enum in file.enum_type })

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