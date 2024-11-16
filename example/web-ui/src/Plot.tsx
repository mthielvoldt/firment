import { useEffect, useRef } from "react";
import { WebglPlot, WebglLine, ColorRGBA } from "webgl-plot";

let wglp: WebglPlot;
let line: WebglLine;
// let data: number[];
let fpsCounter = 0;
let scaleY = 1;
let fpsDivder = 2;

type prop = {
  freq: number;
  amp: number;
  noise?: number;
};

export default function Plot({ freq, amp, noise }: prop) {
  const canvasMain = useRef<HTMLCanvasElement>(null);
  const data = useRef([0]);

  useEffect(() => {
    if (canvasMain.current) {
      const devicePixelRatio = window.devicePixelRatio || 1;
      canvasMain.current.width =
        canvasMain.current.clientWidth * devicePixelRatio;
      canvasMain.current.height =
        canvasMain.current.clientHeight * devicePixelRatio;

      wglp = new WebglPlot(canvasMain.current);
      const numX = 1000;

      line = new WebglLine(new ColorRGBA(0.3, 1, 0.2, 1), numX);
      wglp.addLine(line);
      line.arrangeX();

      function newData() {
        // const newData = Array.from({length:5}).map(Math.random); 
        data.current.push(...Array.from({length:20}).map(Math.random));
      }
      setInterval(newData, 20);
    }
  }, []);

  useEffect(() => {
    let id = 0;
    // let newFrame = () => {
    //   const noise1 = noise || 0.1;
    //   for (let i = 0; i < line.numPoints; i++) {
    //     const ySin = Math.sin(Math.PI * i * freq * Math.PI * 2);
    //     const yNoise = Math.random() - 0.5;
    //     line.setY(i, ySin * amp + yNoise * noise1);
    //   }
    //   // line.replaceArrayY(data.current);
    //   id = requestAnimationFrame(newFrame);
    //   wglp.update();
    // };

    let newFrame = () => {
      // Run this once every fpsDivider.
      if (fpsCounter >= fpsDivder) {
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
    id = requestAnimationFrame(newFrame);

    return () => {
      newFrame = () => {};
      cancelAnimationFrame(id);
    };
  }, [freq, amp, noise]);

  const canvasStyle = {
    width: "500px",
    height: "70vh"
  };

  return (
    <div>
      <canvas style={canvasStyle} ref={canvasMain} />
    </div>
  );
}