#include <stdio.h>
#include <unistd.h>

/*
 * Just note that this is what the `env` program
 * does.
 */

extern char **environ;

int main(int argc, char **argv) {
  for (int i = 0; environ[i] != NULL; i++) {
    printf("%s\n", environ[i]);
  }

  return 0;
}
