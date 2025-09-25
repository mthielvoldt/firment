
export interface Window {
  xOffset: number;
  yOffset: number;
  xScale: number;
  yScale: number;
};

export interface Grid {
  xLabels: AxisLabel[];
  yLabels: AxisLabel[];
};

export interface AxisLabel {
  position: number; // Same units as the data.
  text: string;
};