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
import { calculateXGrid, calculateYGrid, lastVisibleIndex } from "./axisTools";

import * as model from "./plotModel";
import { PlotLabels } from "./PlotLabels";
import PlotStats from "./PlotStats";
// import { profile } from "../profiler";
import { setMessageHandler } from "../mqclient";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";
import PlotCanvas from "./PlotCanvas";

const defaultWindow: Window = {
  xValuesOffset: 0,
  xOffset: -1,
  xScale: 0.002,
  yOffset: -0.5,
  yScale: 0.1
};
const emptyRecord: model.Record = { id: NaN, traces: [], traceLen: 0, indexOffset: 0 };
let traceLenAtLastUpdate = 0;

function getNewGrid(window: Window) {
  // grid should always fill the window. 
  const xLabels = calculateXGrid(window.xScale, window.xOffset);
  const yLabels = calculateYGrid(window.yScale, window.yOffset);

  // console.debug(yLabels);
  return { xLabels, yLabels } as Grid;
}

export default function Plot({ }) {
  const [following, setFollowing] = useState(true);
  const [recordId, setRecordId] = useState(0);
  const [record, setRecord] = useState(emptyRecord);
  // Grid updates whenever window updates: TODO: merge these states.
  const [window, setWindow] = useState(defaultWindow);
  const [grid, setGrid] = useState(getNewGrid(defaultWindow));

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
        const newRecord =
          model.getRecordSlice(recordId, Infinity, traceLen);
        setRecord(newRecord);
      }
    }, 100);
    return () => {
      clearInterval(timerId);
    }
  }, [recordId]);

  if (following && isScrollNeeded(record, window)) {
    // Update window and grid to reflect new indexOfFirstPt
    console.debug("Scrolling");
    setWindow(prevWindow => {
      const xOffset = -(record.indexOffset + record.traceLen) * window.xScale;
      const newWindow = { ...prevWindow, xOffset };
      setGrid(getNewGrid(newWindow));
      return newWindow;
    })
  }

  function isScrollNeeded(record: model.Record, window: Window) {
    const lastData = record.indexOffset + record.traceLen;
    const lastVisble = lastVisibleIndex(window.xScale, window.xOffset);
    return lastData > lastVisble;
  }

  function setCenter(ptrDownX_gl: number, ptrDownY_gl: number) {
    setWindow((prev) => {
      const xOffset = prev.xOffset - ptrDownX_gl;
      const yOffset = prev.yOffset - ptrDownY_gl;
      console.debug(`New Center at ${xOffset}, ${yOffset}`);
      const newWindow = { ...prev, xOffset, yOffset };
      setGrid(getNewGrid(newWindow));
      return newWindow;
    });
  }

  function setScales(xAdjust: number, yAdjust: number) {
    setWindow(prev => {
      const xScale = prev.xScale * xAdjust;
      const yScale = prev.yScale * yAdjust;
      const xOffset = prev.xOffset * xAdjust;
      const yOffset = prev.yOffset * yAdjust;
      const newWindow = { ...prev, xOffset, yOffset, xScale, yScale };
      setGrid(getNewGrid(newWindow));
      return newWindow;
    });
  }

  function changeRecordId(e: React.ChangeEvent<HTMLInputElement>) {
    traceLenAtLastUpdate = 0;
    const newRecordId = Number.parseInt(e.currentTarget.value);
    setRecordId(newRecordId);
  }

  return (
    <div className="widget plot-div">
      <div className="canvas-container">
        <PlotCanvas
          record={record}
          grid={grid}
          view={window}
        />
        <PlotLabels
          grid={grid}
          window={window}
          setCenter={setCenter}
          setScales={setScales}
        />
      </div>
      <PlotStats traces={record.traces} />
      <label>
        Record:
        <input type="number" className="" value={recordId}
          onChange={changeRecordId} />
      </label>
      <label>
        Follow:
        <input type="checkbox" className="" checked={following}
          onChange={e => setFollowing(e.currentTarget.checked)} />
      </label>
    </div>
  );
}
