import { test, expect, type Page } from '@playwright/test';
import path = require('node:path');

test.beforeEach(async ({ page }) => {
  await page.goto('http://localhost:4173/firment/');
  const ipInput = page.getByLabel("Broker IP Address:")
  await ipInput.fill("listpalette.com");
  ipInput.press('Enter');
  await expect(page.getByText(/Active/)).toBeVisible();
  await page.getByRole("form", { name: "Reset" })
    .getByRole("button", { name: "Send" })
    .click();
});


test('Update the firmware', async ({ page }) => {

  // page.on("console", msg => console.log(msg.text()));
  const imageFileInput = page.getByLabel("Choose an image file");
  await imageFileInput.setInputFiles(
    path.join(__dirname, '..', '..', 'build', 'xmc', 'mgt-boot', 'app_offset_latest_signed.bin'));
  
  await page.getByRole("button", {name: "Send Image"}).click();

  const version = page.getByText("FW Version");

  // Improve how to unambiguously identify the FW version. 
  // Fix whatevers making the version not show up. 

  // Get the version we just allegedly uploaded from "version.txt". 
  // look for a match with whats on screen (after a wait)
  // End early if there's a failure notice.
  await expect(version).toHaveText("0.2.0");
  
});