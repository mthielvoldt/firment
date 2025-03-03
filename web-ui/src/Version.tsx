import { useState, useEffect } from "react";
import { setMessageHandler } from "./mqclient";


export default function Version({}) {
  const [VersionState, setVersionState] = useState({'major': 0, 'minor':0, 'patch':0});
  useEffect( () => {
    setMessageHandler("Version", setVersionState);
  }, []);

  return (
    <div className="widget">
      <h4>FW Version</h4>
      <div className="field">
        <p>{VersionState.major}.{VersionState.minor}.{VersionState.patch}</p>
      </div>
    </div>
  );
}