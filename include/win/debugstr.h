/* $Id: debugstr.h,v 1.3 2000-08-30 13:56:37 sandervl Exp $ */
#ifndef __WINE_DEBUGSTR_H
#define __WINE_DEBUGSTR_H

#include "windef.h"

/* These function return a printable version of a string, including
   quotes.  The string will be valid for some time, but not indefinitely
   as strings are re-used.  */

#define debugstr_a(a) a
#define debugstr_w(a) a

//extern LPSTR debugstr_a (LPCSTR s);
//extern LPSTR debugstr_w (LPCWSTR s);
extern LPSTR debugres_a (LPCSTR res);
extern LPSTR debugres_w (LPCWSTR res);
extern void debug_dumpstr (LPCSTR s);

#ifdef __GNUC__
extern int dbg_printf(const char *format, ...) __attribute__((format (printf,1,2)));
#else
extern int dbg_printf(const char *format, ...);
#endif

#endif /* __WINE_DEBUGSTR_H */
