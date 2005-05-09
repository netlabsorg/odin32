/*
 * Utility routines' prototypes etc.
 *
 * Copyright 1998 Bertho A. Stultiens (BS)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __WIDL_UTILS_H
#define __WIDL_UTILS_H

#include "widltypes.h"

#include <stddef.h>	/* size_t */

#if defined(__DEBUG_ALLOC__) && defined(__IBMC__)
void *_xmalloc(size_t, char*, int);
void *_xrealloc(void *, size_t, char*, int);
char *_xstrdup(const char *, char*, int);
#define xmalloc(a)      _xmalloc(a, __FILE__, __LINE__)
#define xrealloc(a, b)  _xrealloc(a, b, __FILE__, __LINE__)
#define xstrdup(a)      _xstrdup(a, __FILE__, __LINE__)
#else
void *xmalloc(size_t);
void *xrealloc(void *, size_t);
char *xstrdup(const char *str);

#ifndef __GNUC__
#define __attribute__(X)
#endif

#endif

int yyerror(const char *s, ...) __attribute__((format (printf, 1, 2)));
int yywarning(const char *s, ...) __attribute__((format (printf, 1, 2)));
void internal_error(const char *file, int line, const char *s, ...) __attribute__((format (printf, 3, 4)));
void error(const char *s, ...) __attribute__((format (printf, 1, 2)));
void warning(const char *s, ...) __attribute__((format (printf, 1, 2)));
void chat(const char *s, ...) __attribute__((format (printf, 1, 2)));

char *dup_basename(const char *name, const char *ext);

#if defined(__IBMC__) || defined(__IBMCPP__)
#define write _write
#define close _close
#define creat _creat

/* Borrowed from Apache NT Port and PHP */

extern char *ap_php_optarg;
extern int   ap_php_optind;
extern int   ap_php_opterr;
extern int   ap_php_optopt;
int ap_php_getopt(int argc, char* const *argv, const char *optstr);

#define getopt  ap_php_getopt
#define optarg  ap_php_optarg
#define optind  ap_php_optind
#define opterr  ap_php_opterr
#define optopt  ap_php_optopt

#endif

#endif
