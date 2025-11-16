import { expect, type Page, type Locator } from '@playwright/test';

async function greaterThan(locator: Locator, rhvalue: number) {
  const valueAsString = await locator.innerText();
  const valueAsInt = Number.parseInt(valueAsString);
  return expect(valueAsInt).toBeGreaterThan(rhvalue);
}

async function lessThan(locator: Locator, rhvalue: number) {
  const valueAsString = await locator.innerText();
  const valueAsInt = Number.parseInt(valueAsString);
  return expect(valueAsInt).toBeLessThan(rhvalue);
}

export async function connect(page: Page) {
  const port = process.env.CI ? '4173' : '5173';
  await page.goto(`http://localhost:${port}/firment/`);

  // Connect to the device.
  await page.getByLabel('Device:Select a device...').selectOption('fmt-ex/4325468');
  await expect(page.getByText(/Active/)).toBeVisible();

  // Open the FW Meta widget to see the uptime.
  await page.getByText('FW Meta').click();

  // Wait for the first FW Meta message to arrive.
  await expect(page.getByTestId('fw-semver')).not.toHaveText('0.0.0');

  const uptimeLocator = page.getByTestId('fw-up-time');
  const uptimeText = await uptimeLocator.textContent();
  if (Number(uptimeText) > 10)
  {
    await resetTarget(page);
    await connect(page);
  }
}

export async function resetTarget(page: Page) {
  
  const uptimeLocator = page.getByTestId('fw-up-time');
  const prevUptimeTxt = await uptimeLocator.innerText();
  const prevUptime = Number.parseInt(prevUptimeTxt);

  page.getByRole("form", { name: "Reset" })
    .getByRole("button", { name: "Send" })
    .click();

  return expect(() => lessThan(uptimeLocator, prevUptime)).toPass();
}