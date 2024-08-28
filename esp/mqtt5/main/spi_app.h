#include "esp_err.h"

#define GPIO_HANDSHAKE 2
#define GPIO_CS 10
#define GPIO_MOSI 11
#define GPIO_MISO 12
#define GPIO_SCLK 13
#define SPI_BUFFER_SZ_BYTES 32
#define MAX_TRANSACTION_LENGTH (SPI_BUFFER_SZ_BYTES * 8)

// #define EXPECTED_VALUE 0x01248EDB7FAA5566
#define EXPECTED_VALUE 0xA5u //24u

#define EXPECTED_LEN 8
#define SAMPLES_PER_REPORT 5

esp_err_t initSpi(void);
esp_err_t waitForSpiRx(uint32_t msTimeout);
