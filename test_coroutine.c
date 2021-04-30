#include <stdio.h>
#include "coroutine.h"

// void test(struct cr*o, int state)
cr_function(test, int state, int par1) {
  cr_begin();
  cr_wait((state % 2 == 1), cr_restart());
  printf("0 %d %d\n", state, par1);
  cr_end(cr_restart());
}

cr_function(test1, int state, int par1, int par2) {
  cr_begin();
  cr_wait((state % 2 == 0), cr_restart());
  printf("1 %d %d %d\n", state, par1, par2);
  cr_end(cr_restart());
}

int main(void) {
  struct cr cor1 = cr_init();
  struct cr cor2 = cr_init();
  for (int i = 1;i <= 10;i++) {
    cr_call(test, cor1, i, i);
    cr_call(test1, cor2, i, i, i);
  }
}
