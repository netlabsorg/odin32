/* stdio.h (emx+gcc) */

#ifndef _EMXSTDIO_H
#define _EMXSTDIO_H

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

#if !defined (BUFSIZ)
#define BUFSIZ 5120
#endif

#if !defined (_FILE_T)
#define _FILE_T
#define _FILE_MEMBERS_HAVE_UNDERSCORE
struct _file2;
struct _FILE
{
  char * _ptr;
  char * _buffer;
  int    _rcount;
  int    _wcount;
  int    _handle;
  int    _flags;
  int    _buf_size;
  int    _tmpidx;
  int    _pid;
  char   _char_buf;
  unsigned char _ungetc_count;
  short  _mbstate;
  int    (*CRTWRAP(_flush))(struct _FILE *, int);
  struct _file2 *_more;
};

typedef struct _FILE FILE;

extern FILE _streamv[];

#define stdin  (&_streamv[0])
#define stdout (&_streamv[1])
#define stderr (&_streamv[2])

#endif

#if !defined (SEEK_SET)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#if !defined (EOF)
#define EOF (-1)
#endif

#if !defined (_IOREAD)
#define _IOREAD 0x01
#define _IOWRT  0x02
#define _IORW   0x04
#define _IOEOF  0x08
#define _IOERR  0x10
#define _IOFBF  0x00
#define _IOLBF  0x20
#define _IONBF  0x40
#endif

#if !defined (FOPEN_MAX)
#define	FOPEN_MAX       14
#endif

#if !defined (FILENAME_MAX)
#define	FILENAME_MAX    260
#endif

#if !defined (TMP_MAX)
#define TMP_MAX         1000
#endif

#if !defined (P_tmpdir)
#define P_tmpdir "."
#define L_tmpnam (sizeof (P_tmpdir) + 13)
#endif

#if !defined (L_cuserid)
#define L_cuserid       9
#endif

#if !defined (_FPOS_T)
#define _FPOS_T
typedef struct
{
  long _pos;
  long _reserved1;
  short _mbstate;
  short _reserved2;
} fpos_t;
#endif

#if !defined (_VA_LIST)
#define _VA_LIST
typedef char *va_list;
#endif

#define clearerr CRTWRAP(clearerr)
#define fclose CRTWRAP(fclose)
#define feof CRTWRAP(feof)
#define ferror CRTWRAP(ferror)
#define fflush CRTWRAP(fflush)
#define fgetc CRTWRAP(fgetc)
#define fgetpos CRTWRAP(fgetpos)
#define fgets CRTWRAP(fgets)
#define fopen CRTWRAP(fopen)
#define fprintf CRTWRAP(fprintf)
#define fputc CRTWRAP(fputc)
#define fputs CRTWRAP(fputs)
#define fread CRTWRAP(fread)
#define freopen CRTWRAP(freopen)
#define fscanf CRTWRAP(fscanf)
#define fseek CRTWRAP(fseek)
#define fsetpos CRTWRAP(fsetpos)
#define ftell CRTWRAP(ftell)
#define fwrite CRTWRAP(fwrite)
#define getchar CRTWRAP(getchar)
#define gets CRTWRAP(gets)
#define perror CRTWRAP(perror)
#define printf CRTWRAP(printf)
#define putchar CRTWRAP(putchar)
#define puts CRTWRAP(puts)
#define remove CRTWRAP(remove)
#define rename CRTWRAP(rename)
#define rewind CRTWRAP(rewind)
#define scanf CRTWRAP(scanf)
#define setbuf CRTWRAP(setbuf)
#define setvbuf CRTWRAP(setvbuf)
#define sprintf CRTWRAP(sprintf)
#define sscanf CRTWRAP(sscanf)
#define tmpfile CRTWRAP(tmpfile)
#define tmpnam CRTWRAP(tmpnam)
#define ungetc CRTWRAP(ungetc)
#define vfprintf CRTWRAP(vfprintf)
#define vprintf CRTWRAP(vprintf)
#define vsprintf CRTWRAP(vsprintf)


void clearerr (FILE *);
int fclose (FILE *);
int feof (FILE *);
int ferror (FILE *);
int fflush (FILE *);
int fgetc (FILE *);
int fgetpos (FILE *, fpos_t *);
char *fgets (char *, int, FILE *);
FILE *fopen (__const__ char *, __const__ char *);
int fprintf (FILE *, __const__ char *, ...);
int fputc (int, FILE *);
int fputs (__const__ char *, FILE *);
size_t fread (void *, size_t, size_t, FILE *);
FILE *freopen (__const__ char *, __const__ char *, FILE *);
int fscanf (FILE *, __const__ char *, ...);
int fseek (FILE *, long, int);
int fsetpos (FILE *, __const__ fpos_t *);
long ftell (FILE *);
size_t fwrite (__const__ void *, size_t, size_t, FILE *);
int getchar (void);
char *gets (char *);
void perror (__const__ char *);
int printf (__const__ char *, ...);
int putchar (int);
int puts (__const__ char *);
int remove (__const__ char *);
int rename (__const__ char *, __const__ char *);
void rewind (FILE *);
int scanf (__const__ char *, ...);
int setbuf (FILE *, char *);
int setvbuf (FILE *, char *, int, size_t);
int sprintf (char *, __const__ char *, ...);
int sscanf (__const__ char *, __const__ char *, ...);
FILE *tmpfile (void);
char *tmpnam (char *);
int ungetc (int, FILE *);
int vfprintf (FILE *, __const__ char *, va_list);
int vprintf (__const__ char *, va_list);
int vsprintf (char *, __const__ char *, va_list);

