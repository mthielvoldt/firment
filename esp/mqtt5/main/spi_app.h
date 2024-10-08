#include "esp_err.h"

#define GPIO_HANDSHAKE 2
#define GPIO_CS 10
#define GPIO_MOSI 11
#define GPIO_MISO 12
#define GPIO_SCLK 13
#define SPI_MAX_FRAME_SZ 64
#define SPI_BUFFER_SZ_BYTES (SPI_MAX_FRAME_SZ + 4) // extra 4 to detect errors
#define TX_PREQUEUE_LEN 2 // bufs for Tx messages not yet in transaction queue
#define TRANSACTION_QUEUE_LEN 2 // each transaction claims a tx_buf and rx_buf
#define NUM_TX_BUFFERS (TRANSACTION_QUEUE_LEN + TX_PREQUEUE_LEN)
#define MAX_TRANSACTION_LENGTH_BITS (SPI_BUFFER_SZ_BYTES * 8)

// #define EXPECTED_VALUE 0x01248EDB 7FAA5566
#define EXPECTED_VALUE 0x01378EDBu //24u

#define EXPECTED_TRANS_LEN_BITS (SPI_MAX_FRAME_SZ * 8)

// These are only used for testing SPI
#define SAMPLES_PER_REPORT 5000
#define MAX_BAD_DAT_TO_STORE 10U

esp_err_t initSpi(void);
bool sendMessage(char *toSend, size_t size);
esp_err_t waitForSpiRx(char *rxWord, uint32_t msTimeout);
