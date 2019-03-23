#include "unity.h"
#include "unity_fixture.h"

#include "bytes.h"

TEST_GROUP(bytes);

TEST_SETUP(bytes) {};
TEST_TEAR_DOWN(bytes) {};

TEST(bytes, test_bytes_16bit_to_high_low) {
  uint16_t data1 = 65535;
  TEST_ASSERT_EQUAL(0xFF, bytes_16bit_to_high(data1));
  TEST_ASSERT_EQUAL(0xFF, bytes_16bit_to_low(data1));

  uint16_t data2 = 0;
  TEST_ASSERT_EQUAL(0x00, bytes_16bit_to_high(data2));
  TEST_ASSERT_EQUAL(0x00, bytes_16bit_to_low(data2));

  uint16_t data3 = 43605;
  TEST_ASSERT_EQUAL(0xAA, bytes_16bit_to_high(data3));
  TEST_ASSERT_EQUAL(0x55, bytes_16bit_to_low(data3));
}

TEST(bytes, test_bytes_high_low_to_16bit) {
  TEST_ASSERT_EQUAL(65535, bytes_high_low_to_16bit(0xFF, 0xFF));
  TEST_ASSERT_EQUAL(0, bytes_high_low_to_16bit(0x00, 0x00));
  TEST_ASSERT_EQUAL(43605, bytes_high_low_to_16bit(0xAA, 0x55));
}

TEST(bytes, test_bytes_set_bit) {
  uint8_t volatile value;

  value = 0;
  bytes_set_bit(&value, 3);
  TEST_ASSERT_EQUAL(8, value);

  value = 4;
  bytes_set_bit(&value, 5);
  TEST_ASSERT_EQUAL(36, value);
}

TEST(bytes, test_bytes_unset_bit) {
  uint8_t volatile value;

  value = 54;
  bytes_unset_bit(&value, 4);
  TEST_ASSERT_EQUAL(38, value);

  value = 3;
  bytes_unset_bit(&value, 4);
  TEST_ASSERT_EQUAL(3, value);
}

TEST(bytes, test_bytes_check_bit) {
  TEST_ASSERT_EQUAL(1, bytes_check_bit(0x04, 2));
  TEST_ASSERT_EQUAL(0, bytes_check_bit(0x08, 2));
  TEST_ASSERT_EQUAL(0, bytes_check_bit(0x02, 2));
  TEST_ASSERT_EQUAL(1, bytes_check_bit(0x26, 2));
  TEST_ASSERT_EQUAL(1, bytes_check_bit(0x26, 5));
  TEST_ASSERT_EQUAL(0, bytes_check_bit(0x06, 5));
  TEST_ASSERT_EQUAL(1, bytes_check_bit(0x01, 0));
  TEST_ASSERT_EQUAL(0, bytes_check_bit(0x02, 0));
}

TEST_GROUP_RUNNER(bytes) {
  RUN_TEST_CASE(bytes, test_bytes_16bit_to_high_low);
  RUN_TEST_CASE(bytes, test_bytes_high_low_to_16bit);
  RUN_TEST_CASE(bytes, test_bytes_set_bit);
  RUN_TEST_CASE(bytes, test_bytes_unset_bit);
  RUN_TEST_CASE(bytes, test_bytes_check_bit);
}
