#include "unity.h"
#include "unity_fixture.h"

#include "lcd.h"

TEST_GROUP(lcd);

uint8_t volatile dummy_cmd;
uint8_t volatile dummy_cmd_change_flag;
uint8_t volatile dummy_data;
uint8_t volatile dummy_data_change_flag;
uint8_t volatile dummy_rs;

void dummy_write_cmd_fn(uint8_t value) {
  dummy_cmd = value;
  dummy_cmd_change_flag = 1;
}

void dummy_write_data_fn(uint8_t value) {
  dummy_data = value;
  dummy_data_change_flag = 1;
}

void dummy_lcd_send_fn(uint8_t rs, uint8_t data) {
  dummy_data = data;
  dummy_rs = rs;
}

void assert_dummy_cmd(uint8_t expected) {
  TEST_ASSERT_EQUAL_MESSAGE(1, dummy_cmd_change_flag, "No cmd change");
  TEST_ASSERT_EQUAL_MESSAGE(expected, dummy_cmd, "Wrong cmd value");
  dummy_cmd_change_flag = 0;
}

void assert_dummy_data(uint8_t expected) {
  TEST_ASSERT_EQUAL_MESSAGE(1, dummy_data_change_flag, "No data change");
  TEST_ASSERT_EQUAL_MESSAGE(expected, dummy_data, "Wrong data value");
  dummy_data_change_flag = 0;
}

TEST_SETUP(lcd) {
  dummy_cmd = 0;
  dummy_cmd_change_flag = 0;
  dummy_data = 0;
  dummy_data_change_flag = 0;
  dummy_rs = 99;
};

TEST_TEAR_DOWN(lcd) {};

TEST(lcd, test_lcd_handle_interrupt) {
  LCDCommand result;
  uint16_t dummy_wait = 100;

  uint8_t expected[32] = {
    LCD__H,
    LCD__E,
    LCD__L,
    LCD__L,
    LCD__O,
    LCD__,
    LCD__S,
    LCD__A,
    LCD__Y,
    LCD__,
    LCD__I,
    LCD__,
    LCD__,
    LCD__,
    LCD__,
    LCD__,

    LCD__W,
    LCD__H,
    LCD__A,
    LCD__T,
    LCD__,
    LCD__S,
    LCD__A,
    LCD__Y,
    LCD__,
    LCD__Y,
    LCD__O,
    LCD__U,
    LCD__,
    LCD__,
    LCD__,
    LCD__
  };

  LCD dummy_lcd = lcd_new();
  dummy_lcd.wait = dummy_wait;
  dummy_lcd.mode = LCD_MODE_READ;

  for(uint8_t n = 0; n < 32; n++) {
    dummy_lcd.characters[n] = expected[n];
  }

  for(uint8_t n = 0; n < 3; n++) {
    // row 1
    lcd_handle_interrupt(&dummy_lcd, &dummy_lcd_send_fn);
    TEST_ASSERT_EQUAL(0, dummy_rs);
    TEST_ASSERT_EQUAL(0x80, dummy_data);

    for(uint8_t i = 0; i < 16; i++) {
      TEST_ASSERT_EQUAL(LCD_MODE_READ, dummy_lcd.mode);
      lcd_handle_interrupt(&dummy_lcd, &dummy_lcd_send_fn);
      TEST_ASSERT_EQUAL(1, dummy_rs);
      TEST_ASSERT_EQUAL(expected[i], dummy_data);
    }

    // row 2
    lcd_handle_interrupt(&dummy_lcd, &dummy_lcd_send_fn);
    TEST_ASSERT_EQUAL(0, dummy_rs);
    TEST_ASSERT_EQUAL(0xC0, dummy_data);

    for(uint8_t i = 16; i < 32; i++) {
      TEST_ASSERT_EQUAL(LCD_MODE_READ, dummy_lcd.mode);
      lcd_handle_interrupt(&dummy_lcd, &dummy_lcd_send_fn);
      TEST_ASSERT_EQUAL(1, dummy_rs);
      TEST_ASSERT_EQUAL(expected[i], dummy_data);
    }

    // TODO test cursor movement to selected field or off screen
    lcd_handle_interrupt(&dummy_lcd, &dummy_lcd_send_fn);

    // wait
    for(uint8_t i = 0; i < dummy_wait; i++) {
      TEST_ASSERT_EQUAL(LCD_MODE_WAIT, dummy_lcd.mode);
      lcd_handle_interrupt(&dummy_lcd, &dummy_lcd_send_fn);
      TEST_ASSERT_EQUAL(LCD_COMMAND_NULL_RS, dummy_rs);
      TEST_ASSERT_EQUAL(LCD_COMMAND_NULL_DATA, dummy_data);
    }

    // write
    for(uint16_t i = 0; i < 500; i++) {
      TEST_ASSERT_EQUAL(LCD_MODE_WRITE, dummy_lcd.mode);
      lcd_handle_interrupt(&dummy_lcd, &dummy_lcd_send_fn);
      TEST_ASSERT_EQUAL(LCD_COMMAND_NULL_RS, dummy_rs);
      TEST_ASSERT_EQUAL(LCD_COMMAND_NULL_DATA, dummy_data);
    }

    dummy_lcd.mode = LCD_MODE_READ;
  }
}

TEST(lcd, test_lcd_digit_to_char) {
  LCD_CHAR expected[10] = {
    LCD__0,
    LCD__1,
    LCD__2,
    LCD__3,
    LCD__4,
    LCD__5,
    LCD__6,
    LCD__7,
    LCD__8,
    LCD__9
  };

  for(uint8_t i = 0; i < 10; i++) {
    TEST_ASSERT_EQUAL(expected[i], lcd_digit_to_char(i));
  }
}

TEST_GROUP_RUNNER(lcd) {
  RUN_TEST_CASE(lcd, test_lcd_handle_interrupt);
  RUN_TEST_CASE(lcd, test_lcd_digit_to_char);
}
