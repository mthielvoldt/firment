#include "spi_slave.h"

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
