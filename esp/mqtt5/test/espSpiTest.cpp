#include <CppUTest/TestHarness.h>

extern "C"
{
#include "../main/fmt_esp_spi.h"
}

TEST_GROUP(spi_app)
{
  bool initSuccess = false;
  uint8_t validPacket[MAX_PACKET_SIZE_BYTES];

  void setup()
  {

    memset(validPacket, 0, sizeof(validPacket));
    initSuccess = true; // Placeholder for init call.

  }
  void teardown()
  {
  }

};

TEST(spi_app, init)
{
  CHECK_TRUE(initSuccess);
  FAIL("forced fail");
}