#include <stdio.h>
#include <string.h>

int main(void) {
  char *string = "Hello World!";

  for (int i = 0; i < strlen(string); i++) {
    printf("i = %d: ch = %c\n", i, string[i]);
  }

  return 0;
}
