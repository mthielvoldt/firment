import * as widgets from './fmt-lib/generated/widgets.pb';
import BrokerAddress from './fmt-lib/BrokerAddress';
import { Log } from './fmt-lib/Log';
import Plot from './fmt-lib/plot/Plot';
import Reset from './fmt-lib/Reset';
import './fmt-lib/App.css'

function App() {

  return (
    <>
      <div className='row-container'>
        <h1>Your Embedded UI</h1>
        <BrokerAddress />
      </div>
      <div className="row-container">
        <div className='widget-column'>
          <h2>Commands</h2>
          <widgets.WaveformCtl />
          <widgets.RunScanCtl />
          <Reset />
        </div>
        <div className='widget-column'>
          <h2>Telemetry</h2>
          <widgets.WaveformTlm />
          <Log />
        </div>
        <div className='plot-column'>
          <h2>Plots</h2>
          <Plot />
        </div>
      </div>
    </>
  )
}

export default App
