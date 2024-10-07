#include "esp_err.h"

#define GPIO_HANDSHAKE 2
#define GPIO_CS 10
#define GPIO_MOSI 11
#define GPIO_MISO 12
#define GPIO_SCLK 13
#define SPI_MAX_FRAME_SZ 64
#define SPI_BUFFER_SZ_BYTES (SPI_MAX_FRAME_SZ + 4) // extra 4 to detect errors
#define MAX_TRANSACTION_LENGTH (SPI_BUFFER_SZ_BYTES * 8)
#define SPI_QUEUE_LEN 4
#define TARGET_TX_QUEUE_DEPTH (SPI_QUEUE_LEN / 2)

// #define EXPECTED_VALUE 0x01248EDB 7FAA5566
#define EXPECTED_VALUE 0x01378EDBu //24u

#define EXPECTED_TRANS_LEN_BITS (SPI_MAX_FRAME_SZ * 8)

// These are only used for testing SPI
#define SAMPLES_PER_REPORT 5000
#define MAX_BAD_DAT_TO_STORE 10U

esp_err_t initSpi(void);
esp_err_t waitForSpiRx(uint8_t *rxWord, uint32_t msTimeout);
