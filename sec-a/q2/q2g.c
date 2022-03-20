#include <stdio.h>
#include <stdlib.h>

void print1(void) { printf("I am print1()!\n"); }

void print2(void) { printf("I am print2()!\n"); }

void print3(void) { printf("I am print3()!\n"); }

int main(void) {
  atexit(print1);
  atexit(print2);
  atexit(print3);
  return 0;
}
