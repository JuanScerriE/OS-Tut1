#include <stdio.h>
#include <unistd.h>

int main(void) {
  char *args[] = {"top", "-d", "2", "-n", "10", NULL};

  if (execv("/usr/bin/top", args) == -1) {
    perror("We have carnage!");
    return -1;
  }

  return 0;
}
