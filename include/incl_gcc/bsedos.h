/** @file
 *
 * GCC-9.2 / kLIBC compatibility shim for <bsedos.h>  (Odin32 build revival).
 *
 * The current kLIBC <bsedos.h> declares DosSetPriority() with a parameter
 * literally named `class` (`ULONG class`), which is a reserved keyword in C++
 * and makes every C++ translation unit that pulls the OS/2 process APIs fail
 * to compile ("expected identifier before ',' token" / "two or more data
 * types...").  The kLIBC headers 0.9.0 was built against (GCC 4.9.2 era) used a
 * non-keyword parameter name, so this is a header regression, not our code.
 *
 * We must not patch the system header, so this file shadows it (Odin's
 * include/incl_gcc is earlier on the -I search path than /usr/include):
 * neutralise the offending C++ keywords, pull the real header via
 * #include_next, then restore them so Odin's own C++ code is unaffected.
 *
 * Odin's own DosSetPriority override lives in include/os2wrap.h; this only
 * makes the *declaration* in the system header parse under C++.
 */
#ifndef ODIN_BSEDOS_CPPKW_SHIM_H
#define ODIN_BSEDOS_CPPKW_SHIM_H

#define class   _os2kw_class

#include_next <bsedos.h>

#undef class

#endif /* ODIN_BSEDOS_CPPKW_SHIM_H */
