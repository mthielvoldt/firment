import { AxisLabel } from "./axisTools";

/** Labels are text elements overlaid on a canvas, which is managed elsewhere.
 * there are 2 groups of labels: one for x-axis labels, one for y.  
 * Ultimately, a label needs to know its position and text content. 
 * Axis labels positionally and value-wise evenly spaced.  
 */

export function PlotLabels({ xAxis }: { xAxis: AxisLabel[] }) {
  const labels = xAxis.map((label, i) => {
    return (
      <div key={i} className="plot-label" style={{
        top: label.position.y,
        left: label.position.x
      }}>
        {label.text}
      </div>
    );
  });

  return (
    <div className="plot-labels-div">
      {labels}
    </div>
  )
}