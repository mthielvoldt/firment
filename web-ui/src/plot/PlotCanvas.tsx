import { useEffect, useRef } from "react";
import { WebglPlot, WebglLine } from "webgl-plot";
import { gridColor, getPlotColors } from "./plotColors";
import { Record } from "./plotModel";
import { Grid, Window } from "./plotTypes";

interface Props {
  record: Record;
  grid: Grid;
  view: Window;
};

let wglp: WebglPlot;

function updateView(grid: Grid, view: Window) {
  const { xLabels, yLabels } = grid;
  // const {yScale, yOffset, xScale, xOffset} = view;

  const xGrid = new WebglLine(gridColor, 2 * xLabels.length);
  // xGrid.scaleX = xScale;
  // xGrid.offsetX = xOffset;
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
  // yGrid.scaleY = yScale;
  // yGrid.offsetY = yOffset;
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

  wglp?.removeAuxLines();
  wglp?.addAuxLine(xGrid);
  wglp?.addAuxLine(yGrid);

  wglp?.linesData.forEach((line) => {
    line.scaleY = view.yScale;
    line.offsetY = view.yOffset;
    line.scaleX = view.xScale;
    line.offsetX = view.xOffset;
  })
  wglp?.update();
}

function replaceDataLines(record: Record, view: Window) {
  if (!wglp) return;
  const { xScale, yScale, xOffset, yOffset } = view;

  wglp.removeDataLines();

  record.traces.forEach((trace, traceIdx) => {
    const color = getPlotColors(traceIdx);
    const line = new WebglLine(color, trace.data.length);
    line.scaleY = yScale;
    line.offsetY = yOffset;
    line.scaleX = xScale;
    line.offsetX = xOffset;

    wglp.addLine(line);
    line.lineSpaceX(view.xValuesOffset, 1);
    line.replaceArrayY(trace.data);
  })
  wglp.update();
}



export default function PlotCanvas({ record, grid, view }: Props) {
  const canvas = useRef<HTMLCanvasElement>(null);
  const prevRecord = useRef({ end: NaN, id: NaN });

  useEffect(() => {
    if (canvas.current) {
      console.log("new wglp. devicePixelRatio: ", window.devicePixelRatio,
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
    if (isIncrementalUpdate(record)) {
      // extendDataLines(record);
      // prevRecord.current = {id: record.id, end: record.indexOffset + record.traceLen };
    }
    else {
      replaceDataLines(record, view);
      console.debug(`Replace data; len: ${record.traceLen}`)
    }
  }, [record]);

  useEffect(() => {
    updateView(grid, view);
  }, [view])

  function isIncrementalUpdate(record: Record) {
    const continues = record.indexOffset == prevRecord.current.end;
    const sameId = record.id == prevRecord.current.id;
    return (continues && sameId);
  }


  return (
    <canvas ref={canvas} />
  )
}