import { Trace } from "./plotModel";
import { AxisLabel } from "./plotTypes";

interface Limits {
  min: number;
  max: number;
};

// Default gridline parameters.
const minGridlineCountX = 5;
const minGridlineCountY = 5;

function scaleOffsetToMinMax(scale: number, offset: number) {
  const min = (-offset - 1) / scale;
  const max = (-offset + 1) / scale;
  return { min, max };
}

/** calculateXGrid
 * Takes X scale and offset.  Returns 
 * @function calculateXGrid
 * @param xScale 
 * @param xOffset 
 * @returns array of labels (position_gl, text)
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
  console.debug(xLabels);
  return xLabels;
}

/**
 * @function calculateYGrid
 * @param min the global minimum across all visible trace slices
 * @param max the global maximum across all visible trace slices
 * @returns the text and y-position in GL-units. 
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
