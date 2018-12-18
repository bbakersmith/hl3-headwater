#include "spi.c"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(atmega_spi);

TEST_SETUP(atmega_spi) {};
TEST_TEAR_DOWN(atmega_spi) {};

TEST(atmega_spi, test_16bit_to_low_high_bytes) {
  uint16_t data1 = 65535;
  TEST_ASSERT_EQUAL(0xFF, spi_get_high_byte(data1));
  TEST_ASSERT_EQUAL(0xFF, spi_get_low_byte(data1));

  uint16_t data2 = 0;
  TEST_ASSERT_EQUAL(0x00, spi_get_high_byte(data2));
  TEST_ASSERT_EQUAL(0x00, spi_get_low_byte(data2));

  uint16_t data3 = 43605;
  TEST_ASSERT_EQUAL(0xAA, spi_get_high_byte(data3));
  TEST_ASSERT_EQUAL(0x55, spi_get_low_byte(data3));
}

TEST_GROUP_RUNNER(atmega_spi) {
  RUN_TEST_CASE(atmega_spi, test_16bit_to_low_high_bytes);
}
