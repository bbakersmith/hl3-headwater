#include "unity.h"
#include "unity_fixture.h"

#include "lcd.h"

TEST_GROUP(lcd);

uint8_t volatile dummy_cmd;
uint8_t volatile dummy_cmd_change_flag;
uint8_t volatile dummy_data;
uint8_t volatile dummy_data_change_flag;

void dummy_write_cmd_fn(uint8_t value) {
  dummy_cmd = value;
  dummy_cmd_change_flag = 1;
}

void dummy_write_data_fn(uint8_t value) {
  dummy_data = value;
  dummy_data_change_flag = 1;
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
};

TEST_TEAR_DOWN(lcd) {};

TEST(lcd, test_lcd_message_screen_new) {
  LCD_CHAR message[16] = {
    LCD__B,
    LCD__A,
    LCD__D,
    LCD__D
  };

  LCDScreen screen = lcd_screen_message_top_new(message, sizeof(*message));

  TEST_ASSERT_EQUAL(1, screen.length);
  TEST_ASSERT_EQUAL(0, screen.index);
  TEST_ASSERT_EQUAL(LCD_ROW_1_COL_1, screen.fields[0].position);
  TEST_ASSERT_EQUAL(4, screen.fields[0].length);
  TEST_ASSERT_EQUAL(-1, screen.fields[0].index);
  TEST_ASSERT_EQUAL(LCD__B, screen.fields[0].value[0]);
  TEST_ASSERT_EQUAL(LCD__A, screen.fields[0].value[1]);
  TEST_ASSERT_EQUAL(LCD__D, screen.fields[0].value[2]);
  TEST_ASSERT_EQUAL(LCD__D, screen.fields[0].value[3]);
}

TEST(lcd, test_lcd_handle_interrupt) {
  // TODO

  LCD_CHAR message_a[16] = {
    LCD__S,
    LCD__T,
    LCD__A,
    LCD__R
  };

  LCD_CHAR message_b1[16] = {
    LCD__B,
    LCD__Y
  };

  LCD_CHAR message_b2[16] = {
    LCD__H,
    LCD__E,
    LCD__L,
    LCD__L,
    LCD__O,
    LCD__W,
    LCD__O,
    LCD__R,
    LCD__L,
    LCD__D
  };

  LCDScreenField field_a = lcd_screen_field_new(message_a, 4, LCD_ROW_1_COL_7);
  LCDScreenField field_b1 = lcd_screen_field_new(message_b1, 2, LCD_ROW_1_COL_15);
  LCDScreenField field_b2 = lcd_screen_field_new(message_b2, 10, LCD_ROW_2_COL_7);

  LCDScreenField fields_a[8] = {field_a};
  LCDScreenField fields_b[8] = {field_b1, field_b2};

  LCDScreen screen_a = lcd_screen_new(fields_a, 1);
  LCDScreen screen_b = lcd_screen_new(fields_b, 2);

  LCDState state = {
    .index = 0,
    .write_cmd_fn = &dummy_write_cmd_fn,
    .write_data_fn = &dummy_write_data_fn
  };

  state.screens[0] = screen_a;
  state.screens[1] = screen_b;

  /* for(uint8_t i = 0; i < 10; i++) { */
    lcd_handle_interrupt(&state);
    assert_dummy_cmd(LCD_ROW_1_COL_7);

    lcd_handle_interrupt(&state);
    assert_dummy_data(LCD__S);
    lcd_handle_interrupt(&state);
    assert_dummy_data(LCD__T);
    lcd_handle_interrupt(&state);
    assert_dummy_data(LCD__A);
    lcd_handle_interrupt(&state);
    assert_dummy_data(LCD__R);
  /* } */

  state.index = 1;

  /* for(uint8_t i = 0; i < 10; i++) { */
    lcd_handle_interrupt(&state);
    /* TEST_ASSERT_EQUAL(0x15, dummy_cmd); */

    lcd_handle_interrupt(&state);
    assert_dummy_data(LCD__B);
    lcd_handle_interrupt(&state);
    assert_dummy_data(LCD__Y);

    lcd_handle_interrupt(&state);
    /* TEST_ASSERT_EQUAL(0xC6, dummy_cmd); */

    lcd_handle_interrupt(&state);
    assert_dummy_data(LCD__H);
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__E); */
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__L); */
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__L); */
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__O); */
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__W); */
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__O); */
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__R); */
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__L); */
    /* lcd_handle_interrupt(&state); */
    /* assert_dummy_data(LCD__D); */
  /* } */

  // create dummy state
  // - multiple screens
  // - dummy data and clk write fns

  // handle interrupt
  // verify values
  // repeat
}

TEST_GROUP_RUNNER(lcd) {
  RUN_TEST_CASE(lcd, test_lcd_message_screen_new);
  RUN_TEST_CASE(lcd, test_lcd_handle_interrupt);
}
