#ifndef spi_mcuDetails_H
#define spi_mcuDetails_H

#include <comm_pcbDetails.h>  // FMT_USES_<transport>

#ifdef FMT_USES_SPI
#define FMT_BUILTIN_CRC 1
#else
#define FMT_BUILTIN_CRC 0
#endif


#endif // spi_mcuDetails_H