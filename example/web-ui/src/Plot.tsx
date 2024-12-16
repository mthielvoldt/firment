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
import { getColorAsString, getPlotColors, gridColor } from "./plotTools";
import * as model from "./plotModel";
import './Plot.css'
import { setMessageHandler } from "./mqclient";
import { PlotLabels, AxisLabel } from "./PlotLabels";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";


const fpsDivder = 2;
const scaleY = 0.8;
let canvasWidthPx = 0, canvasHeight = 0;
let canvasLeftDataPos = 0;
let fpsCounter = 0;
let wglp: (WebglPlot | null) = null;


function replaceLines(numPoints: number, record: number, setLabels: React.Dispatch<React.SetStateAction<AxisLabel[]>> ) {
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
    recalculateGrid(wglp, numPoints, setLabels);
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
      recalculateGrid(wglp, numPoints, setLabels);
      wglp.update();
    }
    fpsCounter++;
    frameId = requestAnimationFrame(() => newFrame(numPoints, recordId, setLabels));
  }
}

// called when numPoints changes. 
function recalculateGrid(wglp: WebglPlot, numPoints: number, setLabels: React.Dispatch<React.SetStateAction<AxisLabel[]>>) {
  // Gl is in WebGL units (2.0 = whole canvas - from -1:1)
  const dataStepGl = 2 / numPoints;
  const minGridlineCount = 8;
  const exactPtsPerGridX = numPoints / minGridlineCount;
  const ptsPerGridX = getNearestRoundNumber(exactPtsPerGridX);
  

  const numGridLines = Math.ceil(numPoints / ptsPerGridX)
  const gridXStepSz = ptsPerGridX * dataStepGl
  const firstLineX = -1 + (ptsPerGridX - canvasLeftDataPos % ptsPerGridX) * dataStepGl;

  wglp.removeAuxLines()
  const xGrid = new WebglLine(gridColor, 2 * numGridLines)
  wglp.addAuxLine(xGrid);
  const newLabels: AxisLabel[] = []; 
  // console.log("xy length", xGrid.xy.length)

  // populate the points.  Each grid line comprises 2 points.
  let rising = true;
  for (let gridLineIndex = 0; gridLineIndex < numGridLines ; gridLineIndex++) {
    const point0Index = gridLineIndex * 2;
    const point1Index = point0Index + 1;
    const xPosGl = firstLineX + gridLineIndex * gridXStepSz;  // gl units [-1,1]
    const xPosPix = Math.floor((xPosGl + 1) * canvasWidthPx / 2);
    newLabels.push({position:{x: xPosPix, y:0}, text:gridLineIndex.toString()})

    xGrid.setX(point0Index, xPosGl);
    xGrid.setY(point0Index, rising? -2:2);
    xGrid.setX(point1Index, xPosGl);
    xGrid.setY(point1Index, rising? 2:-2);
    rising = !rising;
  }
  setLabels(newLabels);
  /** Finds the greatest number that's less than the input that is in the set:
   * {1, 2, 5, 10, 20, 50, 100, 200, 500 ...}  (1,2,or 5 * 10^N).  N a whole #
   * Examples: input=>return 777=>500  45=>20
   */
  function getNearestRoundNumber(input: number) { 
    input = (input > 0) ? input : -input;
    let output = 1;

    while (input > 10) {
      input /= 10;
      output *= 10;
    }
    if (input > 5) {
      output *= 5;
    }
    else if (input > 2) {
      output *= 2;
    }
    return output;
  }

  /** We track the relative position of the zero position in the data and the 
   * first displayed data point in the XY arrays. 
   * - When we replaceLines we init the zeroXPos = 0.  
   * - When we shift the data, we move the zeroXPos. 
   * - When we zoom with gScale and gOffset, zeroXPos doesn't change, because
   *   zooming doesn't change the xy arrays, just global scales.
   */
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
    width: "500px",
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
    <div className="plot-div">
      <canvas style={canvasStyle} ref={canvas} />
      <PlotLabels xAxis={labels}/>
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