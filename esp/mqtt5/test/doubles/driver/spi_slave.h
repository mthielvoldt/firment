#include <stdint.h>
#include <stddef.h>
#include "../esp_err.h"

#define WORD_ALIGNED_ATTR __attribute__((aligned(4)))

#define SPI_SLAVE_TXBIT_LSBFIRST (1 << 0)                                            ///< Transmit command/address/data LSB first instead of the default MSB first
#define SPI_SLAVE_RXBIT_LSBFIRST (1 << 1)                                            ///< Receive data LSB first instead of the default MSB first
#define SPI_SLAVE_BIT_LSBFIRST (SPI_SLAVE_TXBIT_LSBFIRST | SPI_SLAVE_RXBIT_LSBFIRST) ///< Transmit and receive LSB first
#define SPI_SLAVE_NO_RETURN_RESULT (1 << 2)                                          ///< Don't return the descriptor to the host on completion (use `post_trans_cb` to notify instead)

typedef struct
{
  uint32_t flags;
  size_t length;
  size_t trans_len;
  const void *tx_buffer;
  void *rx_buffer;
  void *user;
} spi_slave_transaction_t;

typedef void (*slave_transaction_cb_t)(spi_slave_transaction_t *trans);

typedef struct
{
  int spics_io_num;
  uint32_t flags;
  int queue_size;
  uint8_t mode;
  slave_transaction_cb_t post_setup_cb;
  slave_transaction_cb_t post_trans_cb;
} spi_slave_interface_config_t;

typedef struct
{
  int mosi_io_num;
  int miso_io_num;
  int sclk_io_num;
  int quadwp_io_num;
  int quadhd_io_num;
  int data4_io_num;
  int data5_io_num;
  int data6_io_num;
  int data7_io_num;
  int max_transfer_sz;
  int flags;
  int intr_flags;
  int isr_cpu_id;
} spi_bus_config_t;

typedef enum
{
  // SPI1 can be used as GPSPI only on ESP32
  SPI1_HOST = 0, ///< SPI1
  SPI2_HOST = 1, ///< SPI2
#if SOC_SPI_PERIPH_NUM > 2
  SPI3_HOST = 2, ///< SPI3
#endif
  SPI_HOST_MAX, ///< invalid host value
} spi_host_device_t;

typedef enum
{
  SPI_DMA_DISABLED = 0, ///< Do not enable DMA for SPI
  SPI_DMA_CH_AUTO = 3,  ///< Enable DMA, channel is automatically selected by driver
} spi_common_dma_t;

typedef spi_common_dma_t spi_dma_chan_t;

esp_err_t spi_slave_initialize(
    spi_host_device_t host,
    const spi_bus_config_t *bus_config,
    const spi_slave_interface_config_t *slave_config,
    spi_dma_chan_t dma_chan);

esp_err_t spi_slave_queue_trans(
    spi_host_device_t host,
    const spi_slave_transaction_t *trans_desc,
    int ticks_to_wait);

esp_err_t spi_slave_get_trans_result(
  spi_host_device_t host, 
  spi_slave_transaction_t **trans_desc, 
  int ticks_to_wait);
