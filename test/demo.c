#define GR_DEBUG
#include "../gr.h"

#include <stdio.h>

int
main (void)
{
  gr_log ("gr.h demo");

  // arena
  gr_log ("arena");
  gr_arena_t arena = gr_arena_create (gr_kilobytes (1));
  int32_t *p_x = gr_arena_alloc (&arena, sizeof (int32_t));
  *p_x = 5;
  gr_log ("arena.size (expect 1024): %ld", arena.size);
  gr_log ("arena.next (expect 4): %ld", arena.next);
  gr_log ("gr_arena_avail (expect 1020): %ld", gr_arena_avail (&arena));
  gr_log ("*p_x = (expect 5) %d", *p_x);

  gr_arena_reset (&arena);

  // str view
  gr_log ("gr_str_view");

  gr_str_t str_1 = gr_str_from_cstr ("gr.h demo app");
  gr_str_t str_2 = gr_str_from_cstr ("a demo for gr.h library");

  gr_str_t gr_h_1 = gr_str_substr (str_1, 0, 4);
  gr_str_t gr_h_2 = gr_str_substr (str_2, 11, 4);

  gr_log ("gr_h_1: %.*s", (int32_t)gr_h_1.len, gr_h_1.p);
  gr_log ("gr_h_2: %.*s", (int32_t)gr_h_2.len, gr_h_2.p);
  gr_log ("str_view_eq (expect 1): %d", gr_str_eq (gr_h_1, gr_h_2));

  // dynamic array
  gr_log ("gr_da");
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

  gr_da_remove (arr, 5);
  gr_log ("remove at i=5");

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

  // math
  gr_log ("math");

  float f = gr_lerp (0.0, 1.0, 0.5);
  printf ("lerp: %f\n", f);

  // vectors
  gr_vector_2f_t acceleration = (gr_vector_2f_t){
    .x = 1.0f,
    .y = 2.0,
  };
  gr_vector_2f_t velocity = gr_vector_2f_add (
      gr_vector_2f_zero (), gr_vector_2f_scale (acceleration, 2.0));
  printf ("x: %.3f, y: %.3f\n", velocity.x, velocity.y);
}
