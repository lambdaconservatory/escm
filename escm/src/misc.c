/* misc.c - miscellaneous functions
 * $Id$
 * Author: TAGA Yoshitaka <tagga@tsuda.ac.jp>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#  include <stdlib.h>
#else /* not defined(HAVE_STDLIB_H) */
#  define EXIT_SUCCESS 0
#  define EXIT_FAILURE 1
void exit(int status);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
#endif /* HAVE_STDLIB_H */

#if defined(HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* defined(HAVE_UNISTD_H) */
#if !defined(HAVE_DUP2)
#  if defined(HAVE_FCNTL_H)
#    include <fcntl.h>
#  endif /* defined(HAVE_FCNTL_H) */
#endif /* !defined(HAVE_DUP2) */

#include <stdarg.h>
#include <errno.h>

#include "escm.h"
#include "misc.h"

#ifdef ENABLE_CGI
static int header = FALSE;

/* escm_html_header(&lang, outp)
 */
void
escm_html_header(const struct escm_lang *lang, FILE *outp)
{
  fputs("Content-type: text/html\r\n\r\n", outp);
  fflush(outp);
  header = TRUE;
}
/* escm_text_header(&lang, outp)
 */
void
escm_text_header(const struct escm_lang *lang, FILE *outp)
{
  fputs("Content-type: text/plain\r\n\r\n", outp);
  fflush(outp);
  header = TRUE;
}
#endif /* ENABLE_CGI */
/* escm_error(fmt, ...) - print a warning message and exit the program.
 */
void escm_error(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
#ifdef ENABLE_CGI
  if (escm_cgi) {
    if (!header) {
      fputs("Content-type: text/html\r\n\r\n", stdout);
      fputs("<html><body>", stdout);
    }
    printf("<p>%s: ", escm_prog);
    if (escm_file) printf("%s: ", escm_file);
    if (escm_lineno) printf("%d: ", escm_lineno);
    vfprintf(stdout, fmt, ap);
    fputs("</p></body></html>\n", stdout);
    exit(EXIT_SUCCESS);
  } else {
#endif /* ENABLE_CGI */
    fprintf(stderr, "%s: ", escm_prog);
    if (escm_file) fprintf(stderr, "%s: ", escm_file);
    if (escm_lineno) fprintf(stderr, "%d: ", escm_lineno);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
#ifdef ENABLE_CGI
  }
#endif /* ENABLE_CGI */
}

/*========================================================
 * memory allocation functions with error handling
 *=======================================================*/
/* escm_malloc(size) - malloc() 
 */
void *
escm_malloc(size_t size)
{
  void *ret;

  ret = malloc(size);
  if (ret == NULL) escm_error("memory exhausted");
  return ret;
}
/* escm_realloc(ptr, size) - realloc()
 */
void *
escm_realloc(void *ptr, size_t size)
{
  void *ret;

  if (ptr == NULL) ret = malloc(size);
  else ret = realloc(ptr, size);
  if (ret == NULL) escm_error("memory exhausted");
  return ret;
}

/*========================================================
 * redirection
 *=======================================================*/
void escm_redirect(int from, int to)
{
  int ret;
#ifdef HAVE_DUP2
  ret = dup2(to, from);
#else /* not HAVE_DUP2 */
  close(from);
  ret = fcntl(to, F_DUPFD, from);
#endif /* HAVE_DUP2 */

  if (ret < 0) escm_error("can't redirect a file or stream");
  /* the value itself is ignored. */
}

/*========================================================
 * missing functions
 *========================================================*/
#if !defined(HAVE_ATEXIT)
/* This implementation is NOT CORRECT at all. */
static void (*to_be_called)(void) = NULL;
static void wrapper(int ret, void *arg)
{
  to_be_called();
}
int atexit(void (*func)(void))
{
  if (to_be_called) return -1;
  to_be_called = func;
  on_exit(wrapper, NULL);
  return 0;
}
#endif /* !defined(HAVE_ATEXIT) */

#if !defined(HAVE_STRTOL)
long int
strtol(const char *p, const char *endptr, int base)
{
  long int ret = 0L;

  if (base != 10) escm_error("base must be 10");

  while ('0' < *p && *p < '9') {
    ret = ret * 10L + *p - '0';
  }
  endptr = p;
  return ret;
}
#endif /* HAVE_STRTOL) */

/* end of misc.c */