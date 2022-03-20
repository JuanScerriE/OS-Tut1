#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define ELF_NAME "q2c"

static inline void die(const char *msg) {
  if (msg != NULL) {
    fprintf(stderr, ELF_NAME ": %s\n", msg);
  } else {
    perror(ELF_NAME);
  }

  exit(EXIT_FAILURE);
}

// Consider This

/* static inline int ltoi(long n) { */
/*   while (n > INT_MAX || n < INT_MIN) { */
/*     if (n > INT_MAX) { */
/*       n -= INT_MAX; */
/*     } */

/*     if (n < INT_MIN) { */
/*       n -= INT_MAX; */
/*     } */
/*   } */

/*   return (int)n; */
/* } */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    die("Invalid number of command line arguments");
  }

  char *endptr;
  long time;

  time = strtol(argv[1], &endptr, 10);

  if (time < 0 || *endptr != '\0') {
    die("Expected a positive integer");
  }

  pid_t fork_pid;
  int status;

  if ((fork_pid = fork()) == -1) {
    die(NULL);
  } else if (fork_pid > 0) { // Parent
    printf("\e[1;31mPARENT PID: %d, CHILD PID: %d\e[m\n", getpid(), fork_pid);

    if (wait(&status) == -1) {
      die(NULL);
    }

    if (WIFEXITED(status)) {
      printf("\e[1;33mCHILD STATUS: %d\e[m\n", WEXITSTATUS(status));
    }

    printf("\nchild terminated\n\n");
  } else { // Child
    printf("\e[1;32mCHILD PID: %d, PARENT PID: %d\e[m\n", getpid(), getppid());
    sleep((unsigned int)time);
  }

  return 0;
}
