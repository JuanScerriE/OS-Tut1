#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ELF_NAME "q2b"

static inline void die(const char *msg) {
  if (msg != NULL) {
    fprintf(stderr, ELF_NAME ": %s\n", msg);
  } else {
    perror(ELF_NAME);
  }

  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  pid_t fork_pid;

  if ((fork_pid = fork()) == -1) {
    die(NULL);
  } else if (fork_pid > 0) { // Parent
    printf("\e[1;31mPARENT PID: %d, CHILD PID: %d\e[m\n", getpid(), fork_pid);
  } else { // Child
    printf("\e[1;32mCHILD PID: %d, PARENT PID: %d\e[m\n", getpid(), getppid());
  }

  return 0;
}
