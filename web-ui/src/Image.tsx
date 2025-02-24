import { useState, useEffect } from "react";
import flashPageSize from "./generated/flashPage"
import { sendPacked, setMessageHandler } from "./mqclient";
import { PageStatus, PageStatusEnum, Top } from "./generated/messages";

let timeoutId: ReturnType<typeof setTimeout>;
let activePageIndex = 0;
let pages: ArrayBuffer[] = [];

export default function Image({ }) {
  const [progress, setProgress] = useState("");
  const [image, setImage] = useState(new ArrayBuffer());

  function handleTimeout() {
    pages = [];
    activePageIndex = 0;
    setProgress("Upload timed out");
  }

  function sendPage(data: ArrayBuffer, pageIndex: number, pageCount: number) {
    // TODO: get max payload size from firment_msg_config.json: image-part-max-size
    const imagePartMaxSize = 32;
    const chunkCount = Math.ceil(data.byteLength / imagePartMaxSize);
    let dataIdx = 0;
    let messages: Uint8Array[] = [];
    for (let chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++) {
      dataIdx = chunkIndex * imagePartMaxSize;
      const payload = new Uint8Array(data.slice(dataIdx, dataIdx + imagePartMaxSize));
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
        setProgress("Upload complete.");
      }
      else {
        activePageIndex++;
        sendPage(pages[activePageIndex], activePageIndex, pages.length);
        setProgress(`Upload progres: ${activePageIndex}/${pages.length}`);
      }
    }
    else {
      pages = [];
      activePageIndex = 0;
      clearTimeout(timeoutId);
      setProgress(`Failed at page ${message.pageIndex}`);
    }
  };

  useEffect(() => {
    setMessageHandler("PageStatus", updateStatus);
  }, []);

  async function handleSubmit(submitEvent: React.FormEvent) {
    submitEvent.preventDefault();

    const imageBytes = new Uint8Array(image);
    console.log(imageBytes.slice(0, 512));

    // flashPageSize comes from a #define in a .h file in the firmware port,
    // through cmake 'flash_page_size' property on the MCUPort target.   
    const pageCount = Math.ceil(image.byteLength / flashPageSize);
    setProgress(`Upload progres: 0/${pageCount}`);
    pages = [];
    activePageIndex = 0;

    for (let pageIdx = 0; pageIdx < pageCount; pageIdx++) {
      const pageStartIdx = pageIdx * flashPageSize;
      // slice won't extend past iterable's length, regardless of param: end. 
      const pageData = image.slice(pageStartIdx, pageStartIdx + flashPageSize);
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
    <form className="widget"
      aria-label="FW Update"
      onSubmit={handleSubmit}>
      <p className="widget-head">
        <span>FW Update</span>
        <button type="submit">Send Image</button>
      </p>
      <label>Choose an image file
        <input type="file" name="image-file" accept=".bin"
          onChange={handleFileChange} />
      </label>
      <p>{progress}</p>

    </form>
  )


}