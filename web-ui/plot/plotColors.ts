import { ColorRGBA } from "webgl-plot";

const white =   new ColorRGBA(1,1,1,1);
// const red =     new ColorRGBA(1.0, 0.1, 0.1, 1);
const green =   new ColorRGBA(0.0, 1.0, 0.2, 1);
const blue =    new ColorRGBA(0.0, 0.6, 1.0, 1);
const yellow =  new ColorRGBA(0.8, 0.9, 0.0, 1);
const orange =  new ColorRGBA(1.0, 0.5, 0.0, 1);
const pink =    new ColorRGBA(1.0, 0.0, 0.8, 1);

const colors = [
  green,
  yellow,
  blue,
  pink,
  orange,
  white,
];

export function getPlotColors(i: number) {
  return colors[i];
}

export const gridColor = new ColorRGBA(0.7, 0.6, 0.3, 0.5);

export function getColorAsString(i: number) {
  const c = colors[i];
  return scaleTo255(c.r) + " " + scaleTo255(c.g) + " " + scaleTo255(c.b);
}

function scaleTo255(colorChannel: number) {
  return Math.floor((colorChannel * 255)).toString();
}