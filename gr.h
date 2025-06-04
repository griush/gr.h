#ifndef GR_H
#define GR_H

#include <stddef.h>
#include <string.h>

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

#endif // !GR_H

#ifdef GR_IMPLEMENTATION

#endif // GR_IMPLEMENTATION
