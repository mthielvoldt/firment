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
};
// Configuration for the SPI slave interface
static spi_slave_interface_config_t slvcfg = {
    .mode = 0,
    .spics_io_num = GPIO_CS,
    .queue_size = 3,
    .flags = 0,
    .post_setup_cb = my_post_setup_cb,
    .post_trans_cb = my_post_trans_cb};
// Configuration for the handshake line
static gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = BIT64(GPIO_HANDSHAKE),
};
static int spiTxCount = 0;
static WORD_ALIGNED_ATTR char sendbuf[129] = "";
static WORD_ALIGNED_ATTR char recvbuf[129] = "";
static spi_slave_transaction_t spiTransaction = {
  .length = 1 * 8,
  .rx_buffer = recvbuf,
  .tx_buffer = sendbuf,
  // .flags = 0,
  // .trans_len = 0,
  // .user = NULL
};



// Main application
esp_err_t initSpi(void)
{
  esp_err_t ret;

  // Configure handshake line as output
  gpio_config(&io_conf);
  // Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
  gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
  gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

  // Initialize SPI slave interface
  ret = spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);

  return ret;
}

esp_err_t waitForSpiRx(uint32_t msTimeout)
{
  esp_err_t ret;

  // Clear receive buffer, set send buffer to something sane
  memset(recvbuf, 0xA5, 129);
  sprintf(sendbuf, "This is the receiver, sending data for transmission number %04d.", spiTxCount++);
  
  /* This call enables the SPI slave interface to send/receive to the sendbuf and recvbuf. The transaction is
  initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
  by pulling CS low and pulsing the clock etc. In this specific example, we use the handshake line, pulled up by the
  .post_setup_cb callback that is called as soon as a transaction is ready, to let the master know it is free to transfer
  data.
  */
  ret = spi_slave_transmit(RCV_HOST, &spiTransaction, pdMS_TO_TICKS(msTimeout));

  // spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
  // received data from the master. Print it.
  if (ret == ESP_OK) {
    printf("Received: %s\n", recvbuf);
  } else {
    printf("Timed out receiving SPI.");
  }
  return ret;
}
