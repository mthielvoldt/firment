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