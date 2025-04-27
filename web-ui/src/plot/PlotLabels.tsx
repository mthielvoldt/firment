
import { useRef } from "react";
import { AxisLabel } from "./axisTools";

/** Labels are text elements overlaid on a canvas, which is managed elsewhere.
 * there are 2 groups of labels: one for x-axis labels, one for y.  
 * Ultimately, a label needs to know its position and text content. 
 * Axis labels positionally and value-wise evenly spaced.  
 */

interface Props {
  xLabels: AxisLabel[];
  yLabels: AxisLabel[];
};



export function PlotLabels({ xLabels, yLabels }: Props) {
  const divRef = useRef<HTMLDivElement>(null);

  let axisLabels: {top: number, left: number, text: string }[] = [];
  const widthPx = (divRef.current) ? divRef.current.clientWidth : 0;
  const heightPx = (divRef.current) ? divRef.current.clientHeight : 0;

  // todo: get widthPx, height using ref.  Then test id. 

  xLabels.forEach((xLabel) => {
    axisLabels.push({
      top: 0,
      left: Math.floor((xLabel.position + 1) * widthPx / 2),
      text: xLabel.text
    });
  });

  const CLEARANCE_FOR_X_LABELS_PX = 8;
  yLabels.forEach((yLabel) => {
    const top = Math.floor((-yLabel.position + 1) * heightPx / 2);
    axisLabels.push({
      top,
      left: 0,
      // Prevent X and Y label overlap in upper left corner.
      text: (top > CLEARANCE_FOR_X_LABELS_PX) ? yLabel.text : ""
    });
  });

  const labels = axisLabels.map((label, i) => {
    return (
      <div key={i} className="plot-label" style={{
        top: label.top,
        left: label.left
      }}>
        {label.text}
      </div>
    );
  });

  return (
    <div ref={divRef} className="plot-labels-div">
      {labels}
    </div>
  )
}