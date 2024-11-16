import { useEffect, useRef } from "react";
import { WebglPlot, WebglLine, ColorRGBA } from "webgl-plot";

// Hand-adjustable parameters
const pointsPerSec = 1000;
const messagesPerSec = 25;
const secPerWindow = 1;
const fpsDivder = 2;
const scaleY = 0.8;

// Derived resources
const pointsPerMsg = pointsPerSec / messagesPerSec;
const secPerPoint = 1 / pointsPerSec;
const numPoints = secPerWindow / secPerPoint; // aka points per window
let prevDataTime = 0;
let fpsCounter = 0;
let wglp: (WebglPlot | null);
let line: (WebglLine | null);

type prop = {
  freq: number;
  amp: number;
  noise?: number;
};

export default function Plot({ freq, amp, noise }: prop) {
  const canvas = useRef<HTMLCanvasElement>(null);
  const data = useRef([0]);

  // One-time setup: canvas size, handler for new data.
  useEffect(() => {
    if (canvas.current) {
      const devicePixelRatio = window.devicePixelRatio || 1;
      canvas.current.width = canvas.current.clientWidth * devicePixelRatio;
      canvas.current.height = canvas.current.clientHeight * devicePixelRatio;

      line = new WebglLine(new ColorRGBA(0.3, 1, 0.2, 1), numPoints);
      wglp = new WebglPlot(canvas.current);
      wglp.addLine(line);
      line.arrangeX();

      function handleNewData() {
        const newData =
          Array.from({ length: pointsPerMsg }).map((_, index) => {
            const dt = index * secPerPoint;
            const ySin = Math.sin((prevDataTime + dt) * freq * Math.PI * 2);
            const yNoise = Math.random() - 0.5;
            return ySin + yNoise * (noise || 0);
          });
        prevDataTime += secPerPoint * pointsPerMsg;
        while (prevDataTime > (freq * Math.PI * 2)) {
          prevDataTime -= (freq * Math.PI * 2);
        }
        data.current.push(...newData);
      }
      let intervalId = setInterval(handleNewData, 1000 / messagesPerSec);

      // cleanup
      return () => {
        clearInterval(intervalId);
        wglp = null;
        line = null;
      };
    }

  }, []);

  // Animation engine: updates frame when new data is available.
  useEffect(() => {
    let newFrame = () => {
      // Run this once every fpsDivider.
      if ( wglp && (data.current.length > 0) && (fpsCounter >= fpsDivder) ) {
        fpsCounter = 0;
        wglp.linesData.forEach((line) => {
          const yArray = new Float32Array(data.current);
          data.current = [];
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
      newFrame = () => { };
      cancelAnimationFrame(id);
    };
  }, [freq, amp, noise]);

  const canvasStyle = {
    width: "500px",
    height: "70vh"
  };

  return (
    <div>
      <canvas style={canvasStyle} ref={canvas} />
    </div>
  );
}