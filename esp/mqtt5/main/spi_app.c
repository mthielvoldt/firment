
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "driver/gpio_filter.h"
#include "esp32s3/rom/crc.h"

#include "spi_app.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Please update the following configuration according to your HardWare spec /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef CONFIG_IDF_TARGET_ESP32
#define RCV_HOST HSPI_HOST
#else
#define RCV_HOST SPI2_HOST
#endif

// Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void my_post_setup_cb(spi_slave_transaction_t *trans)
{
  gpio_set_level(GPIO_CLEAR_TO_SEND, 1);
}

// Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans)
{
  gpio_set_level(GPIO_CLEAR_TO_SEND, 0);
}

// Configuration for the SPI bus
static spi_bus_config_t buscfg = {
    .mosi_io_num = GPIO_MOSI,
    .miso_io_num = GPIO_MISO,
    .sclk_io_num = GPIO_SCLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .data4_io_num = -1,
    .data5_io_num = -1,
    .data6_io_num = -1,
    .data7_io_num = -1,
    .max_transfer_sz = SPI_BUFFER_SZ_BYTES,
    .flags = 0,
    .intr_flags = 0,
    .isr_cpu_id = 0,
};

// Configuration for the SPI slave interface
static spi_slave_interface_config_t slvcfg = {
    .mode = 1, // (CPOL, CPHA) = (0,1) Clock is low when Sub enabled, data valid on second edge (falling).
    .spics_io_num = GPIO_CS,
    .queue_size = TRANSACTION_QUEUE_LEN,
    .flags = SPI_SLAVE_BIT_LSBFIRST,
    .post_setup_cb = my_post_setup_cb,
    .post_trans_cb = my_post_trans_cb,
};

// Configuration for the comm control lines
static const gpio_config_t clearToSendPin_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = BIT64(GPIO_CLEAR_TO_SEND),
};
static const gpio_config_t messagePendingPin_conf = {
  .intr_type = GPIO_INTR_DISABLE,
  .mode = GPIO_MODE_OUTPUT,
  .pin_bit_mask = BIT64(GPIO_MESSAGE_WAITING),
};

static int spiTxCount = 0;
static int lastRealMessagePosition = 0;

typedef struct buff_s
{
  char data[SPI_BUFFER_SZ_BYTES];
} buff_t;

// Memory blocks; all buffers point to various locations in these.
static WORD_ALIGNED_ATTR buff_t txBufs[NUM_TX_BUFFERS];
static WORD_ALIGNED_ATTR buff_t txNada = {0};
static WORD_ALIGNED_ATTR buff_t rxBufs[TRANSACTION_QUEUE_LEN] = {0};

// The transactions must be static - spi driver only keeps a pointer to them
static spi_slave_transaction_t transactions[TRANSACTION_QUEUE_LEN] = {0};

/** When we don't have a message to send (no message pending from web), we leave
 * the tx_buffer ptr pointing to txNada, and don't advance nextTxIndex.  We can
 * transmit from txNada over and over until we have something to send. In
 * contrast, rx_buffer must advance on every transaction so so we don't clobber
 * data rx data hasn't been processed.
 * Accordingly, rx_buffer is in lock-step with rxBufs, and tx_buffer has
 * an extra layer of indirection through txBufPtrs.*/

static struct txPreQueue_s
{
  uint32_t numWaiting;
  uint32_t readIdx;
} txPreQ = {0};

// Main application
esp_err_t initSpi(void)
{
  esp_err_t ret;

  for (int i = 0; i < TRANSACTION_QUEUE_LEN; i++)
  {
    transactions[i].length = MAX_TRANSACTION_LENGTH_BITS;
    transactions[i].rx_buffer = &rxBufs[i];
  };

  // Configure handshake line as output
  gpio_config(&clearToSendPin_conf);
  gpio_config(&messagePendingPin_conf);
  // Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
  gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

  gpio_glitch_filter_handle_t sck_filter;
  gpio_pin_glitch_filter_config_t sck_filter_config = {
      .clk_src = GLITCH_FILTER_CLK_SRC_DEFAULT,
      .gpio_num = GPIO_SCLK,
  };
  gpio_new_pin_glitch_filter(&sck_filter_config, &sck_filter);
  gpio_glitch_filter_enable(sck_filter);

  // gpio_glitch_filter_handle_t mosi_filter;
  // gpio_pin_glitch_filter_config_t mosi_filter_config = {
  //     .clk_src = GLITCH_FILTER_CLK_SRC_DEFAULT,
  //     .gpio_num = GPIO_MOSI,
  // };
  // gpio_new_pin_glitch_filter(&mosi_filter_config, &mosi_filter);
  // gpio_glitch_filter_enable(mosi_filter);

  // Initialize SPI slave interface
  ret = spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);

  return ret;
}

