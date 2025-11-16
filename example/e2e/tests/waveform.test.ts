import { test, expect, type Page } from '@playwright/test';
import { connect, resetTarget } from './shared';

test.beforeEach(async ({ page }) => {
  await connect(page);
  await page.getByText('WaveformTlm').click();
  await page.getByText('WaveformCtl').click();
  await page.getByText('ScanCtl').click();
  await page.getByText('ManualSnapCtl').click();
});

test.afterEach(async ({page}) => {
  await resetTarget(page);
});


test('WaveformCtl can enable and disable channel A', async ({ page }) => {

  // page.on("console", msg => console.log(msg.text()));
  const voltageV = page.getByTestId("tlm-voltageV");
  const waveformCtl = page.getByRole("form", { name: "WaveformCtl" });

  await waveformCtl.getByLabel("channel").selectOption({ index: 0 });
  await waveformCtl.getByLabel("enabled").setChecked(false);
  await waveformCtl.getByLabel("shape").selectOption("SINE");
  await waveformCtl.getByLabel("amplitudeV").fill("0.5");
  await waveformCtl.getByLabel("frequencyHz").fill("0.2");
  await waveformCtl.getByLabel("offsetV").fill("0.5");
  await waveformCtl.getByRole("button", { name: "Send" }).click();

  await expect(voltageV).toHaveText(/0.000/);

  await waveformCtl.getByLabel("enabled").setChecked(true);
  await waveformCtl.getByRole("button", { name: "Send" }).click();
  await expect(voltageV).not.toHaveText(/0.000/);
});

test('Streaming shows right stats', async ({ page }) => {
  // let errorCount = 0;
  // page.on("console", msg => {
  //   if (msg.type() === "error") {
  //     errorCount++;
  //     // console.error(msg.text());
  //   }
  // });

  
  // Command SINE on Channel A with range: [0.4,0.6]
  const waveformCtl = page.getByRole("form", { name: "WaveformCtl" });
  await waveformCtl.getByLabel('enabled').check();
  await waveformCtl.getByLabel("channel").selectOption({ index: 0 }); // CHAN_A
  await waveformCtl.getByLabel("shape").selectOption("SINE")
  await waveformCtl.getByLabel('amplitudeV').fill('0.2');
  await waveformCtl.getByLabel('frequencyHz').fill('5');
  await waveformCtl.getByLabel('offsetV').fill('0.5');
  await waveformCtl.getByRole('button', { name: 'Send' }).click();

  // Turn on Scanning at 10Hz
  const scanCtl = page.getByLabel('ScanCtl');
  await scanCtl.getByLabel('streamFreq').selectOption('100');
  await scanCtl.getByLabel('probe_0').selectOption('CHAN_A');
  await scanCtl.getByRole('button', { name: 'Send' }).click();


  const AAvgLocator = page.getByTestId('CHAN_A-ave');

  // This expect makes it clear if the element is found.
  await expect(AAvgLocator).toBeVisible({ timeout: 4000 });

  // Wait for average to be between 0.4 and 0.6 (it should converge on 0.5)
  await expect(async () => {
    const avgAsString = await AAvgLocator.innerText();
    const avg = Number(avgAsString);
    expect(avg).toBeLessThan(0.6);
    expect(avg).toBeGreaterThan(0.4);
  }).toPass({ intervals: [1000], timeout: 4000 });
});

test('Snapshot shows right stats', async ({ page }) => {
  // Command SINE on Channel A with range: [0.4,0.6]
  const waveformCtl = page.getByRole("form", { name: "WaveformCtl" });
  await waveformCtl.getByLabel('enabled').check();
  await waveformCtl.getByLabel("channel").selectOption({ index: 0 }); // CHAN_A
  await waveformCtl.getByLabel("shape").selectOption("SINE")
  await waveformCtl.getByLabel('amplitudeV').fill('0.2');
  await waveformCtl.getByLabel('frequencyHz').fill('5');
  await waveformCtl.getByLabel('offsetV').fill('0.5');
  await waveformCtl.getByRole('button', { name: 'Send' }).click();

  // Turn on Scanning at 10Hz
  const scanCtl = page.getByLabel('ScanCtl');
  await scanCtl.getByLabel('streamFreq').selectOption('STREAM_DISABLED');
  await scanCtl.getByLabel('snapFreq').selectOption('FREQ_1KHZ');
  await scanCtl.getByLabel('probe_0').selectOption('CHAN_A_INV');
  await scanCtl.getByRole('button', { name: 'Send' }).click();

  await page.waitForTimeout(500);

  const manualSnap = page.getByLabel("ManualSnapCtl");
  await manualSnap.getByRole('button', {name: 'Send'}).click();

  const signalCount = page.getByTestId('CHAN_A_INV-count');
  const signalMin = page.getByTestId('CHAN_A_INV-min');
  const signalMax = page.getByTestId('CHAN_A_INV-max');

  // This expect makes it clear if the element is found.
  await expect(signalCount).toHaveText('512', { timeout: 5000 });
  await expect(signalMin).toHaveText('-0.700');
  await expect(signalMax).toHaveText('-0.300');
});
