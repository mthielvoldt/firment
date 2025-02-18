import { useState } from "react";

export default function Image({}) {
  // const [progressState, setProgressState] = useState("");
  const [image, setImage] = useState("");

  function handleSubmit(e: React.FormEvent) {
    e.preventDefault();
    console.log(image);
    
  }
  async function handleFileChange(e: React.ChangeEvent<HTMLInputElement> ) {
    e.preventDefault();
    if (e.currentTarget.files) {
      const imageData = await e.currentTarget.files[0].text();
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
        <input type="file" name="image-file" accept=".hex" 
          onChange={handleFileChange}/>
      </label>

    </form>
  )


}