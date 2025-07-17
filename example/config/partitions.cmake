# This file sets where in program memory (flash) the code will be stored at 
# the various lifecycle stages (downloading, active-service, last-good-backup) 
#
# These values will be used to configure partitions.h, used by the bootloader.
#
# Details
# ===========================================================================
# In partitions, "DIRECT" implies non-cached access, used for program/erase.
# In MCUs without Pmem cache, "DIRECT" and "CACHED" values will be the same.
#
# TEST_RESULT_ADDRESS
# To ensure new images are capable of downloading new FW images, the BL expects
# the application to download and write a nonce to flash at TEST_RESULT_ADDRESS
# the first time it it runs.  
# On subsequent boots, the BL will check for this nonce to be present,
# and will restore the backup image if the nonce is not present or invalid.
#
# IMAGE_HEADER_SIZE
# Arm MCUs often have alignment requirements for the vector table.  Refer to 
# Vector Table Offset Register (VTOR) section in MCU reference.
# Generally, the alignment required is the power of 2 that can contain the whole 
# vector table.  Example: table has beween 65-128 words: alignm to 128*4 = 512B.
# Regardless of VT alignment requirement, the minimum IMAGE_HEADER_SIZE is 256.

if(${MCU_VARIANT} STREQUAL "XMC4700")
  set(PARTITION_SIZE              0x40000) # 256kB
  set(SECTOR_SIZE                 0x40000) # 256kB (5.5s erases)
  set(PMEM_ROOT_ADDRESS_DIRECT        0x0C000000) # ARCH_FLASH_OFFSET_DIRECT
  set(PMEM_ROOT_ADDRESS_CACHED        0x08000000)
  set(PARTITION_ACTIVE_ADDRESS_DIRECT 0x0C040000) # sector 9
  set(PARTITION_ACTIVE_ADDRESS_CACHED 0x08040000) # for linker
  set(PARTITION_UPDATE_ADDRESS        0x0C080000) # sector 10
  set(PARTITION_BACKUP_ADDRESS        0x0C0C0000) # sector 11
  set(TEST_RESULT_ADDRESS             0x0C01C000) # sector 7 (last 16kB sector)
  set(IMAGE_HEADER_SIZE 1024)
elseif(${MCU_VARIANT} STREQUAL "stm32l476")
  set(PARTITION_SIZE              0x40000) # 256kB
  set(SECTOR_SIZE                 0x800) # 2kB (sector = erase block)
  set(PMEM_ROOT_ADDRESS_DIRECT        0x08000000) # bank 1
  set(PMEM_ROOT_ADDRESS_CACHED        0x08000000) # no cache; same as prev.
  set(PARTITION_ACTIVE_ADDRESS_DIRECT 0x08040000) # bank 1
  set(PARTITION_ACTIVE_ADDRESS_CACHED 0x08040000) # no cache; same as prev.
  set(PARTITION_UPDATE_ADDRESS        0x08080000) # bank 2
  set(PARTITION_BACKUP_ADDRESS        0x080C0000) # bank 2
  set(TEST_RESULT_ADDRESS             0x080FF800) # last page of flash
  set(IMAGE_HEADER_SIZE 512)
endif()
