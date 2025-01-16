/** This is a utility to mock a sine wave coming in from MQTT.
 * It is useful for debugging the Plot component. (time series view)
 */

// Hand-adjustable parameters
const pointsPerSec = 200;
const messagesPerSec = 20;
const freq = 1;
const noise = 0.1;

// Derived resources
const secPerPoint = 1 / pointsPerSec;
const pointsPerMsg = pointsPerSec / messagesPerSec;
let prevDataTime = 0;

type ProbeSignal = {
  id: number;
  value: number;
};
type ProbeSignals = {
  probeSignals: ProbeSignal[];
};
type updateModelFn = (signals: ProbeSignals) => void

function mockPeriodicData(updateModel: updateModelFn) {
  const newData =
    Array.from({ length: pointsPerMsg }).map((_, index) => {
      const dt = index * secPerPoint;
      const ySin = Math.sin((prevDataTime + dt) * freq * Math.PI * 2);
      const yNoise = Math.random() - 0.5;
      return ySin + yNoise * (noise || 0);
    });

  // advance the prevDataTime to reflect these data points.
  prevDataTime += secPerPoint * pointsPerMsg;
  while (prevDataTime > (1 / freq)) {
    console.log("reached full period. ")
    prevDataTime -= (1 / freq);
  }

  // Each new data point turns into a ProbeSignals message, and triggers a call
  // to updateModel().
  newData.forEach((value) => {
    let signals: ProbeSignals = { probeSignals: [{ id: 0, value: value }] };
    updateModel(signals);
  });
}

export default function mockSetMessageHandler(messageName: string,
  updateModel: updateModelFn) {
  console.log("Mocking ", messageName, " messages.");

  const intervalId = setInterval(() => {
    mockPeriodicData(updateModel)
  }, 1000 / messagesPerSec);

  return () => { clearInterval(intervalId) };
}