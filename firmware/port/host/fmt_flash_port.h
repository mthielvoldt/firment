
// TODO: (1) configure these values in cmake to consolidate SOT.
// TODO: (2) make both these values overridable by tests.
#define FLASH_PAGE_SIZE 256U

#ifndef FMT_IMAGE_DOWNLOAD_SECTOR_SIZE
// 256kB
#define FMT_IMAGE_DOWNLOAD_SECTOR_SIZE  0x40000
#endif
