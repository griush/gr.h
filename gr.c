#include "gr.h"

#include <stdlib.h>
#include <string.h>

/*
 * arena
 */
gr_arena_t
gr_arena_create (size_t bytes)
{
  gr_arena_t r = (gr_arena_t){
    .p = NULL,
    .size = 0,
    .next = 0,
  };

  void *d = gr_malloc (bytes);
  if (d == NULL)
    {
      return r; // NULL arena
    }

  r.p = d;
  r.size = bytes;
  return r;
}

void *
gr_arena_alloc (gr_arena_t *arena, size_t bytes)
{
  size_t avail = arena->size - arena->next;
  if (avail < bytes)
    {
      return NULL;
    }

  void *r = arena->p + arena->next;
  arena->next += bytes;
  return r;
}

void
gr_arena_reset (gr_arena_t *arena)
{
  gr_free (arena->p);
  arena->size = 0;
  arena->next = 0;
}

size_t
gr_arena_used (gr_arena_t *arena)
{
  return arena->next;
}

size_t
gr_arena_avail (gr_arena_t *arena)
{
  return arena->size - arena->next;
}

/*
 * str_view
 */

gr_str_t
gr_str_empty ()
{
  return (gr_str_t){
    .p = NULL,
    .len = 0,
  };
}

gr_str_t
gr_str_from_cstr (const char *str)
{
  return (gr_str_t){
    .p = str,
    .len = strlen (str),
  };
}

bool
gr_str_eq (gr_str_t a, gr_str_t b)
{
  return a.len == b.len && memcmp (a.p, b.p, a.len) == 0;
}

gr_str_t
gr_str_substr (gr_str_t str, size_t start, size_t len)
{
  if (str.p == NULL || str.len == 0 || start >= str.len)
    {
      return gr_str_empty ();
    }

  size_t available = str.len - start;
  if (len > available)
    {
      len = available;
    }

  return (gr_str_t){ .len = len, .p = str.p + start };
}

/*
 * da (dynamic array)
 */
typedef struct
{
  size_t capacity;
  size_t count;
  char data[1];
} _gr_da_header;

#define GR_DA_HEADER(ptr)                                                     \
  ((_gr_da_header *)((char *)(ptr) - offsetof (_gr_da_header, data)))

void *
_gr_da_append (void **arr, size_t elem_size, void *val)
{
  if (*arr == NULL)
    {
      size_t cap = 4;
      _gr_da_header *header = (_gr_da_header *)gr_malloc (
          sizeof (_gr_da_header) + (cap - 1) * elem_size);
      if (!header)
        return NULL;
      header->capacity = cap;
      header->count = 0;
      *arr = header->data;
    }

  _gr_da_header *header = GR_DA_HEADER (*arr);
  if (header->count == header->capacity)
    {
      size_t new_cap = header->capacity * 2;
      header = (_gr_da_header *)gr_realloc (
          header, sizeof (_gr_da_header) + (new_cap - 1) * elem_size);
      if (!header)
        return NULL;
      header->capacity = new_cap;
      *arr = header->data;
    }

  void *dest = header->data + elem_size * header->count;
  memcpy (dest, val, elem_size);
  header->count++;
  return *arr;
}

void
_gr_da_free (void **arr)
{
  if (arr && *arr)
    {
      gr_free (GR_DA_HEADER (*arr));
      *arr = NULL;
    }
}

size_t
_gr_da_count (void *arr)
{
  if (!arr)
    return 0;
  return GR_DA_HEADER (arr)->count;
}

size_t
_gr_da_capacity (void *arr)
{
  if (!arr)
    return 0;
  return GR_DA_HEADER (arr)->capacity;
}

void
_gr_da_remove (void *arr, size_t elem_size, size_t i)
{
  if (!arr)
    return;

  _gr_da_header *h = GR_DA_HEADER (arr);

  if (i >= h->count)
    return;

  memmove (arr + i * elem_size, arr + (i + 1) * elem_size,
           (h->count - i - 1) * elem_size);

  h->count -= 1;
}

