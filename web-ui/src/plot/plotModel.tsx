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
type Trace = {
  testPointId: number;
  testPointName: string;
  data: number[];
};

/** a collection of Traces */
export interface Record {
  traces: Trace[];
  traceLen: number;
}

// An array of Records.
let data: Trace[][] = [[]];
let lastSignals: ProbeSignal[] = [];

/* This tracks the last record in the data table, which is where new
signals get appended.  It indicates the start of the trace data that hasn't
been read yet by getNewData */
let firstUnreadIndex = 0;

export function handleProbeSignals(signals: ProbeSignals) {
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
    firstUnreadIndex = 0; // with a new record, we haven't read any yet.
    console.log("newRecord: ", newRecord);
  }
  signals.probeSignals.forEach((signal, index) => {
    data[data.length - 1][index].data.push(signal.value);
  })
}


export function getRecord(record: number): Record {
  let traces: Trace[] = [];
  let traceLen = 0;
  if (record < data.length) {
    traces = data[record];
    traceLen = (traces.length > 0) ? traces[0].data.length : 0;

    /* If this is the last record, which grows as new signals arrive, update the
    variable that keeps track of how much has been read. */
    if (record === data.length -1)
      firstUnreadIndex = traceLen;
  }
  return { traces, traceLen }
}

/** Returns true iff:
 * - recordId is last in the table, since new data only ever goes there.
 * - The record has at least one trace.
 * - The traces in this record are longer than firstUnreadIndex indicates.
 * */
export function hasNewData(recordId: number): boolean {
  return (
    recordId === (data.length - 1) && 
    data[recordId].length > 0 && 
    data[recordId][0].data.length > firstUnreadIndex
  );
}

export function getNewData(recordId: number) {
  const unreadTraceTails = data[recordId].map((trace) => (
    new Float32Array(trace.data.slice(firstUnreadIndex))
  ));
  firstUnreadIndex = data[recordId][0].data.length;
  return unreadTraceTails;
}