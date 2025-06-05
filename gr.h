#ifndef GR_H
#define GR_H

#include <stddef.h>
#include <stdbool.h>

#include <stdlib.h>
#include <string.h>

/*
 * define auto properly
 */
#if __STDC_VERSION__ >= 202311L
  // C23 or later
  // auto is native
#elif defined(__GNUC__) || defined(__clang__)
  #define auto __auto_type
#else
  #error "This compiler does not support auto-style type inference"
#endif

/*
 * str_view
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const char* p;
  size_t len;
} gr_str_view_t;

static inline bool gr_str_view_eq(gr_str_view_t a, gr_str_view_t b) {
  return a.len == b.len && memcmp(a.p, b.p, a.len) == 0;
}

#ifdef __cplusplus
}
#endif

/*
 * da (dynamic array)
 */
#define gr_da_append(arr, val)                                                 \
  do {                                                                         \
    auto _tmp = (val);                                                         \
    _gr_da_append((void**)&(arr), sizeof(*(arr)), &_tmp);                      \
  } while (0)
#define gr_da_free(arr) _gr_da_free((void**)&(arr))
#define gr_da_count(arr) _gr_da_count(arr)
#define gr_da_capacity(arr) _gr_da_capacity(arr)

#ifdef __cplusplus
extern "C" {
#endif

void* _gr_da_append(void** arr, size_t elem_size, void* val);
void _gr_da_free(void** arr);
size_t _gr_da_count(void* arr);
size_t _gr_da_capacity(void* arr);

#ifdef __cplusplus
}
#endif

#endif // !GR_H

#ifdef GR_IMPL

/*
 * da (dynamic array)
 */
typedef struct {
  size_t capacity;
  size_t count;
  char data[1]; // flexible array member trick
} _gr_da_header;

#define GR_DA_HEADER(ptr)                                                      \
  ((_gr_da_header*)((char*)(ptr) - offsetof(_gr_da_header, data)))

void* _gr_da_append(void** arr, size_t elem_size, void* val) {
  if (*arr == NULL) {
    size_t cap = 4;
    _gr_da_header* header =
        (_gr_da_header*)malloc(sizeof(_gr_da_header) + (cap - 1) * elem_size);
    if (!header)
      return NULL;
    header->capacity = cap;
    header->count = 0;
    *arr = header->data;
  }

  _gr_da_header* header = GR_DA_HEADER(*arr);
  if (header->count == header->capacity) {
    size_t new_cap = header->capacity * 2;
    header = (_gr_da_header*)realloc(header, sizeof(_gr_da_header) +
                                                 (new_cap - 1) * elem_size);
    if (!header)
      return NULL;
    header->capacity = new_cap;
    *arr = header->data;
  }

  void* dest = header->data + elem_size * header->count;
  memcpy(dest, val, elem_size);
  header->count++;
  return *arr;
}

void _gr_da_free(void** arr) {
  if (arr && *arr) {
    free(GR_DA_HEADER(*arr));
    *arr = NULL;
  }
}

size_t _gr_da_count(void* arr) {
  if (!arr)
    return 0;
  return GR_DA_HEADER(arr)->count;
}

size_t _gr_da_capacity(void* arr) {
  if (!arr)
    return 0;
  return GR_DA_HEADER(arr)->capacity;
}

#endif // GR_IMPLEMENTATION
