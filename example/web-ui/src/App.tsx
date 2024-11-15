import { useEffect } from 'react'
import { setupMq, teardownMq} from './mqclient';

import * as widgets from './generated/widgets.pb';
import './App.css'

function App() {
  useEffect( () => {
    setupMq();
    return teardownMq;
  }, []);

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
          <widgets.Log />
        </div>
      </div>
    </>
  )
}

export default App
