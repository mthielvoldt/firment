import { useState, useEffect } from "react";
import { setMessageHandler } from "./mqclient";


export default function Version({ }) {
  const [VersionState, setVersionState] = useState({
    major: 0,
    minor: 0,
    patch: 0,
    buildId: 0,
    upTime: 0,
    deviceId: 0,
  });
  useEffect(() => {
    setMessageHandler("Version", setVersionState);
  }, []);

  return (
    <div className="widget">
      <h4>FW Version</h4>
      <dl className="telemetry">
        <dt>Semantic Version</dt>
        <dd data-testid="fw-semver">
          {VersionState.major}.{VersionState.minor}.{VersionState.patch}
        </dd>
        <dt>Build Id</dt>
        <dd data-testid="fw-build-id">{VersionState.buildId}</dd>
        <dt>Device Id</dt>
        <dd data-testid="mcu-id">{VersionState.deviceId}</dd>
        <dt>Up Time</dt>
        <dd data-testid="fw-up-time">{VersionState.upTime}</dd>
      </dl>
    </div>
  );
}