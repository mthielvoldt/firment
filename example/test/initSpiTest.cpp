#include <CppUTest/TestHarness.h>

extern "C"
{
  #include <init_transport.h>
}

TEST_GROUP(spi_example)
{
};


TEST(spi_example, initSpi)
{
  // Tests that the example config's init function succeeds.
  CHECK_TRUE(project_initTransport());
}