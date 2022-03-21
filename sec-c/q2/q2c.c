#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "linenoise.h"

#define DEFAULT_SIZE 16

static inline void die(const char *restrict format, ...) {
  va_list ap;

  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);

  if (format[0] != '\0' && format[strlen(format) - 1] == ':') {
    fputc(' ', stderr);
    perror(NULL);
  } else {
    fputc('\n', stderr);
  }

  exit(1);
}

static inline void *emalloc(size_t size) {
  void *ptr;

  if ((ptr = malloc(size)) == NULL)
    die("malloc:");

  return ptr;
}

static inline void *erealloc(void *ptr, size_t size) {
  if ((ptr = realloc(ptr, size)) == NULL) {
    die("realloc:");
  }

  return ptr;
}

struct token {
  char *str;  /* pointer to string */
  size_t len; /* length of the current string */
  size_t alc; /* allocated size of the string */
};

void token_grow(struct token *tok, size_t need) {
  tok->alc = tok->alc > 0 ? tok->alc : 1;

  while (tok->alc < need)
    tok->alc <<= 1;

  tok->str = erealloc(tok->str, sizeof(char) * tok->alc);
}

void token_init(struct token *tok, size_t est) {
  tok->str = NULL;
  tok->len = 0;
  tok->alc = 0;

  if (est > 0)
    token_grow(tok, est);
}

struct token *token_new(void) {
  struct token *tok = emalloc(sizeof(struct token));
  token_init(tok, DEFAULT_SIZE);
  return tok;
}

void token_shallow_free(struct token *tok) {
  if (tok != NULL) {
    free(tok);
  }
}

void token_deep_free(struct token *tok) {
  if (tok != NULL) {
    if (tok->str != NULL) {
      free(tok->str);
    }

    free(tok);
  }
}

void token_append(struct token *tok, char ch) {
  if (tok->len >= tok->alc)
    token_grow(tok, tok->alc + 1);

  tok->str[tok->len++] = ch;
}

struct token_vec {
  struct token **arr; /* pointer to array of token pointers */
  size_t len;         /* length of the current array */
  size_t alc;         /* allocated size of array */
};

void token_vec_grow(struct token_vec *vec, size_t need) {
  vec->alc = vec->alc > 0 ? vec->alc : 1;

  while (vec->alc < need)
    vec->alc <<= 1;

  vec->arr = erealloc(vec->arr, sizeof(struct token *) * vec->alc);
}

void token_vec_init(struct token_vec *vec, size_t est) {
  vec->arr = NULL;
  vec->len = 0;
  vec->alc = 0;

  if (est > 0)
    token_vec_grow(vec, est);
}

struct token_vec *token_vec_new(void) {
  struct token_vec *vec = emalloc(sizeof(struct token_vec));
  token_vec_init(vec, DEFAULT_SIZE);
  return vec;
}

void token_vec_shallow_free(struct token_vec *vec) {
  if (vec != NULL) {
    free(vec);
  }
}

/*
 * This frees the strings which we allocate for the tokens. My
 * dilemma is how I am going to deal with properly managing this
 * because the token_vec_get_list() function returns an array of
 * the strings in the token_vec. But this means that the the
 * array has to be freed as well as the strings. The problem is
 * that I want to make this one function call away. But the way
 * it is right now requires you to call token_vec_deep_free() to
 * free up all the strings then call free() on the allocated
 * array.
 *
 * Well I could make this take in a parameter. Which is the
 * allocated array of integers. Obviously, if we did not get ask
 * for the conversion of the vector into a string array we just
 * pass in NULL for the second parameter.
 */
void token_vec_deep_free(struct token_vec *vec, char **list) {
  if (vec != NULL) {
    if (vec->arr != NULL) {
      for (size_t i = 0; i < vec->len; i++) {
        token_deep_free(vec->arr[i]);
      }

      if (list != NULL) {
        free(list);
      }

      free(vec->arr);
    }

    free(vec);
  }
}

void token_vec_append(struct token_vec *vec, struct token *tok) {
  if (vec->len >= vec->alc)
    token_vec_grow(vec, vec->alc + 1);

  vec->arr[vec->len++] = tok;
}

char **token_vec_get_list(struct token_vec *vec) {
  char **list = emalloc(sizeof(char *) * (vec->len + 1));

  for (size_t i = 0; i < vec->len; i++) {
    list[i] = vec->arr[i]->str;
  }

  list[vec->len] = NULL;

  return list;
}

struct peek_stream {
  char *str;
  size_t pos;
  size_t len;
};

