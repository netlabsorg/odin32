/*
 * Compilers that uses ILP32, LP64 or P64 type models
 * for both Win32 and Win64 are supported by this file.
 *
 * Copyright (C) 1999 Patrik Stridvall
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

#ifndef __WINE_BASETSD_H
#define __WINE_BASETSD_H

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

/*
 * Win32 was easy to implement under Unix since most (all?) 32-bit
 * Unices uses the same type model (ILP32) as Win32, where int, long
 * and pointer are 32-bit.
 *
 * Win64, however, will cause some problems when implemented under Unix.
 * Linux/{Alpha, Sparc64} and most (all?) other 64-bit Unices uses
 * the LP64 type model where int is 32-bit and long and pointer are
 * 64-bit. Win64 on the other hand uses the P64 (sometimes called LLP64)
 * type model where int and long are 32 bit and pointer is 64-bit.
 */

/* Type model independent typedefs */
/* The __intXX types are native types defined by the MS C compiler.
 * Apps that make use of them before they get defined here, can
 * simply add to the command line:
 *    -D__int8=char -D__int16=short -D__int32=int "-D__int64=long long"
 */
#if !defined(_MSC_VER) && !defined(__WIDL__)
#  ifndef __int8
#    define __int8  char
#  endif
#  ifndef __int16
#    define __int16 short
#  endif
#  ifndef __int32
#    define __int32 int
#  endif
#  ifndef __int64
#    if defined(__WIN32OS2__) && (__IBMC__ < 400) && (__IBMCPP__ < 360) && !defined(__WATCOMC__) && !defined(__EMX__)
#      define __int64 double
#    elif !defined(__WIN32OS2__) || defined(__EMX__)
#      define __int64 long long
#    endif
#  endif
#endif /* !defined(_MSC_VER) */

/* FIXME: DECLSPEC_ALIGN should be declared only in winnt.h, but we need it here too */
#ifndef DECLSPEC_ALIGN
# if defined(_MSC_VER) && (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#  define DECLSPEC_ALIGN(x) __declspec(align(x))
# elif defined(__GNUC__)
#  define DECLSPEC_ALIGN(x) __attribute__((aligned(x)))
# else
#  define DECLSPEC_ALIGN(x)
# endif
#endif

typedef signed char      INT8, *PINT8;
typedef signed short     INT16, *PINT16;
typedef signed int       INT32, *PINT32;
typedef unsigned char    UINT8, *PUINT8;
typedef unsigned short   UINT16, *PUINT16;
typedef unsigned int     UINT32, *PUINT32;
typedef signed int       LONG32, *PLONG32;
typedef unsigned int     ULONG32, *PULONG32;
typedef unsigned int     DWORD32, *PDWORD32;

#ifdef _MSC_VER
typedef signed __int64   INT64, *PINT64;
typedef unsigned __int64 UINT64, *PUINT64;
typedef signed __int64   LONG64, *PLONG64;
typedef unsigned __int64 ULONG64, *PULONG64;
typedef unsigned __int64 DWORD64, *PDWORD64;
#elif defined(__WIN32OS2__) && (__IBMC__ < 400) && (__IBMCPP__ < 360) && !defined(__WATCOMC__) && !defined(__EMX__)
typedef __int64 INT64, *PINT64;
typedef __int64 UINT64, *PUINT64;
typedef __int64 LONG64, *PLONG64;
typedef __int64 ULONG64, *PULONG64;
typedef __int64 DWORD64, *PDWORD64;
#else
typedef signed __int64   DECLSPEC_ALIGN(8) INT64, *PINT64;
typedef unsigned __int64 DECLSPEC_ALIGN(8) UINT64, *PUINT64;
typedef signed __int64   DECLSPEC_ALIGN(8) LONG64, *PLONG64;
typedef unsigned __int64 DECLSPEC_ALIGN(8) ULONG64, *PULONG64;
typedef unsigned __int64 DECLSPEC_ALIGN(8) DWORD64, *PDWORD64;
#endif

/* Win32 or Win64 dependent typedef/defines. */

#ifdef _WIN64

typedef signed __int64   INT_PTR, *PINT_PTR;
typedef signed __int64   LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
typedef unsigned __int64 DWORD_PTR, *PDWORD_PTR;

#define MAXINT_PTR 0x7fffffffffffffff
#define MININT_PTR 0x8000000000000000
#define MAXUINT_PTR 0xffffffffffffffff

typedef int HALF_PTR, *PHALF_PTR;
typedef unsigned int UHALF_PTR, *PUHALF_PTR;

#define MAXHALF_PTR 0x7fffffff
#define MINHALF_PTR 0x80000000
#define MAXUHALF_PTR 0xffffffff

#else /* FIXME: defined(_WIN32) */

#ifndef INT_PTR_D
#define INT_PTR_D
typedef int INT_PTR, *PINT_PTR;
#endif
typedef long LONG_PTR, *PLONG_PTR;
#ifndef UINT_PTR_D
#define UINT_PTR_D
typedef unsigned int UINT_PTR, *PUINT_PTR;
#endif
typedef unsigned long ULONG_PTR, *PULONG_PTR;
#ifndef DWORD_PTR_D
#define DWORD_PTR_D
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
#endif

#define MAXINT_PTR 0x7fffffff
#define MININT_PTR 0x80000000
#define MAXUINT_PTR 0xffffffff

typedef signed short HALF_PTR, *PHALF_PTR;
typedef unsigned short UHALF_PTR, *PUHALF_PTR;

#define MAXUHALF_PTR 0xffff
#define MAXHALF_PTR 0x7fff
#define MINHALF_PTR 0x8000

#endif /* defined(_WIN64) || defined(_WIN32) */

typedef LONG_PTR SSIZE_T, *PSSIZE_T;
typedef ULONG_PTR SIZE_T, *PSIZE_T;

/* Some Wine-specific definitions */

/* Architecture dependent settings. */
/* These are hardcoded to avoid dependencies on config.h in Winelib apps. */
#if defined(__i386__)
# undef  WORDS_BIGENDIAN
# undef  BITFIELDS_BIGENDIAN
# define ALLOW_UNALIGNED_ACCESS
#elif defined(__sparc__)
# define WORDS_BIGENDIAN
# define BITFIELDS_BIGENDIAN
# undef  ALLOW_UNALIGNED_ACCESS
#elif defined(__powerpc__)
# define WORDS_BIGENDIAN
# define BITFIELDS_BIGENDIAN
# undef  ALLOW_UNALIGNED_ACCESS
#elif defined(__ALPHA__)
# undef  WORDS_BIGENDIAN
# undef  BITFIELDS_BIGENDIAN
# undef  ALLOW_UNALIGNED_ACCESS
#elif !defined(RC_INVOKED) && !defined(__WIDL__)
# error Unknown CPU architecture!
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif /* !defined(__WINE_BASETSD_H) */
