#define GR_IMPL
#include "../gr.h"

#include <stdio.h>

const char* str1 = "gr.h demo";
const char* str2 = "a gr.h demo";

int main(void) {
  printf("gr.h demo\n");

  printf("gr_str_view\n");

  gr_str_view_t gr_h_1 = (gr_str_view_t){
      .p = str1,
      .len = 4,
  };
  gr_str_view_t gr_h_2 = (gr_str_view_t){
      .p = str2 + 2,
      .len = 4,
  };

  printf("str_view_eq (expect 1): %d\n", gr_str_view_eq(gr_h_1, gr_h_2));

  printf("gr_da\n");
  i8* arr = NULL;

  for (int i = 0; i < 10; ++i) {
    gr_da_append(arr, i * 10);
  }

  for (size_t i = 0; i < gr_da_count(arr); ++i) {
    printf("%d\n", arr[i]);
  }

  gr_da_free(arr);
}