void peek_stream_init(struct peek_stream *stream, char *str) {
  stream->str = str;
  stream->pos = 0;
  stream->len = strlen(str);
}

struct peek_stream *peek_stream_new(char *str) {
  struct peek_stream *stream = emalloc(sizeof(struct peek_stream));
  peek_stream_init(stream, str);
  return stream;
}

void peek_stream_shallow_free(struct peek_stream *stream) {
  if (stream != NULL) {
    free(stream);
  }
}

void peek_stream_deep_free(struct peek_stream *stream) {
  if (stream != NULL) {
    if (stream->str != NULL) {
      free(stream->str);
    }

    free(stream);
  }
}

char peek_stream_peek(struct peek_stream *stream) {
  if (stream->pos < stream->len)
    return stream->str[stream->pos];

  return '\0';
}

char peek_stream_next(struct peek_stream *stream) {
  if (stream->pos < stream->len)
    return stream->str[stream->pos++];

  return '\0';
}

bool peek_stream_end(struct peek_stream *stream) {
  if (stream->pos < stream->len)
    return false;

  return true;
}

struct token_vec *tokenise(char *line) {
  struct peek_stream stream;
  peek_stream_init(&stream, line);

  struct token_vec *vec = token_vec_new();

  char ch;
  struct token *tok;

  while (!peek_stream_end(&stream)) {
    ch = peek_stream_next(&stream);

    if (!isspace(ch) && ch != '\0') {
      tok = token_new();

      do {
        token_append(tok, ch);
        ch = peek_stream_next(&stream);
      } while (!isspace(ch) && ch != '\0');

      token_append(tok, '\0');
      token_vec_append(vec, tok);
    }
  }

  return vec;
}

bool sh_exit(char **args) { return false; }

char *builtins_str[] = {"exit", NULL};

bool (*builtins_func[])(char **) = {&sh_exit};

char *sh_launch(char **args, bool *continue_loop) {
  pid_t pid;
  int status;

  for (size_t i = 0; builtins_str[i] != NULL; i++) {
    if (!strcmp(args[0], builtins_str[i])) {
      *continue_loop = builtins_func[i](args);
      return args[0];
    }
  }

  if ((pid = fork()) == -1) {
    perror("fork");
  } else if (pid > 0) { // Parent
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
    }

    *continue_loop = true;
  } else { // Child
    if (execvp(args[0], args) == -1) {
      perror("execvp");
      *continue_loop = false;
    }
  }

  return args[0];
}

char *prompt_init(const char *prompt) {
  size_t size = strlen(prompt);
  char *dy_prompt = emalloc(sizeof(char) * (size + 1));
  strncpy(dy_prompt, prompt, size + 1);
  return dy_prompt;
}

// Prompt can definitely be handled better. Plus
// you would really need a good parser to make the
// prompt better and more customisable.
char *prompt_update(char *prompt, char *command) {
  size_t prompt_size = strlen(prompt);
  size_t command_size = strlen(command);
  char *dy_prompt = emalloc(sizeof(char) * (prompt_size + command_size + 1));
  /*
   * This is an example of how it is being done.
   *
   * Consider these two null-terminated strings:
   *
   * Hello\0 and World\0
   *
   *           0 1 2 3 4 5
   * 0 1 2 3 4 5 6 7 8 9 10
   * H e l l o W o r l d \0
   * ^         ^       ^  ^
   * |         |       |  `- dy_prompt + command_size + prompt_size + 1
   * |         |       |
   * |         |       `- dy_prompt + command_size + prompt_size
   * |         |
   * |         `- dy_prompt + command_size
   * |
   * `- dy_prompt
   */
  strncpy(dy_prompt, command, command_size);
  strncpy(dy_prompt + command_size, prompt,
          prompt_size + 1); // + 1 to padd at the end with \0
  return dy_prompt;
}

/*
 * I should also take into consideration the possible benefit of
 * using a fixed amount of memory approach. It is really not
 * ideal. But it is possible and could technically work.
 */

int main(void) {
  bool continue_loop = true;
  char *line;
  char *command;
  char **args;
  struct token_vec *vec;

  char *prompt = prompt_init("> ");

  while (continue_loop == true && (line = linenoise(prompt)) != NULL) {
    if (*line != '\0') {
      vec = tokenise(line);
      args = token_vec_get_list(vec);
      command = sh_launch(args, &continue_loop);
      free(prompt);
      prompt = prompt_update(" > ", command);
      token_vec_deep_free(vec, args);
    }

    free(line);
  }

  free(prompt);

  return 0;
}
