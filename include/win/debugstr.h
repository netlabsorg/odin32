#ifndef __WINE_DEBUGSTR_H
#define __WINE_DEBUGSTR_H

#include "windef.h"

/* These function return a printable version of a string, including
   quotes.  The string will be valid for some time, but not indefinitely
   as strings are re-used.  */

#define debugstr_a(a) debugstr_an(a, 80)
#define debugstr_w(a) debugstr_wn(a, 80)

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
