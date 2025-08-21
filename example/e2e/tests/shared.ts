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

export async function connectedAndReset(page: Page) {
  const port = process.env.CI ? '4174' : '5173';
  await page.goto(`http://localhost:${port}/firment/`);
  await expect(page.getByText(/Active/)).toBeVisible();

  const upTimeLocator = page.getByTestId('fw-up-time');

  await expect(() => greaterThan(upTimeLocator, 5)).toPass({ timeout: 5000 });

  await page.getByRole("form", { name: "Reset" })
    .getByRole("button", { name: "Send" })
    .click();

  await expect(() => lessThan(upTimeLocator, 5)).toPass();
}