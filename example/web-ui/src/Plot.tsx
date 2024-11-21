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

/* Replace the above line with the following one to mock Ghost Probe signal. */
// import { default as setMessageHandler } from "./mockSignal";

type ProbeSignal = {
  id: number;
  value: number;
};
type ProbeSignals = {
  probeSignals: ProbeSignal[];
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
let data: number[][] = [[]];
let lastSignals: ProbeSignal[] = [];
let firstUnrenderedIndex = 0;

function replaceLines(numPoints: number) {
  if (wglp !== null) {
    console.log("replace lines")
    wglp.removeAllLines();
    wglp.clear();

    for (let i = 0; i < data.length; i++) {
      const line = new WebglLine(getPlotColors(i), numPoints);
      line.arrangeX();
      if (numPoints < data[0].length) {
        line.replaceArrayY(data[i].slice(-numPoints));
      } else {
        line.replaceArrayY(
          Array(numPoints - data[i].length).fill(0).concat(data[i]));
      }
      wglp.addLine(line);
    }
  }
}


export default function Plot({ }) {
  const [numPoints, setNumPoints] = useState(1000);
  const canvas = useRef<HTMLCanvasElement>(null);

  // Setup canvas, Plot and line objects, handler for new data.
  useEffect(() => {
    if (canvas.current) {
      if (wglp === null) {
        console.log("Setup canvas, wglp === null. numPoints: ", numPoints);
        const devicePixelRatio = window.devicePixelRatio || 1;
        canvas.current.width = canvas.current.clientWidth * devicePixelRatio;
        canvas.current.height = canvas.current.clientHeight * devicePixelRatio;
        wglp = new WebglPlot(canvas.current);
      }


      let clearHandler = setMessageHandler("ProbeSignals", (signals: ProbeSignals) => {
        const idsSame =
          (signals.probeSignals.length == lastSignals.length) &&
          signals.probeSignals.reduce((accum, signal, index) =>
            accum && (signal.id == lastSignals[index].id), true);

        lastSignals = signals.probeSignals;

        if (!idsSame) {
          // Need to mutate the original data object, not replace.
          for (let index = 0; index < data.length; index++) {
            if (index < signals.probeSignals.length) {
              data[index] = [];
            } else {
              data.pop();
            }
          }
          for (let index = data.length; index < signals.probeSignals.length; index++) {
            data.push([]);
          }
          // data = Array.from({ length: signals.probeSignals.length }, () => []);
          console.log(data);
          firstUnrenderedIndex = 0;
          replaceLines(numPoints);
        }
        signals.probeSignals.forEach((signal, index) => {
          data[index].push(signal.value);
        })
      })

      // cleanup
      return () => {
        console.log("Cleanup canvas.");
        clearHandler();
        (wglp) && wglp.removeAllLines();
      };
    }
  }, []);

  useEffect(() => { replaceLines(numPoints); }, [numPoints]);

  // Animation engine: updates frame when new data is available.
  useEffect(() => {
    console.log("Setup animation.")
    let newFrame = () => {
      // Run this once every fpsDivider.
      if (wglp && (data[0].length > firstUnrenderedIndex) && (fpsCounter >= fpsDivder)) {
        fpsCounter = 0;

        wglp.linesData.forEach((line, i) => {
          const yArray = new Float32Array(data[i].slice(firstUnrenderedIndex));
          (line as WebglLine).shiftAdd(yArray);
        });
        // indicate we've now rendered all the data.
        firstUnrenderedIndex = data[0].length;

        wglp.gScaleY = scaleY;
        wglp.update();
      }
      fpsCounter++;
      requestAnimationFrame(newFrame);
    }
    let id = requestAnimationFrame(newFrame);

    return () => {
      console.log("Cleanup animation.")
      newFrame = () => { };
      cancelAnimationFrame(id);
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
      <label>
        Num Points:
        <input type="number" className="" value={numPoints} step={numPoints / 2}
          onChange={changeNumPoints} />
      </label>
    </div>
  );
}