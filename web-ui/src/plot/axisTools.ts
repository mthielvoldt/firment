import { Trace } from "./plotModel";
import { AxisLabel } from "./plotTypes";

// Default gridline parameters.
const minGridlineCountX = 8;
const minGridlineCountY = 5;

/** calculateXGrid
 * Takes X scale and offset.  Returns 
 * @function calculateXGrid
 * @param xScale 
 * @param xOffset 
 * @returns array of labels (position_gl, text)
 */
export function calculateXGrid(numPoints: number, canvasLeftDataPos: number) {
  // WebGL units (2.0 = whole canvas - from -1:1)
  const CANVAS_WIDTH = 2;
  const glPerPt = CANVAS_WIDTH / numPoints;
  const gridStepPts = getNearestStepSize(numPoints / minGridlineCountX);

  const numGridLines = Math.ceil(numPoints / gridStepPts)
  const gridXStepGl = gridStepPts * glPerPt
  const firstLineDataPos =
    Math.ceil(canvasLeftDataPos / gridStepPts) * gridStepPts;

  /** Mod (%) operator gives the distance from canvasLeftDataPos to the nearest
   * gridLine ***moving towards zero***.  If zero is to the right (inside canvas)
   * then we can use this directly.  If it's to the left (off canvas), we find 
   * that off-canvas gridline then offset one gridline back into the canvas.
   */
  const firstLineOffset = canvasLeftDataPos > 0 ? gridStepPts : 0;
  const firstLineXGl = (firstLineOffset - canvasLeftDataPos % gridStepPts)
    * glPerPt - 1.0;

  const xLabels: AxisLabel[] = [];
  for (let gridLineIndex = 0; gridLineIndex < numGridLines; gridLineIndex++) {
    const xPosGl = firstLineXGl + gridLineIndex * gridXStepGl;  // gl units [-1,1]
    
    xLabels.push({
      position: xPosGl,
      text: (firstLineDataPos + gridLineIndex * gridStepPts).toString()
    });
  }
  return xLabels;
}

export function getGlobalMinMax(traces: Trace[]): { min: number; max: number } {
  let min = Number.POSITIVE_INFINITY;
  let max = Number.NEGATIVE_INFINITY;

  traces.forEach(trace => {
    trace.data.forEach(value => {
      if (value < min) min = value;
      if (value > max) max = value;
    });
  });

  if (min === Number.POSITIVE_INFINITY) min = NaN;
  if (max === Number.NEGATIVE_INFINITY) max = NaN;

  return { min, max };
}

/**
 * @function calculateYGrid
 * @param minValue the global minimum across all visible trace slices
 * @param maxValue the global maximum across all visible trace slices
 * @returns the text and y-position in GL-units. 
 */
export function calculateYGrid(minValue: number, maxValue: number) {

  // Work in data units at first to have gridlines land on sensible data values.
  const exactRange_dat = (maxValue - minValue);
  const gridStep_dat = getNearestStepSize(exactRange_dat / minGridlineCountY);
  const numGridLines = Math.ceil(exactRange_dat / gridStep_dat) + 1;
  
  // The window will be from one gridline above maxValue to one gridline below
  // minValue.  Calculate scale and offset to convert data into gl frame. 
  const topLinePos_dat =
    Math.ceil(maxValue / gridStep_dat) * gridStep_dat;  // top of window.

  // 1.0(top_gl) = (top_dat * scale) + offset.  Algebra gets you the next line.
  // const viewRange_dat = (numGridLines - 1) * gridStep_dat;
  // const yScale = (2 / viewRange_dat);
  // const yOffset = 1.0 - (topLinePos_dat * yScale);

  // console.debug({gridStep_dat, numGridLines, viewRange_dat, topLinePos_dat, yScale})

  const yLabels: AxisLabel[] = [];
  for (let gridLineIndex = 0; gridLineIndex < numGridLines; gridLineIndex++) {
    const linePos_dat = topLinePos_dat - gridLineIndex * gridStep_dat;
    
    yLabels.push({
      position: linePos_dat,
      text: linePos_dat.toPrecision(2)
    });
  }
  return yLabels;
}

/** Finds the greatest number that's less than the input that is in the set:
   * {0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500 ...}  
   * In other words: {1,2, or 5} * 10^N. Where N is an integer.
   * Examples: input=>return 777=>500  45=>20  0.0345=>0.02
   */
function getNearestStepSize(input: number) {
  if (input === 0 || !Number.isFinite(input))
    return 0.2;

  input = (input > 0) ? input : -input;
  let output = 1;

  /* Move input to the range [1, 10] by either multiplying or dividing by 10,
  collecting the powers of 10 in output. */
  while (input > 10) {
    input /= 10;
    output *= 10;
  }
  while (input < 1.0) {
    input *= 10;
    output /= 10;
  }
  /* Apply the final multiplier {1, 2, or 5} */
  if (input > 5) {
    output *= 5;
  }
  else if (input > 2) {
    output *= 2;
  }
  return output;
}