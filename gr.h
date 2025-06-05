#ifndef GR_H
#define GR_H

#include <stddef.h>
#include <stdbool.h>

#include <stdlib.h>
#include <string.h>

/*
 * define cstd
 */
#define GR_CSTD_23 202311L
#define GR_CSTD_17 201710L
#define GR_CSTD_11 201112L
#define GR_CSTD_99 199901L
#define GR_CSTD_89 198901L

#if __STDC_VERSION__ >= 202311L
  #define GR_CSTD GR_CSTD_23
#elif __STDC_VERSION__ >= 201710L
  #define GR_CSTD GR_CSTD_17
#elif __STDC_VERSION__ >= 201112L
  #define GR_CSTD GR_CSTD_11
#elif __STDC_VERSION__ >= 199901L
  #define GR_CSTD GR_CSTD_99
#elif defined(__STDC__)
  #define GR_CSTD GR_CSTD_89
#else
  #define GR_CSTD 1L
#endif

/*
 * define compiler
 */
#if defined(__clang__)
  #define GR_COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
  #define GR_COMPILER_GCC 1
#elif defined(_MSC_VER)
  #define GR_COMPILER_MSVC 1
#elif defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
  #define GR_COMPILER_INTEL 1
#elif defined(__TINYC__)
  #define GR_COMPILER_TINYC 1
#else
  #define GR_COMPILER_UNKNOWN 1
#endif

#if defined(_WIN32) && !defined(__MINGW32__)
  #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
  #endif
#endif

#ifndef GR_STATIC_ASSERT
  #if GR_CSTD >= GR_CSTD_11
    #define GR_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
  #else
    #error "TODO: define static_assert for older std"
  #endif
#endif

#include <stdint.h>
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

GR_STATIC_ASSERT(sizeof(u8)  == sizeof(i8), "incorrect type size");
GR_STATIC_ASSERT(sizeof(u16) == sizeof(i16), "incorrect type size");
GR_STATIC_ASSERT(sizeof(u32) == sizeof(i32), "incorrect type size");
GR_STATIC_ASSERT(sizeof(u64) == sizeof(i64), "incorrect type size");

GR_STATIC_ASSERT(sizeof(u8)  == 1, "incorrect type size");
GR_STATIC_ASSERT(sizeof(u16) == 2, "incorrect type size");
GR_STATIC_ASSERT(sizeof(u32) == 4, "incorrect type size");
GR_STATIC_ASSERT(sizeof(u64) == 8, "incorrect type size");

/*
 * define auto properly
 */
#if GR_CSTD >= GR_CSTD_23
  // C23 or later
  // auto is native
#elif GR_COMPILER_GCC || GR_COMPILER_CLANG
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

static inline gr_str_view_t gr_str_view_empty() {
  return (gr_str_view_t){
      .p = NULL,
      .len = 0,
  };
}

static inline gr_str_view_t gr_str_view_from_cstr(const char* str) {
  return (gr_str_view_t){
      .p = str,
      .len = strlen(str),
  };
}

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

#endif // GR_IMPL
