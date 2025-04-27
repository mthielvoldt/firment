
export interface AxisLabel {
  position: number;
  text: string;
};

// Default gridline parameters.
const minGridlineCountX = 8;
const minGridlineCountY = 5;

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

export function calculateYGrid(minValue: number, maxValue: number) {
  // WebGL units (2.0 = whole canvas - from -1:1)
  const rangeHeight = (maxValue - minValue);
  const gridStep = getNearestStepSize(rangeHeight / minGridlineCountY);
  const numGridLines = Math.ceil(rangeHeight / gridStep) + 1;
  const firstLinePos =
    Math.ceil(maxValue / gridStep) * gridStep;  // top of window.
  
  
  const yLabels: AxisLabel[] = [];
  for (let gridLineIndex = 0; gridLineIndex < numGridLines; gridLineIndex++) {
    const yPosGl = firstLinePos - gridLineIndex * gridStep;  // gl units [-1,1]
    
    yLabels.push({
      position: yPosGl,
      text: yPosGl.toPrecision(2)
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