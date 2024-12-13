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
import { getColorAsString, getPlotColors } from "./plotTools";
import * as model from "./plotModel";
import { setMessageHandler } from "./mqclient";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";


const fpsDivder = 2;
const scaleY = 0.8;
let fpsCounter = 0;
let wglp: (WebglPlot | null) = null;


function replaceLines(numPoints: number, record: number) {
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
    wglp.update();
  }
}

let frameId = 0;
function newFrame(recordId: number) {
  // Run this once every fpsDivider.
  if (wglp !== null) {

    // New data in the record we're currently plotting.
    if (fpsCounter >= fpsDivder && model.hasNewData(recordId)) {
      fpsCounter = 0;
      const newData = model.getNewData(recordId);
      wglp.linesData.forEach((line, i) => {
        (line as WebglLine).shiftAdd(newData[i]);
      });

      wglp.gScaleY = scaleY;
      wglp.update();
    }
    fpsCounter++;
    frameId = requestAnimationFrame(() => newFrame(recordId));
  }
}


export default function Plot({ }) {
  const [numPoints, setNumPoints] = useState(1000);
  const [recordId, setRecordId] = useState(0);
  const canvas = useRef<HTMLCanvasElement>(null);

  // One-time setup: canvas, Plot and line objects, handler for new data.
  useEffect(() => {
    if (canvas.current) {
      if (wglp === null) {
        console.log("Setup canvas, wglp === null. numPoints: ", numPoints);
        const devicePixelRatio = window.devicePixelRatio || 1;
        canvas.current.width = canvas.current.clientWidth * devicePixelRatio;
        canvas.current.height = canvas.current.clientHeight * devicePixelRatio;
        wglp = new WebglPlot(canvas.current);
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
    replaceLines(numPoints, recordId);

    frameId = requestAnimationFrame(() => newFrame(recordId));

    return () => {
      console.log("Cleanup animation.")
      cancelAnimationFrame(frameId);
      if (wglp !== null) {
        wglp.removeAllLines();
        wglp.clear();
      }
    };
  }, [numPoints, recordId]);

  const canvasStyle = {
    width: "500px",
    height: "70vh"
  };
  const legend = model.getLegend(recordId).map((name, i) => {
    const rgbStr = "rgb(" + getColorAsString(i) + ")";
    const colorStyle = {backgroundColor: rgbStr}
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
    <div>
      <canvas style={canvasStyle} ref={canvas} />
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