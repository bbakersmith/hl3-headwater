#include "unity.h"
#include "unity_fixture.h"

#include "headwater_ui.h"

TEST_GROUP(headwater_ui);

TEST_SETUP(headwater_ui) {};

TEST_TEAR_DOWN(headwater_ui) {};

TEST(headwater_ui, test_headwater_ui_modify_with_restrictions) {
  TEST_ASSERT_EQUAL(
    12,
    headwater_ui_modify_with_restrictions(12, 0, 5, 250)
  );

  TEST_ASSERT_EQUAL(
    22,
    headwater_ui_modify_with_restrictions(12, 10, 5, 250)
  );

  TEST_ASSERT_EQUAL(
    5,
    headwater_ui_modify_with_restrictions(12, -2000, 5, 250)
  );

  TEST_ASSERT_EQUAL(
    250,
    headwater_ui_modify_with_restrictions(12, 2000, 5, 250)
  );
}

TEST(headwater_ui, test_headwater_ui_modify_with_restrictions_16) {
  TEST_ASSERT_EQUAL(
    1234,
    headwater_ui_modify_with_restrictions_16(1234, 0, 5, 2500)
  );

  TEST_ASSERT_EQUAL(
    1244,
    headwater_ui_modify_with_restrictions_16(1234, 10, 5, 2500)
  );

  TEST_ASSERT_EQUAL(
    5,
    headwater_ui_modify_with_restrictions_16(1234, -2000, 5, 2500)
  );

  TEST_ASSERT_EQUAL(
    2500,
    headwater_ui_modify_with_restrictions_16(1234, 2000, 5, 2500)
  );
}

TEST_GROUP_RUNNER(headwater_ui) {
  RUN_TEST_CASE(headwater_ui, test_headwater_ui_modify_with_restrictions);
  RUN_TEST_CASE(headwater_ui, test_headwater_ui_modify_with_restrictions_16);
}
