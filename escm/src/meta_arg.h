/* meta_argc.h - add meta-argument functionality
 * $Id$
 * Author: TAGA Yoshitaka <tagga@tsuda.ac.jp>
 */
#ifndef META_ARG_H
#  define META_ARG_H 1
#include <stdio.h> /* defines NULL and FILE */

#include "escm.h"
#include "misc.h"
#define XPROG escm_prog
#define XFILE escm_file
#define XLINENO escm_lineno
#define XERROR0(str) escm_error(str)
#define XERROR1(fmt, arg) escm_error(fmt, arg)

/* action handler CGI program */
#define META_ACTION 1

#ifndef _
#  define _(str) str
#endif /* !_ */

#ifdef META_ACTION
FILE * meta_expand(int *pinterpc, char ***pinterpv,
		   int *pscriptc, char ***pscriptv,
		   char *optstr, char *path);
#else /* !META_ACTION */
FILE * meta_expand(int *pinterpc, char ***pinterpv,
		   int *pscriptc, char ***pscriptv,
		   char *optstr);
#endif /* META_ACTION */

#endif /* !META_ARG_H */
