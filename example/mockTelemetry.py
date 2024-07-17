from time import sleep, time
import atexit
import json
import paho.mqtt.client as mqtt

broker = "localhost"
port = 1883

topics = {
  "WaveformControl": ["amplitude", "frequency", "offset"],
  "GridStatus": ["voltage", "current"]
}

unacked_publish = set()
mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, transport="websockets")
start_time = time()

def on_publish(client, userdata, mid, reason_code, properties):
  print(f"message sent {mid}")

def cleanup():
  mqttc.disconnect()
  mqttc.loop_stop()
  print("disconnected")

atexit.register(cleanup)
mqttc.on_publish = on_publish
mqttc.user_data_set(unacked_publish)
mqttc.connect(broker, port)
mqttc.loop_start()

while True:
  # Our application produce some messages
  time_running = time() - start_time

  for topic, fields in topics.items():
    data = {}
    for field in fields:
      data[field] = time_running
    
    msg_info = mqttc.publish(topic, json.dumps(data), qos=1)
    print(topic, data)
  try:
    msg_info.wait_for_publish(timeout=1.0)
  except ValueError:
    print("message queue full")
  except RuntimeError as e:
    print(e)
  
  sleep(1.0)
  