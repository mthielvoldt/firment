import { useState, useEffect } from "react";
import { setMessageHandler } from "./mqclient";


interface LogMessage {
  text: string;
  value: number;
  count: number;
}
export function Log({}) {
  const [LogState, setLogState] = useState([{id: 0, text: ""}]);

  function appendToLog(newLogMessage: LogMessage) {
    setLogState(prevLogState => {
      if (newLogMessage.count > prevLogState[prevLogState.length - 1].id)
      {
        let newState = prevLogState.slice(Math.max(prevLogState.length - 10, 0));
        newState.push({
          id: newLogMessage.count,
          text: newLogMessage.count + "	" + newLogMessage.text + newLogMessage.value
        });
        return newState;
      } else {
        // Enforce log message ID's always increase to avoid duplicate keys.
        return prevLogState;
      }
    });
  }
  useEffect( () => {
    setMessageHandler("Log", appendToLog);
  }, []);

  const messages = LogState.map((message) => 
    <p key={message.id}>{message.text}</p>)

  return (
    <details className="widget">
      <summary>Log</summary>
      {messages}
    </details>
  )
}