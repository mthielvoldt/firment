import { useState, useEffect } from "react";
import { setupMq } from './mqclient';

interface Status { colors: string; text: string };
interface ConnectOptions {
  connected: Status, stale: Status, connecting: Status, disconnected: Status
};
const statuses: ConnectOptions = {
  connected: { colors: "success-colors", text: "Active Data" },
  stale: { colors: "warn-colors", text: "Connected, No data" },
  connecting: { colors: "warn-colors", text: "Connecting" },
  disconnected: { colors: "problem-colors", text: "Not Connected" }
};

export default function BrokerAddress({ }) {
  const [address, setAddress] = useState("listpalette.com");
  const [connectStatus, setConnectStatus] = useState<keyof ConnectOptions>("disconnected");
  let staleTimeout: NodeJS.Timeout;

  useEffect(() => {
    connectToBroker();
  }, []);

  function handleSubmit(e: React.FormEvent) {
    e.preventDefault();
    connectToBroker();
  }

  function connectToBroker() {
    clearTimeout(staleTimeout);
    setConnectStatus("connecting");
    setupMq(address, onSubscribe, onMessage, onConnectFail);
  }
  
  function onSubscribe() {
    setConnectStatus("stale");
  }
  function onMessage() {
    clearTimeout(staleTimeout);
    if (connectStatus !== "connected") setConnectStatus("connected");
    staleTimeout = setTimeout( () => {setConnectStatus("stale")}, 2000);
  }
  function onConnectFail() {
    clearTimeout(staleTimeout);
    setConnectStatus("disconnected");
  }

  return (
    <form onSubmit={handleSubmit}>
      <label>
        Broker IP Address:
        <input type="text" value={address}
          onChange={(e) => setAddress(e.currentTarget.value)} />
      </label>
      <span className={"pill " + statuses[connectStatus].colors}>
        {statuses[connectStatus].text}
      </span>
    </form>
  )
}