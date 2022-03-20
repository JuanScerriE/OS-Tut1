#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  char *buf;  /* pointer to buffer */
  size_t len; /* length of the current buffer */
  size_t alc; /* allocated size of the buffer */
};

void token_grow(struct token *tok, size_t need) {
  tok->alc = tok->alc > 0 ? tok->alc : 1;

  while (tok->alc < need)
    tok->alc <<= 1;

  tok->buf = (char *)erealloc(tok->buf, sizeof(char) * tok->alc);
}

void token_init(struct token *tok, size_t est) {
  tok->buf = NULL;
  tok->len = 0;
  tok->alc = 0;

  if (est > 0)
    token_grow(tok, est);
}

struct token *token_new(void) {
  struct token *tok = (struct token *)emalloc(sizeof(struct token));
  token_init(tok, DEFAULT_SIZE);
  return tok;
}

void token_append(struct token *tok, char ch) {
  if (tok->len >= tok->alc)
    token_grow(tok, tok->alc + 1);

  tok->buf[tok->len++] = ch;
}

struct token_vec {
  struct token *arr;
  size_t len;
  size_t alc;
};

void token_vec_grow(struct token_vec *vec, size_t need) {
  vec->alc = vec->alc > 0 ? vec->alc : 1;

  while (vec->alc < need)
    vec->alc <<= 1;

  vec->arr =
      (struct token *)erealloc(vec->arr, sizeof(struct token) * vec->alc);
}

void token_vec_init(struct token_vec *vec, size_t est) {
  vec->arr = NULL;
  vec->len = 0;
  vec->alc = 0;

  if (est > 0)
    token_vec_grow(vec, est);
}

struct token_vec *token_vec_new(void) {
  struct token_vec *vec = (struct token_vec *)emalloc(sizeof(struct token_vec));
  token_vec_init(vec, DEFAULT_SIZE);
  return vec;
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
  struct peek_stream *stream = (struct peek_stream *)emalloc(sizeof(struct peek_stream));
  peek_stream_init(stream, str);
  return stream;
}

void peek_stream_free(struct peek_stream *stream) {
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

/* void token_vec_append(struct token_vec *vec, struct token *tok) { */
/*   if (tok->len >= tok->alc) */
/*     token_grow(tok, tok->alc + 1); */

/*   tok->buf[tok->len++] = ch; */
/* } */

struct token_vec *tokenise(char *line) {
  struct stream peek_stream;
  stream_init(&peek_stream, line);

  struct token_vec toks;
  token_vec_init(&toks, TOK_VEC_SIZE);

  char ch;

  while (!stream_end(&peek_stream)) {
    ch = stream_next(&peek_stream);

    if (!isspace(ch) && ch != '\0') {
      token_init(toks[toks.len], TOK_SIZE);

      do {
        token_append(toks[len], ch);
        ch = stream_next(&peek_stream);
      } while (!isspace(ch) && ch != '\0');

      len++;
    }
  }

  printf("len = %lu\n", len);

  toks[len] = NULL;

  return toks;
}

int main(void) {
  char *line;

  while ((line = linenoise("> ")) != NULL) {
    if (*line != '\0') {
      struct token **toks = tokenise(line);
      for (int i = 0; toks[i] != NULL; i++) {
        printf("%s\n", toks[i]->buf);
      }
      /* printf("echo: %s\n", line); */
    }

    free(line);
  }

  return 0;
}
