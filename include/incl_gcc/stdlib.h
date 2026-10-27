/** @file
 *
 * GCC-9.2 / kLIBC compatibility shim for <stdlib.h>  (Odin32 build revival).
 *
 * odin.h defines the lowercase min()/max() macros this Wine-era codebase uses
 * everywhere, but the kLIBC <stdlib.h> (under C++) drops them, so any file that
 * includes <stdlib.h> after odin.h loses min/max and fails to compile
 * (e.g. src/kernel32/conbuffer.cpp).  Odin's include/incl_gcc is earlier on the
 * -I search path than /usr/include, so this shim pulls the real header via
 * #include_next and then re-asserts min/max.
 *
 * Guarded on __WIN32OS2__ (defined on the command line for every Odin compile
 * via Config.kmk, so it is true for Odin TUs regardless of whether odin.h has
 * been included yet at this point, and absent when system / non-Odin code is
 * compiled) so plain C / non-Odin users of <stdlib.h> are left exactly as the
 * system header leaves them.  No include guard here on purpose: re-asserting
 * after every include is harmless (the #ifndef checks make it idempotent).
 */
#include_next <stdlib.h>

#if defined(__WIN32OS2__) && !defined(ODIN_NO_MINMAX)
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif /* __WIN32OS2__ && !ODIN_NO_MINMAX */
