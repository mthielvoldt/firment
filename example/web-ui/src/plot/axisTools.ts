/** Finds the greatest number that's less than the input that is in the set:
   * {1, 2, 5, 10, 20, 50, 100, 200, 500 ...}  (1,2,or 5 * 10^N).  N a whole #
   * Examples: input=>return 777=>500  45=>20
   */
export function getNearestRoundNumber(input: number) {
  input = (input > 0) ? input : -input;
  let output = 1;

  while (input > 10) {
    input /= 10;
    output *= 10;
  }
  if (input > 5) {
    output *= 5;
  }
  else if (input > 2) {
    output *= 2;
  }
  return output;
}