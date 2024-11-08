#include "esp_err.h"
#include <fmt_sizes.h>

#define GPIO_HANDSHAKE 9
#define GPIO_CS 10
#define GPIO_MOSI 11
#define GPIO_MISO 12
#define GPIO_SCLK 13

#define SPI_BUFFER_SZ_BYTES (MAX_PACKET_SIZE_BYTES + 4) // extra 4 to detect errors

#define TX_PREQUEUE_LEN 2 // bufs for Tx messages not yet in transaction queue
#define TRANSACTION_QUEUE_LEN 2 // each transaction claims a tx_buf and rx_buf
#define NUM_TX_BUFFERS (TRANSACTION_QUEUE_LEN + TX_PREQUEUE_LEN)
#define MAX_TRANSACTION_LENGTH_BITS (SPI_BUFFER_SZ_BYTES * 8)
#define EXPECTED_TRANS_LEN_BITS (MAX_PACKET_SIZE_BYTES * 8)

// These are only used for testing SPI
#define SAMPLES_PER_REPORT 5000
#define MAX_BAD_DAT_TO_STORE 10U

esp_err_t initSpi(void);

/** Copies toSend buffer to an outbound Tx queue, appending a CRC suffix.
 * @param toSend toSend[0] must indicate the length of the subsequent data.*/
bool sendMessage(uint8_t *toSend);
esp_err_t waitForSpiRx(uint8_t *rxWord, uint32_t msTimeout);
