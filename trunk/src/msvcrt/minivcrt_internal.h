/*
 * Header file for the stripped down version of MSVCRT that only
 * contains functions specific to the MS VC Runtime (internal definitions).
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _minivcrt_internal_h_
#define _minivcrt_internal_h_

#ifdef __cplusplus
extern "C" {
#endif

void MSVCRT__set_errno(int err);

/*char*  msvcrt_strndup(const char*,unsigned int);*/
LPWSTR msvcrt_wstrndup(LPCWSTR, unsigned int);

#define MSVCRT_wchar_t  WCHAR
#define MSVCRT_size_t   size_t

#define MSVCRT__errno       _errno
#define MSVCRT__getdrive    _getdrive
#ifndef __EMX__
#define MSVCRT__fullpath    _fullpath
#endif
#define MSVCRT_malloc       malloc

#define TRACE 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#define FIXME 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#define debugstr_w(a) ""
#define dprintf(a) do {} while(0)

#ifdef __cplusplus
}
#endif

#endif /* _minivcrt_internal_h_ */
