#ifndef GR_H
#define GR_H

/*
 * commonly used includes
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

/*
 * define cstd
 */
#define GR_CSTD_23 202311L
#define GR_CSTD_17 201710L
#define GR_CSTD_11 201112L
#define GR_CSTD_99 199901L
#define GR_CSTD_89 198901L

#ifdef __STDC_VERSION__
#define GR_CSTD __STDC_VERSION__
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
#define GR_STATIC_ASSERT(cond, msg) _Static_assert (cond, msg)
#else
#define GR_STATIC_ASSERT(cond, msg)                                           \
  typedef char static_assertion_failed[(cond) ? 1 : -1]
#endif // GR_CSTD >= GR_CSTD_11
#endif // !STATIC_ASSERT

#ifndef GR_DEBUG_TRAP
#if GR_COMPILER_MSVC
#if _MSC_VER < 1300
#define GR_DEBUG_TRAP() __asm int 3
#else
#define GR_DEBUG_TRAP() __debugbreak ()
#endif
#else
#define GR_DEBUG_TRAP() __builtin_trap ()
#endif
#endif

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
 * other helpers
 */
#ifndef gr_bit
#define gr_bit(x) (1 << (x))
#endif

#ifndef gr_min
#define gr_min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef gr_max
#define gr_max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef gr_min3
#define gr_min3(a, b, c) gr_min (gr_min (a, b), c)
#endif

#ifndef gr_max3
#define gr_max3(a, b, c) gr_max (gr_max (a, b), c)
#endif

#ifndef gr_clamp
#define gr_clamp(x, lower, upper) gr_min (gr_max ((x), (lower)), (upper))
#endif

#ifndef gr_clamp01
#define gr_clamp01(x) gr_clamp ((x), 0, 1)
#endif

#ifndef gr_is_between
#define gr_is_between(x, lower, upper) (((lower) <= (x)) && ((x) <= (upper)))
#endif

#ifndef gr_abs
#define gr_abs(x) ((x) < 0 ? -(x) : (x))
#endif

#ifndef gr_kilobytes
#define gr_kilobytes(x) ((x) * (size_t)(1024))
#define gr_megabytes(x) (gr_kilobytes (x) * (size_t)(1024))
#define gr_gigabytes(x) (gr_megabytes (x) * (size_t)(1024))
#define gr_terabytes(x) (gr_gigabytes (x) * (size_t)(1024))
#endif

/*
 * override allocator
 */
#ifndef gr_malloc
#define gr_malloc malloc
#endif

#ifndef gr_realloc
#define gr_realloc realloc
#endif

#ifndef gr_free
#define gr_free free
#endif

/*
 * log
 */
#ifdef GR_DEBUG
/*
 * must define GR_DEBUG before using
 */
#define gr_log(fmt, ...)                                                      \
  fprintf (stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define gr_log(fmt, ...)
#endif

/*
 * arena
 */
typedef struct
{
  void *p;

  /* index of next allocation */
  size_t next;

  /* size in bytes */
  size_t size;
} gr_arena_t;

gr_arena_t gr_arena_create (size_t bytes);
void *gr_arena_alloc (gr_arena_t *arena, size_t bytes);
void gr_arena_reset (gr_arena_t *arena);

size_t gr_arena_used (gr_arena_t *arena);
size_t gr_arena_avail (gr_arena_t *arena);

/*
 * str
 */
typedef struct
{
  const char *p;
  size_t len;
} gr_str_t;

/*
 * creates empty str_view
 */
gr_str_t gr_str_empty ();

/*
 * creates str_view from a cstr
 */
gr_str_t gr_str_from_cstr (const char *str);

/*
 * tests two str_view for the same len and data
 */
bool gr_str_eq (gr_str_t a, gr_str_t b);

/*
 * creates a string view substr
 * returns gr_str_view_empty () on failure
 */
gr_str_t gr_str_substr (gr_str_t str, size_t start, size_t len);

/*
 * da (dynamic array)
 */
#define gr_da_append(arr, val)                                                \
  do                                                                          \
    {                                                                         \
      auto _tmp = (val);                                                      \
      _gr_da_append ((void **)&(arr), sizeof (*(arr)), &_tmp);                \
    }                                                                         \
  while (0)
#define gr_da_free(arr) _gr_da_free ((void **)&(arr))
#define gr_da_count(arr) _gr_da_count (arr)
#define gr_da_capacity(arr) _gr_da_capacity (arr)
#define gr_da_remove(arr, i) _gr_da_remove ((arr), sizeof (*(arr)), (i))
#define gr_da_swap_remove(arr, i)                                             \
  _gr_da_swap_remove ((arr), sizeof (*(arr)), (i))
#define gr_da_clear(arr) _gr_da_clear (arr)

void *_gr_da_append (void **arr, size_t elem_size, void *val);
void _gr_da_free (void **arr);
size_t _gr_da_count (void *arr);
size_t _gr_da_capacity (void *arr);
void _gr_da_remove (void *arr, size_t elem_size, size_t i);
void _gr_da_swap_remove (void *arr, size_t elem_size, size_t i);
void _gr_da_clear (void *arr);

/*
 * math
 */
#define gr_lerp(a, b, t) (a + (b - a) * t)

typedef struct
{
  float x;
  float y;
} gr_vector_2f_t;

typedef struct
{
  float x;
  float y;
  float z;
} gr_vector_3f_t;

gr_vector_2f_t gr_vector_2f_zero ();
gr_vector_2f_t gr_vector_2f_add (gr_vector_2f_t va, gr_vector_2f_t vb);
gr_vector_2f_t gr_vector_2f_sub (gr_vector_2f_t va, gr_vector_2f_t vb);
gr_vector_2f_t gr_vector_2f_scale (gr_vector_2f_t va, float scalar);
float gr_vector_2f_dot (gr_vector_2f_t va, gr_vector_2f_t vb);

gr_vector_3f_t gr_vector_3f_zero ();
gr_vector_3f_t gr_vector_3f_add (gr_vector_3f_t va, gr_vector_3f_t vb);
gr_vector_3f_t gr_vector_3f_sub (gr_vector_3f_t va, gr_vector_3f_t vb);
gr_vector_3f_t gr_vector_3f_scale (gr_vector_3f_t va, float scalar);
float gr_vector_3f_dot (gr_vector_3f_t va, gr_vector_3f_t vb);

#endif // !GR_H
