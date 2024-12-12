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
import { setMessageHandler } from "./mqclient";
import { getPlotColors } from "./plotTools";
import { TestPointId } from "./generated/mcu_1.es6";

/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";

type ProbeSignal = {  // candidate rename: ProbeSample
  id: number;
  value: number;
};
type ProbeSignals = { // SampleSet
  probeSignals: ProbeSignal[];
};

type Series = {
  testPointId: number;
  testPointName: string;
  data: number[];
};

const fpsDivder = 2;
const scaleY = 0.8;

let fpsCounter = 0;
let wglp: (WebglPlot | null) = null;
/* array of arrays of numbers.  
[
  [ probe0_point0, probe0_point1, probe0_point2 ...],
  [ probe1_point0, probe1_point1, probe1_point2 ...],
  [ probe2_point0, probe2_point1, probe2_point2 ...],
  ... up to the number of probes reporting data.
]
*/
let data: Series[][] = [[]];
let lastSignals: ProbeSignal[] = [];
let firstUnrenderedIndex = 0;

function handleProbeSignals(signals: ProbeSignals) {
  const idsSame =
    (signals.probeSignals.length == lastSignals.length) &&
    signals.probeSignals.reduce((accum, signal, index) =>
      accum && (signal.id == lastSignals[index].id), true);

  lastSignals = signals.probeSignals;

  // If probes->signals routing has changed, start new data row.
  if (!idsSame) {
    const newRecord = signals.probeSignals.map((signal, index) => (
      {
        testPointId: signal.id,
        testPointName: TestPointId[signal.id],
        data: [],
      }
    ));
    data.push(newRecord);
    firstUnrenderedIndex = 0;
    console.log("newRecord: ", newRecord);
  }
  signals.probeSignals.forEach((signal, index) => {
    data[data.length-1][index].data.push(signal.value);
  })
}

// View
function replaceLines(numPoints: number) {
  if (wglp !== null) {
    console.log("replace lines.  numPoints: ", numPoints);

    for (let i = 0; i < data[data.length-1].length; i++) {
      const line = new WebglLine(getPlotColors(i), numPoints);
      line.arrangeX();
      if (numPoints < data[data.length-1][0].data.length) {
        line.replaceArrayY(data[data.length-1][i].data.slice(-numPoints));
      } else {
        line.replaceArrayY(
          Array(numPoints - data[data.length-1][i].data.length).fill(0).concat(data[i]));
      }
      wglp.addLine(line);
    }
  }
}


export default function Plot({ }) {
  const [numPoints, setNumPoints] = useState(1000);
  const [recordId, setRecordId] = useState(0);

  const legend = data[recordId] ? 
    data[recordId].map((signal) => signal.testPointName) :
    [];
  console.log("legend: ",legend);
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
      let clearHandler = setMessageHandler("ProbeSignals", handleProbeSignals);

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

    // If numPoints changed, trigger a full line-replacement because each line
    // stores numPoints as immutable. 
    firstUnrenderedIndex = 0;
    let frameId = 0;

    let newFrame = () => {
      // Run this once every fpsDivider.
      if (wglp && data[recordId] && data[recordId][0] &&
          (data[recordId][0].data.length > firstUnrenderedIndex) && (fpsCounter >= fpsDivder)) {
        fpsCounter = 0;

        // If probe routing has changed, replace all lines.
        if (firstUnrenderedIndex === 0) {
          replaceLines(numPoints);
        } else {
          wglp.linesData.forEach((line, i) => {
            const yArray = new Float32Array(data[recordId][i].data.slice(firstUnrenderedIndex));
            (line as WebglLine).shiftAdd(yArray);
          });
        }
        // indicate we've now rendered all the data.
        firstUnrenderedIndex = data[recordId][0].data.length;

        wglp.gScaleY = scaleY;
        wglp.update();
      }
      fpsCounter++;
      frameId = requestAnimationFrame(newFrame);
    }
    frameId = requestAnimationFrame(newFrame);

    return () => {
      console.log("Cleanup animation.")
      newFrame = () => { };
      cancelAnimationFrame(frameId);
      if (wglp !== null) {
        wglp.removeAllLines();
        wglp.clear();
      }
    };
  }, [numPoints]);

  const canvasStyle = {
    width: "500px",
    height: "70vh"
  };

  function changeNumPoints(e: React.ChangeEvent<HTMLInputElement>) {
    e.preventDefault();
    setNumPoints(Number.parseInt(e.currentTarget.value));
  }

  return (
    <div>
      <canvas style={canvasStyle} ref={canvas} />
      <div className="row-container">

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