#ifndef fmt_esp_uart_H
#define fmt_esp_uart_H

#include <esp_err.h>
#include <fmt_sizes.h>
#include <stdbool.h>

#define FMT_UART_BAUD 115200
#define FMT_UART_NUM UART_NUM_1

#define FMT_GPIO_UART_TX 5
#define FMT_GPIO_UART_RX 4

#define FMT_MSG_COUNT_PER_MQTT_MAX 10
#define FMT_UART_TX_BUFF_SZ (FMT_MSG_COUNT_PER_MQTT_MAX * MAX_PACKET_SIZE_BYTES)

#define FMT_UART_RX_BUFF_SZ \
  (UART_HW_FIFO_LEN(UART_NUM_1) + MAX_PACKET_SIZE_BYTES) // HW_FIFO_LEN is 128

esp_err_t initUart(void);
bool uart_hasData(void);
esp_err_t waitForUartRx(uint8_t *buffer, uint32_t timeout_ms);

bool sendPacketUart(uint8_t *packet, size_t size);

#endif // fmt_esp_uart_H