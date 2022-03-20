#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int errno;

int main(void) {
  char *args[] = {"gtop", "-d", "2", "-n", "10", NULL};

  if (execv("/usr/bin/gtop", args) == -1) {
    fprintf(stderr, "We have carnage! [%d]: ", errno);
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  return 0;
}
