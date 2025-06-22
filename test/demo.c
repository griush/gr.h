#define GR_DEBUG
#include "../gr.h"

#include <stdio.h>

int
main (void)
{
  gr_log ("gr.h demo");

  // str view
  printf ("\ngr_str_view\n");

  gr_str_view_t str_1 = gr_str_view_from_cstr ("gr.h demo app");
  gr_str_view_t str_2 = gr_str_view_from_cstr ("a demo for gr.h library");

  gr_str_view_t gr_h_1 = gr_str_view_substr (str_1, 0, 4);
  gr_str_view_t gr_h_2 = gr_str_view_substr (str_2, 11, 4);

  gr_log ("gr_h_1: %.*s", (int32_t)gr_h_1.len, gr_h_1.p);
  gr_log ("gr_h_2: %.*s", (int32_t)gr_h_2.len, gr_h_2.p);
  gr_log ("str_view_eq (expect 1): %d", gr_str_view_eq (gr_h_1, gr_h_2));

  // dynamic array
  printf ("\ngr_da\n");
  int32_t *arr = NULL;

  for (int i = 0; i < 10; ++i)
    {
      gr_da_append (arr, i * 10);
    }

  for (size_t i = 0; i < gr_da_count (arr); ++i)
    {
      printf ("%d\n", arr[i]);
    }

  gr_da_swap_remove (arr, 5);
  gr_log ("swap remove at i=5");

  for (size_t i = 0; i < gr_da_count (arr); ++i)
    {
      printf ("%d\n", arr[i]);
    }

  gr_da_clear (arr);
  gr_log ("cleared array");
  for (size_t i = 0; i < gr_da_count (arr); ++i)
    {
      printf ("%d\n", arr[i]);
    }

  gr_da_free (arr);
}
