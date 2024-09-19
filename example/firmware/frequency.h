/** System frequencies and periods
 * 
 * Conventions: 
 * - All frequencies in Hz
 * - "TICKS" represent a duration as measured in ticks of a configured timer.
 */

#define SYS_FREQ 144000000ul

#define PRESCALE_128_FREQ (SYS_FREQ / 128u)
#define PRESCALE_128_TICKS_IN_1MS (PRESCALE_128_FREQ / 1000u)
#define PRESCALE_128_TICKS_IN_10MS (PRESCALE_128_FREQ / 100u)

#define WAVE_UPDATE_FREQ 1000.0F