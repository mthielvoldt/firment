import { useEffect } from 'react'
import setupMq from './mqclient';

import * as widgets from './generated/widgets.pb';
import './App.css'

function App() {
  useEffect(setupMq, []);

  return (
    <>
      <h1>Your Embedded UI</h1>
      <div className="row-container">
        <div>
          <h2>Commands</h2>
          <widgets.WaveformCtl />
        </div>
        <div>
          <h2>Telemetry</h2>
          <widgets.WaveformTlm />
        </div>
      </div>
    </>
  )
}

export default App
