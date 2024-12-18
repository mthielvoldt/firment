export function profile() {
  
  let a = Array.from({length: 1000}).map((_, index) => index);
  let c: number[] = [];
  const b = a.slice();
  const n = 10000;
  
  let startTime = Date.now();
  for (let i = 0; i < n; i++) {
    c = a.concat(b);
  }
  console.log("concat duration: ", Date.now() - startTime, "len(c): ", c.length);

  a = Array.from({length: 1000}).map((_, index) => index);
  startTime = Date.now();
  for (let i = 0; i < n; i++) {
    a = b.slice();
    a.push(...b);
  }
  console.log("push duration: ", Date.now() - startTime, "len(a): ", a.length);
}