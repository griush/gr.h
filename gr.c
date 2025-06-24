#include "gr.h"

#include <stddef.h>
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
  free (arena->p);
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
