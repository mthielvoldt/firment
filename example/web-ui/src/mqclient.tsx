import mqtt, { MqttClient } from "mqtt"; // import namespace "mqtt"
import * as pb from "./generated/mcu_1.es6.js";

let client: MqttClient;
let callbacks = {}
let ranOnce = false;

export default function setupMq() {
  if (ranOnce) {
    return;
  }
  console.log("Attempting to connect");
  ranOnce = true;
  client = mqtt.connect("mqtt://localhost:8080");
  client.on("connect", () => {
    console.log(`Subscribing to Top`);
    client.subscribe("Top", (err) => {
      if (!err) { console.log("Subscribed to Top"); }
    })
  });

  client.on("message", (topic, buffer) => {
    // Parse the protobuf buffer
    let message = pb.Top.decode(buffer, buffer.length);
    console.log("decoded = ", JSON.stringify(message));

    // call the state updater for the widget this message addresses. 
    const subMsgType = Object.keys(message)[0];
    const newState = message[subMsgType];
    callbacks[subMsgType](newState);

    // client.end();
  });
}

/*
This is called by generated widget code to register a callback that updates the
widget when a new message is received.
*/
export function addTopicCallback(topic: string, callback) {
  callbacks[topic] = callback;
}

