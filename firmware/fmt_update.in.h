/**
 * fw_update.h -- Generated file.  See fw_update.in.h
 * Provides address where new images should be written (so BL will see them).
 *   This comes from CMake, as it is shared understanding with the bootloader.
 */

#include "messages.pb.h"
#include <stdint.h>

// Source: project-level .cmake (these values shared with bootloader).
#define FMT_IMAGE_DOWNLOAD_ADDRESS @FMT_IMAGE_DOWNLOAD_ADDRESS@
#define FMT_IMAGE_DOWNLOAD_SECTOR_SIZE @FMT_IMAGE_DOWNLOAD_SECTOR_SIZE@

// Source: firment_msg_config.json  "image-part-max-size"
#define IMAGE_PART_MAX_SIZE @IMAGE_PART_MAX_SIZE@

#define USE_ImageData
void handleImageData(ImageData msg);
