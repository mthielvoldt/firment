#include <CppUTest/TestHarness.h>

extern "C" {
  #include <fmt_spi.h>
}

TEST_GROUP(fmt_spi)
{
  extern ARM_DRIVER_SPI spi3;
  void setup()
  {

  }
  void teardown()
  {

  }
};

TEST(fmt_spi, init)
{
  spiCfg_t cfg = {
    .spiModuleNo = 3,
    .spiModule = spi3,
    .msgWaitingIOC = ,
    .
  }
}