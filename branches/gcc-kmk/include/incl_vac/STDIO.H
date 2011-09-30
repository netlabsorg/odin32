#if (defined(__IBMC__) || defined(__IBMCPP__))
#pragma info( none )
#ifndef __CHKHDR__
   #pragma info( none )
#endif
#pragma info( restore )
#endif

#ifndef __stdio_h
   #define __stdio_h

   #ifdef __cplusplus
      extern "C" {
   #endif

   #ifndef  _LNK_CONV
      #ifdef _M_I386
         #define _LNK_CONV   _Optlink
      #else
         #define _LNK_CONV
      #endif
   #endif

   #ifndef _IMPORT
      #ifdef __IMPORTLIB__
         #define _IMPORT _Import
      #else
         #define _IMPORT
      #endif
   #endif

   /********************************************************************/
   /*  <stdio.h> header file                                           */
   /*                                                                  */
   /*  IBM VisualAge C++ for OS/2, Version 3.00                        */
   /*  (C) Copyright IBM Corp. 1991, 1995.                             */
   /*  - Licensed Material - Program-Property of IBM                   */
   /*  - All rights reserved                                           */
   /*                                                                  */
   /********************************************************************/

   #ifndef __size_t
      #define __size_t
      typedef unsigned int size_t;
   #endif

   enum _OPERATIONS { _IOINIT, _IOREAD, _IOWRITE, _IOREPOSITION,
                      _IOFLUSH, _IOUNDEFOP };

   #define _MAX_UNGET 2

   #if (defined(__IBMC__) || defined(__IBMCPP__))
   #pragma pack( 1 )
   typedef struct __file       /* visible portion of the FILE struct */
      {
      unsigned char     *_bufPtr;
      unsigned long int  _count;
      unsigned long int  _userFlags;
      unsigned long int  _bufLen;
      unsigned long int  _ungetCount;
      int                _tempStore;
      unsigned char      _ungetBuf[_MAX_UNGET];
      enum _OPERATIONS   _lastOp;
      char               _filler;
      } FILE;
   #pragma pack( )
   #else
   typedef struct __file       /* visible portion of the FILE struct */
      {
      unsigned char     *_bufPtr;
      unsigned long int  _count;
      unsigned long int  _userFlags;
      unsigned long int  _bufLen;
      unsigned long int  _ungetCount;
      int                _tempStore;
      enum _OPERATIONS   _lastOp;
      unsigned char      _ungetBuf[_MAX_UNGET];
      char               _filler;
      } FILE;
   #endif

   typedef struct __fpos_t      /* Definition of file positioning structure */
      {
      long int __fpos_elem[2];
      } fpos_t;

   #ifndef NULL
      #if (defined(__EXTENDED__)  || defined( __cplusplus ))
         #define NULL 0
      #else
         #define NULL ((void *)0)
      #endif
   #endif

   #define _IOFBF              1   /* Buffer modes */
   #define _IOLBF              2
   #define _IONBF              3
   #define BUFSIZ           4096   /* Default buffer size */
   #define EOF              (-1)
   #define L_tmpnam          260   /* Maximum length of temporary names */
   #define FOPEN_MAX          20   /* Minimum number of open files guaranteed */
   #define FILENAME_MAX      260   /* Maximum file name length */
   #define SEEK_SET            0   /* fseek constants */
   #define SEEK_CUR            1
   #define SEEK_END            2
   #define TMP_MAX        100000   /* Maximum guaranteed unique file names */

   #define _IOEOF         0x0001   /* EOF flag mask */
   #define _IOERR         0x0002   /* ERR flag mask */

#ifdef _M_I386
   typedef char *__va_list;
#endif

#ifdef __THW_PPC__
  #include <stdarg.h>
#endif

   /* Standard stream pointers. */

   extern FILE * const _IMPORT stdin;
   extern FILE * const _IMPORT stdout;
   extern FILE * const _IMPORT stderr;

   #define stdin          stdin
   #define stdout         stdout
   #define stderr         stderr

   /* Function declarations. */

   #ifndef CRTWRAP
   #define CRTWRAP(a)   odin_##a
   #endif
#ifndef ORIGINAL_VAC_FUNCTIONS
////   #define fprintf   CRTWRAP(fprintf)  
   #define fscanf   CRTWRAP(fscanf)  
////   #define printf   CRTWRAP(printf)  
   #define scanf   CRTWRAP(scanf)  
////   #define sprintf   CRTWRAP(sprintf)  
////   #define sscanf   CRTWRAP(sscanf)  
   #define clearerr   CRTWRAP(clearerr)  
   #define fclose   CRTWRAP(fclose)  
   #define fdopen   CRTWRAP(fdopen)  
   #define feof   CRTWRAP(feof)  
   #define ferror   CRTWRAP(ferror)  
   #define fflush   CRTWRAP(fflush)  
   #define fgetc   CRTWRAP(fgetc)  
   #define fgetpos   CRTWRAP(fgetpos)  
   #define fgets   CRTWRAP(fgets)  
   #define fopen   CRTWRAP(fopen)  
   #define fputc   CRTWRAP(fputc)  
   #define fputs   CRTWRAP(fputs)  
   #define fread   CRTWRAP(fread)  
   #define freopen   CRTWRAP(freopen)  
   #define fseek   CRTWRAP(fseek)  
   #define fsetpos   CRTWRAP(fsetpos)  
   #define ftell   CRTWRAP(ftell)  
   #define fwrite   CRTWRAP(fwrite)  
   #define getc   CRTWRAP(getc)  
   #define getchar  CRTWRAP(getchar)  
   #define gets   CRTWRAP(gets)  
   #define perror   CRTWRAP(perror)  
   #define putc   CRTWRAP(putc)  
   #define putchar   CRTWRAP(putchar)  
   #define puts   CRTWRAP(puts)  
   #define remove   CRTWRAP(remove)  
   #define rename   CRTWRAP(rename)  
   #define rewind   CRTWRAP(rewind)  
   #define setbuf   CRTWRAP(setbuf)  
   #define setvbuf   CRTWRAP(setvbuf)  
   #define tmpfile   CRTWRAP(tmpfile)  
   #define tmpnam   CRTWRAP(tmpnam)  
   #define ungetc   CRTWRAP(ungetc)  
   #define vfprintf   CRTWRAP(vfprintf)  
   #define vprintf   CRTWRAP(vprintf)  
////   #define vsprintf   CRTWRAP(vsprintf)  
#endif

   int      _IMPORT _LNK_CONV fprintf( FILE *, const char *, ... );
   int      _IMPORT _LNK_CONV fscanf( FILE *, const char *, ... );
   int      _IMPORT _LNK_CONV printf( const char *, ... );
   int      _IMPORT _LNK_CONV scanf( const char *, ... );
   int      _IMPORT _LNK_CONV sprintf( char *, const char *, ... );
   int      _IMPORT _LNK_CONV sscanf( const char *, const char *, ... );
   void     _IMPORT _LNK_CONV clearerr( FILE * );
   int      _IMPORT _LNK_CONV fclose( FILE * );
   int      _IMPORT _LNK_CONV feof( FILE * );
   int      _IMPORT _LNK_CONV ferror( FILE * );
   int      _IMPORT _LNK_CONV fflush( FILE * );
   int      _IMPORT _LNK_CONV fgetc( FILE * );
   int      _IMPORT _LNK_CONV fgetpos( FILE *, fpos_t * );
   char   * _IMPORT _LNK_CONV fgets( char *, int, FILE * );
   FILE   * _IMPORT _LNK_CONV fopen( const char *, const char * );
   int      _IMPORT _LNK_CONV fputc( int, FILE * );
   int      _IMPORT _LNK_CONV fputs( const char *, FILE * );
   size_t   _IMPORT _LNK_CONV fread( void *, size_t, size_t, FILE * );
   FILE   * _IMPORT _LNK_CONV freopen( const char *, const char *, FILE * );
   int      _IMPORT _LNK_CONV fseek( FILE *, long int, int );
   int      _IMPORT _LNK_CONV fsetpos( FILE *, const fpos_t * );
   long int _IMPORT _LNK_CONV ftell( FILE * );
   size_t   _IMPORT _LNK_CONV fwrite( const void *, size_t, size_t, FILE * );
   int              _LNK_CONV getc( FILE * );
   int              _LNK_CONV getchar( void );
   char   * _IMPORT _LNK_CONV gets( char * );
   void     _IMPORT _LNK_CONV perror( const char * );
   int              _LNK_CONV putc( int, FILE * );
   int              _LNK_CONV putchar( int );
   int      _IMPORT _LNK_CONV puts( const char * );
   int      _IMPORT _LNK_CONV remove( const char * );
   int      _IMPORT _LNK_CONV rename( const char *, const char * );
   void     _IMPORT _LNK_CONV rewind( FILE * );
   void     _IMPORT _LNK_CONV setbuf( FILE *, char * );
   int      _IMPORT _LNK_CONV setvbuf( FILE *, char *, int, size_t );
   FILE   * _IMPORT _LNK_CONV tmpfile( void );
   char   * _IMPORT _LNK_CONV tmpnam( char * );
   int      _IMPORT _LNK_CONV ungetc( int, FILE * );
   int      _IMPORT _LNK_CONV vfprintf( FILE *, const char *, __va_list );
   int      _IMPORT _LNK_CONV vprintf( const char *, __va_list );
   int      _IMPORT _LNK_CONV vsprintf( char *, const char *, __va_list );

   #define __getc(p) ( \
      (  ((p)->_ungetCount==0) && ((p)->_count!=0))? \
         (--(p)->_count, (int)(*(p)->_bufPtr++)) :\
         fgetc((p)) )

   #define __putc(c, p) ( \
      ((((p)->_tempStore = (c)) != '\n') && \
      ((p)->_lastOp == _IOWRITE) && ((p)->_count < (p)->_bufLen)) ? \
         (++(p)->_count, *(p)->_bufPtr++ = (p)->_tempStore): \
         fputc((p)->_tempStore,(p)) )

   #ifndef __MULTI__
      #ifdef __cplusplus
         inline int getc( FILE * p )        { return __getc( p ); }
         inline int putc( int c, FILE * p ) { return __putc( c, p ); }
      #else
         #define getc( p )    __getc( (p) )
         #define putc( c, p ) __putc( (c), (p) )
      #endif
   #else
      #ifdef __cplusplus
         inline int getc( FILE * p )        { return fgetc( p ); }
         inline int putc( int c, FILE * p ) { return fputc( c, p ); }
      #else
