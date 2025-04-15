import { test, expect, type Page } from '@playwright/test';
import * as path from 'path';
import * as fs from 'fs';

function getBuildIdData() {
  const appBinary = path.resolve( __dirname, 
    "../../build/xmc/mgt-boot/app_offset_signed.bin")
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

  await expect(async () => {
    const id = Number(await buildId.innerText());
    expect(id).toBeLessThan(Date.now()/1000);
    expect(id).toBeGreaterThan(1744665208);
  }).toPass({ intervals: [250], timeout: 2000 });
});


test('Update the firmware', async ({ page }) => {
  const longTimeout = 3*60_000;
  test.setTimeout(longTimeout + 60_000);
  const fastExpect = expect.configure({timeout: 1000});
  const slowExpect = expect.configure({timeout: longTimeout});

  // page.on("console", msg => console.log(msg.text()));
  const imageFileInput = page.getByLabel("Choose an image file");
  const buildId = page.getByTestId("fw-build-id");
  const {appBinary, appBuildId} = getBuildIdData();
  const progress = page.getByTestId("upload-progress");
  const resetBtn = page.getByRole("form", { name: "Reset" })
    .getByRole("button", { name: "Send" })

  await expect(async () => {
    const id = Number(await buildId.innerText());
    expect(id).toBeLessThan(Number(appBuildId));
    expect(id).toBeGreaterThan(1744665208);
  }).toPass({ intervals: [250], timeout: 2000 });

  await imageFileInput.setInputFiles(appBinary);
  await fastExpect(imageFileInput).toHaveValue(/app_offset_signed.bin/);
  
  await page.getByRole("button", {name: "Send Image"}).click();

  await fastExpect(progress).toHaveText(/in progress/);
  await slowExpect(progress).toHaveText(/complete/);
  await resetBtn.click();

  // TODO: End early if there's a failure notice.
  await slowExpect(buildId).toHaveText(appBuildId);
});