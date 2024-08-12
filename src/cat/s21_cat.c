#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct catFlags {
  int b, e, n, s, t, v;
} catFlags;

void parse_arguments(int argc, char *argv[]);
void handle_option(int opt, catFlags *flags);
void cat(FILE *fp, catFlags *flags);
void flag_n(int *line_number, int blank_b);
void flag_t();
int flag_v(char character, char *prev_character);
void flag_b(char character, int *blank_b, int *line_number);

int main(int argc, char *argv[]) {
  parse_arguments(argc, argv);

  return 0;
}

void cat(FILE *fp, catFlags *flags) {
  int line_number = 0;
  char character, prev_character = '\n';
  int blank_s = 0, blank_b = 0;
  int first_char = 0;

  while ((character = getc(fp)) != EOF) {
    if (prev_character == '\n') {
      first_char = 1;
    } else {
      first_char = 0;
    }

    if (flags->s && first_char) {
      if (character == '\n') {
        if (blank_s) {
          continue;
        } else {
          blank_s = 1;
        }
      } else {
        blank_s = 0;
      }
    }

    if (flags->b && first_char) {
      flag_b(character, &blank_b, &line_number);
    }

    if (flags->n && first_char && !flags->b) {
      flag_n(&line_number, blank_b);
    }

    if (flags->t && character == '\t') {
      flag_t();
      prev_character = character;
      continue;
    }

    if (flags->e && character == '\n') {
      printf("$");
    }

    if (flags->v) {
      if (flag_v(character, &prev_character)) continue;
    }

    prev_character = character;

    printf("%c", character);
  }
}

void handle_option(int opt, catFlags *flags) {
  switch (opt) {
    case 'b':
      flags->b = 1;
      break;
    case 'n':
      flags->n = 1;
      break;
    case 's':
      flags->s = 1;
      break;
    case 'e':
      flags->v = 1;
      flags->e = 1;
      break;
    case 't':
      flags->v = 1;
      flags->t = 1;
      break;
    case 'v':
      flags->v = 1;
      break;
    case 'E':
      flags->e = 1;
      break;
    case 'T':
      flags->t = 1;
      break;
    case '?':
      fprintf(stderr, "cat: illegal option -- %c\n", optopt);
      exit(1);
      break;
    default:
      fprintf(stderr, "Unexpected error\n");
      exit(1);
      break;
  }
}

void parse_arguments(int argc, char *argv[]) {
  int opt;

  opterr = 0;

  catFlags flags = {0, 0, 0, 0, 0, 0};

  struct option long_options[] = {{"number-nonblank", 0, NULL, 'b'},
                                  {"number", 0, NULL, 'n'},
                                  {"squeeze-blank", 0, NULL, 's'},
                                  {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "+bnstevET", long_options, NULL)) !=
         -1) {
    handle_option(opt, &flags);
  }

  if (optind == argc) {
    cat(stdin, &flags);
  } else {
    for (int i = optind; i < argc; i++) {
      FILE *fp = fopen(argv[i], "r");
      if (fp == NULL) {
        fprintf(stderr, "cat: %s: No such file or directory\n", argv[i]);
        exit(1);
      } else {
        cat(fp, &flags);
        fclose(fp);
      }
    }
  }
}

void flag_n(int *line_number, int blank_b) {
  if (!blank_b) {
    printf("%6d\t", ++(*line_number));
  }
}

void flag_t() {
  putchar('^');
  putchar('I');
}

int flag_v(char character, char *prev_character) {
  int flag = 0;

  if (!isascii(character) && !isprint(character)) {
    putchar('M');
    putchar('-');
    character = toascii(character);
  }
  if (iscntrl(character) && character != '\n' && character != '\t') {
    putchar('^');
    putchar(character == 127 ? '?' : character + 64);
    *prev_character = character;
    flag = 1;
  }
  return flag;
}

void flag_b(char character, int *blank_b, int *line_number) {
  if (character == '\n') {
    if (!(*blank_b)) {
      *blank_b = 1;
    }
  } else {
    *blank_b = 0;
    printf("%6d\t", ++(*line_number));
  }
}