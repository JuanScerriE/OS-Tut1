#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define ELF_NAME "q1a&q1b"

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
  int status;

  if ((fork_pid = fork()) == -1) {
    die(NULL);
  } else if (fork_pid > 0) { // Parent
    printf("\e[1;31mPARENT PID: %d, CHILD PID: %d\e[m\n", getpid(), fork_pid);

    // 0 -> blocking wait
    if (waitpid(fork_pid, &status, 0) == -1) {
      die(NULL);
    }

    if (WIFEXITED(status)) {
      printf("\e[1;33mCHILD STATUS: %d\e[m\n", WEXITSTATUS(status));
    }
  } else { // Child
    printf("\e[1;32mCHILD PID: %d, PARENT PID: %d\e[m\n", getpid(), getppid());

    if (execl("/usr/bin/ps", "ps", "-f", NULL) == -1) {
      die(NULL);
    }
  }

  return 0;
}
