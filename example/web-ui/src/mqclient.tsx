import mqtt, { MqttClient } from "mqtt"; // import namespace "mqtt"

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
    const topics = Object.keys(callbacks);
    console.log(`Subscribing to ${topics}`);
    topics.forEach(topic => {
      client.subscribe(topic, (err) => {
        if (!err) {
          const testMsg = JSON.stringify({
            'amplitude': 0.5, 'frequency': 0.5, 'offset': 0.5
          });
          client.publish(topic, testMsg);
        }
      });
    });

  });

  client.on("message", (topic, message) => {
    // message is Buffer
    console.log(topic, message.toString());
    // call the state updater for the widget that subscribes to this topic. 
    const newState = JSON.parse(message.toString());
    callbacks[topic](newState);

    // client.end();
  });
}

export function addTopicCallback(topic: string, callback) {
  callbacks[topic] = callback;
}

