# Overview
Provides remote probing of variables in firmware over the fmt_comms link.  Probing can be either: 
- streamed continuously at sample-rates below the comms link bandwidth.
- snapshots at higher sample-rates

Both continuous streaming and snapshots may be run simultaneously.

## Test Points
Each variable to be probed is registered as a "test point" with a unique:
 - TestPointId
 - type, 
 - static source address
 - converter function (optional)

## runScanCtl 
The runScanCtl message requests which test points to sample and a scan frequency.

# Snapshots
Snapshots are designed to capture variable values across events of interest by continuously scanning the requested test points into a circular buffer.  When a trigger event happens, scanning is scheduled to stop after a fixed number of remaining samples.  The number of remaining samples to be taken before stopping depends on the buffer size and the window factor.  

## Window Factor
Type: float.  Valid range is [0.0, 1.0].
0.0 places the trigger event at the left of the window, showing only what happens *after* the event.  
1.0 places the trigger event at the right, showing only what happened *before* the event.  

## States
Snapshots are independent of streaming.  The snapshot process can be in the following states:
- No Request: initial state, no test points requested, not scanning. 
- Ongoing Scan: scanning, awating trigger. 
- Finishing Scan: trigger recorded, scanning to fill post-event buffer allotment.
- Sending Buffer: not scanning, awaiting full buffer to be sent out.

Once finished Sending Buffer, the state machine returns to Ongoing Scan.  




```
write index at trigger time:    v
buffer:                      =======
