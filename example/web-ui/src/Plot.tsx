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
import { setMessageHandler } from "./mqclient";
/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";


const fpsDivder = 2;
const scaleY = 0.8;
let canvasLeftDataPos = 0;
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
    canvasLeftDataPos = traceLen - numPoints;
    recalculateGrid(wglp, numPoints);
    wglp.update();
  }
}

let frameId = 0;
function newFrame(numPoints: number, recordId: number) {
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
      recalculateGrid(wglp, numPoints);
      wglp.update();
    }
    fpsCounter++;
    frameId = requestAnimationFrame(() => newFrame(numPoints, recordId));
  }
}

// called when numPoints changes. 
function recalculateGrid(wglp: WebglPlot, numPoints: number) {
  // Sz is in window units (2 = whole window - -1:1)
  const dataStepSz = 2 / numPoints;
  const minGridlineCount = 8;
  const exactPtsPerGridX = numPoints / minGridlineCount;
  const ptsPerGridX = getNearestRoundNumber(exactPtsPerGridX);
  

  const numGridLines = Math.ceil(numPoints / ptsPerGridX)
  const gridXStepSz = ptsPerGridX * dataStepSz
  const firstLineX = -1 + (ptsPerGridX - canvasLeftDataPos % ptsPerGridX) * dataStepSz;

  wglp.removeAuxLines()
  const xGrid = new WebglLine(gridColor, 2 * numGridLines)
  wglp.addAuxLine(xGrid);
  // console.log("xy length", xGrid.xy.length)

  // populate the points.  Each grid line comprises 2 points.
  let rising = true;
  for (let gridLineIndex = 0; gridLineIndex < numGridLines ; gridLineIndex++) {
    const point0Index = gridLineIndex * 2;
    const point1Index = point0Index + 1;
    xGrid.setX(point0Index, firstLineX + gridLineIndex * gridXStepSz);
    xGrid.setY(point0Index, rising? -2:2);
    xGrid.setX(point1Index, firstLineX + gridLineIndex * gridXStepSz);
    xGrid.setY(point1Index, rising? 2:-2);
    rising = !rising;
  }

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


  /** How does x grid keep up with dataLine.shiftAdd()?
   * Should I call recalculateGrid each time I shiftAdd()?
   * ShiftAdd() calls setY for every point in full numPoints. Wow! 
   * so yeah, recalcing wouldn't be inoridnately much work. 
   * 
   * keeping track of where zero is could be a hassle.  As I keep shifting data,
   * I need to keep track of how far I've shifted total.  Alternatively, I could
   * have my grid lines divvy up the whole space, and run shiftAdd on them too.
   * So let's say I have numPoints (for data line) = 10, and I have grid at 5 marks.
   * I'm seeing this: 
   * 2 3 4 5 6 7 8 9 10 11
   *       |          |   
   * So here's the deal, the X's for the data lines are fixed to the window, 
   * distributed linearly by line.arrangeX(). 
   * But unless we want as many points for the grid lines, shifting by the same 
   * number won't work, we need to move the X values (way fewer points to move).
   * 
   * We're going to need to know the offset, so keeping track isn't out of the way.
   * Let's track zero position in the data. 
   * - When we replaceLines we init the zeroXPos = 0.  
   * - When we shift the data, we move the zeroXPos. 
   * - When we zoom with gScale and gOffset, zeroXPos doesn't change, because
   *   it represents the relative position of the line's first point and the data's zero. 
   *   and we haven't moved the line data. 
   */
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

    frameId = requestAnimationFrame(() => newFrame(numPoints, recordId));

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