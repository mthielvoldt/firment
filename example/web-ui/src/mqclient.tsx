import mqtt, { MqttClient } from "mqtt"; // import namespace "mqtt"
import * as pb from "./generated/mcu_1.es6.js";

let client: MqttClient;
/** messageHandlers is an object containing:
 * keys: widget/message names, values: the setState functions of those widgets.
 * When an MQTT message comes in, after decoding, the message name is looked up,
 * which will be the same as the name of a widget.  That lets us call the 
 * right widget's setState function, passing it that message's data.*/
let messageHandlers: { [index: string]: ({ }) => void } = {}
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

  client.on("message", (_, buffer) => {
    // Parse the protobuf buffer
    try {
      let message = pb.Top.decode(buffer, buffer.length);
      console.log("decoded = ", JSON.stringify(message));
      // call the state updater for the widget this message addresses. 
      const subMsgType = Object.keys(message)[0];
      const newState = Object.values(message)[0];
      if (messageHandlers.hasOwnProperty(subMsgType))
        messageHandlers[subMsgType](newState);
      else
        console.warn("message handler not found for message, ", subMsgType);
    }
    catch (error) {
      console.error(error);
      // messageHandlers["Log"](pb.Log.fromObject({count: 0, text: "error", value: 0}));
    }

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
type MessageHandler = (message: any) => void;
export function setMessageHandler(messageName: string, callback: MessageHandler) {
  messageHandlers[messageName] = callback;
  return () => { delete messageHandlers[messageName]; }
}

export function sendMessage(message_name: string, state_obj: object) {
  let message = { [message_name]: state_obj }
  const packet: Uint8Array = pb.Top.encodeDelimited(message).finish();
  const packetStr = new TextDecoder().decode(packet);
  client.publish("edge-bound", packetStr);
  console.log(JSON.stringify(message), "packet: ", packet);
}
