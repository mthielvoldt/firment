
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/gpio_filter.h"
#include "esp32s3/rom/crc.h"

#include "fmt_esp_uart.h"
#include <fmt_uart_frame.h>

static volatile bool packetReady = false;

static void setPacketReady(const uint8_t *rxPacket);

esp_err_t initUart(void)
{
  /* Configure parameters of an UART driver,
   * communication pins and install the driver */
  uart_config_t uart_config = {
      .baud_rate = FMT_UART_BAUD,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };
  ESP_ERROR_CHECK(uart_param_config(FMT_UART_NUM, &uart_config));

  ESP_ERROR_CHECK(uart_set_pin(FMT_UART_NUM,
                               FMT_GPIO_UART_TX,
                               FMT_GPIO_UART_RX,
                               UART_PIN_NO_CHANGE,
                               UART_PIN_NO_CHANGE));

  const int intr_alloc_flags = 0, event_q_size = 0;
  ESP_ERROR_CHECK(uart_driver_install(FMT_UART_NUM,
                                      FMT_UART_RX_BUFF_SZ,
                                      FMT_UART_TX_BUFF_SZ,
                                      event_q_size, NULL, intr_alloc_flags));

  setPacketReadyCallback(setPacketReady);
  return ESP_OK;
}

bool uart_hasData(void)
{
  size_t rxDataLength = 0;
  ESP_ERROR_CHECK(uart_get_buffered_data_len(FMT_UART_NUM, &rxDataLength));
  return (rxDataLength > 0);
}

bool sendPacketUart(uint8_t *packet, size_t size)
{
  const uint8_t startCode[] = {START_CODE};
  int ret0 = uart_write_bytes(FMT_UART_NUM, startCode, START_CODE_SIZE);
  int ret1 = uart_write_bytes(FMT_UART_NUM, packet, size);
  return ((ret0 > 0) && (ret1 > 0));
}

esp_err_t waitForUartRx(uint8_t *buffer, uint32_t timeout_ms)
{
  uint8_t startCodeBuff[START_CODE_SIZE];
  uint8_t *dest;
  rxParams_t nextRx = getStartCode();
  packetReady = false;

  while (!packetReady)
  {
    dest = (!nextRx.position) ? startCodeBuff : buffer - START_CODE_SIZE;

    int result = uart_read_bytes(FMT_UART_NUM, dest + nextRx.position,
                                 nextRx.length, pdMS_TO_TICKS(timeout_ms));

    if (result == nextRx.length)
    {
      nextRx = handleRxSegment(dest); // calls setPacketReady when packet done.
    }
    else
    {
      return ESP_ERR_TIMEOUT;
    }
  }
  return ESP_OK;
}

void setPacketReady(const uint8_t *packet)
{
  packetReady = true;
}
