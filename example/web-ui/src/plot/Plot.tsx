/** Ghost Probe Plot widget
 * A react component that delivers an oscilloscope-like interface to the time-
 * series data from an embedded target. 
 * 
 * The data is delivered by MQTT message over WebSockets.  mqclient.tsx provides
 * an interface through setMessageHandler to register a state updater function to
 * refresh the view when new data comes in.
 */
import React, { useEffect, useRef, useState } from "react";
import { WebglPlot, WebglLine } from "webgl-plot";
import { getColorAsString, getPlotColors } from "./plotColors";
import { calculateXGrid, AxisLabel, calculateYGrid } from "./axisTools";
import { Stats, StatsText } from "./traceStats";
import * as model from "./plotModel";
import { PlotLabels } from "./PlotLabels";
import './Plot.css'
// import { profile } from "../profiler";

import { setMessageHandler } from "../mqclient";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";


const fpsDivder = 2;
const scaleY = 0.8;
let canvasWidthPx = 0, canvasHeightPx = 0;
let canvasLeftDataPos = 0;
let fpsCounter = 0;
let wglp: (WebglPlot | null) = null;
let recordStats: Stats[];

function updateGrid(numPoints: number, setLabels: React.Dispatch<React.SetStateAction<AxisLabel[]>>) {
  const { xGrid, xLabels } =
    calculateXGrid(numPoints, canvasLeftDataPos, canvasWidthPx);
  const { yGrid, yLabels } =
    calculateYGrid(-1, 1.1, canvasHeightPx, scaleY);
  setLabels(xLabels.concat(yLabels));
  wglp && wglp.removeAuxLines();
  wglp && wglp.addAuxLine(xGrid);
  wglp && wglp.addAuxLine(yGrid);
}

function replaceLines(numPoints: number, { traces, traceLen }: model.Record,
  setLabels: React.Dispatch<React.SetStateAction<AxisLabel[]>>,
  setStats: React.Dispatch<React.SetStateAction<StatsText[]>>) {

  if (wglp !== null) {
    console.log("replace lines.  numPoints: ", numPoints);

    recordStats = Array<Stats>(traces.length);

    for (let i = 0; i < traces.length; i++) {
      const line = new WebglLine(getPlotColors(i), numPoints);
      const trace = traces[i];
      wglp.addLine(line);
      line.arrangeX();

      const visibleData = (numPoints < traceLen) ?
        trace.data.slice(-numPoints) :
        trace.data;

      recordStats[i] = (new Stats(visibleData));
      line.replaceArrayY(numPoints < traceLen ?
        visibleData :
        Array(numPoints - trace.data.length).fill(0).concat(visibleData));
    }
    canvasLeftDataPos = traceLen - numPoints;
    setStats(recordStats.map(traceStats => traceStats.text));
    updateGrid(numPoints, setLabels);
    wglp.update();
  }
}

let frameId = 0;
function newFrame(numPoints: number, recordId: number,
  setLabels: React.Dispatch<React.SetStateAction<AxisLabel[]>>,
  setStats: React.Dispatch<React.SetStateAction<StatsText[]>>) {

  // Run this once every fpsDivider.
  if (wglp !== null) {

    // New data in the record we're currently plotting.
    if (fpsCounter >= fpsDivder && model.hasNewData(recordId)) {
      fpsCounter = 0;
      const newData = model.getNewData(recordId);
      canvasLeftDataPos += newData[0].length;
      wglp.linesData.forEach((line, i) => {
        (line as WebglLine).shiftAdd(newData[i]);
        recordStats[i].update(newData[i]);
      });

      wglp.gScaleY = scaleY;
      wglp.gScaleX = 1.0;
      setStats(recordStats.map(traceStats => traceStats.text));
      updateGrid(numPoints, setLabels);
      wglp.update();
    }
    fpsCounter++;
    // Frame rate sets the frequency of polling the data model + update.
    frameId = requestAnimationFrame(
      () => newFrame(numPoints, recordId, setLabels, setStats));
  }
}


