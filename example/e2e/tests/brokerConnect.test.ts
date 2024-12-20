import { test, expect, type Page } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  await page.goto('http://localhost:5173/firment/');
});

test('Title is correct', async ({ page }) => {
  // Expect a title "to contain" a substring.
  await expect(page).toHaveTitle("Embedded Project");
});

test('Loads displaying not connected', async ({page}) => {
  await expect(page.getByText("Not Connected")).toBeVisible();
});

test('Successfully connects', async ({ page }) => {
  const ipInput = page.getByLabel("Broker IP Address:")
  await ipInput.fill("listpalette.com");
  ipInput.press('Enter');
  await expect(page.getByText(/Active/)).toBeVisible();
});