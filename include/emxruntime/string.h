/* string.h (emx+gcc) */

#ifndef _EMXSTRING_H
#define _EMXSTRING_H

#ifndef CRTWRAP
#define CRTWRAP(a)   emx_##a
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#if !defined (_SIZE_T)
#define _SIZE_T
typedef unsigned long size_t;
#endif

#if !defined (NULL)
#if defined (__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#define memchr CRTWRAP(memchr)
#define memcmp CRTWRAP(memcmp)
#define memcpy CRTWRAP(memcpy)
#define memmove CRTWRAP(memmove)
#define memset CRTWRAP(memset)

void *memchr (__const__ void *, int, size_t);
int  memcmp (__const__ void *, __const__ void *, size_t);
void *memcpy (void *, __const__ void *, size_t);
void *memmove (void *, __const__ void *, size_t);
void *memset (void *, int, size_t);

#define strcat CRTWRAP(strcat)
#define strchr CRTWRAP(strchr)
#define strcmp CRTWRAP(strcmp)
#define strcoll CRTWRAP(strcoll)
#define strcpy CRTWRAP(strcpy)
#define strcspn CRTWRAP(strcspn)
#define strerror CRTWRAP(strerror)
#define strlen CRTWRAP(strlen)
#define strncat CRTWRAP(strncat)
#define strncmp CRTWRAP(strncmp)
#define strncpy CRTWRAP(strncpy)
#define strpbrk CRTWRAP(strpbrk)
#define strrchr CRTWRAP(strrchr)
#define strspn CRTWRAP(strspn)
#define strstr CRTWRAP(strstr)
#define strtok CRTWRAP(strtok)
#define strxfrm CRTWRAP(strxfrm)

char *strcat (char *, __const__ char *);
char *strchr (__const__ char *, int);
int strcmp (__const__ char *, __const__ char *);
int strcoll (__const__ char *, __const__ char *);
char *strcpy (char *, __const__ char *);
size_t strcspn (__const__ char *, __const__ char *);
char *strerror (int);
size_t strlen (__const__ char *);
char *strncat (char *, __const__ char *, size_t);
int strncmp (__const__ char *, __const__ char *, size_t);
char *strncpy (char *, __const__ char *, size_t);
char *strpbrk (__const__ char *, __const__ char *);
char *strrchr (__const__ char *, int);
size_t strspn (__const__ char *, __const__ char *);
char *strstr (__const__ char *, __const__ char *);
char *strtok (char *, __const__ char *);
size_t strxfrm (char *, __const__ char *, size_t);


#if !defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)

#if !defined (_MEMDIF_EQ)
#define _MEMDIF_EQ 0xffffffff
#endif


#define memccpy CRTWRAP(memccpy)
#define memicmp CRTWRAP(memicmp)

void *memccpy (void *, __const__ void *, int, size_t);
int memicmp (__const__ void *, __const__ void *, size_t);

#define strdup CRTWRAP(strdup)
#define stricmp CRTWRAP(stricmp)
#define strlwr CRTWRAP(strlwr)
#define strnicmp CRTWRAP(strnicmp)
#define strnset CRTWRAP(strnset)
#define strrev CRTWRAP(strrev)
#define strset CRTWRAP(strset)
#define strupr CRTWRAP(strupr)

#define strsep CRTWRAP(strsep)

char *strdup (__const__ char *);
int stricmp (__const__ char *, __const__ char *);
char *strlwr (char *);
int strnicmp (__const__ char *, __const__ char *, size_t);
char *strnset (char *, int, size_t);
char *strrev (char *);
char *strset (char *, int);
char *strupr (char *);

char *strsep (char **, __const__ char *); /* BSD */

#endif

#define _memcount CRTWRAP(_memcount)
#define _memdif CRTWRAP(_memdif)
#define _memrchr CRTWRAP(_memrchr)
#define _memswap CRTWRAP(_memswap)
#define _strncpy CRTWRAP(_strncpy)
#define _memccpy CRTWRAP(_memccpy)
#define _memicmp CRTWRAP(_memicmp)
#define _strdup CRTWRAP(_strdup)
#define _stricmp CRTWRAP(_stricmp)
#define _strlwr CRTWRAP(_strlwr)
#define _strnicmp CRTWRAP(_strnicmp)
#define _strnset CRTWRAP(_strnset)
#define _strrev CRTWRAP(_strrev)
#define _strset CRTWRAP(_strset)
#define _strupr CRTWRAP(_strupr)
#define _strsep CRTWRAP(_strsep)

#if (!defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)) \
    || defined (_WITH_UNDERSCORE)

size_t _memcount (__const__ void *, int, size_t);
size_t _memdif (__const__ void *, __const__ void *, size_t);
void *_memrchr (__const__ void *, int, size_t);
void _memswap (void *, void *, size_t);
char *_strncpy (char *, __const__ char *, size_t);

void *_memccpy (void *, __const__ void *, int, size_t);
int _memicmp (__const__ void *, __const__ void *, size_t);
char *_strdup (__const__ char *);
int _stricmp (__const__ char *, __const__ char *);
char *_strlwr (char *);
int _strnicmp (__const__ char *, __const__ char *, size_t);
char *_strnset (char *, int, size_t);
char *_strrev (char *);
char *_strset (char *, int);
char *_strupr (char *);
char *_strsep (char **, __const__ char *);

#endif


#if defined (__cplusplus)
}
#endif

#endif /* not _STRING_H */
