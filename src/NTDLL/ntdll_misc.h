#ifndef __WINE_NTDLL_MISC_H
#define __WINE_NTDLL_MISC_H

#include "ntdef.h"
#include "winnt.h"

/* debug helper */
extern LPCSTR debugstr_as( const STRING *str );
extern LPCSTR debugstr_us( const UNICODE_STRING *str );

#ifdef DEBUG
extern void dump_ObjectAttributes (const OBJECT_ATTRIBUTES *ObjectAttributes);
#else
#define dump_ObjectAttributes(a)
#endif

extern void dump_UnicodeString(const UNICODE_STRING *us, BOOLEAN showstring);

#endif
