/*
 * Utility routines' prototypes etc.
 *
 * Copyright 1998,2000 Bertho A. Stultiens (BS)
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

#ifndef __WMC_UTILS_H
#define __WMC_UTILS_H

#ifndef __WMC_WMCTYPES_H
#include "wmctypes.h"
#endif

#include <stddef.h>	/* size_t */

void *xmalloc(size_t);
void *xrealloc(void *, size_t);
char *xstrdup(const char *str);

int yyerror(const char *s, ...) __attribute__((format (printf, 1, 2)));
int xyyerror(const char *s, ...) __attribute__((format (printf, 1, 2)));
int yywarning(const char *s, ...) __attribute__((format (printf, 1, 2)));
void internal_error(const char *file, int line, const char *s, ...) __attribute__((format (printf, 3, 4)));
void error(const char *s, ...) __attribute__((format (printf, 1, 2)));
void warning(const char *s, ...) __attribute__((format (printf, 1, 2)));

char *dup_basename(const char *name, const char *ext);

WCHAR *xunistrdup(const WCHAR * str);
WCHAR *unistrcpy(WCHAR *dst, const WCHAR *src);
int unistrlen(const WCHAR *s);
int unistricmp(const WCHAR *s1, const WCHAR *s2);
int unistrcmp(const WCHAR *s1, const WCHAR *s2);

#if defined(__IBMC__) || defined(__IBMCPP__)

#undef strcasecmp
INT WINAPI strcasecmp(LPCSTR p1, LPCSTR p2);

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
