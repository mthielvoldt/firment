# Project structuring
6. gpio_common.h (fix + move + test)
12. break fmt_rx -> project_comms dependency. 
13. move USE_xxx to firmentConfig

## Consolidating HW-data
Consolidate HW-specifc values into a single choke-point in port/ Cmake files.
Inputs: 
- config.cmake containing: set(VAR "") for project-discretion items.
- MCU headers parsed in cmake - header *selection* in port CML only (not .h)
- config.json parsed in cmake (for ui-shareables?)

Outputs: 
- Configured .h for firmware  (multiple, or single?)
- Configured .ts (or .json?) for ui.

1. flash_page_size: HW.h (parse) > {fmt_flash_port.h, web-ui config'd file}
7. move SOT for all project-discretion build-time items to firmentConfig.cmake
  - firment_msg_config.json, ghostProbe, waveform configs. 
9. make waveform and ghost probe opt-in using firmentConfig.cmake

## Making HW-data test-settable
2. fmt_flash_port.h defines -> something adjustable by tests.
8. firmentConfig.cmake sector size. 
10. ISR_Config.h items. 

## Code smells
3. host/RTE_Device.h move RTE_IOC_t typedef to fmt_ioc.h
14. gpio in project_comms.h = feature envy.

# New Tests
4. host/core_cm4.h spy on NVIC priority functions.
  - Test valid use in consuming inits, eg. fmt_initSpi().
5. host/cmsis_gcc.h exploding fake for __BKPT. 
11. .proto to provide one of each supported type of message and field.