#include "unity.h"
#include "unity_fixture.h"

#include "headwater_lcd.h"

TEST_GROUP(headwater_lcd);

TEST_SETUP(headwater_lcd) {};

TEST_TEAR_DOWN(headwater_lcd) {};

TEST(headwater_lcd, test_headwater_lcd_update_main_bpm_1234) {
  LCD dummy_lcd = lcd_new();

  headwater_lcd_update_main_bpm(&dummy_lcd, 1234);

  TEST_ASSERT_EQUAL(LCD__1, dummy_lcd.characters[0]);
  TEST_ASSERT_EQUAL(LCD__2, dummy_lcd.characters[1]);
  TEST_ASSERT_EQUAL(LCD__3, dummy_lcd.characters[2]);
  TEST_ASSERT_EQUAL(LCD__DOT, dummy_lcd.characters[3]);
  TEST_ASSERT_EQUAL(LCD__4, dummy_lcd.characters[4]);
}

TEST(headwater_lcd, test_headwater_lcd_update_main_bpm_9007) {
  LCD dummy_lcd = lcd_new();

  headwater_lcd_update_main_bpm(&dummy_lcd, 9007);

  TEST_ASSERT_EQUAL(LCD__9, dummy_lcd.characters[0]);
  TEST_ASSERT_EQUAL(LCD__0, dummy_lcd.characters[1]);
  TEST_ASSERT_EQUAL(LCD__0, dummy_lcd.characters[2]);
  TEST_ASSERT_EQUAL(LCD__DOT, dummy_lcd.characters[3]);
  TEST_ASSERT_EQUAL(LCD__7, dummy_lcd.characters[4]);
}

TEST(headwater_lcd, test_headwater_lcd_update_main_bpm_56) {
  LCD dummy_lcd = lcd_new();

  headwater_lcd_update_main_bpm(&dummy_lcd, 56);

  TEST_ASSERT_EQUAL(LCD__, dummy_lcd.characters[0]);
  TEST_ASSERT_EQUAL(LCD__, dummy_lcd.characters[1]);
  TEST_ASSERT_EQUAL(LCD__5, dummy_lcd.characters[2]);
  TEST_ASSERT_EQUAL(LCD__DOT, dummy_lcd.characters[3]);
  TEST_ASSERT_EQUAL(LCD__6, dummy_lcd.characters[4]);
}

TEST(headwater_lcd, test_headwater_lcd_update_main_bpm_0) {
  LCD dummy_lcd = lcd_new();

  headwater_lcd_update_main_bpm(&dummy_lcd, 0);

  TEST_ASSERT_EQUAL(LCD__, dummy_lcd.characters[0]);
  TEST_ASSERT_EQUAL(LCD__, dummy_lcd.characters[1]);
  TEST_ASSERT_EQUAL(LCD__0, dummy_lcd.characters[2]);
  TEST_ASSERT_EQUAL(LCD__DOT, dummy_lcd.characters[3]);
  TEST_ASSERT_EQUAL(LCD__0, dummy_lcd.characters[4]);
}

TEST_GROUP_RUNNER(headwater_lcd) {
  RUN_TEST_CASE(headwater_lcd, test_headwater_lcd_update_main_bpm_1234);
  RUN_TEST_CASE(headwater_lcd, test_headwater_lcd_update_main_bpm_9007);
  RUN_TEST_CASE(headwater_lcd, test_headwater_lcd_update_main_bpm_56);
  RUN_TEST_CASE(headwater_lcd, test_headwater_lcd_update_main_bpm_0);
}
