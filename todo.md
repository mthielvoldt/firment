# Doing 
1. flash_page_size: Excise from Web-ui, replacing with ESP buffer size.

# Project structuring
12. break fmt_rx -> project_comms dependency? (why?)

## Consolidating HW-data
Consolidate HW-specifc values into a single choke-point in port/ Cmake files.
Inputs: 
- config.cmake containing: set(VAR "") for project-discretion items.
- MCU headers parsed in cmake - header *selection* in port CML only (not .h)
- config.json parsed in cmake (for ui-shareables?)

Outputs: 
- Configured .h for firmware  (multiple, or single?)
- Configured .ts (or .json?) for ui.

7. move SOT for all project-discretion build-time items to firmentConfig.cmake
  - firment_msg_config.json, ghostProbe, waveform configs. 
9. make waveform and ghost probe opt-in using firmentConfig.cmake

## Making HW-data test-settable
2. fmt_flash_port.h defines -> something adjustable by tests.
8. firmentConfig.cmake sector size. 
10. ISR_Config.h items. 

## Code smells

15. fmt_spi implements too much. Break some of this out.
  - CRC checks
  - message serialization
  - queue management (drop counters)

# Performance
12. Make a more performant interface for GPIO.

# New Tests
4. host/core_cm4.h spy on NVIC priority functions.
  - Test valid use in consuming inits, eg. fmt_initSpi().
5. host/cmsis_gcc.h exploding fake for __BKPT. 
11. .proto to provide one of each supported type of message and field.