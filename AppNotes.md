# Common Message Schemes
Several message schemes are common in embedded systems.  Here we get a sense of how Firment supports them, and which it supports more easily. 

## Contextual Command-Response
- A Packet is defined with a metadata header that might include:
  - Start key (a special byte sequence)
  - Length, possibly of the payload or the whole packet.
  - Command / Response ID that routes this packet to actions. 
  - Address or Selector for identifying the target of the action.
  - Index for identifying this packet's number in a longer sequence.
  - Checksum or CRC for error checking the packet.

In a thread separate from the GUI's main thread:
- Wait for new data to be received
- Look for start key (or other signal of being in sync with sender)
- Assemble enough of the packet to parse the length.
- Assemble the remainder of the packet.
- Validate with Checksum.
- Process the message.

Processing the received message (the last step above) often involves first checking that the message matches a command that was recently sent.  In some command-response schemes, the response can *only be decoded* in the context of the command that requested it.  Accordingly, the command sender must keep a record of the command(s) that is/are still awaiting responses and match response to command.  Some of these schemes gracefully handle out-of-order responses, or irrelevant messages, some do not. 

Firment provides for each message to be fully described by the .proto file(s) so no additional context is needed for decoding.  In other words, you'll never need to know which "command" an incoming message "responds to" just for decode.  You may still decide to keep a record of sent command(s) or the expected response in order to validate the subordinate processor received (and possibly acted correctly upon) the command.  The next section discusses some built-in options for this. 

# Retry Schemes
Inter-processor comms is a dangerous journey for data: Signal degradation, bad connectors and pre-occupied CPUs all take a swipe at our messages.  If a command is important, we need to provide some resilience to it, and often the only thing we can do is try resending it.  

In command-response approaches, the "awaiting response" queue facilitates retries.  If the next message you receive doesn't match (in some way) the command that's waiting, fire the command again.

But since we don't need to match responses to command for context-based decoding, we're free to employ different retry solutions too.


## Shouting into Void
Perhaps the simplest retry scheme is to always re-send commands, forever.  I call this "Shouting into Void".  "Persistent commands" might be a more charitable name.

### Upsides
- No need to acknowledge (ACK) or wait for ACKs, which simplifies app code.
- Inherently corrects Sub/Controller state misalignment - no state alignment checks needed.
- De-couples Controller and Sub code (is a Sub even present? Who cares!)

### Downsides
- Consumes bandwidth.
- Less compatible with power-saving designs that sleep for long periods. 
- Bloats log files with meaningless data.
- Covers up evidence of Sub MCU resets (often a HW problem).
- No inherent detection of commands being ignored. 
- Upsides only apply if Controller doesn't need knowledge of Sub's state for other reasons.

### Implementation
To pull this off, just send the desired commands in a periodic function.  These commands must be idempotent.  It is possible to send some commands this way, and use other schemes for different commands.

## One Command at a Time (OCaaT)
In this scheme, after sending a command, the controller waits for a specific response before sending another command.  Because every command blocks the next for some time, a pending-send queue is advisable so the Controller's applicaiton code can queue up several commands without waiting.

### Upsides
- Controller need not consider out-of-order responses. 
- No possibility of command send order getting shuffled by retries.

### Downsides
- All commands get blocked if any one is repeatedly mis-handled by the Sub.  Limiting the number of retries can provide an escape.
- Command bandwidth is reduced due to a network waterfall.  Delay depends on Sub's responsiveness.

Command re-sends can be triggered on either of two conditions:
1. The next message received is not the expected response.
    - **-** Intolerant of the Sub speaking unless answering the Controller.
    - **-** If Sub never sends a response, it never retries. 
    - **+** No timeout handling code required.
2. Timeout. 
    - **+** Tolerant of the Sub advertising things, such as periodic telemetry.
    - **-** Requires setting, checking, handling timeouts.
    - Worst case for blocking command queue: `timeout-time * max-resend-count`.


## Expected Response Queue
This is similar to the "commands awaiting response" queue, except what is queued is the message we expect to follow a command, and a time limit for receiving it.  Each time we receive a message, we iterate through our expected-response list for a match.  If we find a match, we remove that expected response.  Every tick, we evaluate whether any expectations have timed-out.  When that happens, we clear the original expectation and re-send the command, which creates a new expectation with a new timeout time.

### Upsides
- Command transmission is not blocked on round-trips.  Bandwidth is beter utilized.
- No need for special "ACK" messages - just the normal messages that would indicate proper command execution. 

### Downsides
- Violates separation-of-concerns.  The transport is now checking the Sub's execution logic.  Re-sending commands may not be the right tool to fix the problem. 
- Re-sends potentially shuffle the order of commands, clobbering fresh with stale.
- To get around the previous downside, we could scan the expected response queue for those that match the *type* of expected response for the presently considered command.  Those that match, we could remove as stale.  However! to scan for type match, we must decode the expected responses.  Further, if the response message has several fields, the situation looks even worse.

## CRC Acks
