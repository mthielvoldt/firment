/** An alternate solution to the PlotLabels React component.
 * Not currently finished or in-use.
 */

interface LabelerConfig {
  textStyles: "";
};

export class PlotLabeler {
  // private rect: DOMRect;
  private labelDivs: HTMLDivElement[];
  private textStyle: string;

  constructor(div: HTMLDivElement, options?: LabelerConfig) {
    // this.rect = div.getClientRects()[0];
    this.textStyle = options ? options.textStyles : ""; 
    this.labelDivs = [];
    this.labelDivs.push(document.createElement("div"));
    this.labelDivs[0].className = "label-div"
    this.labelDivs[0].setAttribute("style", this.textStyle);
    const label0Text = document.createTextNode("Hey");


    this.labelDivs[0].appendChild(label0Text);
    div.appendChild(this.labelDivs[0]);
    console.log("PlotLabeler Constructor.");



  }

  public cleanup(): void {
    console.debug("Cleanup PlotLabeler");
    this.labelDivs.forEach((div) => {
      div.remove();
    })
  }
  
}