#ifndef ORIGINAL_VAC_FUNCTIONS
         #undef getc
         #undef putc
#endif
         #define getc( p )  fgetc( (p) )
         #define putc( c, p ) fputc( (c), (p) )
      #endif
   #endif

   #ifdef __cplusplus
      inline int getchar( void )   { return getc( stdin ); }
      inline int putchar( int c ) { return putc( c, stdout ); }
   #else
#ifndef ORIGINAL_VAC_FUNCTIONS
      #undef getchar
      #undef putchar
#endif
      #define getchar( )   getc( stdin )
      #define putchar( c ) putc( (c), stdout )
   #endif

   /* clearerr, feof, and ferror macros - single threaded only */

   #ifndef __MULTI__
      #define clearerr( stream ) ( ( void )( (stream)->_userFlags = 0L ) )
      #define feof( stream )     ( ( int  )( (stream)->_userFlags & _IOEOF ) )
      #define ferror( stream )   ( ( int  )( (stream)->_userFlags & _IOERR ) )
   #endif

   #if (defined(__EXTENDED__)  || defined( __cplusplus ))

      #define _IO_WRITETHRU  0x0004   /* WRITETHRU mask */

    #if (defined(__IBMC__) || defined(__IBMCPP__))
      #if defined(_M_I386) || !defined(NO_PROTOTYPE)
       #pragma map( fprintf, "_fprintfieee" )
       #pragma map( printf , "_printfieee"  )
       #pragma map( sprintf, "_sprintfieee" )
       #pragma map( fscanf , "_fscanfieee"  )
       #pragma map( scanf  , "_scanfieee"   )
       #pragma map( sscanf , "_sscanfieee"  )
      #endif

      #pragma map( vfprintf, "_vfprintfieee" )
      #pragma map( vprintf , "_vprintfieee"  )
      #pragma map( vsprintf, "_vsprintfieee" )
    #else
     #ifndef NO_PROTOTYPE
      #pragma Alias( fprintf, "_fprintfieee" )
      #pragma Alias( printf , "_printfieee"  )
      #pragma Alias( sprintf, "_sprintfieee" )
      #pragma Alias( fscanf , "_fscanfieee"  )
      #pragma Alias( scanf  , "_scanfieee"   )
      #pragma Alias( sscanf , "_sscanfieee"  )
     #endif

      #pragma Alias( vfprintf, "_vfprintfieee" )
      #pragma Alias( vprintf , "_vprintfieee"  )
      #pragma Alias( vsprintf, "_vsprintfieee" )
    #endif

      int _LNK_CONV _fcloseall( void );
      int _LNK_CONV _rmtmp( void );

      #define P_tmpdir "\\"
      FILE * _LNK_CONV fdopen( int, const char *);
      int    _LNK_CONV fgetchar( void );
      int    _LNK_CONV fileno( FILE * );
      int    _LNK_CONV flushall( void );
      int    _LNK_CONV fputchar( int );
      char * _LNK_CONV tempnam( char *, char * );
      int    _LNK_CONV unlink( const char * );

      FILE * _LNK_CONV _fdopen( int, const char *);
      int    _LNK_CONV _fgetchar( void );
      int    _LNK_CONV _fileno( FILE * );
      int    _LNK_CONV _flushall( void );
      int    _LNK_CONV _fputchar( int );
      char * _LNK_CONV _tempnam( char *, char * );
      int    _LNK_CONV _unlink( const char * );

      int    _LNK_CONV _set_crt_msg_handle( int );

     #if (defined(__IBMC__) || defined(__IBMCPP__))
     /*RLW - this ensures fdopen() will be remapped to odin_fdopen()
             except when we explicitly want the unwrapped version */
     #ifdef ORIGINAL_VAC_FUNCTIONS
      #pragma map( fdopen  , "_fdopen"   )
     #endif
      #pragma map( fgetchar, "_fgetchar" )
      #pragma map( fileno  , "_fileno"   )
      #pragma map( flushall, "_flushall" )
      #pragma map( fputchar, "_fputchar" )
      #pragma map( tempnam , "_tempnam"  )
      #ifndef __unlink
         #define __unlink
         #pragma map( unlink  , "_unlink"   )
      #endif
     #else
      #pragma Alias( fdopen  , "_fdopen"   )
      #pragma Alias( fgetchar, "_fgetchar" )
      #pragma Alias( fileno  , "_fileno"   )
      #pragma Alias( flushall, "_flushall" )
      #pragma Alias( fputchar, "_fputchar" )
      #pragma Alias( tempnam , "_tempnam"  )
      #ifndef __unlink
         #define __unlink
         #pragma Alias( unlink  , "_unlink"   )
      #endif
     #endif

   #else

    #if defined(__IBMC__) || defined(__IBMCPP__)
      #if defined(_M_I386) || !defined(NO_PROTOTYPE)
        #pragma map( fprintf, "_fprintf_ansi" )
        #pragma map( printf , "_printf_ansi"  )
        #pragma map( sprintf, "_sprintf_ansi" )
        #pragma map( fscanf , "_fscanf_ansi"  )
        #pragma map( scanf  , "_scanf_ansi"   )
        #pragma map( sscanf , "_sscanf_ansi"  )
      #endif
    #else
      #ifndef NO_PROTOTYPE
        #pragma Alias( fprintf, "_fprintf_ansi" )
        #pragma Alias( printf , "_printf_ansi"  )
        #pragma Alias( sprintf, "_sprintf_ansi" )
        #pragma Alias( fscanf , "_fscanf_ansi"  )
        #pragma Alias( scanf  , "_scanf_ansi"   )
        #pragma Alias( sscanf , "_sscanf_ansi"  )
      #endif
    #endif

   #endif

   #ifdef __cplusplus
      }
   #endif

#endif

#if (defined(__IBMC__) || defined(__IBMCPP__))
#pragma info( none )
#ifndef __CHKHDR__
   #pragma info( restore )
#endif
#pragma info( restore )
#endif
