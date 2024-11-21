import React, { useState } from "react";
import { setupMq } from './mqclient';


export default function BrokerAddress({ }) {
  const [address, setAddress] = useState("localhost")

  function handleSubmit(e: React.FormEvent) {
    e.preventDefault();
    setupMq(address);
  }

  return (
    <form onSubmit={handleSubmit}>
      <label>
        Broker IP Address:
        <input type="text" value={address}
          onChange={(e) => setAddress(e.currentTarget.value)} />
      </label>
    </form>
  )
}