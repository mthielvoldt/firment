# Project structuring

## Consolidating HW-data
Consolidate HW-specifc values into a single choke-point in port/ Cmake files.
Inputs: 
- config.cmake containing: set(VAR "")
- MCU headers parsed in cmake - header *selection* in port CML only (not .h)
- config.json parsed in cmake (for ui-shareables?)

Outputs: 
- Configured .h for firmware  (multiple, or single?)
- Configured .ts (or .json?) for ui.

1. flash_page_size: HW.h (parse) > {fmt_flash_port.h, web-ui config'd file}

