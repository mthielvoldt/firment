import { WebglLine } from "webgl-plot";
import { gridColor } from "./plotColors";

export interface AxisLabel {
  position: { x: number, y: number };
  text: string;
};

// Default gridline parameters.
const minGridlineCountX = 8;
const minGridlineCountY = 5;

/** Called when numPoints changes or new active record data shifts the plot. 
* We track the relative position of the zero position in the data and the 
* first displayed data point in the XY arrays. 
* - When we replaceLines we init the zeroXPos = 0.  
* - When we shift the data, we move the zeroXPos. 
* - When we zoom with gScale and gOffset, zeroXPos doesn't change, because
*   zooming doesn't change the xy arrays, just global scales.
*/
export function calculateXGrid(numPoints: number, canvasLeftDataPos: number,
  canvasWidthPx: number) {

  // "Gl" variable suffix is in WebGL units (2.0 = whole canvas - from -1:1)
  const glPerPt = 2 / numPoints;
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

  const xGrid = new WebglLine(gridColor, 2 * numGridLines)
  const xLabels: AxisLabel[] = [];

  // populate the points.  Each grid line comprises 2 points.
  let rising = true;
  for (let gridLineIndex = 0; gridLineIndex < numGridLines; gridLineIndex++) {
    const point0Index = gridLineIndex * 2;
    const point1Index = point0Index + 1;
    const xPosGl = firstLineXGl + gridLineIndex * gridXStepGl;  // gl units [-1,1]
    const xPosPix = Math.floor((xPosGl + 1) * canvasWidthPx / 2);
    xLabels.push({
      position: { x: xPosPix, y: 0 },
      text: (firstLineDataPos + gridLineIndex * gridStepPts).toString()
    });

    xGrid.setX(point0Index, xPosGl);
    xGrid.setY(point0Index, rising ? -2 : 2);
    xGrid.setX(point1Index, xPosGl);
    xGrid.setY(point1Index, rising ? 2 : -2);
    rising = !rising;
  }
  return { xGrid, xLabels };
}

export function calculateYGrid(minValue: number, maxValue: number, 
  canvasHeightPx: number, scaleY: number) {
  // "Gl" variable suffix is in WebGL units (2.0 = whole canvas - from -1:1)
  const rangeHeight = (maxValue - minValue);
  const gridStep = getNearestStepSize(rangeHeight / minGridlineCountY);
  const numGridLines = Math.ceil(rangeHeight / gridStep) + 1;
  const firstLinePos =
    Math.ceil(maxValue / gridStep) * gridStep;  // top of window.
  
  const yGrid = new WebglLine(gridColor, 2 * numGridLines);
  const yLabels: AxisLabel[] = [];

  // populate the points.  Each grid line comprises 2 points.
  let leftToRight = true;
  for (let gridLineIndex = 0; gridLineIndex < numGridLines; gridLineIndex++) {
    const yPosGl = firstLinePos - gridLineIndex * gridStep;  // gl units [-1,1]
    const yPosPix = Math.floor((-yPosGl + 1/scaleY) * canvasHeightPx * scaleY / 2);
    yLabels.push({
      position: { x: 0, y: yPosPix },
      text: yPosPix < 8 ? "" : yPosGl.toPrecision(2)
    });
    
    const point0Index = gridLineIndex * 2;
    const point1Index = point0Index + 1;
    yGrid.setX(point0Index, leftToRight ? -2 : 2);
    yGrid.setY(point0Index, yPosGl);
    yGrid.setX(point1Index, leftToRight ? 2 : -2);
    yGrid.setY(point1Index, yPosGl);
    leftToRight = !leftToRight;
  }
  return { yGrid, yLabels };
}

/** Finds the greatest number that's less than the input that is in the set:
   * {0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500 ...}  
   * In other words: {1,2, or 5} * 10^N. Where N is an integer.
   * Examples: input=>return 777=>500  45=>20  0.0345=>0.02
   */
function getNearestStepSize(input: number) {
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