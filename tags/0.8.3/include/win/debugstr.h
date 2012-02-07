#ifndef __WINE_DEBUGSTR_H
#define __WINE_DEBUGSTR_H

#include "windef.h"

/* These function return a printable version of a string, including
   quotes.  The string will be valid for some time, but not indefinitely
   as strings are re-used.  */

#ifdef DEBUG
#define debugstr_a(a) debugstr_an(a, 80)
#define debugstr_w(a) debugstr_wn(a, 80)
#else
#define debugstr_a(a) ""
#define debugstr_w(a) ""
#endif

//extern LPSTR debugstr_a (LPCSTR s);
//extern LPSTR debugstr_w (LPCWSTR s);
extern LPSTR debugres_a (LPCSTR res);
extern LPSTR debugres_w (LPCWSTR res);
extern void debug_dumpstr (LPCSTR s);

#ifdef __GNUC__

#ifdef printf
#undef printf
#endif

extern int dbg_printf(const char *format, ...) __attribute__((format (printf,1,2)));

#define printf emx_printf

#else
extern int dbg_printf(const char *format, ...);
#endif

#endif /* __WINE_DEBUGSTR_H */
