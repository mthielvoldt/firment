#include "esp_err.h"

#define GPIO_HANDSHAKE      2
#define GPIO_MOSI           11
#define GPIO_MISO           13
#define GPIO_SCLK           12
#define GPIO_CS             10


esp_err_t initSpi(void);
esp_err_t waitForSpiRx(uint32_t msTimeout);
