#include <CppUTest/TestHarness.h>

extern "C"
{
#include <fmt_comms.h>
#include <fmt_uart.h>
#include <fmt_crc.h>
#include <comm_test.h>
#include <pb_encode.h>
}

#define COUNT_SOME 5
#define COUNT_MORE_THAN_SOME 15
#define COUNT_ALOT 500
#define COUNT_MORE_THAN_ALOT 1000

extern ARM_DRIVER_USART Driver_UART0;
extern FMT_DRIVER_CRC Driver_CRC0; // Need to worry about concurrent access?

TEST_GROUP(fmt_uart)
{
  bool initSuccess = true;
  const uartCfg_t cfg = {
      .driverId = 0,
      .driver = &Driver_UART0, // provided by comm_test_helper.h
      .baudHz = 112500,
      .irqPriority = 4,
  };
  void setup()
  {
    fmt_startTxChain = uart_startTxChain;
    fmt_linkTransport = uart_linkTransport;
    initSuccess = fmt_initUart(&cfg);
    initSuccess = initSuccess && fmt_initComms();
  }
};

TEST(fmt_uart, initSucceeds)
{
  CHECK_TRUE(initSuccess);
}

// TEST(fmt_uart, )
// {

// }