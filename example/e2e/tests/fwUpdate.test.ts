import { test, expect, type Page } from '@playwright/test';
import * as path from 'path';
import * as fs from 'fs';

function getBuildIdData() {
  const appBinary = path.resolve(
    __dirname, "../../build/xmc/mgt-boot/app_offset_signed.bin")
  const buildTimeFile = path.resolve(
    __dirname, '../../build/xmc/build_time.txt');
  const appBuildId = fs.readFileSync(buildTimeFile, 'ascii');
  return {appBinary, appBuildId}
}

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

test('Build Id is visible', async ({ page }) => {
  const buildId = page.getByTestId("fw-build-id");

  const {appBuildId} = getBuildIdData()

  await expect(buildId).toHaveText(appBuildId);
})

test.skip('Update the firmware', async ({ page }) => {

  // page.on("console", msg => console.log(msg.text()));
  const imageFileInput = page.getByLabel("Choose an image file");
  await imageFileInput.setInputFiles(
    path.join(__dirname, '..', '..', 'build', 'xmc', 'mgt-boot', 'app_offset_signed.bin'));
  
  await page.getByRole("button", {name: "Send Image"}).click();

  const buildId = page.getByTestId("fw-build-id");

  // Improve how to unambiguously identify the FW version. 
  // Fix whatevers making the version not show up. 

  // Get the version we just allegedly uploaded from "version.txt". 
  // look for a match with whats on screen (after a wait)
  // End early if there's a failure notice.
  await expect(buildId).toHaveText("0.2.0");
  
});