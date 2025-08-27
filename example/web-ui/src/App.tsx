
import {widgets, BrokerAddress, Image, Log, Plot, Reset, Version} from 'firment-ui'
import 'firment-ui/src/App.css'
import 'firment-ui/src/plot/Plot.css'

function App() {

  return (
    <>
      <div className='row-container'>
        <h1>Firment Function Generator Example</h1>
        <BrokerAddress showBrokerUrl={true}/>
      </div>
      <div className="row-container">
        <div className='widget-column'>
          <h2>Commands</h2>
          <widgets.WaveformCtl />
          <widgets.RunScanCtl />
          <Image />
          <Reset />
        </div>
        <div className='widget-column'>
          <h2>Telemetry</h2>
          <Version />
          <widgets.WaveformTlm />
          <widgets.FirmentErrorTlm />
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
