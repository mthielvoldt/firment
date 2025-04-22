import { expect, type Page } from '@playwright/test';

export async function connectedAndReset(page: Page) {
  await page.goto('http://localhost:4173/firment/');
  await expect(page.getByText(/Active/)).toBeVisible();
  await page.getByRole("form", { name: "Reset" })
    .getByRole("button", { name: "Send" })
    .click();
}