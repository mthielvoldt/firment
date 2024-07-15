from time import sleep, time
import atexit
import paho.mqtt.client as mqtt

broker = "localhost"
port = 1883

publish_KeyError_message = """
on_publish() is called with a mid not present in unacked_publish
This is due to an unavoidable race-condition:
* publish() return the mid of the message sent.
* mid from publish() is added to unacked_publish by the main thread
* on_publish() is called by the loop_start thread
While unlikely (because on_publish() will be called after a network round-trip),
  this is a race-condition that COULD happen

The best solution to avoid race-condition is using the msg_info from publish()
We could also try using a list of acknowledged mid rather than removing from pending list,
but remember that mid could be re-used !
"""
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
  msg_info = mqttc.publish("paho/test/topic", time() - start_time, qos=1)
  try:
    msg_info.wait_for_publish(timeout=1.0)
  except ValueError:
    print("message queue full")
  except RuntimeError as e:
    print(e)
  
  sleep(1.0)
  