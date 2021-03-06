#include <stdio.h>
#include <stdlib.h>

#include "linenoise.h"

int main(void) {
  char *line;

  while ((line = linenoise("> ")) != NULL) {
    if (*line != '\0') {
      printf("echo: %s\n", line);
    }

    free(line);
  }

  return 0;
}
