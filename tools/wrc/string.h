/* string.h (emx+gcc) */

#ifndef _STRING_H
#define _STRING_H

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

void *memchr (__const__ void *, int, size_t);
int memcmp (__const__ void *, __const__ void *, size_t);
void *memcpy (void *, __const__ void *, size_t);
void *memmove (void *, __const__ void *, size_t);
void *memset (void *, int, size_t);

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


void *memccpy (void *, __const__ void *, int, size_t);
int memicmp (__const__ void *, __const__ void *, size_t);

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
