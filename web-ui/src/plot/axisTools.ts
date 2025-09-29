import { Trace } from "./plotModel";
import { AxisLabel } from "./plotTypes";

interface Limits {
  min: number;
  max: number;
};

// Default gridline parameters.
const minGridlineCountX = 5;
const minGridlineCountY = 5;

export function scaleOffsetToMinMax(scale: number, offset: number) {
  const min = (-offset - 1) / scale;
  const max = (-offset + 1) / scale;
  return { min, max };
}

export function minMaxToScaleOffset(min: number, max: number) {
  const scale = 2 / (max - min);
  const offset = -((max + min) / 2) * scale;
  return { scale, offset };
}

/** calculateXGrid
 * Takes X scale and offset.  Returns 
 * @function calculateXGrid
 * @param scale X-axis scale from equation: position_gl = data * scale + offset
 * @param offset X-axis offset in same equation.  (gl units)
 * @returns Array of x-axis labels (position, text).  Positions are in GL-units.
 *   Labels are at "nice" visible numbers and are positioned along window top.
 */
export function calculateXGrid(scale: number, offset: number) {

  const limits = scaleOffsetToMinMax(scale, offset);
  const { baseValue, step, count } = limitsToGridlines(limits, minGridlineCountX);

  const xLabels: AxisLabel[] = [];
  for (let gridLineIndex = 0; gridLineIndex < count; gridLineIndex++) {
    const linePos_dat = baseValue + gridLineIndex * step;

    xLabels.push({
      position: dataToGl(linePos_dat, scale, offset),
      text: linePos_dat.toString()
    });
  }
  return xLabels;
}

/**
 * @function calculateYGrid
 * @param scale Y-axis scale from equation: position_gl = data * scale + offset
 * @param offset Y-axis offset in same equation.  (gl units)
 * @returns Array of y-axis labels (position, text).  Positions are in GL-units.
 *   Labels are at "nice" visible numbers and are positioned along window left.
 */
export function calculateYGrid(scale: number, offset: number) {

  const limits = scaleOffsetToMinMax(scale, offset);
  const { baseValue, step, count } = limitsToGridlines(limits, minGridlineCountY);

  const yLabels: AxisLabel[] = [];
  for (let gridLineIndex = 0; gridLineIndex < count; gridLineIndex++) {
    const linePos_dat = baseValue + gridLineIndex * step;

    yLabels.push({
      position: dataToGl(linePos_dat, scale, offset),
      text: linePos_dat.toPrecision(2)
    });
  }
  return yLabels;
}

function limitsToGridlines({ min, max }: Limits, minLineCount: number) {
  // Work in data units at first to have gridlines land on sensible data values.
  const exactRange = (max - min);
  const step = getNearestStepSize(exactRange / minLineCount);
  const count = Math.ceil(exactRange / step) + 1;

  // The window will be from one gridline above maxValue to one gridline below
  // minValue.  Calculate scale and offset to convert data into gl frame. 
  const baseValue =
    Math.floor(min / step) * step;  // bottom or left of window. 

  return { baseValue, step, count };
}

export function globalMinMaxFromTraceTails(traces: Trace[], xScale: number) {
  let min = Number.POSITIVE_INFINITY;
  let max = Number.NEGATIVE_INFINITY;

  traces.forEach(trace => {
    trace.data.slice(-2/xScale).forEach(value => {
      if (value < min) min = value;
      if (value > max) max = value;
    });
  });

  return (max >= min) ? { min, max } : {min: NaN, max: NaN} as Limits; 
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

function dataToGl(input: number, scale: number, offset: number) {
  return (input * scale) + offset;
}
