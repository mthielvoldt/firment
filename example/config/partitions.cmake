# This file sets where in program memory (flash) the code will be stored at 
# the various lifecycle stages (downloading, active-service, last-good-backup) 
#
# These values will be used to configure partitions.h, used by the bootloader.
#
# "DIRECT" implies non-cached access, which must be used for program/erase.
# In MCUs without Pmem cache, "DIRECT" and "CACHED" values will be the same.

set(PARTITION_SIZE              0x40000) # 512kB
set(SECTOR_SIZE                 0x40000) # 256kB (5.5s erases)
set(PMEM_ROOT_ADDRESS_DIRECT        0x0C000000) # ARCH_FLASH_OFFSET_DIRECT
set(PMEM_ROOT_ADDRESS_CACHED        0x08000000)
set(PARTITION_ACTIVE_ADDRESS_DIRECT 0x0C040000) # sector 9
set(PARTITION_ACTIVE_ADDRESS_CACHED 0x08040000) # for linker
set(PARTITION_UPDATE_ADDRESS        0x0C080000) # sector 10
set(PARTITION_BACKUP_ADDRESS        0x0C0C0000) # sector 11

# Arm MCUs often have alignment requirements for the vector table.
# Refer to for Vector Table Offset Register (VTOR) section in MCU reference.
# IMAGE_HEADER_SIZE must be a multiple of the VT alignment requirement.  
set(IMAGE_HEADER_SIZE 1024)