/* fork.c - `pipe', `fork' and `exec' version server.
 * $Id$
 *
 * Author: TAGA Yoshitaka <tagga@tsuda.ac.jp>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#include <errno.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif /* HAVE_SYS_WAIT_H */
#include "escm.h"

static void
sigterm(void)
{
  kill(0, SIGTERM);
}

FILE *
escm_popen(char * const argv[])
{
  int fd[2];
  pid_t pid;

  if (pipe(fd) < 0) escm_error(NULL);
  if ((pid = fork()) < 0)  escm_error(NULL);
  if (pid > 0) { /* parent process */
    FILE *pipe;

    close(fd[0]); /* read */
    pipe = fdopen(fd[1], "w"); /* write */
    if (pipe == NULL) {
      int this_errno = errno;
      kill(pid, SIGTERM);
      errno = this_errno;
      escm_error(NULL);
    }
    return pipe;
  } else { /* child process */
    atexit(sigterm);

    close(fd[1]); /* write */
    /* connect fd[0] to the child process's stdin */
    if (dup2(fd[0], 0) < 0) escm_error(NULL);
    /* Invoke the interpreter */
    if (argv[0]) execvp(argv[0], argv);
    /* never reached if successful. */
    escm_error(gettext("can't invoke - %s"), argv[0]);
    return NULL; /* dummy */
  }
}

int
escm_pclose(FILE *pipe)
{
  int status;

  fflush(pipe);
  fclose(pipe);
  /* wait till the child process exits. */
  wait(&status);
  if (WIFEXITED(status)) return WEXITSTATUS(status);
  else return -1;
}
/* end of fork.c */
