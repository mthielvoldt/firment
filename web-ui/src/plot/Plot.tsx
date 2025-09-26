/** Ghost Probe Plot widget
 * A react component that delivers an oscilloscope-like interface to the time-
 * series data from an embedded target. 
 * 
 * The data is delivered by MQTT message over WebSockets.  mqclient.tsx provides
 * an interface through setMessageHandler to register a state updater function to
 * refresh the view when new data comes in.
 * 
 * This component is responsible to its children for: 
 * - Calculating new scales and offsets, which determine the window.
 * - Calculating appropriate grid lines for that window. 
 * 
 * Props are passed to children with GL (-1, 1) coordinates, meaning <Plot>
 * does not need to know about pixels (width, height) at all.
 * All Pixel-GL translations are done by <PlotLabels>
 * 
 */
import React, { useEffect, useState } from "react";
import { Window, Grid } from "./plotTypes";
import { calculateXGrid, calculateYGrid } from "./axisTools";

import * as model from "./plotModel";
import { PlotLabels } from "./PlotLabels";
import PlotStats from "./PlotStats";
// import { profile } from "../profiler";
import { setMessageHandler } from "../mqclient";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";
import PlotCanvas from "./PlotCanvas";

const defaultWindow: Window = { xOffset: -1, xScale: 0.002, yOffset: -0.5, yScale: 0.1 }
const emptyGrid: Grid = { xLabels: [], yLabels: [] };
let traceLenAtLastUpdate = 0;

function getNewGrid(indexOffset: number, window: Window): Grid {

  const canvasLeftDataPos = (window.xOffset + 1) / window.xScale + indexOffset;
  const numPointsVisible = 2 / window.xScale;
  const xLabels = calculateXGrid(numPointsVisible, canvasLeftDataPos);

  // const { min, max } = getGlobalMinMax(traces);  // The old way
  const min = (-window.yOffset - 1) / window.yScale; // (-.5) * 10 = -5
  const max = (-window.yOffset + 1) / window.yScale; // (1.5) * 10 = 15

  // grid should always fill the window. 
  const yLabels = calculateYGrid(min, max);

  console.debug(yLabels);
  return { xLabels, yLabels };
}

export default function Plot({ }) {
  const [numPoints, setNumPoints] = useState(1000);
  const [recordId, setRecordId] = useState(0);
  const [traces, setTraces] = useState<model.Trace[]>([]);
  const [window, setWindow] = useState(defaultWindow);
  const [grid, setGrid] = useState(emptyGrid);
  let startXScale = defaultWindow.xScale
  let startYScale = defaultWindow.yScale;

  function setCenter(ptrDownX_gl: number, ptrDownY_gl: number) {
    setWindow((prev) => {
      const xOffset = prev.xOffset - ptrDownX_gl;
      const yOffset = prev.yOffset - ptrDownY_gl;
      startXScale = prev.xScale;
      startYScale = prev.yScale;
      console.log(`New Center at ${xOffset}, ${yOffset}`);
      return { ...prev, xOffset, yOffset };
    });
  }

  function setScales(xAdjust: number, yAdjust: number) {
    setWindow(prev => {
      const xScale = xAdjust * startXScale;
      const yScale = yAdjust * startYScale;
      const xOffset = 
      console.log(`Scale Adjust: ${xAdjust}, ${yAdjust}`)
      return { ...prev, xScale, yScale };
    });
  }

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

  // Drives updates on a fixed interval.  This interval gets replaced when the
  // record or numPoints changes.
  useEffect(() => {
    const timerId = setInterval(() => {
      const traceLen = model.getTraceLen(recordId);
      if (traceLen > traceLenAtLastUpdate) {
        traceLenAtLastUpdate = traceLen;
        const { traces: newTraces, indexOfFirstPt } =
          model.getRecordSlice(recordId, numPoints);
        const newGrid = getNewGrid(indexOfFirstPt, window);
        setGrid(newGrid);
        setTraces(newTraces);
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
          traces={traces}
          grid={grid}
          window={window}
        />
        <PlotLabels
          grid={grid}
          window={window}
          setCenter={setCenter}
          setScales={setScales}
        />
      </div>
      <PlotStats traces={traces} />
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
