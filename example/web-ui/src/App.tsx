import { useState } from 'react'
import * as widgets from './generated/widgets.pb';
import './App.css'

function App() {

  return (
    <>
      <h1>Your Embedded UI</h1>
      <div className="row-container">
        <div>
          <h2>Commands</h2>
          <widgets.WaveformControl/>
        </div>
        <div>
          <h2>Telemetry</h2>
          <widgets.GridStatus/>
        </div>
      </div>
    </>
  )
}

export default App
