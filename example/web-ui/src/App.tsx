import { useEffect } from 'react'
import { setupMq, teardownMq} from './mqclient';

import * as widgets from './generated/widgets.pb';
import { Log } from './Log';
import Plot from './Plot';
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
          <widgets.RunScanCtl />
        </div>
        <div>
          <h2>Telemetry</h2>
          <widgets.WaveformTlm />
          <Log />
        </div>
        <div>
          <h2>Plots</h2>
          <Plot amp={0.5} freq={2} noise={0.1}/>
        </div>
      </div>
    </>
  )
}

export default App
