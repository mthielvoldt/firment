import { useEffect, useRef } from "react";
import { WebglPlot, WebglLine } from "webgl-plot";
import { gridColor, getPlotColors } from "./plotColors";
import { Trace } from "./plotModel";
import { AxisLabel } from "./axisTools";

interface Props {
  numPoints: number;
  xLabels: AxisLabel[];
  yLabels: AxisLabel[];
  traces: Trace[];
};

let wglp: WebglPlot;

function updateGridlines(xLabels: AxisLabel[], yLabels: AxisLabel[]) {
  const xGrid = new WebglLine(gridColor, 2 * xLabels.length);

  let rising = true;
  for (let gridLineIndex = 0; gridLineIndex < xLabels.length; gridLineIndex++) {
    const point0Index = gridLineIndex * 2;
    const point1Index = point0Index + 1;
    xGrid.setX(point0Index, xLabels[gridLineIndex].position);
    xGrid.setY(point0Index, rising ? -2 : 2);
    xGrid.setX(point1Index, xLabels[gridLineIndex].position);
    xGrid.setY(point1Index, rising ? 2 : -2);
    rising = !rising;
  }

  const yGrid = new WebglLine(gridColor, 2 * yLabels.length);
  let leftToRight = true;
  for (let gridLineIndex = 0; gridLineIndex < yLabels.length; gridLineIndex++) {
    const point0Index = gridLineIndex * 2;
    const point1Index = point0Index + 1;
    yGrid.setX(point0Index, leftToRight ? -2 : 2);
    yGrid.setY(point0Index, yLabels[gridLineIndex].position);
    yGrid.setX(point1Index, leftToRight ? 2 : -2);
    yGrid.setY(point1Index, yLabels[gridLineIndex].position);
    leftToRight = !leftToRight;
  }

  wglp && wglp.removeAuxLines();
  wglp && wglp.addAuxLine(xGrid);
  wglp && wglp.addAuxLine(yGrid);
}

function updateDataLines(traces: Trace[], numPoints: number) {
  if (!wglp) return;

  wglp.removeDataLines();
  
  traces.forEach((trace, traceIdx) => {
    const color = getPlotColors(traceIdx);
    const line = new WebglLine(color, numPoints);
    wglp.addLine(line);
    line.arrangeX();
    line.replaceArrayY(trace.data);
  })
}

function updateView(props: Props) {
  updateGridlines(props.xLabels, props.yLabels);
  updateDataLines(props.traces, props.numPoints);
  wglp?.update();
}

export default function PlotCanvas(props: Props) {
  const canvas = useRef<HTMLCanvasElement>(null);

  useEffect(() => {
    if (canvas.current) {
      console.log("new wglp. window.devicePixelRatio ", window.devicePixelRatio, 
        "clientWidth: ", canvas.current.clientWidth);
      canvas.current.width = canvas.current.clientWidth;
      canvas.current.height = canvas.current.clientHeight;
      wglp = new WebglPlot(canvas.current);
    }

    return () => {
      console.log("Cleanup canvas.");
      wglp && wglp.removeAllLines();
      wglp && wglp.clear();
    };
  }, []);

  useEffect(() => {
    updateView(props);
  }, [props]);


  return (
    <canvas ref={canvas} />
  )
}