import { test, expect, type Page } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  await page.goto('http://localhost:5173/firment/');
});

test('Title is correct', async ({ page }) => {
  // Expect a title "to contain" a substring.
  await expect(page).toHaveTitle(/Embedded Project/);
});

test('WaveformCtl can enable and disable channel A', async ({ page }) => {

  page.on("console", msg => console.log(msg.text()));

  const currentMa = page.getByText("currentMa");
  const voltageV = page.getByText("voltageV");

  const ipInput = page.getByLabel("Broker IP Address:")
  await ipInput.fill("listpalette.com");
  await ipInput.press('Enter');
  await expect(currentMa).toHaveText(/5/);


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


  // await page.getByRole('link', { name: 'Get started' }).click();

  // Expects page to have a heading with the name of Installation.
  // await expect(page.getByRole('heading', { name: 'Installation' })).toBeVisible();
});
