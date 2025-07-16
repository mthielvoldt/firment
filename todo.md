# Doing 
- Config STM clock in BL so calcs are fast. 
- Look into using higer-mem versions of Auth checks. 

# Flash
- Remove erase from interface, letting write handle pre-erase. 
  - affects BL, ST, XMC
  - Requires partitions to align with erase-blocks
- Get fast flash program working on ST. 

# Project structuring
- break fmt_rx -> project_comms dependency? (why?)

## Consolidating HW-data
Consolidate HW-specifc values into a single choke-point in port/ Cmake files.
- Inputs: 
  - config.cmake containing: set(VAR "") for project-discretion items.
  - MCU headers parsed in cmake - header *selection* in port CML only (not .h)
  - config.json parsed in cmake (for ui-shareables?)
- Outputs: 
  - Configured .h for firmware  (multiple, or single?)
  - Configured .ts (or .json?) for ui.

- move SOT for all project-discretion build-time items to firmentConfig.cmake
  - ghostProbe, waveform configs. 
- make waveform and ghost probe opt-in using firmentConfig.cmake

## Making HW-data test-settable
- firmentConfig.cmake sector size. 

- ISR_Config.h items. 

## Code smells

- fmt_spi implements too much. Break some of this out.
  - CRC checks
  - message serialization
  - queue management (drop counters)

- fmt_sizes.h needs naming attention. -> fmt_message_geometry.h. Excise PRIORITY. 

# Performance
- GPIO perfornace (idea is to alias dereferences, all lookup moving to init)

# New Tests
- HW tests for fmt_flash_write()
- host/core_cm4.h spy on NVIC priority functions.
  - Test valid use in consuming inits, eg. fmt_initSpi().
- host/cmsis_gcc.h exploding fake for __BKPT. 
- .proto to provide one of each supported type of message and field.