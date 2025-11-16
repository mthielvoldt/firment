/** System frequencies and periods
 * 
 * Conventions: 
 * - All frequencies in Hz
 * - "TICKS" represent a duration as measured in ticks of a configured timer.
 */

#define PERIODIC_A_FREQ 1000U
#define PERIODIC_A_PERIOD_US (1000000U / PERIODIC_A_FREQ)
#define WAVE_UPDATE_FREQ 1000.0F
#define GP_SNAP_FREQ 1000U
#define GP_STREAM_FREQ 200U

#define GP_STREAM_DIVIDER (PERIODIC_A_FREQ / GP_STREAM_FREQ )
