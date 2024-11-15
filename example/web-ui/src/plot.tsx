import { useEffect, useRef } from 'react';
import Plot from 'react-plotly.js';
import Plotly from 'plotly.js-dist';

export default function LineGraph() {

  const plotRef = useRef(null);
  const intervalRef = useRef(null);

  useEffect(() => {
    let count = 0;
    const samplesPer = 10;
    
    // Update the graph every 200ms with new data
    intervalRef.current = setInterval(() => {
      // Extend traces with the new data points
      const newData = Array.from({length: samplesPer}, () => Math.random());
      count += samplesPer;
      Plotly.extendTraces(
        plotRef.current.el, 
        { y: [newData] },
        [0]);

      // Start sliding the x axis once we reach >200 samples.
      if (count > 200) {
        Plotly.relayout(plotRef.current.el, {xaxis: {range: [count-200, count]}});
      }
    }, 250);

    return () => clearInterval(intervalRef.current); // Cleanup on component unmount
  }, []);

  return (
    <Plot
      ref={plotRef}
      data={[{ y: [], type: 'line' }]}
      layout={{ width: 500, height: 300, title: { text: 'Scrolling Plot' } }}
      useResizeHandler={true}
      style={{ width: '100%', height: '100%' }}
    />
  );
}