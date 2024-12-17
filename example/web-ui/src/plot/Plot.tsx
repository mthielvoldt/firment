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
import { recalculateGrid, AxisLabel } from "./axisTools";
import * as model from "./plotModel";
import './Plot.css'
import { setMessageHandler } from "../mqclient";
import { PlotLabels } from "./PlotLabels";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";


const fpsDivder = 2;
const scaleY = 0.8;
let canvasWidthPx = 0, canvasHeight = 0;
let canvasLeftDataPos = 0;
let fpsCounter = 0;
let wglp: (WebglPlot | null) = null;

function updateGrid(numPoints: number, setLabels: React.Dispatch<React.SetStateAction<AxisLabel[]>>) {
  const { xGrid, newLabels } =
    recalculateGrid(numPoints, canvasLeftDataPos, canvasWidthPx);
  setLabels(newLabels);
  wglp && wglp.removeAuxLines();
  wglp && wglp.addAuxLine(xGrid);
}

function replaceLines(numPoints: number, record: number, setLabels: React.Dispatch<React.SetStateAction<AxisLabel[]>>) {
  if (wglp !== null) {
    console.log("replace lines.  numPoints: ", numPoints);

    const { traces, traceLen } = model.getRecord(record);

    for (let i = 0; i < traces.length; i++) {
      const line = new WebglLine(getPlotColors(i), numPoints);
      const trace = traces[i];
      wglp.addLine(line);

      line.arrangeX();
      if (numPoints < traceLen) {
        line.replaceArrayY(trace.data.slice(-numPoints));
      } else {
        line.replaceArrayY(
          Array(numPoints - trace.data.length).fill(0).concat(trace.data));
      }
    }
    canvasLeftDataPos = traceLen - numPoints;
    updateGrid(numPoints, setLabels);
    wglp.update();
  }
}

let frameId = 0;
function newFrame(numPoints: number, recordId: number, setLabels: React.Dispatch<React.SetStateAction<AxisLabel[]>>) {
  // Run this once every fpsDivider.
  if (wglp !== null) {

    // New data in the record we're currently plotting.
    if (fpsCounter >= fpsDivder && model.hasNewData(recordId)) {
      fpsCounter = 0;
      const newData = model.getNewData(recordId);
      canvasLeftDataPos += newData[0].length;
      wglp.linesData.forEach((line, i) => {
        (line as WebglLine).shiftAdd(newData[i]);
      });

      wglp.gScaleY = scaleY;
      wglp.gScaleX = 1.0;
      updateGrid(numPoints, setLabels);
      wglp.update();
    }
    fpsCounter++;
    frameId = requestAnimationFrame(() => newFrame(numPoints, recordId, setLabels));
  }
}


export default function Plot({ }) {
  const [numPoints, setNumPoints] = useState(1000);
  const [recordId, setRecordId] = useState(0);
  const [labels, setLabels] = useState<AxisLabel[]>([]);
  const canvas = useRef<HTMLCanvasElement>(null);

  // One-time setup: canvas, Plot and line objects, handler for new data.
  useEffect(() => {
    if (canvas.current) {
      if (wglp === null) {
        console.log("Setup canvas, wglp === null. numPoints: ", numPoints);
        const devicePixelRatio = window.devicePixelRatio || 1;
        canvasWidthPx = canvas.current.width = canvas.current.clientWidth * devicePixelRatio;
        canvasHeight = canvas.current.height = canvas.current.clientHeight * devicePixelRatio;
        wglp = new WebglPlot(canvas.current);
        console.log("width, height", canvasWidthPx, canvasHeight);
      }

      // Model: register function that mutates data model on message rx.
      let clearHandler = setMessageHandler("ProbeSignals",
        model.handleProbeSignals);

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
    replaceLines(numPoints, recordId, setLabels);

    frameId = requestAnimationFrame(() => newFrame(numPoints, recordId, setLabels));

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
      <div className="legend-item" key={i}>
        <div className="color-box" style={colorStyle}></div><span>{name}</span>
      </div>
    );
  });

  function changeNumPoints(e: React.ChangeEvent<HTMLInputElement>) {
    e.preventDefault();
    setNumPoints(Number.parseInt(e.currentTarget.value));
  }

  return (
    <div className="widget plot-div">
      <canvas style={canvasStyle} ref={canvas} />
      <PlotLabels xAxis={labels} />
      <div className="legend-container">
        {legend}
      </div>
      <label>
        Record:
        <input type="number" className="" value={recordId}
          onChange={(e) => setRecordId(Number.parseInt(e.currentTarget.value))} />
      </label>
      <label>
        Num Points:
        <input type="number" className="" value={numPoints} step={numPoints / 2}
          onChange={changeNumPoints} />
      </label>
    </div>
  );
}
