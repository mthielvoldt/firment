
export interface StatsText {
  name: string;
  min: string;
  max: string;
  ave: string;
};

export class Stats {
  name: string;
  min: number;
  max: number;
  count: number;
  private sum: number;

  constructor(name: string, initialData?: number[]) {
    this.name = name;
    this.count = 0;
    this.sum = 0;
    this.min = this.max = NaN;
    if (initialData) {
      this.update(initialData);
    }
  }

  update(newData: number[] | Float32Array) {
    this.count += newData.length;

    /** TS complains about invoking unions of functions when those functions 
     * have generic params (which reduce does).
     * https://stackoverflow.com/questions/56884065/typed-arrays-and-union-types */
    if (newData instanceof Float32Array) {
      this.sum = newData.reduce((sum, item) => sum + item, this.sum);
      this.min = newData.reduce((min, item) => Math.min(min, item), this.min || Infinity);
      this.max = newData.reduce((max, item) => Math.max(max, item), this.max || -Infinity);
    } else {
      this.sum = newData.reduce((sum, item) => sum + item, this.sum);
      this.min = newData.reduce((min, item) => Math.min(min, item), this.min || Infinity);
      this.max = newData.reduce((max, item) => Math.max(max, item), this.max || -Infinity);
    }
  }

  get ave() {
    return this.sum / this.count;
  }

  get text() {
    return {
      name: this.name,
      min: toText(this.min),
      max: toText(this.max),
      ave: toText(this.ave)
    };
  }
}

function toText(num: number) {
  const sigFigs = 3
  const nonExpString = num.toPrecision(sigFigs);
  const expString = num.toExponential(sigFigs - 1);

  // prefer non-exponential representation if they're equal length.
  // otherwise, take the shortest representation.
  return (nonExpString.length <= expString.length) ? nonExpString : expString;
}