#define _fill CRTWRAP(_fill)
#define _flush CRTWRAP(_flush)
#define _rmtmp CRTWRAP(_rmtmp)

int _fill (FILE *);
int _flush (int, FILE *);
int _rmtmp (void);

int _emx_getc_inline (FILE *);
int _emx_putc_inline (int, FILE *);

extern __inline__ int _feof (FILE *_s)
{
  return (_s->_flags & _IOEOF ? 1 : 0);
}

extern __inline__ int _ferror (FILE *_s)
{
  return (_s->_flags & _IOERR ? 1 : 0);
}

/* Do not use this function in application programs! */

extern __inline__ int _emx_getc_inline (FILE *_s)
{
  return (--_s->_rcount >= 0
          ? (unsigned char)*_s->_ptr++
          : _fill (_s));
}

/* Do not use this function in application programs! */

extern __inline__ int _emx_putc_inline (int _c, FILE *_s)
{
  return (--_s->_wcount >= 0 && (_c != '\n' || !(_s->_flags & _IOLBF))
          ? (unsigned char)(*_s->_ptr++ = (char)_c)
          : _flush (_c, _s));
}

#if defined (__MT__)

#define getc(s)   fgetc(s)
#define putc(c,s) fputc(c,s)

#else

#define getc(s)   _emx_getc_inline(s)
#define putc(c,s) _emx_putc_inline(c,s)

#endif

extern __inline__ int getchar (void) { return getc (stdin); }
extern __inline__ int putchar (int _c) { return putc (_c, stdout); }


#if !defined (__STRICT_ANSI__)

/* POSIX.1 */

#define fdopen CRTWRAP(fdopen)
#define fileno CRTWRAP(fileno)
/* ctermid() */
FILE *fdopen (int, __const__ char *);
int fileno (FILE *);

extern __inline__ int fileno (FILE *_s) { return _s->_handle; }

#endif


#if !defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)

#define cuserid CRTWRAP(cuserid)
#define getw CRTWRAP(getw)
#define fcloseall CRTWRAP(fcloseall)
#define fgetchar CRTWRAP(fgetchar)
#define flushall CRTWRAP(flushall)
#define fputchar CRTWRAP(fputchar)
#define pclose CRTWRAP(pclose)
#define popen CRTWRAP(popen)
#define putw CRTWRAP(putw)
#define setbuffer CRTWRAP(setbuffer)
#define snprintf CRTWRAP(snprintf)
#define tempnam CRTWRAP(tempnam)
#define vfscanf CRTWRAP(vfscanf)
#define vscanf CRTWRAP(vscanf)
#define vsnprintf CRTWRAP(vsnprintf)
#define vsscanf CRTWRAP(vsscanf)


char *cuserid (char *);
int getw (FILE *);
int fcloseall (void);
int fgetchar (void);
int flushall (void);
int fputchar (int);
int pclose (FILE *);
FILE *popen (__const__ char *, __const__ char *);
int putw (int, FILE *);
int setbuffer (FILE *, char *, size_t);
int snprintf (char *, size_t, __const__ char *, ...);
char *tempnam (__const__ char *, __const__ char *);
int vfscanf (FILE *, __const__ char *, va_list);
int vscanf (__const__ char *, va_list);
int vsnprintf (char *, size_t, __const__ char *, va_list);
int vsscanf (__const__ char *, __const__ char *, va_list);

#endif


#if (!defined (__STRICT_ANSI__) && !defined (_POSIX_SOURCE)) \
    || defined (_WITH_UNDERSCORE)

#define _fcloseall CRTWRAP(_fcloseall)
#define _fassign CRTWRAP(_fassign)
#define _fdopen CRTWRAP(_fdopen)
#define _fgetchar CRTWRAP(_fgetchar)
#define _flushall CRTWRAP(_flushall)
#define _fputchar CRTWRAP(_fputchar)
#define _fseek_hdr CRTWRAP(_fseek_hdr)
#define _fsetmode CRTWRAP(_fsetmode)
#define _fsopen CRTWRAP(_fsopen)
#define _getw CRTWRAP(_getw)
#define _mfclose CRTWRAP(_mfclose)
#define _mfopen CRTWRAP(_mfopen)
#define _popen CRTWRAP(_popen)
#define _pclose CRTWRAP(_pclose)
#define _putw CRTWRAP(_putw)
#define _setbuffer CRTWRAP(_setbuffer)
#define _snprintf CRTWRAP(_snprintf)
#define _tempnam CRTWRAP(_tempnam)
#define _vsnprintf CRTWRAP(_vsnprintf)

int _fcloseall (void);
FILE *_fassign (FILE *, FILE *);
FILE *_fdopen (int, __const__ char *);
int _fgetchar (void);
int _flushall (void);
int _fputchar (int);
int _fseek_hdr (FILE *);
int _fsetmode (FILE *, __const__ char *);
FILE *_fsopen (__const__ char *, __const__ char *, int);
int _getw (FILE *);
char *_mfclose (FILE *);
FILE *_mfopen (char *, __const__ char *, size_t, int);
int _pclose (FILE *);
FILE *_popen (__const__ char *, __const__ char *);
int _putw (int, FILE *);
int _setbuffer (FILE *, char *, size_t);
int _snprintf (char *, size_t, __const__ char *, ...);
char *_tempnam (__const__ char *, __const__ char *);
int _vsnprintf (char *, size_t, __const__ char *, va_list);

#endif


#if defined (__cplusplus)
}
#endif

#endif /* not _STDIO_H */
