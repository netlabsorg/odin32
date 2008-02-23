/* ctype.h (emx+gcc) */

#ifndef _EMXCTYPE_H
#define _EMXCTYPE_H

#ifndef CRTWRAP
#define CRTWRAP(a)   emx_##a
#endif

#if defined (__cplusplus)
extern "C" {
#endif

extern unsigned char emx__ctype[];

/* ASCII char classification table - binary compatible */
#define _UPPER        0x0001  /* C1_UPPER */
#define _LOWER        0x0002  /* C1_LOWER */
#define _DIGIT        0x0004  /* C1_DIGIT */
#define _SPACE        0x0008  /* C1_SPACE */
#define _PUNCT        0x0010  /* C1_PUNCT */
#define _CONTROL      0x0020  /* C1_CNTRL */
#define _BLANK        0x0040  /* C1_BLANK */
#define _HEX          0x0080  /* C1_XDIGIT */
#define _LEADBYTE     0x8000
#define _ALPHA       (0x0100|_UPPER|_LOWER)  /* (C1_ALPHA|_UPPER|_LOWER) */

#if defined (__cplusplus)

extern inline int isalnum (int _c)
  { return (emx__ctype+1)[_c] & (_UPPER|_LOWER|_DIGIT); }
extern inline int isalpha (int _c)
  { return (emx__ctype+1)[_c] & (_UPPER|_LOWER); }
extern inline int iscntrl (int _c)
  { return (emx__ctype+1)[_c] & (_CNTRL); }
extern inline int isdigit (int _c)
  { return (emx__ctype+1)[_c] & (_DIGIT); }
extern inline int isgraph (int _c)
  { return (emx__ctype+1)[_c] & (_PUNCT|_UPPER|_LOWER|_DIGIT); }
extern inline int islower (int _c)
  { return (emx__ctype+1)[_c] & (_LOWER); }
extern inline int isprint (int _c)
  { return (emx__ctype+1)[_c] & (_PRINT); }
extern inline int ispunct (int _c)
  { return (emx__ctype+1)[_c] & (_PUNCT); }
extern inline int isspace (int _c)
  { return (emx__ctype+1)[_c] & (_SPACE); }
extern inline int isupper (int _c)
  { return (emx__ctype+1)[_c] & (_UPPER); }
extern inline int isxdigit (int _c)
  { return (emx__ctype+1)[_c] & (_XDIGIT); }

#else

#define isalnum(c)  ((emx__ctype+1)[c] & (_UPPER|_LOWER|_DIGIT))
#define isalpha(c)  ((emx__ctype+1)[c] & (_UPPER|_LOWER))
#define iscntrl(c)  ((emx__ctype+1)[c] & (_CNTRL))
#define isdigit(c)  ((emx__ctype+1)[c] & (_DIGIT))
#define isgraph(c)  ((emx__ctype+1)[c] & (_PUNCT|_UPPER|_LOWER|_DIGIT))
#define islower(c)  ((emx__ctype+1)[c] & (_LOWER))
#define isprint(c)  ((emx__ctype+1)[c] & (_PRINT))
#define ispunct(c)  ((emx__ctype+1)[c] & (_PUNCT))
#define isspace(c)  ((emx__ctype+1)[c] & (_SPACE))
#define isupper(c)  ((emx__ctype+1)[c] & (_UPPER))
#define isxdigit(c) ((emx__ctype+1)[c] & (_XDIGIT))

int (isalnum)(int);
int (isalpha)(int);
int (iscntrl)(int);
int (isdigit)(int);
int (isgraph)(int);
int (islower)(int);
int (isprint)(int);
int (ispunct)(int);
int (isspace)(int);
int (isupper)(int);
int (isxdigit)(int);

#define toupper CRTWRAP(toupper)
#define tolower CRTWRAP(tolower)
#define _toupper CRTWRAP(_toupper)
#define _tolower CRTWRAP(_tolower)

int toupper (int);
int tolower (int);
int _toupper (int);
int _tolower (int);

#endif

#if !defined (CTYPE_FUN)
extern __inline__ int _toupper (int _c) { return (_c-'a'+'A'); }
extern __inline__ int _tolower (int _c) { return (_c-'A'+'a'); }
extern __inline__ int toupper(int _c)
  {return (islower(_c) ? _toupper(_c) : _c);}
extern __inline__ int tolower(int _c)
  {return (isupper(_c) ? _tolower(_c) : _c);}
#endif

#if !defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)

int (toascii)(int);
int (isascii)(int);

#define isascii(c)  ((unsigned)(c) <= 0x7f)
#define toascii(c)  ((c) & 0x7f)

#endif

#if defined (__cplusplus)
}
#endif

#endif /* not emx__ctype_H */
