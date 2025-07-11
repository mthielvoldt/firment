/**
 * fw_update.h -- Generated file.  See fw_update.in.h
 * Provides address where new images should be written (so BL will see them).
 *   This comes from CMake, as it is shared understanding with the bootloader.
 */

#include "messages.pb.h"
#include <stdint.h>

// Source: project-level .cmake (these values shared with bootloader).
#define FMT_IMAGE_DOWNLOAD_ADDRESS @PARTITION_UPDATE_ADDRESS@
#define FMT_IMAGE_DOWNLOAD_PARTITION_SIZE @PARTITION_SIZE@

// Source: firment_msg_config.json  "data-msg-payload-size-max"
#define DATA_MSG_PAYLOAD_SIZE_MAX @DATA_MSG_PAYLOAD_SIZE_MAX@
#define UPDATE_PAGE_SIZE @UPDATE_PAGE_SIZE@

typedef void (*callback_t)(void);

/**
 * Set a callback to execute just before the first page-write to flash. 
 * Commonly used to save a message for a bootloader. 
 */
void fmt_setFirstPageReceivedCallback(callback_t onDownloadStart);

/**
 * Set a callback that executes once a new image is successfully fully saved.
 * Commonly used to trigger a bootloader action, and/or reset the CPU.
 */
void fmt_setDownloadFinishCallback(callback_t onDownloadComplete);

#define USE_ImageData
bool handleImageData(ImageData msg);
