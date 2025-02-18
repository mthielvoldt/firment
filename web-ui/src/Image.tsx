import { useState } from "react";
import flashPageSize from "./generated/flashPage"
import { sendMessage } from "./mqclient";

function sendPage(data: ArrayBuffer, pageIndex: number, pageCount: number) {
  // TODO: get max payload size from firment_msg_config.json: image-part-max-size
  const imagePartMaxSize = 32;

  const chunkCount = Math.ceil(data.byteLength / imagePartMaxSize);
  let dataIdx = 0;
  for (let chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++ ) {
    dataIdx = chunkIndex * imagePartMaxSize;
    const payload = new Uint8Array(data.slice(dataIdx, dataIdx + imagePartMaxSize));
    console.log("payload", payload);


    sendMessage("ImageData", {
      pageIndex,
      pageCount,
      chunkIndex,
      chunkCountInPage: chunkCount,
      payload,
    }, false)
  }
}

export default function Image({}) {
  const [progress, setProgress] = useState("");
  const [image, setImage] = useState(new ArrayBuffer());

  async function  handleSubmit(submitEvent: React.FormEvent) {
    submitEvent.preventDefault();
    
    const imageBytes = new Uint8Array(image);
    console.log(imageBytes.slice(0, 512));

    // flashPageSize comes from a #define in a .h file in the firmware port,
    // through cmake 'flash_page_size' property on the MCUPort target.   
    const pageCount = Math.ceil(image.byteLength / flashPageSize);
    setProgress(`Upload progres: 0/${pageCount}`);

    for (let pageIdx = 0; pageIdx < 1; pageIdx++) {
      const pageStartIdx = pageIdx * flashPageSize;
      // slice won't extend past iterable's length, regardless of param: end. 
      const pageData = image.slice(pageStartIdx, pageStartIdx + flashPageSize);
      sendPage(pageData, pageIdx, pageCount);

      // wait for page status.
      // const status = await getPageStatus();
      // if (status == WRITE_SUCCESS) {
      //   setProgressState(`Upload progres: ${pageIdx}/${pageCount}`);
      // }
      // else if (status == WRITE_FAIL) {
      //   setProgressState(`Upload Failed at page ${pageIdx}`);
      // }
    }

    setProgress("Upload complete.");

  }
  async function handleFileChange(e: React.ChangeEvent<HTMLInputElement> ) {
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
          onChange={handleFileChange}/>
      </label>
      <p>{progress}</p>

    </form>
  )


}