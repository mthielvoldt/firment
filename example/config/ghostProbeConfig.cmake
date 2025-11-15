
set(ENABLE_GHOST_PROBE 1)
message(STATUS "Ghost Probe module enabled.")

set(GP_NUM_PROBES 4)  # Maximum is 10.
message(STATUS "${GP_NUM_PROBES} Ghost probes available.")

# Buffer size should be a multiple of sizeof(float) (4).
set(GP_BUFFER_SIZE 2048)
message(STATUS "Ghost Probe buffer size: ${GP_BUFFER_SIZE} bytes.")

