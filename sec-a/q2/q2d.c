#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

int main(void) {
  pid_t pid = getpid();
  printf("PID = %d\n\n", pid);

  char *string = "HelloWorlD!";
  int count = strlen(string);

  print_string(string, count, true);
  printf("\nGap\n\n");
  print_string(string, count, false);

  return 0;
}
