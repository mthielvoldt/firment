#include "esp_err.h"

#define GPIO_HANDSHAKE 2
#define GPIO_CS 10
#define GPIO_MOSI 11
#define GPIO_MISO 12
#define GPIO_SCLK 13
#define SPI_BUFFER_SZ_BYTES 32
#define MAX_TRANSACTION_LENGTH (SPI_BUFFER_SZ_BYTES * 8)
#define SPI_QUEUE_LEN 12
#define TARGET_TX_QUEUE_DEPTH 8

// #define EXPECTED_VALUE 0x01248EDB7FAA5566
#define EXPECTED_VALUE 0xA524u //24u

#define EXPECTED_LEN 16
#define SAMPLES_PER_REPORT 5000
#define MAX_BAD_DAT_TO_STORE 10U

esp_err_t initSpi(void);
esp_err_t waitForSpiRx(uint32_t msTimeout);
