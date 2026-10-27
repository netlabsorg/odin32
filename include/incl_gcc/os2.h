/** @file
 *
 * GCC-9.2 / kLIBC compatibility shim for <os2.h>  (Odin32 build revival).
 *
 * With USE_OS2_TOOLKIT_HEADERS the system <os2.h> pulls <os2tk.h> and the
 * Toolkit's <os2def.h>, which on this system does not define PCSZ (the
 * GCC-native <os2emx.h> path does).  Odin's DosXxx wrappers and a number of
 * sources use PCSZ as "const char *", so provide it after the real header.
 * Odin's own definitions (winos2def.h, odincrt.h, os2wrap.h) use the same type.
 *
 * Odin's include/incl_gcc is earlier on the -I path than /usr/include, so this
 * file shadows the system <os2.h> and chains to it with #include_next.
 */
#ifndef ODIN_OS2_H_SHIM
#define ODIN_OS2_H_SHIM

/* Remember, before the system header runs, whether the source itself asked for
   the Workplace Shell API.  (INCL_PM sets INCL_WINWORKPLACE inside pm.h, which
   says nothing about the source wanting WPS, so test only what the source set.) */
#if defined(INCL_WPCLASS) || defined(INCL_WP) || defined(INCL_WINWORKPLACE)
#define ODIN_OS2_WANT_WPS
#endif

#include_next <os2.h>

/* pm.h only includes <pmwp.h> for IBM C (#ifndef __IBMC__ ... #else #include
   <pmwp.h>), so with GCC HOBJECT, WinCreateObject, CO_* etc. are never declared
   even when INCL_WPCLASS / INCL_WINWORKPLACE is set.  Include it explicitly. */
#ifdef ODIN_OS2_WANT_WPS
#include <pmwp.h>
#endif

/* Declared as a macro alias by os2wrap2.h in some translation units; only add
   the typedef when it is not already a macro and nobody typedef'd it. */
#if !defined(PCSZ) && !defined(ODIN_PCSZ_DEFINED)
#define ODIN_PCSZ_DEFINED
typedef const char *PCSZ;
#endif

#endif /* ODIN_OS2_H_SHIM */
