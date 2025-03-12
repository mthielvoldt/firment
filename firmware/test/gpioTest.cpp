#include <CppUTest/TestHarness.h>

extern "C" {
  #include <fmt_gpio.h>
  #include <gpio_pcbDetails.h>
}

TEST_GROUP(fmt_gpio)
{
  void setup()
  {

  }
  void teardown()
  {

  }
};

TEST(fmt_gpio, initGpioInPin)
{
  CHECK_TRUE(fmt_initGpioInPin(LED_0_PIN_ID, INPUT_MODE_TRISTATE));
}

TEST(fmt_gpio, initGpioInPin_pinIdUnavailable_fails)
{
  CHECK_FALSE(fmt_initGpioInPin(255, INPUT_MODE_TRISTATE));
}

TEST(fmt_gpio, initGpioOutPin)
{
  CHECK_TRUE(fmt_initGpioOutPin(LED_0_PIN_ID, OUTPUT_MODE_PUSH_PULL));
}

TEST(fmt_gpio, initGpioOutPin_pinIdUnavailable_fails)
{
  CHECK_FALSE(fmt_initGpioOutPin(255, OUTPUT_MODE_PUSH_PULL));
}