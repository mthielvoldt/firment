import mqtt, { MqttClient, MqttProtocol } from "mqtt"; // import namespace "mqtt"
import * as pb from "./generated/messages";
import { Reader } from "protobufjs/minimal.js";

let client: MqttClient;
/** messageHandlers is an object containing:
 * keys: widget/message names, values: the setState functions of those widgets.
 * When an MQTT message comes in, after decoding, the message name is looked up,
 * which will be the same as the name of a widget.  That lets us call the 
 * right widget's setState function, passing it that message's data.*/
let messageHandlers: { [index: string]: ({ }) => void } = {}
let ranOnce = false;

export function setupMq(brokerHost: string, onSubscribe: () => void,
  onMessage: () => void) {
  if (ranOnce) {
    teardownMq();
  }
  console.log("Attempting to connect");
  ranOnce = true;
  const protocol: MqttProtocol = "mqtts";
  const options = {
    host: brokerHost,
    port: 8883,
    protocol: protocol,
    clientId: 'firment-client' + crypto.randomUUID(),
    rejectUnauthorized: false
  }

  client = mqtt.connect(options);

  client.on("connect", () => {
    client.subscribe("hq-bound", (err) => {
      if (!err) { 
        console.log("Subscribed to 'hq-bound'");
        onSubscribe();
      }
      else { console.error("Failed to subscribe to hq-bound."); }
    })
  });

  client.on("error", (error) => {
    console.error("Error while attempting to connect.", error);
  })
  // client.on("packetsend", (packet) => {console.log("packetsend: ", packet)});
  // client.on("packetreceive", (packet) => {console.log("packetreceive: ", packet)});
  client.on("disconnect", () => { console.log("disconnect") });
  client.on("close", () => { console.log("close") });
  client.on("end", () => { console.log("end") });


  client.on("message", (_, buffer) => {
    onMessage();
    // Parse the protobuf buffer
    let msgsDecoded = 0;
    const reader = Reader.create(buffer);
    while (reader.pos < reader.len) {
      try {
        let message = pb.Top.decodeDelimited(reader);

        // console.log("decoded = ", JSON.stringify(message));
        // call the state updater for the widget this message addresses. 
        const subMsgType = Object.keys(message)[0];
        const newState = Object.values(message)[0];
        if (messageHandlers.hasOwnProperty(subMsgType)) {
          messageHandlers[subMsgType](newState);
          msgsDecoded++
        }
        else
          console.warn("message handler not found for message, ", subMsgType);
      }
      catch (error) {
        console.error(error);
        // messageHandlers["Log"](pb.Log.fromObject({count: 0, text: "error", value: 0}));
      }
    }
    console.info("Messages: ", msgsDecoded, "in ", buffer.length, " bytes.");

    // client.end();
  });
}

export function teardownMq() {
  console.log("Disconnecting from broker.");
  client.end();
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

export function sendMessage(message_name: string, state_obj: object, verbose=true) {
  let message = { [message_name]: state_obj }
  const packet: Uint8Array = pb.Top.encodeDelimited(message).finish();
  client.publish("edge-bound", packet as Buffer);
  if (verbose)
    console.log(JSON.stringify(message), "packet: ", packet);
}
