import { test, expect, type Page } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  await page.goto('http://localhost:5173/firment/');
  const ipInput = page.getByLabel("Broker IP Address:")
  await ipInput.fill("listpalette.com");
  ipInput.press('Enter');
  await expect(page.getByText(/Active/)).toBeVisible();
});

test.afterEach(async ({ page }) => {
  await page.getByRole("form", { name: "Reset" })
    .getByRole("button", { name: "Send" })
    .click();
})


test('WaveformCtl can enable and disable channel A', async ({ page }) => {

  // page.on("console", msg => console.log(msg.text()));
  const voltageV = page.getByText("voltageV");
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

test('ChannelA shows right stats for Sin', async ({ page }) => {

  // page.on("console", msg => console.log(msg.text()));

  const waveformCtl = page.getByRole("form", { name: "WaveformCtl" });

  // Command sine wave at 1.2Hz, ranging 0 to 1 on Channel A
  await waveformCtl.getByLabel('enabled').check();
  await waveformCtl.getByLabel("channel").selectOption({ index: 0 }); // Sine
  await waveformCtl.getByLabel('amplitudeV').fill('0.5');
  await waveformCtl.getByLabel('frequencyHz').fill('1');
  await waveformCtl.getByLabel('offsetV').fill('0.5');
  await waveformCtl.getByRole('button', { name: 'Send' }).click();

  // Turn on Scanning at 10Hz
  const scanCtl = page.getByLabel('RunScanCtl');
  await scanCtl.getByLabel('freq').selectOption('10');
  await scanCtl.getByLabel('probe_0').selectOption('2');
  await scanCtl.getByRole('button', { name: 'Send' }).click();

  // Switch plot view to active record.
  const recordInput = page.getByLabel('Record:');
  await recordInput.fill('1');
  recordInput.press('Enter');

  // Wait for average to be between 0.4 and 0.6 (it should converge on 0.5)
  await expect(async () => {
    const avgAsString = await page.getByTestId('CHAN_A-ave').innerText();
    const avg = Number(avgAsString);
    expect(avg).toBeLessThan(0.6);
    expect(avg).toBeGreaterThan(0.4);
  }).toPass({ intervals: [1000], timeout: 10000 });
});
