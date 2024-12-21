import { test, expect, type Page } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  await page.goto('http://localhost:5173/firment/');
  const ipInput = page.getByLabel("Broker IP Address:")
  await ipInput.fill("listpalette.com");
  ipInput.press('Enter');
  await expect(page.getByText(/Active/)).toBeVisible();
});


test('WaveformCtl can enable and disable channel A', async ({ page }) => {

  // page.on("console", msg => console.log(msg.text()));
  const voltageV = page.getByText("voltageV");
  const waveformCtl = page.getByRole("form", {name: "WaveformCtl"});

  await waveformCtl.getByLabel("channel").selectOption({index: 0});
  await waveformCtl.getByLabel("enabled").setChecked(false);
  await waveformCtl.getByLabel("shape").selectOption("SINE");
  await waveformCtl.getByLabel("amplitudeV").fill("0.5");
  await waveformCtl.getByLabel("frequencyHz").fill("0.2");
  await waveformCtl.getByLabel("offsetV").fill("0.5");
  await waveformCtl.getByRole("button", {name: "Send"}).click();

  await expect(voltageV).toHaveText(/0.000/);

  await waveformCtl.getByLabel("enabled").setChecked(true);
  await waveformCtl.getByRole("button", {name: "Send"}).click();
  await expect(voltageV).not.toHaveText(/0.000/);
});

test('ChannelA shows right stats for Sin', async ({ page }) => {

  // page.on("console", msg => console.log(msg.text()));

  // Turn on Channel A.
  test.skip('test', async ({ page }) => {
    await page.getByLabel('enabled').check();
    await page.getByLabel('enabled').press('Tab');
    await page.getByLabel('UNSPECIFIEDSINESQUARESAWTOOTHDCshape').press('ArrowDown');
    await page.getByLabel('UNSPECIFIEDSINESQUARESAWTOOTHDCshape').press('ArrowDown');
    await page.getByLabel('UNSPECIFIEDSINESQUARESAWTOOTHDCshape').press('Tab');
    await page.getByLabel('amplitudeV').fill('0.5');
    await page.getByLabel('frequencyHz').click();
    await page.getByLabel('frequencyHz').fill('01');
    await page.getByLabel('frequencyHz').press('ArrowLeft');
    await page.getByLabel('frequencyHz').fill('1');
    await page.getByLabel('frequencyHz').press('ArrowDown');
    await page.getByLabel('frequencyHz').press('ArrowUp');
    await page.getByLabel('offsetV').click();
    await page.getByLabel('offsetV').fill('0.5');
    await page.getByLabel('offsetV').press('Enter');
    await page.getByLabel('WaveformCtl').getByRole('button', { name: 'Send' }).click();
    await page.getByLabel('SCAN_DISABLEDFREQ_2_HZFREQ_10_HZFREQ_30_HZFREQ_100_HZFREQ_250_HZFREQ_500_HZFREQ_').selectOption('10');
    await page.getByLabel('DISCONNECTEDCHAN_ACHAN_A_INVCHAN_A_PLUSCHAN_A_PLUS_INVCHAN_Bprobe_0').selectOption('2');
    await page.getByLabel('RunScanCtl').getByRole('button', { name: 'Send' }).click();
    await page.getByLabel('Record:').click();
    await page.getByLabel('Record:').press('ArrowUp');
    await expect(page.locator('tbody')).toContainText('CHAN_A');
    await page.getByRole('cell', { name: 'ave' }).click();
    await page.getByRole('cell', { name: 'CHAN_A' }).click();
    await expect(page.locator('tbody')).toContainText('0.500');
  });

  // Turn on 


  // switch the record to 1.
  await page.getByLabel("Record:").press('ArrowUp');

});