void
_gr_da_swap_remove (void *arr, size_t elem_size, size_t i)
{
  if (!arr)
    return;

  _gr_da_header *h = GR_DA_HEADER (arr);

  if (i < h->count)
    return;

  char *bytes = (char *)arr;

  memcpy (bytes + i * elem_size, bytes + (h->count - 1) * elem_size,
          elem_size);

  h->count -= 1;
}

void
_gr_da_clear (void *arr)
{
  if (!arr)
    return;

  GR_DA_HEADER (arr)->count = 0;
}

/*
 * test
 */
void
gr_test_case_run (gr_test_case_t test_case)
{
  fprintf (stderr, "running test %.*s\n", (int)test_case.name.len, test_case.name.p);

  gr_test_result_t result = test_case.func();

  if (result == GR_TEST_PASSED)
    {
      fprintf (stderr, "passed\n");
    }
  else
    {
      fprintf (stderr, "failed\n");
    }
}

void
gr_test_case_run_suite (gr_str_t suite_name, gr_test_case_t *tests, uint32_t count)
{
  fprintf (stderr, "==== running %.*s ====\n", (int)suite_name.len, suite_name.p);

  for (uint32_t i = 0; i < count; i++)
    {
      gr_test_case_run (tests[i]);
    }
}

gr_test_result_t
gr_test_expect (bool condition)
{
  if (condition)
    {
      return GR_TEST_PASSED;
    }
  else
    {
      return GR_TEST_FAILED;
    }
}

gr_test_result_t
gr_test_expect_eqli (int32_t expected, int32_t actual)
{
  if (expected == actual)
    {
      return GR_TEST_PASSED;
    }
  else
    {
      return GR_TEST_FAILED;
    }
}

gr_test_result_t
gr_test_expect_eqlf (float expected, float actual, float tolerance)
{
  if (gr_abs (expected - actual) <= gr_abs(tolerance))
    {
      return GR_TEST_PASSED;
    }
  else
    {
      return GR_TEST_FAILED;
    }
}

/*
 * math
 */
gr_vector_2f_t
gr_vector_2f_zero ()
{
  return (gr_vector_2f_t){
    .x = 0.0f,
    .y = 0.0f,
  };
}

gr_vector_2f_t
gr_vector_2f_add (gr_vector_2f_t va, gr_vector_2f_t vb)
{
  return (gr_vector_2f_t){
    .x = va.x + vb.x,
    .y = va.y + vb.y,
  };
}

gr_vector_2f_t
gr_vector_2f_sub (gr_vector_2f_t va, gr_vector_2f_t vb)
{
  return (gr_vector_2f_t){
    .x = va.x - vb.x,
    .y = va.y - vb.y,
  };
}

gr_vector_2f_t
gr_vector_2f_scale (gr_vector_2f_t va, float scalar)
{
  return (gr_vector_2f_t){
    .x = va.x * scalar,
    .y = va.y * scalar,
  };
}

float
gr_vector_2f_dot (gr_vector_2f_t va, gr_vector_2f_t vb)
{
  return va.x * vb.x + va.y * vb.y;
}

gr_vector_3f_t
gr_vector_3f_add (gr_vector_3f_t va, gr_vector_3f_t vb)
{
  return (gr_vector_3f_t){
    .x = va.x + vb.x,
    .y = va.y + vb.y,
    .z = va.z + vb.z,
  };
}

gr_vector_3f_t
gr_vector_3f_sub (gr_vector_3f_t va, gr_vector_3f_t vb)
{
  return (gr_vector_3f_t){
    .x = va.x - vb.x,
    .y = va.y - vb.y,
    .z = va.z - vb.z,
  };
}

float
gr_vector_3f_dot (gr_vector_3f_t va, gr_vector_3f_t vb)
{
  return va.x * vb.x + va.y * vb.y + va.z * vb.z;
}

gr_vector_3f_t
gr_vector_3f_scale (gr_vector_3f_t va, float scalar)
{
  return (gr_vector_3f_t){
    .x = va.x * scalar,
    .y = va.y * scalar,
    .z = va.z * scalar,
  };
}
