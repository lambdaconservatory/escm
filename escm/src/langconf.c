/* langconf.c - make deflang.c
 * $Id$
 * Author: TAGA Yoshitaka <tagga@tsuda.ac.jp>
 */
#include "config.h"
#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  define EXIT_SUCCESS 0
#  define EXIT_FAILURE 1
   void exit(int status);
#endif /* HAVE_STDLIB_H */
#include <ctype.h>
#include "escm.h"
#include "misc.h"
#include "cmdline.h"

struct escm_lang* parse_lang(const char* name);

static void
put_string(const char* str, FILE* outp)
{
  const char* p = str;
  fputc('\"', outp);
  while (*p) {
    if (*p == '\"' || *p == '\\') {
      fputc('\\', outp);
      fputc(*p, outp);
    } else if (*p == '\n') {
      fputc('\\', outp);
      fputc('n', outp);
    } else {
      fputc(*p, outp);
    }
    p++;
  }
  fputc('\"', outp);
}
static void
put_string_or_null(const char* str, FILE* outp)
{
  if (str) {
    put_string(str, outp);
  } else {
    fputs("NULL", outp);
  }
}
static void
put_list(char** ptr, FILE* outp)
{
  int i;
  for (i = 0; ptr[i]; i++) {
    fputc(' ', outp);
    put_string_or_null(ptr[i], outp);
    fputc(',', outp);
  }
  
}

static void
put_three(char* tag, struct escm_form_three three, FILE* outp)
{
  fputs("  { /* ", outp);
  fputs(tag, outp);
  fputs(" */\n", outp);
  fputs("    ", outp);
  put_string_or_null(three.prefix, outp);
  fputs(",\n", outp);
  fputs("    ", outp);
  put_string_or_null(three.infix, outp);
  fputs(",\n", outp);
  fputs("    ", outp);
  put_string_or_null(three.suffix, outp);
  fputs(",\n", outp);
  fputs("  },\n", outp);
}

static void
put_two(char* tag, struct escm_form_two two, FILE* outp)
{
  fputs("  { /* ", outp);
  fputs(tag, outp);
  fputs(" */\n", outp);
  fputs("    ", outp);
  put_string_or_null(two.prefix, outp);
  fputs(",\n", outp);
  fputs("    ", outp);
  put_string_or_null(two.suffix, outp);
  fputs(",\n", outp);
  fputs("  },\n", outp);
}

int
main(int argc, const char** argv)
{
  struct escm_lang* lang = NULL;
  if (argc != 2) {
    fprintf(stderr, "Usage: %s ./lang/LANG\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  lang = parse_lang(argv[1]);
  if (lang == NULL) {
    fprintf(stderr, "Invalid language configuration: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }
#ifdef ESCM_BACKEND
  lang->backend = parse_cmdline(ESCM_BACKEND);
#endif /* ESCM_BACKEND */
  printf("/* src/deflang.c - generated from %s */\n", argv[1]);
  fputs(
"#ifdef HAVE_CONFIG_H\n"
"#include \"config.h\"\n"
"#endif\n"
"#include \"escm.h\"\n", stdout);
  fputs("static char *argv[] = {", stdout);
  put_list(lang->backend, stdout);
  fputs("};\n", stdout);
  fputs("struct escm_lang deflang = {\n", stdout);
  /* name */
  fputs("  ", stdout);
  put_string(lang->name, stdout);
  fputs(", /* name */\n", stdout);
  /* id_type */
  fputs("  ", stdout);
  if (lang->id_type == ESCM_ID_LISP) {
    fputs("ESCM_ID_LISP, /* id_type */\n", stdout);
  } else if (lang->id_type == ESCM_ID_UPPER) {
    fputs("ESCM_ID_UPPER, /* id_type */\n", stdout);
  } else if (lang->id_type == ESCM_ID_TITLE) {
    fputs("ESCM_ID_TITLE, /* id_type */\n", stdout);
  } else {
    fputs("ESCM_ID_LOWER, /* id_type */\n", stdout);
  }
  /* backend */
  fputs("  argv, /* backend */\n", stdout);
  /* literal */
  put_two("literal", lang->literal, stdout);
  /* display */
  put_two("display", lang->display, stdout);
  /* bind */
  put_three("bind", lang->bind, stdout);
  /* assign */
  put_three("assign", lang->assign, stdout);
  /* nil */
  fputs("  ", stdout);
  put_string_or_null(lang->nil, stdout);
  fputs(", /* nil */\n", stdout);
  /* newline */
  fputs("  ", stdout);
  put_string_or_null(lang->newline, stdout);
  fputs(", /* newline */\n", stdout);
  /* init */
  fputs("  ", stdout);
  put_string_or_null(lang->init, stdout);
  fputs(", /* init */\n", stdout);
  /* finish */
  fputs("  ", stdout);
  put_string_or_null(lang->finish, stdout);
  fputs(", /* finish */\n", stdout);
  fputs("};\n", stdout);
  fputs("/* end of deflang.c */\n", stdout);
  return 0;
}

/* end of langconf.c */