import mqtt from "mqtt"; // import namespace "mqtt"

const topic = 'paho/test/topic';
let ranOnce = false;

export default function setupMq() {
  if (ranOnce) {
    return;
  }
  console.log("Attempting to connect");
  ranOnce = true;
  const client = mqtt.connect("mqtt://localhost:1883");
  client.on("connect", () => {
    console.log(`Subscribing to ${topic}`);
    client.subscribe(topic, (err) => {
      if (!err) {
        client.publish(topic, "Hello mqtt");
      }
    });
  });

  client.on("message", (topic, message) => {
    // message is Buffer
    console.log(topic, message.toString());
    // client.end();
  });
}

