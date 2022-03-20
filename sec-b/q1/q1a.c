#include <unistd.h>
#include <stdio.h>

int main(void) {
  if (execl("/usr/bin/top", "top", "-d", "2", "-n", "10", NULL) == -1) {
    perror("We have carnage!");
    return -1;
  }

  return 0;
}
