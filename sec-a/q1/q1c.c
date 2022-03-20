#include <stdbool.h>
#include <stdio.h>

#include <string.h>

void print_string(char *p_string, int p_count, bool p_reverse) {
  if (p_reverse) {
    for (int i = p_count - 1; i >= 0; i--) {
      printf("i = %d: ch = %c\n", i, p_string[i]);
    }
  } else {
    for (int i = 0; i < p_count; i++) {
      printf("i = %d: ch = %c\n", i, p_string[i]);
    }
  }
}

/*
 * This has a problem because reading with the
 * reverse case. It never actually terminates.
 * This is because everytime it arrives at 0
 * it loops backaround to a positive number so it
 * continues subtracting and as you can imagine
 * the number would be very large. Plus I think
 * I fucked up in using size_t everywhere because
 * It is really meant to represent the size of an
 * object in memory so we should be careful when
 * we use size_t.
 */
void borked_print_string(char *p_string, int p_count, bool p_reverse) {
  if (p_reverse) {
    for (size_t i = p_count - 1; i > 0; i--) {
      printf("i = %lu: ch = %c\n", i, p_string[i]);
    }
  } else {
    for (size_t i = 0; i < p_count; i++) {
      printf("i = %lu: ch = %c\n", i, p_string[i]);
    }
  }
}

int main(void) {
  char *string = "HelloWorlD!";
  int count = strlen(string);

  print_string(string, count, true);
  printf("\nGap\n\n");
  print_string(string, count, false);

  return 0;
}
