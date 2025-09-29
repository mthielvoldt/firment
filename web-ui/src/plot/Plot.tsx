/** Ghost Probe Plot widget
 * A react component that delivers an oscilloscope-like interface to the time-
 * series data from an embedded target. 
 * 
 * The data is delivered by MQTT message over WebSockets.  mqclient.tsx provides
 * an interface through setMessageHandler to register a state updater function to
 * refresh the view when new data comes in.
 * 
 * This component is responsible to its children for: 
 * - Calculating new scales and offsets, which determine the view.
 * - Calculating appropriate grid lines for that view. 
 * 
 * Props are passed to children with GL (-1, 1) coordinates, meaning <Plot>
 * does not need to know about pixels (width, height) at all.
 * All Pixel-GL translations are done by <PlotLabels>
 * 
 */
import React, { useEffect, useState, useRef } from "react";
import { View, Grid } from "./plotTypes";
import { calculateXGrid, calculateYGrid, scaleOffsetToMinMax } from "./axisTools";

import * as model from "./plotModel";
import { PlotLabels } from "./PlotLabels";
import PlotStats from "./PlotStats";
// import { profile } from "../profiler";
import { setMessageHandler } from "../mqclient";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";
import PlotCanvas from "./PlotCanvas";

const defaultWindow: View = {
  xValuesOffset: 0,
  xOffset: -1,
  xScale: 0.002,
  yOffset: 0,
  yScale: 0.8
};
const emptyRecord: model.Record = { id: NaN, traces: [], traceLen: 0, indexOffset: 0 };

export default function Plot({ }) {
  const fetchCount = useRef(0);
  const [following, setFollowing] = useState(true);
  const [recordId, setRecordId] = useState(0);
  const [record, setRecord] = useState(emptyRecord);
  const [view, setView] = useState(defaultWindow);

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

      // Auto-Update the record ID if following and probes are moved.
      if (following) {
        const activeRecordId = model.getActiveRecordId();
        if (recordId !== activeRecordId) {
          fetchCount.current = 0;
          clearInterval(timerId); // prevent getting another record slice.
          setRecordId(activeRecordId);
          return; 
        }
      }

      const traceLen = model.getTraceLen(recordId);
      if (traceLen > fetchCount.current) {
        fetchCount.current = traceLen;
        const newRecord =
          model.getRecordSlice(recordId, Infinity, traceLen);
        setRecord(newRecord);
      }
    }, 500);
    console.debug("New Interval. ID: ", timerId);

    return () => {
      clearInterval(timerId);
    }
  }, [recordId, following]);

  if (following && isScrollNeeded(record, view)) {
    // Update view and grid to reflect new indexOfFirstPt
    console.debug("Scrolling");
    setView(prevWindow => {
      const xOffset = -(record.indexOffset + record.traceLen) * view.xScale;
      return { ...prevWindow, xOffset };
    })
  }

  const grid: Grid = {
    xLabels: calculateXGrid(view.xScale, view.xOffset),
    yLabels: calculateYGrid(view.yScale, view.yOffset)
  };

  function isScrollNeeded(record: model.Record, view: View) {
    const lastData = record.indexOffset + record.traceLen;
    const visible = scaleOffsetToMinMax(view.xScale, view.xOffset);
    return (lastData > visible.max) || (lastData < visible.min);
  }

  function setCenter(ptrDownX_gl: number, ptrDownY_gl: number) {
    setFollowing(false);
    setView((prev) => {
      const xOffset = prev.xOffset - ptrDownX_gl;
      const yOffset = prev.yOffset - ptrDownY_gl;
      return { ...prev, xOffset, yOffset };
    });
  }

  function setScales(xAdjust: number, yAdjust: number) {
    setView(prev => {
      const xScale = prev.xScale * xAdjust;
      const yScale = prev.yScale * yAdjust;
      const xOffset = prev.xOffset * xAdjust;
      const yOffset = prev.yOffset * yAdjust;
      return { ...prev, xOffset, yOffset, xScale, yScale };
    });
  }

  function changeRecordId(e: React.ChangeEvent<HTMLInputElement>) {
    fetchCount.current = 0;
    const newRecordId = Number.parseInt(e.currentTarget.value);
    setFollowing(false);
    setRecordId(newRecordId);
  }

  return (
    <div className="widget plot-div">
      <div className="canvas-container">
        <PlotCanvas
          record={record}
          grid={grid}
          view={view}
        />
        <PlotLabels
          grid={grid}
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
