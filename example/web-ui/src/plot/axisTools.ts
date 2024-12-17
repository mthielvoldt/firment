import { WebglLine } from "webgl-plot";
import { gridColor } from "./plotColors";

export interface AxisLabel {
  position: { x: number, y: number };
  text: string;
};

// Default gridline parameters.
const minGridlineCount = 8;

/** Called when numPoints changes or new active record data shifts the plot. 
* We track the relative position of the zero position in the data and the 
* first displayed data point in the XY arrays. 
* - When we replaceLines we init the zeroXPos = 0.  
* - When we shift the data, we move the zeroXPos. 
* - When we zoom with gScale and gOffset, zeroXPos doesn't change, because
*   zooming doesn't change the xy arrays, just global scales.
*/
export function recalculateGrid(numPoints: number, canvasLeftDataPos: number, 
  canvasWidthPx: number) {

 // "Gl" variable suffix is in WebGL units (2.0 = whole canvas - from -1:1)
 const dataStepGl = 2 / numPoints;
 const ptsPerGridX = getNearestRoundNumber(numPoints / minGridlineCount);

 const numGridLines = Math.ceil(numPoints / ptsPerGridX)
 const gridXStepGl = ptsPerGridX * dataStepGl
 const firstLineDataPos =
   Math.ceil(canvasLeftDataPos / ptsPerGridX) * ptsPerGridX;

 /** Mod (%) operator gives the distance from canvasLeftDataPos to the nearest
  * gridLine ***moving towards zero***.  If zero is to the right (inside canvas)
  * then we can use this directly.  If it's to the left (off canvas), we find 
  * that off-canvas gridline then offset one gridline back into the canvas.
  */
 const firstLineOffset = canvasLeftDataPos > 0 ? ptsPerGridX : 0;
 const firstLineXGl = (firstLineOffset - canvasLeftDataPos % ptsPerGridX)
   * dataStepGl - 1.0;

 const xGrid = new WebglLine(gridColor, 2 * numGridLines)
 const newLabels: AxisLabel[] = [];
 // console.log("xy length", xGrid.xy.length)

 // populate the points.  Each grid line comprises 2 points.
 let rising = true;
 for (let gridLineIndex = 0; gridLineIndex < numGridLines; gridLineIndex++) {
   const point0Index = gridLineIndex * 2;
   const point1Index = point0Index + 1;
   const xPosGl = firstLineXGl + gridLineIndex * gridXStepGl;  // gl units [-1,1]
   const xPosPix = Math.floor((xPosGl + 1) * canvasWidthPx / 2);
   newLabels.push({
     position: { x: xPosPix, y: 0 },
     text: (firstLineDataPos + gridLineIndex * ptsPerGridX).toString()
   });

   xGrid.setX(point0Index, xPosGl);
   xGrid.setY(point0Index, rising ? -2 : 2);
   xGrid.setX(point1Index, xPosGl);
   xGrid.setY(point1Index, rising ? 2 : -2);
   rising = !rising;
 }
 return { xGrid, newLabels };
}

/** Finds the greatest number that's less than the input that is in the set:
   * {1, 2, 5, 10, 20, 50, 100, 200, 500 ...}  (1,2,or 5 * 10^N).  N a whole #
   * Examples: input=>return 777=>500  45=>20
   */
function getNearestRoundNumber(input: number) {
  input = (input > 0) ? input : -input;
  let output = 1;

  while (input > 10) {
    input /= 10;
    output *= 10;
  }
  if (input > 5) {
    output *= 5;
  }
  else if (input > 2) {
    output *= 2;
  }
  return output;
}