
import { useRef } from "react";
import { Grid, Window } from "./plotTypes";

/** Labels are text elements overlaid on a canvas, which is managed elsewhere.
 * there are 2 groups of labels: one for x-axis labels, one for y.  
 * Ultimately, a label needs to know its position and text content. 
 * Axis labels positionally and value-wise evenly spaced.  
 */

interface Props {
  grid: Grid;
  window: Window;
  zoomHandler: (xOffset: number, yOffset: number, xAdjust: number, yAdjust: number) => void;
};



export function PlotLabels(props: Props) {
  const divRef = useRef<HTMLDivElement>(null);

  let axisLabels: { top: number, left: number, text: string }[] = [];
  const widthPx = (divRef.current) ? divRef.current.clientWidth : 0;
  const heightPx = (divRef.current) ? divRef.current.clientHeight : 0;

  function setNewCenter(e: React.PointerEvent<HTMLDivElement>) {
    if (!divRef.current) return;
    const divRect = divRef.current.getBoundingClientRect();
    const xOffset = (e.clientX - divRect.left) / divRect.width;
    const yOffset = (e.clientY - divRect.top) / divRect.height;
    props.zoomHandler(xOffset, yOffset, 1, 1);
  }

  props.grid.xLabels.forEach((xLabel) => {
    axisLabels.push({
      top: 0,
      left: Math.floor((xLabel.position + 1) * widthPx / 2),
      text: xLabel.text
    });
  });

  const CLEARANCE_FOR_X_LABELS_PX = 8;
  props.grid.yLabels.forEach((yLabel) => {
    // convert data to gl units
    const position_gl = (yLabel.position * props.window.yScale) + props.window.yOffset;
    // convert to pixels
    const top = Math.floor((-position_gl + 1) * heightPx / 2);
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
    <div
      ref={divRef}
      className="plot-labels-div"
      onPointerDown={setNewCenter}
    >
      {labels}
    </div>
  )
}