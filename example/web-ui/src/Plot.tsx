/** Ghost Probe Plot widget
 * A react component that delivers an oscilloscope-like interface to the time-
 * series data from an embedded target. 
 * 
 * The data is delivered by MQTT message over WebSockets.  mqclient.tsx provides
 * an interface through addTopicCallback to register a state updater function to
 * refresh the view when new data comes in.
 */
import { ChangeEvent, useEffect, useRef, useState } from "react";
import { WebglPlot, WebglLine, ColorRGBA } from "webgl-plot";
import { addTopicCallback } from "./mqclient";

// Hand-adjustable parameters
const pointsPerSec = 1000;
const messagesPerSec = 25;
const secPerWindow = 1;
const fpsDivder = 2;
const scaleY = 0.8;

// Derived resources
const pointsPerMsg = pointsPerSec / messagesPerSec;
let prevDataTime = 0;

const secPerPoint = 1 / pointsPerSec;
let fpsCounter = 0;
let wglp: (WebglPlot | null) = null;
let line: (WebglLine | null) = null;
let data: number[] = [];
let firstUnrenderedIndex = 0;

type prop = {
  freq: number;
  amp: number;
  noise?: number;
};

export default function Plot({ freq, amp, noise }: prop) {
  const [numPoints, setNumPoints] = useState(secPerWindow / secPerPoint);
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
      line = new WebglLine(
        new ColorRGBA(Math.random() / 2 + 0.5, Math.random() / 2 + 0.5, Math.random(), 1),
        numPoints);
      // wglp.removeAllLines();
      wglp.addLine(line);
      line.arrangeX();
      if (numPoints < data.length) {
        line.replaceArrayY(data.slice(-numPoints));
      } else {
        line.replaceArrayY(Array(numPoints - data.length).fill(0).concat(data));
      }

      /**
       * TODO: extract this to a test-code file.
       */
      // function mockPeriodicData() {
      //   const newData =
      //     Array.from({ length: pointsPerMsg }).map((_, index) => {
      //       const dt = index * secPerPoint;
      //       const ySin = Math.sin((prevDataTime + dt) * freq * Math.PI * 2);
      //       const yNoise = Math.random() - 0.5;
      //       return ySin + yNoise * (noise || 0);
      //     });
      //   prevDataTime += secPerPoint * pointsPerMsg;
      //   while (prevDataTime > (freq * Math.PI * 2)) {
      //     prevDataTime -= (freq * Math.PI * 2);
      //   }
      //   data.push(...newData);
      // }
      // let intervalId = setInterval(mockPeriodicData, 1000 / messagesPerSec);

      type ProbeSignal = {
        id: number;
        value: number;
      };
      type ProbeSignals = {
        probeSignals: ProbeSignal[];
      };

      addTopicCallback("ProbeSignals", (signals: ProbeSignals) => {
        data.push(signals.probeSignals[0].value);
      })

      // cleanup
      return () => {
        console.log("Cleanup canvas.");
        (typeof intervalId !== "undefined") && clearInterval(intervalId);
        (wglp) && wglp.removeAllLines();
        line = null;
      };
    }
  }, [numPoints]);

  // Animation engine: updates frame when new data is available.
  useEffect(() => {
    console.log("Setup animation.")
    let newFrame = () => {
      // Run this once every fpsDivider.
      if (wglp && (data.length > firstUnrenderedIndex) && (fpsCounter >= fpsDivder)) {
        fpsCounter = 0;
        wglp.linesData.forEach((line) => {
          const yArray = new Float32Array(
            data.slice(firstUnrenderedIndex));

          // indicate we've now rendered all the data.
          firstUnrenderedIndex = data.length;
          (line as WebglLine).shiftAdd(yArray);
        });

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
  }, [freq, amp, noise, numPoints]);

  const canvasStyle = {
    width: "500px",
    height: "70vh"
  };

  function changeNumPoints(e: ChangeEvent) {
    e.preventDefault();
    setNumPoints(Number.parseInt(e.currentTarget.value));
  }

  return (
    <div>
      <canvas style={canvasStyle} ref={canvas} />
      <label>
        Num Points:
        <input type="number" className="" value={numPoints} step={numPoints/2}
          onChange={changeNumPoints} />
      </label>
    </div>
  );
}