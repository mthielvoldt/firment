import { useState, useEffect } from "react";
import {updatePageSize, dataMsgPayloadSizeMax} from "./generated/updatePage"
import { sendPacked, setMessageHandler } from "./mqclient";
import { PageStatus, PageStatusEnum, Top } from "./generated/messages";

let timeoutId: ReturnType<typeof setTimeout>;
let activePageIndex = 0;
let pages: ArrayBuffer[] = [];

export default function FWUpdate({ }) {
  const [progress, setProgress] = useState("No upload yet");
  const [image, setImage] = useState(new ArrayBuffer());

  function handleTimeout() {
    pages = [];
    activePageIndex = 0;
    setProgress("Failed: timeout");
  }

  function sendPage(data: ArrayBuffer, pageIndex: number, pageCount: number) {
    const chunkCount = Math.ceil(data.byteLength / dataMsgPayloadSizeMax);
    let dataIdx = 0;
    let messages: Uint8Array[] = [];
    for (let chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++) {
      dataIdx = chunkIndex * dataMsgPayloadSizeMax;
      const payload = new Uint8Array(data.slice(dataIdx, dataIdx + dataMsgPayloadSizeMax));
      const msg = {
        ImageData: {
          pageIndex,
          pageCount,
          chunkIndex,
          chunkCountInPage: chunkCount,
          payload,
        }
      }
      messages[chunkIndex] = Top.encodeDelimited(msg).finish();
    }
    sendPacked(messages);
    
    clearTimeout(timeoutId);
    timeoutId = setTimeout(handleTimeout, 30000);
  }

  function updateStatus(message: PageStatus) {
    console.log(`PageStatus. Page ${message.pageIndex}, success: ${message.status}`);
    if (pages.length == 0)
      return
    if (message.status === PageStatusEnum.WRITE_SUCCESS) {
      if (message.pageIndex === (pages.length - 1 )) {
        pages = [];
        activePageIndex = 0;
        clearTimeout(timeoutId);
        setProgress("Upload complete");
      }
      else {
        activePageIndex++;
        sendPage(pages[activePageIndex], activePageIndex, pages.length);
        setProgress(`Upload in progress: ${activePageIndex}/${pages.length}`);
      }
    }
    else {
      pages = [];
      activePageIndex = 0;
      clearTimeout(timeoutId);
      setProgress(`Failed: bad message at page: ${message.pageIndex}`);
    }
  };

  useEffect(() => {
    setMessageHandler("PageStatus", updateStatus);
  }, []);

  async function handleSubmit(submitEvent: React.FormEvent) {
    submitEvent.preventDefault();

    const imageBytes = new Uint8Array(image);
    console.log(imageBytes.slice(0, 512));

    // updatePageSize comes from cmake var 'UPDATE_PAGE_SIZE' in firment CML.
    const pageCount = Math.ceil(image.byteLength / updatePageSize);
    setProgress(`Upload in progress: 0/${pageCount}`);
    pages = [];
    activePageIndex = 0;

    for (let pageIdx = 0; pageIdx < pageCount; pageIdx++) {
      const pageStartIdx = pageIdx * updatePageSize;
      // slice won't extend past iterable's length, regardless of param: end. 
      const pageData = image.slice(pageStartIdx, pageStartIdx + updatePageSize);
      pages.push(pageData);
    }
    // Kick off the first page transfer.
    sendPage(pages[activePageIndex], activePageIndex, pageCount);
  }

  async function handleFileChange(e: React.ChangeEvent<HTMLInputElement>) {
    e.preventDefault();
    if (e.currentTarget.files) {
      const imageData = await e.currentTarget.files[0].arrayBuffer();
      // const imageU32 = new Uint32Array(imageData);
      setImage(imageData);
    }
  }

  return (
    <details className="widget">
      <summary>FW Update</summary>
    <form aria-label="FW Update" onSubmit={handleSubmit}>
      <label>Choose an image file<br />
        <input type="file" name="image-file" accept=".bin"
          onChange={handleFileChange} />
      </label>
      <br />
      <button type="submit">Send Image</button>
      <p data-testid="upload-progress">{progress}</p>
    </form>
    </details>
  )


}