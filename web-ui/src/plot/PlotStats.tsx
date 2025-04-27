import { Stats, StatsText } from "./traceStats";
import { getColorAsString } from "./plotColors";
import { Trace } from "./plotModel";

interface Props { traces: Trace[] };

export default function PlotStats({ traces }: Props) {

  let recordStats = traces.map((trace) => new Stats(trace.testPointName, trace.data));
  const legendText = recordStats.map(traceStats => traceStats.text);

  function getLegendRow({ name, min, max, ave }: StatsText, i: number) {
    const rgbStr = "rgb(" + getColorAsString(i) + ")";
    const colorStyle = { backgroundColor: rgbStr }
    return (
      <tr key={i}>
        <td> <span className="color-box" style={colorStyle}></span> </td>
        <td>{name}</td>
        <td data-testid={name + "-min"}>{min}</td>
        <td data-testid={name + "-max"}>{max}</td>
        <td data-testid={name + "-ave"}>{ave}</td>
      </tr>
    );
  };
  const legend = legendText.map(getLegendRow);

  return (
    <table className="legend">
      <thead>
        <tr>
          <th>Probe</th><th>Test point</th><th>min</th><th>max</th><th>ave</th>
        </tr>
      </thead>
      <tbody>{legend}</tbody>
    </table>
  )
}