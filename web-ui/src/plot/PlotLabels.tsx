
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
  setCenter: (ptrDownX_gl: number, ptrDownY_gl: number) => void;
  setScales: (xAdjust: number, yAdjust: number) => void;
};



export function PlotLabels(props: Props) {
  const divRef = useRef<HTMLDivElement>(null);
  const dragOrigin = useRef<{ x: number, y: number } | null>(null);

  let axisLabels: { top: number, left: number, text: string }[] = [];
  const widthPx = (divRef.current) ? divRef.current.clientWidth : 0;
  const heightPx = (divRef.current) ? divRef.current.clientHeight : 0;

  function onPointerDown(e: React.PointerEvent<HTMLDivElement>) {
    if (!divRef.current) return;
    const divRect = divRef.current.getBoundingClientRect();

    dragOrigin.current = { x: e.clientX, y: e.clientY };
    (e.target as HTMLDivElement).setPointerCapture(e.pointerId);

    const xOffsetPx = e.clientX - divRect.left;
    const yOffsetPx = e.clientY - divRect.top;
    // convert to Gl units
    const ptrDownX_gl = (xOffsetPx * 2 / divRect.width) - 1;
    const ptrDownY_gl = (-yOffsetPx * 2 / divRect.height) + 1;

    console.log(`ptr: ${ptrDownX_gl}, ${ptrDownY_gl}`)
    props.setCenter(ptrDownX_gl, ptrDownY_gl);
  }

  function onPointerUp(e: React.PointerEvent<HTMLDivElement>) {
    if (!divRef.current || !dragOrigin.current) return;
    dragOrigin.current = null;
    (e.target as HTMLDivElement).releasePointerCapture(e.pointerId);
  }

  function onPointerMove(e: React.PointerEvent<HTMLDivElement>) {
    if (!dragOrigin.current) return;
    const deltaX = e.clientX - dragOrigin.current.x;
    const deltaY = e.clientY - dragOrigin.current.y;
    console.log(`deltas ${deltaX}, ${deltaY}}`);
    
    const xAdjust = 2**(deltaX/20);
    const yAdjust = 2**(deltaY/20);
    props.setScales(xAdjust, yAdjust);
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
      onPointerDown={onPointerDown}
      onPointerUp={onPointerUp}
      onPointerMove={onPointerMove}
    >
      {labels}
    </div>
  )
}