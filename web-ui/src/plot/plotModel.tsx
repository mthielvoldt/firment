/** Ghost Probe time-series data model
 * 
*/

import { TestPointId } from "../generated/messages";

type ProbeSignal = {  // candidate rename: ProbeSample
  id: number;
  value: number;
};
type ProbeSignals = { // SampleSet
  probeSignals: ProbeSignal[];
};
export interface Trace {
  testPointId: number;
  testPointName: string;
  data: number[];
};

/** a collection of Traces */
export interface Record {
  traces: Trace[];
  traceLen: number;
  indexOffset: number;
  id: number;
}

// An array of Records.
let data: Trace[][] = [];
let lastSignals: ProbeSignal[] = [];


export function handleProbeSignals(signals: ProbeSignals) {
  if (signals.probeSignals.length === 0) return;

  const idsSame =
    (signals.probeSignals.length == lastSignals.length) &&
    signals.probeSignals.reduce((accum, signal, index) =>
      accum && (signal.id == lastSignals[index].id), true);

  lastSignals = signals.probeSignals;

  // If probes->signals routing has changed, start new data row.
  if (!idsSame) {
    const newRecord = signals.probeSignals.map((signal) => (
      {
        testPointId: signal.id,
        testPointName: TestPointId[signal.id],
        data: [],
      }
    ));
    data.push(newRecord);
    console.log("newRecord: ", newRecord);
  }
  signals.probeSignals.forEach((signal, index) => {
    data[data.length - 1][index].data.push(signal.value);
  })
}

/*
 * getSlicedRecord(recordId, maxLength, lastPoint?)
 * - returns Record {traces, traceLen} | {[], 0} if recordId not found. 
 * - if desiredEnd is not provided: returns latest data. 
 */
export function getRecordSlice(
  record: number,
  maxLength = Infinity,
  desiredEnd = Infinity) {

  let traces: Trace[] = [];
  let traceLen = 0;
  let sliceStart = 0;

  if (data[record] && data[record].length) {
    traceLen = data[record][0].data.length; // guaranteed to have at least one trace.
    
    let sliceEnd = Math.min(traceLen, desiredEnd);
    sliceStart = Math.max(0, (sliceEnd - maxLength));

    data[record].forEach((fullTrace) => {
      traces.push({
        testPointId: fullTrace.testPointId,
        testPointName: fullTrace.testPointName,
        data: fullTrace.data.slice(sliceStart, sliceEnd)
      });
    })
  }
  return { traces, traceLen, indexOffset: sliceStart, id: record } as Record;
}

export function getTraceLen(record: number): number {
  return (data[record]) ? data[record][0].data.length : 0;
}
