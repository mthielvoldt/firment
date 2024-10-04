/* SPI Slave example, receiver (uses SPI Slave driver to communicate with sender)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "driver/gpio_filter.h"

#include "spi_app.h"

/*
SPI receiver (slave) example.

This example is supposed to work together with the SPI sender. It uses the standard SPI pins (MISO, MOSI, SCLK, CS) to
transmit data over in a full-duplex fashion, that is, while the master puts data on the MOSI pin, the slave puts its own
data on the MISO pin.

This example uses one extra pin: GPIO_HANDSHAKE is used as a handshake pin. After a transmission has been set up and we're
ready to send/receive data, this code uses a callback to set the handshake pin high. The sender will detect this and start
sending a transaction. As soon as the transaction is done, the line gets set low again.
*/

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
  gpio_set_level(GPIO_HANDSHAKE, 1);
}

// Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans)
{
  gpio_set_level(GPIO_HANDSHAKE, 0);
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
    .queue_size = SPI_QUEUE_LEN,
    .flags = SPI_SLAVE_BIT_LSBFIRST,
    .post_setup_cb = my_post_setup_cb,
    .post_trans_cb = my_post_trans_cb,
};

// Configuration for the handshake line
static gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = BIT64(GPIO_HANDSHAKE),
};
static int spiTxCount = 0;
static WORD_ALIGNED_ATTR char txBufs[SPI_BUFFER_SZ_BYTES * SPI_QUEUE_LEN] = {};
static WORD_ALIGNED_ATTR char rxBufs[SPI_BUFFER_SZ_BYTES * SPI_QUEUE_LEN] = {};
static spi_slave_transaction_t spiTransactions[SPI_QUEUE_LEN] = {};

// Main application
esp_err_t initSpi(void)
{
  esp_err_t ret;

  for (int i = 0; i < SPI_QUEUE_LEN; i++)
  {
    spiTransactions[i] = (spi_slave_transaction_t){
        .length = MAX_TRANSACTION_LENGTH,
        .rx_buffer = &rxBufs + (i * SPI_BUFFER_SZ_BYTES),
        .tx_buffer = &txBufs + (i * SPI_BUFFER_SZ_BYTES),
    };
  }

  // Configure handshake line as output
  gpio_config(&io_conf);
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

esp_err_t waitForSpiRx(uint8_t *rxMsg, uint32_t msTimeout)
{
  static int32_t transmissionsInQueue = 0;
  static uint32_t nextTransIndex = 0;

  spi_slave_transaction_t *rxdTransaction = NULL;

  // Clear receive buffer, set send buffer to something sane
  // memset(rxBufs, 0x0, SPI_BUFFER_SZ_BYTES);
  // sprintf(txBufs, "This is the receiver, sending data for transmission number %04d.", spiTxCount++);

  /* This call enables the SPI slave interface to send/receive to the txBufs and rxBufs. The transaction is
  initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
  by pulling CS low and pulsing the clock etc. In this specific example, we use the handshake line, pulled up by the
  .post_setup_cb callback that is called as soon as a transaction is ready, to let the master know it is free to transfer
  data.
  */

  // (re)fill txQueue
  while (transmissionsInQueue < TARGET_TX_QUEUE_DEPTH)
  {
    if (spi_slave_queue_trans(RCV_HOST, &spiTransactions[nextTransIndex], 0) == ESP_OK)
    {
      transmissionsInQueue++;
      if (++nextTransIndex == SPI_QUEUE_LEN)
        nextTransIndex = 0;
    }
    else
    {
      break; // Getting here means the queue is full, so move on to processing Rx.
    }
  }

  // The timeout is important so lower priority tasks can breathe when there isn't data.
  esp_err_t ret = spi_slave_get_trans_result(RCV_HOST, &rxdTransaction, pdMS_TO_TICKS(msTimeout));
  if (ret == ESP_OK)
  {
    bool crcGood = true; // placeholder.
    /* Fmt SPI uses a fixed-length scheme that always transmits the max length*/
    bool lenGood = rxdTransaction->trans_len == EXPECTED_TRANS_LEN_BITS;
    if (!lenGood) 
      ret = ESP_ERR_INVALID_SIZE;
    else if (!crcGood)
      ret = ESP_ERR_INVALID_CRC;
    else // All good.
    {
      spiTxCount++;
      // subtract the num bytes we received (we transmitted at least this many)
      // TODO: this should be the num of bytes to transmit, not "transmissions"
      transmissionsInQueue -= rxdTransaction->trans_len;
      if (transmissionsInQueue < 0)
        transmissionsInQueue = 0;
      memcpy(rxMsg, rxdTransaction->rx_buffer, rxdTransaction->trans_len >> 3);
    }
  }
  return ret;
}

void checkForSPIErrors(spi_slave_transaction_t *rxdTransaction)
{
  static int datErrTotal = 0;
  static uint16_t badDat[MAX_BAD_DAT_TO_STORE] = {};
  static uint32_t badDati = 0;
  static int lenErrCount = 0;
  static int lenTotal = 0;
  static int lenMin = SAMPLES_PER_REPORT;
  static int lenMax = 0;
  static int setCount = 0;
  static bool firstSet = true;
  uint32_t *rxbuf = (rxdTransaction->rx_buffer);

  if (rxdTransaction->trans_len != EXPECTED_TRANS_LEN_BITS)
  {
    lenErrCount++;
  }
  else
  {
    if (*rxbuf != EXPECTED_VALUE)
    {
      datErrTotal++;
      if (badDati < MAX_BAD_DAT_TO_STORE)
      {
        badDat[badDati] = *rxbuf;
        badDati++;
      }
    }
  }

  if (spiTxCount >= SAMPLES_PER_REPORT) // If this passes, we Rx'd something, so rxbuf not NULL.
  {
    lenTotal += lenErrCount;
    setCount++;
    lenMax = (!firstSet && lenErrCount > lenMax) ? lenErrCount : lenMax;
    lenMin = (lenErrCount < lenMin) ? lenErrCount : lenMin;
    float avgLenErrPerSet = (float)lenTotal / setCount;
    printf("Set %d with %d\n", setCount, spiTxCount);
    printf("Current value: %04lX\tLen: %d\n", rxbuf[0], rxdTransaction->trans_len);
    printf(" Len errors-> max: %d\tmin: %d\tAvg: %f\tDat errors total: %d\n",
           lenMax, lenMin, avgLenErrPerSet, datErrTotal);

    if (badDati)
    {
      printf("Bad Data: ");
      for (uint32_t i = 0; i < badDati; i++)
      {
        printf("%02X ", badDat[i]);
      }
      printf("\n");
      badDati = 0;
    }

    spiTxCount = lenErrCount = 0;
    firstSet = false;
  }
}
