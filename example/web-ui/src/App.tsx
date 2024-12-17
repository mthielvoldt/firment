import * as widgets from './generated/widgets.pb';
import BrokerAddress from './BrokerAddress';
import { Log } from './Log';
import Plot from './plot/Plot';
import './App.css'

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
