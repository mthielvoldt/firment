/** Ghost Probe Plot widget
 * A react component that delivers an oscilloscope-like interface to the time-
 * series data from an embedded target. 
 * 
 * The data is delivered by MQTT message over WebSockets.  mqclient.tsx provides
 * an interface through setMessageHandler to register a state updater function to
 * refresh the view when new data comes in.
 */
import React, { useEffect, useState } from "react";
import { calculateXGrid, AxisLabel, calculateYGrid } from "./axisTools";

import * as model from "./plotModel";
import { PlotLabels } from "./PlotLabels";
import PlotStats from "./PlotStats";
// import { profile } from "../profiler";
import { setMessageHandler } from "../mqclient";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";
import PlotCanvas from "./PlotCanvas";

interface Grid {
  xLabels: AxisLabel[];
  yLabels: AxisLabel[];
};
interface PlotData {
  grid: Grid;
  traces: model.Trace[];
};

const emptyGrid: Grid = { xLabels: [], yLabels: [] };
const emptyPlot: PlotData = { grid: emptyGrid, traces: [] };
let traceLenAtLastUpdate = 0;


function getNewGrid(numPoints: number, canvasLeftDataPos: number): Grid {
  const xLabels =
    calculateXGrid(numPoints, canvasLeftDataPos);
  const yLabels =
    calculateYGrid(-1, 1.1);
  return { xLabels, yLabels };
}



export default function Plot({ }) {
  const [numPoints, setNumPoints] = useState(1000);
  const [recordId, setRecordId] = useState(0);
  const [plotData, setPlotData] = useState(emptyPlot);

  // One-time setup: canvas, Plot and line objects, handler for new data.
  useEffect(() => {
    // Model: register function that mutates data model on message rx.
    const clearHandler = setMessageHandler("ProbeSignals",
      model.handleProbeSignals);

    console.log("call Plot effect");
    // cleanup
    return () => {
      console.log("cleanup Plot effect");
      clearHandler();
    };
  }, []);

  useEffect(() => {
    const timerId = setInterval(() => {
      const traceLen = model.getTraceLen(recordId);
      if (traceLen > traceLenAtLastUpdate) {
        traceLenAtLastUpdate = traceLen;
        const { traces, indexOfFirstPt } =
          model.getRecordSlice(recordId, numPoints);
        const grid = getNewGrid(numPoints, indexOfFirstPt);
        setPlotData({ grid, traces });
      }
    }, 100);
    return () => {
      clearInterval(timerId);
    }
  }, [recordId, numPoints])


  function changeRecordId(e: React.ChangeEvent<HTMLInputElement>) {
    traceLenAtLastUpdate = 0;
    const newRecordId = Number.parseInt(e.currentTarget.value);
    setRecordId(newRecordId);
  }

  function changeNumPoints(e: React.ChangeEvent<HTMLInputElement>) {
    e.preventDefault();
    setNumPoints(Number.parseInt(e.currentTarget.value));
  }

  return (
    <div className="widget plot-div">
      <div className="canvas-container">
        <PlotCanvas
          numPoints={numPoints}
          xLabels={plotData.grid.xLabels}
          yLabels={plotData.grid.yLabels}
          traces={plotData.traces}
        />
        <PlotLabels
          xLabels={plotData.grid.xLabels}
          yLabels={plotData.grid.yLabels}
        />
      </div>
      <PlotStats traces={plotData.traces} />
      <label>
        Record:
        <input type="number" className="" value={recordId}
          onChange={changeRecordId} />
      </label>
      <label>
        Num Points:
        <input type="number" className="" value={numPoints} step={numPoints / 2}
          onChange={changeNumPoints} />
      </label>
    </div>
  );
}