export default function Plot({ }) {
  const [numPoints, setNumPoints] = useState(1000);
  const [recordId, setRecordId] = useState(0);

  // not expensive.  Reasonable to do whenever any state, eg labels, changes.
  let record = model.getRecord(recordId); // get the record right away.
  // console.log("rerender.  record.traces.length", record.traces.length);

  const initStatsText: StatsText = { min: "", max: "", ave: "" };
  const [statsText, setStatsText] = useState<StatsText[]>([]);
  const [labels, setLabels] = useState<AxisLabel[]>([]);
  const canvas = useRef<HTMLCanvasElement>(null);

  // One-time setup: canvas, Plot and line objects, handler for new data.
  useEffect(() => {
    if (canvas.current) {
      if (wglp === null) {
        console.log("Setup canvas, wglp === null. numPoints: ", numPoints);
        const devicePixelRatio = window.devicePixelRatio || 1;
        canvasWidthPx = canvas.current.width = canvas.current.clientWidth * devicePixelRatio;
        canvasHeightPx = canvas.current.height = canvas.current.clientHeight * devicePixelRatio;
        wglp = new WebglPlot(canvas.current);
        console.log("width, height", canvasWidthPx, canvasHeightPx);
      }

      // Model: register function that mutates data model on message rx.
      let clearHandler = setMessageHandler("ProbeSignals",
        model.handleProbeSignals);

      // profile();

      // cleanup
      return () => {
        console.log("Cleanup canvas.");
        clearHandler();
        (wglp) && wglp.removeAllLines();
      };
    }
  }, []);

  // Animation engine: updates frame when new data is available.
  useEffect(() => {
    console.log("Setup animation.");

    /* If numPoints or recordId changed, this function is re-called.  Full 
    line-replacement is needed because each line stores numPoints as immutable. */
    replaceLines(numPoints, record, setLabels, setStatsText);

    frameId = requestAnimationFrame(
      () => newFrame(numPoints, recordId, setLabels, setStatsText));

    return () => {
      console.log("Cleanup animation.")
      cancelAnimationFrame(frameId);
      if (wglp !== null) {
        wglp.removeDataLines();
        wglp.clear();
      }
    };
  }, [numPoints, recordId]);

  const canvasStyle = {
    width: "100%",
    height: "70vh"
  };

  const legend = model.getLegend(recordId).map((name, i) => {
    const rgbStr = "rgb(" + getColorAsString(i) + ")";
    const colorStyle = { backgroundColor: rgbStr }
    return (
      <tr key={i}>
        <td> <span className="color-box" style={colorStyle}></span> </td>
        <td>{name}</td>
        <td>{statsText[i].min}</td>
        <td>{statsText[i].max}</td>
        <td>{statsText[i].ave}</td>
      </tr>
    );
  });

  function changeRecordId(e: React.ChangeEvent<HTMLInputElement>) {
    const newRecordId = Number.parseInt(e.currentTarget.value);
    setRecordId(newRecordId);

    // resize statsText to match the number of traces in the new record.
    // prevents attempting to access an undefined statsText item right after 
    // changing the record to one with more traces than the previous.
    record = model.getRecord(newRecordId);
    setStatsText(record.traces.map(() => initStatsText));
  }

  function changeNumPoints(e: React.ChangeEvent<HTMLInputElement>) {
    e.preventDefault();
    setNumPoints(Number.parseInt(e.currentTarget.value));
  }

  return (
    <div className="widget plot-div">
      <canvas style={canvasStyle} ref={canvas} />
      <PlotLabels xAxis={labels} />
      <table className="legend">
        <thead>
          <tr>
            <th>Probe</th><th>Test point</th><th>min</th><th>max</th><th>ave</th>
          </tr>
        </thead>
        <tbody>{legend}</tbody>
      </table>
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
