import { test, expect, type Page, type Locator } from '@playwright/test';
import { connectedAndReset } from './shared';
import * as path from 'path';
import * as fs from 'fs';
import { send } from 'process';

function getBuildIdData() {
  const pcb = process.env.PCB_ID;
  const appBinary = path.resolve(__dirname,
    `../../build/pcb${pcb}/mgt-boot/app_offset_signed.bin`)
  const buildTimeFile = path.resolve(__dirname,
    `../../build/pcb${pcb}/build_time.txt`);
  const appBuildId = fs.readFileSync(buildTimeFile, 'ascii');
  return { appBinary, appBuildId }
}

test.beforeEach(async ({ page }) => {
  await connectedAndReset(page);
  await page.getByText('FW Update').click();
});

test('Build Id is visible', async ({ page }) => {
  const buildId = page.getByTestId("fw-build-id");

  await expect(async () => {
    const id = Number(await buildId.innerText());
    expect(id).toBeLessThan(Date.now() / 1000);
    expect(id).toBeGreaterThan(1744665208);
  }).toPass({ intervals: [1000], timeout: 5000 });
});


test('Update the firmware', async ({ page }) => {
  test.setTimeout(4 * 60_000);
  const fastExpect = expect.configure({ timeout: 1000 });
  const medExpect = expect.configure({ timeout: 7000 });
  const slowExpect = expect.configure({ timeout: 45_000 });

  // page.on("console", msg => console.log(msg.text()));
  const imageFileInput = page.getByLabel("Choose an image file");
  const buildId = page.getByTestId("fw-build-id");
  const { appBinary, appBuildId } = getBuildIdData();
  const progress = page.getByTestId("upload-progress");
  const resetBtn = page.getByRole("form", { name: "Reset" }).getByRole("button", { name: "Send" });
  const sendBtn = page.getByRole("button", { name: "Send Image" });

  let id = 0;
  await expect(async () => {
    id = Number(await buildId.innerText());
    expect(id).toBeLessThan(Date.now() / 1000);
    expect(id).toBeGreaterThan(1744665208);
  }).toPass({ intervals: [1000], timeout: 5000 });

  test.skip(id === Number(appBuildId), "Firmware already Latest.");

  await imageFileInput.setInputFiles(appBinary);
  await fastExpect(imageFileInput).toHaveValue(/app_offset_signed.bin/);

  await sendBtn.click();

  await fastExpect(progress).toHaveText(/Upload in progress: 0/);
  let { pagesComplete, pageCount } = getProgressData(await progress.innerText());

  while (pagesComplete < pageCount) {
    await medExpect(progress).not.toHaveText(`Upload in progress: ${pagesComplete}/${pageCount}`);
    const progressText = await progress.innerText();
    if (progressText === "Upload complete")
      break;
    if (progressText.startsWith("Failed"))
      throw new Error(progressText);
    const { pagesComplete: newPagesComplete } = getProgressData(progressText);
    expect(newPagesComplete).toBeGreaterThan(pagesComplete);
    pagesComplete = newPagesComplete;
  }

  await medExpect(progress).toHaveText(/complete/);
  await resetBtn.click();
  await slowExpect(buildId).toHaveText(appBuildId);
});

function getProgressData(progressText) {
  const matches = progressText.match(/(\d+)\/(\d+)/);
  if (!matches) {
    throw new Error("Failed to extract pageCount.");
  }
  const pagesComplete = parseInt(matches[1], 10);
  const pageCount = parseInt(matches[2], 10);
  return { pagesComplete, pageCount };
}