#define GR_DEBUG
#include "../gr.h"

gr_test_result_t
test_empty_da ()
{
  int32_t *arr = NULL;
  if (gr_test_expect (gr_da_capacity (arr) == 0) != GR_TEST_PASSED)
    {
      return GR_TEST_FAILED;
    }
  
  if (gr_test_expect (gr_da_count (arr) == 0) != GR_TEST_PASSED)
    {
      return GR_TEST_FAILED;
    }

  return GR_TEST_PASSED;
}

gr_test_result_t
test_da_append ()
{
  int32_t *arr = NULL;
  gr_da_append(arr, 5);

  if (gr_test_expect (arr[0] == 5) != GR_TEST_PASSED)
    {
      return GR_TEST_FAILED;
    }
  if (gr_test_expect (gr_da_count (arr) == 1) != GR_TEST_PASSED)
    {
      return GR_TEST_FAILED;
    }
  if (gr_test_expect (gr_da_capacity (arr) == 4) != GR_TEST_PASSED)
    {
      return GR_TEST_FAILED;
    }

  return GR_TEST_PASSED;
}
  
int
main (void)
{
  gr_test_case_t da_suite[] = {
    {gr_str_from_cstr ("da_empty_array"), test_empty_da},
    {gr_str_from_cstr ("da_append"), test_da_append},
  };

  gr_test_case_run_suite (gr_str_from_cstr ("dynamic array"), da_suite, sizeof(da_suite) / sizeof(da_suite[0]));

  return 0;
}