bool sendMessage(uint8_t *toSend)
{
  bool success = false;
  if (txPreQ.numWaiting < TX_PREQUEUE_LEN)
  {
    // Size must be a multiple of 2 (for 16-bit CRC).  Pad with byte if needed.
    uint32_t crcPosition = getCRCPosition(toSend);
    uint16_t crc = crc16_le(0x00, (uint8_t*)toSend, crcPosition);

    uint32_t writeIdx = txPreQ.readIdx + txPreQ.numWaiting;
    if (writeIdx >= NUM_TX_BUFFERS)
      writeIdx -= NUM_TX_BUFFERS;
    
    *(uint16_t*)(&txBufs[writeIdx].data[crcPosition]) = crc;
    memcpy(&txBufs[writeIdx], toSend, crcPosition);
    txPreQ.numWaiting++;

    lastRealMessagePosition = txPreQ.numWaiting + TRANSACTION_QUEUE_LEN;
    gpio_set_level(GPIO_MESSAGE_WAITING, 1);

    success = true;
  }
  return success;
}

const void *getNextTxBuffer(void)
{
  const void *nextTxBuffer;

  if (txPreQ.numWaiting > 0)
  {
    nextTxBuffer = &txBufs[txPreQ.readIdx];
    txPreQ.numWaiting--;
    if (++txPreQ.readIdx >= NUM_TX_BUFFERS)
      txPreQ.readIdx = 0;
  }
  else
  {
    nextTxBuffer = &txNada;
  }
  return nextTxBuffer;
}

esp_err_t addToTransactionQueue(void)
{
  static uint32_t nextTransIndex = 0;

  // prep the next transaction.
  spi_slave_transaction_t *nextTransaction = &transactions[nextTransIndex];
  nextTransaction->tx_buffer = getNextTxBuffer();

  // try to enqueue it.  If this fails, stop trying.
  esp_err_t ret = spi_slave_queue_trans(RCV_HOST, nextTransaction, 0);
  if (ret == ESP_OK)
  {
    if (++nextTransIndex == TRANSACTION_QUEUE_LEN)
      nextTransIndex = 0;
  }
  return ret;
}

/** Enables the SPI slave interface to send the txBufs and receive into rxBufs.
 * However, it will not actually happen until the main device starts a hardware
 * transaction by pulling CS low and pulsing the clock. */
esp_err_t waitForSpiRx(uint8_t *rxMsg, uint32_t msTimeout)
{
  spi_slave_transaction_t *rxdTransaction;
  static uint32_t numTransactionsQueued = 0;

  while (numTransactionsQueued < TRANSACTION_QUEUE_LEN)
  {
    if (addToTransactionQueue() == ESP_OK)
      numTransactionsQueued++;
    else
      break; // If here, we tried to add to a full transaction queue: (an error)
  }

  // The timeout is important so lower priority tasks can breathe when there isn't data.
  volatile esp_err_t ret =
      spi_slave_get_trans_result(RCV_HOST, &rxdTransaction, pdMS_TO_TICKS(msTimeout));

  if (ret == ESP_OK)
  {
    // Regardless of CRC/Length, we lost an item from the transaction queue.
    numTransactionsQueued--;

    if (--lastRealMessagePosition <= 0) {
      gpio_set_level(GPIO_MESSAGE_WAITING, 0);
    }

    bool crcGood = true; // placeholder.
    /* Fmt SPI uses a fixed-length scheme that always transmits the max length*/
    bool lenGood = rxdTransaction->trans_len == EXPECTED_TRANS_LEN_BITS;
    if (!lenGood)
      ret = ESP_ERR_INVALID_SIZE;
    else if (!crcGood)
      ret = ESP_ERR_INVALID_CRC;
    else if (rxdTransaction->rx_buffer < &rxBufs || rxdTransaction->rx_buffer > &rxBufs[TRANSACTION_QUEUE_LEN - 1])
      ret = ESP_ERR_INVALID_RESPONSE;
    else // All good.
    {
      spiTxCount++;
      memcpy(rxMsg, rxdTransaction->rx_buffer, rxdTransaction->trans_len >> 3);
    }
  }
  return ret;
}
