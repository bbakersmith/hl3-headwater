#include "unity_fixture.h"

static void run_all_tests(void)
{
  RUN_TEST_GROUP(api);
  RUN_TEST_GROUP(bytes);
  RUN_TEST_GROUP(headwater_api);
  RUN_TEST_GROUP(headwater_state);
}

int main(int argc, const char * argv[])
{
  return UnityMain(argc, argv, run_all_tests);
}
