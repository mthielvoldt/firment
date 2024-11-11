import mqtt, { MqttClient } from "mqtt"; // import namespace "mqtt"
import * as pb from "./generated/mcu_1.es6.js";

let client: MqttClient;
/** callbacks is an object containing:
 * keys: widget/message names, values: the setState functions of those widgets.
 * When an MQTT message comes in, after decoding, the message name is looked up,
 * which will be the same as the name of a widget.  That lets us call the 
 * right widget's setState function, passing it that message's data.*/
let callbacks = {}
let ranOnce = false;

export function setupMq() {
  if (ranOnce) {
    return;
  }
  console.log("Attempting to connect");
  ranOnce = true;
  client = mqtt.connect("mqtt://localhost:8080");
  client.on("connect", () => {
    client.subscribe("hq-bound", (err) => {
      if (!err) { console.log("Subscribed to 'hq-bound'"); }
      else { console.error("Failed to subscribe to hq-bound."); }
    })
  });

  client.on("message", (topic, buffer) => {
    // Parse the protobuf buffer
    let message
    try {
      message = pb.Top.decode(buffer, buffer.length);
    }
    catch (error) {
      console.error(error);
      // callbacks["Log"](pb.Log.fromObject({count: 0, text: "error", value: 0}));
    }
    console.log("decoded = ", JSON.stringify(message));

    // call the state updater for the widget this message addresses. 
    const subMsgType = Object.keys(message)[0];
    const newState = message[subMsgType];
    callbacks[subMsgType](newState);

    // client.end();
  });
}

export function teardownMq() {
  if (client.connected) {
    console.log("Disconnecting from broker.");
    client.end();
  }
}

/*
This is called by generated widget code to register a callback that updates the
widget when a new message is received.
*/
export function addTopicCallback(topic: string, callback) {
  callbacks[topic] = callback;
}

export function sendMessage(message_name: string, state_obj: object) {
  let message = { [message_name]: state_obj }
  let packet = pb.Top.encodeDelimited(message).finish();
  client.publish("edge-bound", packet);
  console.log(JSON.stringify(message), "packet: ", packet);
}
