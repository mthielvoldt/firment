import { test, expect, type Page } from '@playwright/test';

test.beforeEach(async ({ page }) => {
  const port = process.env.CI ? '4173' : '5173';
  await page.goto(`http://localhost:${port}/firment/`);
});

test('Title is correct', async ({ page }) => {
  // Expect a title "to contain" a substring.
  await expect(page).toHaveTitle("Embedded Project");
});

test('Successfully connects', async ({page}) => {
  await expect(page.getByText(/Connected/)).toBeVisible();
  await page.getByLabel('Device:Select a device...').selectOption('fmt-ex/4325468');
  await expect(page.getByText(/Active/)).toBeVisible();
});

test('Fails for bad URL', async ({ page }) => {
  const ipInput = page.getByLabel("Broker URL:")
  await ipInput.fill("bogus-url.com");
  ipInput.press('Enter');

  const expectIn8s = expect.configure({timeout: 8000});
  await expectIn8s(page.getByText("Not Connected")).toBeVisible();
});
