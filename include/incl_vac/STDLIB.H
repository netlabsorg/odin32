#if (defined(__IBMC__) || defined(__IBMCPP__))
#pragma info( none )
#ifndef __CHKHDR__
   #pragma info( none )
#endif
#pragma info( restore )
#endif

#ifndef __stdlib_h
   #define __stdlib_h

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
   /*  <stdlib.h> header file                                          */
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

   #ifndef __wchar_t
     #define __wchar_t
     typedef unsigned short wchar_t;
   #endif

   typedef struct _div_t
       {
       int quot;        /* quotient of integer division       */
       int rem;         /* remainder of integer division      */
       } div_t;

   typedef struct _ldiv_t
       {
       long int quot;   /* quotient of long integer division  */
       long int rem;    /* remainder of long integer division */
       } ldiv_t;

   #ifndef NULL
      #if (defined(__EXTENDED__)  || defined( __cplusplus ))
         #define NULL 0
      #else
         #define NULL ((void *)0)
      #endif
   #endif

   #define EXIT_SUCCESS      0
   #define EXIT_FAILURE      8
   #define RAND_MAX      32767

   #ifdef __BSEXCPT__
      typedef unsigned long _System
        _EH_RTN(PEXCEPTIONREPORTRECORD, struct _EXCEPTIONREGISTRATIONRECORD *,
                PCONTEXTRECORD, PVOID);
   #elif defined(_INC_EXCPT)
      typedef unsigned long __stdcall
        _EH_RTN(struct _EXCEPTION_RECORD*, PVOID, struct _CONTEXT*, PVOID);
   #else
     #ifdef __OS2__
      typedef unsigned long  _System  _EH_RTN(void *,void *,void *,void *);
     #else
      typedef unsigned long __stdcall _EH_RTN(void *,void *,void *,void *);
     #endif
   #endif

   extern int _IMPORT _LNK_CONV _eh_del(_EH_RTN *);
   extern int _IMPORT _LNK_CONV _eh_add(_EH_RTN *);
   extern int _IMPORT _mb_cur_max;
   #define MB_CUR_MAX _mb_cur_max

   /* function prototypes */

   #ifdef _M_I386
   int      _Builtin __abs( int );
   long int _Builtin __labs( long int );
   #endif

      #ifndef CRTWRAP
      #define CRTWRAP(a)   odin_##a
      #endif
#ifndef ORIGINAL_VAC_FUNCTIONS
      #undef calloc
      #undef free
      #undef malloc
      #undef realloc
      #undef _heapmin
      #undef _msize
      #define calloc    CRTWRAP(calloc)
      #define free      CRTWRAP(free)
      #define malloc    CRTWRAP(malloc)
      #define realloc   CRTWRAP(realloc)
      #define _heapmin  CRTWRAP(_heapmin)
      #define _msize    CRTWRAP(_msize)
#endif

   double   _IMPORT _LNK_CONV atof( const char * );
   int      _IMPORT _LNK_CONV atoi( const char * );
   long int _IMPORT _LNK_CONV atol( const char * );
   double   _IMPORT _LNK_CONV strtod( const char *, char ** );
   long int _IMPORT _LNK_CONV strtol( const char *, char **, int );
   unsigned long int _IMPORT _LNK_CONV strtoul( const char *, char **, int );
   int      _IMPORT _LNK_CONV rand( void );
   void     _IMPORT _LNK_CONV srand( unsigned int );
   void *   (_IMPORT _LNK_CONV calloc)( size_t, size_t );
   void     (_IMPORT _LNK_CONV free)( void * );
   void *   (_IMPORT _LNK_CONV malloc)( size_t );
   void *   (_IMPORT _LNK_CONV realloc)( void *, size_t );
   void     _IMPORT _LNK_CONV abort( void );
   int      _IMPORT _LNK_CONV atexit( void ( * )( void ) );
   void     _IMPORT _LNK_CONV exit( int );
   char *   _IMPORT _LNK_CONV getenv( const char * );
   int      _IMPORT _LNK_CONV system( const char * );
   void *   _IMPORT _LNK_CONV bsearch( const void *, const void *, size_t, size_t,
                              int ( * _LNK_CONV __compare )( const void *, const void * ) );
   void     _IMPORT _LNK_CONV qsort( void *, size_t, size_t,
                            int ( * _LNK_CONV __compare )( const void *, const void * ) );
   div_t    _IMPORT _LNK_CONV div( int, int );
   ldiv_t   _IMPORT _LNK_CONV ldiv( long int, long int );
   int      _IMPORT _LNK_CONV mblen( const char *, size_t );
   int      _IMPORT _LNK_CONV mbtowc( wchar_t *, const char *, size_t );
   int      _IMPORT _LNK_CONV wctomb( char *, wchar_t );
   size_t   _IMPORT _LNK_CONV mbstowcs( wchar_t *, const char *, size_t );
   size_t   _IMPORT _LNK_CONV wcstombs( char *, const wchar_t *, size_t );

 #ifdef _M_I386
   #ifdef __cplusplus
      inline int      _LNK_CONV abs ( int x )      { return __abs ( x ); }
      inline long int _LNK_CONV labs( long int l ) { return __labs( l ); }
   #else
      int      _IMPORT _LNK_CONV abs ( int );
      long int _IMPORT _LNK_CONV labs( long int );

      #pragma info( none )
      #define  abs( x )   __abs ( (x) )
      #define  labs( x )  __labs( (x) )
      #pragma info( restore )
   #endif
 #endif

   #ifndef __ANSI__

   #ifndef __SAA_L2__

      #ifndef max
         #define max(a,b) (((a) > (b)) ? (a) : (b))
      #endif
      #ifndef min
         #define min(a,b) (((a) < (b)) ? (a) : (b))
      #endif

      #ifdef _M_I386
      #ifndef _alloca
         void * _Builtin __alloca( size_t );
         #pragma info( none )
         #define _alloca( x ) __alloca( (x) )
         #define alloca( x ) __alloca( (x) )
         #pragma info( restore )
      #endif

      unsigned char _Builtin __parmdwords( void );
      #endif

      long double _IMPORT _LNK_CONV _atold( const char * );
      char * _IMPORT _LNK_CONV  ecvt( double, int, int *, int * );
      char * _IMPORT _LNK_CONV _ecvt( double, int, int *, int * );
      char * _IMPORT _LNK_CONV  fcvt( double, int, int *, int * );
      char * _IMPORT _LNK_CONV _fcvt( double, int, int *, int * );
      int    _IMPORT _LNK_CONV _freemod( unsigned long );
      char * _IMPORT _LNK_CONV _fullpath(char *, char *, size_t);
      char * _IMPORT _LNK_CONV  gcvt( double, int, char * );
      char * _IMPORT _LNK_CONV _gcvt( double, int, char * );
      int    (_IMPORT _LNK_CONV _heapmin)( void );
      char * _IMPORT _LNK_CONV  itoa( int, char *, int );
      char * _IMPORT _LNK_CONV _itoa( int, char *, int );
      int    _IMPORT _LNK_CONV _loadmod( char *, unsigned long * );
      char * _IMPORT _LNK_CONV  ltoa( long, char *, int );
      char * _IMPORT _LNK_CONV _ltoa( long, char *, int );
      size_t _IMPORT _LNK_CONV _msize(void *);
      int    _IMPORT _LNK_CONV  putenv( const char * );
      int    _IMPORT _LNK_CONV _putenv( const char * );
      long double _IMPORT _LNK_CONV  strtold( const char *, char ** );
      long double _IMPORT _LNK_CONV _strtold( const char *, char ** );
      char * _IMPORT _LNK_CONV  ultoa( unsigned long, char *, int );
      char * _IMPORT _LNK_CONV _ultoa( unsigned long, char *, int );

    #if (defined(__IBMC__) || defined(__IBMCPP__))
      #pragma map( ecvt    , "_ecvt"    )
      #pragma map( fcvt    , "_fcvt"    )
      #pragma map( gcvt    , "_gcvt"    )
      #pragma map( itoa    , "_itoa"    )
      #pragma map( ltoa    , "_ltoa"    )
      #pragma map( ultoa   , "_ultoa"   )
      #pragma map( putenv  , "_putenv"  )
      #pragma map( _strtold, "strtold"  )
    #else
      #pragma Alias( ecvt    , "_ecvt"    )
      #pragma Alias( fcvt    , "_fcvt"    )
      #pragma Alias( gcvt    , "_gcvt"    )
      #pragma Alias( itoa    , "_itoa"    )
      #pragma Alias( ltoa    , "_ltoa"    )
      #pragma Alias( ultoa   , "_ultoa"   )
      #pragma Alias( putenv  , "_putenv"  )
      #pragma Alias( _strtold, "strtold"  )
    #endif

      #ifndef __malloc_h

       #ifdef __TILED__
          void * _IMPORT _LNK_CONV _tcalloc( size_t, size_t );
          void   _IMPORT _LNK_CONV _tfree( void * );
          void * _IMPORT _LNK_CONV _tmalloc( size_t );
          void * _IMPORT _LNK_CONV _trealloc( void *, size_t );
          void * _IMPORT _LNK_CONV _theapmin(void);
         #ifndef __DEBUG_ALLOC__
          #pragma map( calloc , "_tcalloc"  )
          #pragma map( free   , "_tfree"    )
          #pragma map( malloc , "_tmalloc"  )
          #pragma map( realloc, "_trealloc" )
          #pragma map( heapmin, "_theapmin" )
         #endif
       #endif

       #ifdef __DEBUG_ALLOC__
         #ifdef __TILED__
          void * _IMPORT _LNK_CONV _debug_tcalloc( size_t, size_t, const char *, size_t );
          void   _IMPORT _LNK_CONV _debug_tfree( void *, const char *, size_t );
          void * _IMPORT _LNK_CONV _debug_tmalloc( size_t, const char *, size_t );
          void * _IMPORT _LNK_CONV _debug_trealloc( void *, size_t, const char *, size_t );
          int    _IMPORT _LNK_CONV _debug_theapmin( const char *, size_t );
          void   _IMPORT _LNK_CONV _theap_check( void );
          void   _IMPORT _LNK_CONV _tdump_allocated( int );
          void   _IMPORT _LNK_CONV _tdump_allocated_delta( int );

          void   _IMPORT _LNK_CONV __theap_check( const char *,size_t );
          void   _IMPORT _LNK_CONV __tdump_allocated( int ,const char *, size_t);
          void   _IMPORT _LNK_CONV __tdump_allocated_delta( int, const char *, size_t);

          #pragma info( none )
          #define _tcalloc(x,y)  _debug_tcalloc( (x), (y), __FILE__, __LINE__ )
          #define _tfree( x )    _debug_tfree( (x), __FILE__, __LINE__ )
          #define _tmalloc( x )  _debug_tmalloc( (x), __FILE__, __LINE__ )
          #define _trealloc(x,y) _debug_trealloc( (x), (y), __FILE__, __LINE__ )
          #define _theapmin( )   _debug_theapmin( __FILE__, __LINE__ )

          #define calloc(x,y)    _debug_tcalloc( (x), (y), __FILE__, __LINE__ )
          #define free( x )      _debug_tfree( (x), __FILE__, __LINE__ )
          #define malloc( x )    _debug_tmalloc( (x), __FILE__, __LINE__ )
          #define realloc(x,y)   _debug_trealloc( (x), (y), __FILE__, __LINE__ )
          #define _heapmin( )    _debug_theapmin( __FILE__, __LINE__ )

          #define _heap_check()            __theap_check(__FILE__,__LINE__)
          #define _dump_allocated(x)       __tdump_allocated((x),__FILE__,__LINE__)
          #define _dump_allocated_delta(x) __tdump_allocated_delta((x),__FILE__,__LINE__)

          #pragma info( restore )
         #else

#ifndef ORIGINAL_VAC_FUNCTIONS
         #define _debug_calloc    CRTWRAP(_debug_calloc)
         #define _debug_free      CRTWRAP(_debug_free)
         #define _debug_malloc    CRTWRAP(_debug_malloc)
         #define _debug_realloc   CRTWRAP(_debug_realloc)
         #define _heap_check      CRTWRAP(_heap_check)
         #define _dump_allocated  CRTWRAP(_dump_allocated)
         #define _dump_allocated_delta   CRTWRAP(_dump_allocated_delta)
         #define _debug_heapmin   CRTWRAP(_debug_heapmin)
         #define __heap_check     CRTWRAP(__heap_check)
         #define __dump_allocated CRTWRAP(__dump_allocated)
         #define __dump_allocated_delta   CRTWRAP(__dump_allocated_delta)
#endif
          void * _IMPORT _LNK_CONV _debug_calloc( size_t, size_t, const char *, size_t );
          void   _IMPORT _LNK_CONV _debug_free( void *, const char *, size_t );
          void * _IMPORT _LNK_CONV _debug_malloc( size_t, const char *, size_t );
          void * _IMPORT _LNK_CONV _debug_realloc( void *, size_t, const char *, size_t );
          int    _IMPORT _LNK_CONV _debug_heapmin( const char *, size_t );
          void   _IMPORT _LNK_CONV _heap_check( void );
          void   _IMPORT _LNK_CONV _dump_allocated( int );
          void   _IMPORT _LNK_CONV _dump_allocated_delta( int );
          void   _IMPORT _LNK_CONV __heap_check( const char *,size_t );
          void   _IMPORT _LNK_CONV __dump_allocated( int ,const char *, size_t);
          void   _IMPORT _LNK_CONV __dump_allocated_delta( int, const char *, size_t );

        #if (defined(__IBMC__) || defined(__IBMCPP__))
          #pragma info( none )
        #endif
#ifndef ORIGINAL_VAC_FUNCTIONS
         #undef calloc
         #undef free
         #undef malloc
         #undef realloc
         #undef _heapmin
         #undef _heap_check
         #undef _dump_allocated
         #undef _dump_allocated_delta
#endif
          #define calloc( x, y )  _debug_calloc( (x), (y), __FILE__, __LINE__ )
          #define free( x )       _debug_free( (x), __FILE__, __LINE__ )
          #define malloc( x )     _debug_malloc( (x), __FILE__, __LINE__ )
          #define realloc( x, y ) _debug_realloc( (x), (y), __FILE__, __LINE__ )
          #define _heapmin( )     _debug_heapmin( __FILE__, __LINE__ )

          #define _heap_check()            __heap_check(__FILE__,__LINE__)
          #define _dump_allocated(x)       __dump_allocated((x),__FILE__,__LINE__)
          #define _dump_allocated_delta(x) __dump_allocated_delta((x),__FILE__,__LINE__)

        #if (defined(__IBMC__) || defined(__IBMCPP__))
          #pragma info( restore )
        #endif
         #endif
       #else
          #define _heap_check( )
          #define _dump_allocated( x )
          #define _theap_check( )
          #define _tdump_allocated( x )
          #define _dump_allocated_delta( x )
          #define _tdump_allocated_delta( x )
       #endif

      #endif

      #ifdef __MULTI__
         int  _IMPORT _LNK_CONV _beginthread( void ( * _LNK_CONV thread )( void * ), void *, unsigned, void * );
         void _IMPORT _LNK_CONV _endthread( void );
         void ** _IMPORT _LNK_CONV _threadstore( void );
      #endif

    #if (defined(__IBMC__) || defined(__IBMCPP__))
      #pragma map( atof  , "_atofieee"   )
      #pragma map( strtod, "_strtodieee" )
    #else
      #pragma Alias( atof  , "_atofieee"   )
      #pragma Alias( strtod, "_strtodieee" )
    #endif

      #if defined(__EXTENDED__)

        #ifdef _M_I386
         void  _Builtin __enable( void );
         void  _Builtin __disable( void );

         #define _enable( )  __enable( )
         #define _disable( ) __disable( )
        #endif

         #ifndef errno
            #ifdef __MULTI__
               int * _IMPORT _LNK_CONV _errno( void );
               #if (defined(__IBMC__) || defined(__IBMCPP__))
               #pragma info( none )
               #endif
               #define errno (*_errno( ))
               #if (defined(__IBMC__) || defined(__IBMCPP__))
               #pragma info( restore )
               #endif
            #else
               extern int _IMPORT errno;
               #if (defined(__IBMC__) || defined(__IBMCPP__))
               #pragma info( none )
               #endif
               #define errno errno
               #if (defined(__IBMC__) || defined(__IBMCPP__))
               #pragma info( restore )
               #endif
            #endif
         #endif

         #ifndef _doserrno
            #ifdef __MULTI__
               int * _IMPORT _LNK_CONV __doserrno(void);
               #if (defined(__IBMC__) || defined(__IBMCPP__))
               #pragma info( none )
               #endif
               #define _doserrno (*__doserrno( ))
               #if (defined(__IBMC__) || defined(__IBMCPP__))
               #pragma info( restore )
               #endif
            #else
               extern int _IMPORT _doserrno;
               #if (defined(__IBMC__) || defined(__IBMCPP__))
               #pragma info( none )
               #endif
               #define _doserrno _doserrno
               #if (defined(__IBMC__) || defined(__IBMCPP__))
               #pragma info( restore )
               #endif
            #endif
         #endif

         extern char ** _IMPORT _environ;
         extern unsigned char _IMPORT _osmajor;
         extern unsigned char _IMPORT _osminor;
         extern unsigned char _IMPORT _osmode;

         #define environ _environ

         #define DOS_MODE        0       /* Real Address Mode */
         #define OS2_MODE        1       /* Protected Address Mode */

         typedef int ( __onexit_t )( void );
         typedef __onexit_t * onexit_t;

         /* Sizes for buffers used by the _makepath() and _splitpath() functions.*/
         /* Note that the sizes include space for null terminating character.    */

         #define _MAX_PATH       260   /* max. length of full pathname           */
         #define _MAX_DRIVE      3     /* max. length of drive component         */
         #define _MAX_DIR        256   /* max. length of path component          */
         #define _MAX_FNAME      256   /* max. length of file name component     */
         #define _MAX_EXT        256   /* max. length of extension component     */

         void           _IMPORT _LNK_CONV _exit( int );
         onexit_t       _IMPORT _LNK_CONV  onexit( onexit_t );
         onexit_t       _IMPORT _LNK_CONV _onexit( onexit_t );

       #ifdef _M_I386
         unsigned long  _Builtin  __lrotl(unsigned long, int);
         unsigned long  _Builtin  __lrotr(unsigned long, int);
         unsigned short _Builtin  __srotl(unsigned short, int);
         unsigned short _Builtin  __srotr(unsigned short, int);
         unsigned char  _Builtin  __crotl(unsigned char, int);
         unsigned char  _Builtin  __crotr(unsigned char, int);
         #define  _rotl( x, y )   __lrotl( (x), (y) )
         #define  _rotr( x, y )   __lrotr( (x), (y) )
         #define  _lrotl( x, y )  __lrotl( (x), (y) )
         #define  _lrotr( x, y )  __lrotr( (x), (y) )
         #define  _srotl( x, y )  __srotl( (x), (y) )
         #define  _srotr( x, y )  __srotr( (x), (y) )
         #define  _crotl( x, y )  __crotl( (x), (y) )
         #define  _crotr( x, y )  __crotr( (x), (y) )
       #else
         unsigned int   _rotl (unsigned int, int);
         unsigned int   _rotr (unsigned int, int);
         unsigned long  _lrotl(unsigned long, int);
         unsigned long  _lrotr(unsigned long, int);
         unsigned short _srotl(unsigned short, int);
         unsigned short _srotr(unsigned short, int);
         unsigned char  _crotl(unsigned char, int);
         unsigned char  _crotr(unsigned char, int);
       #endif

         void           _IMPORT _LNK_CONV _makepath( char *, char *, char *, char *, char * );
         void           _IMPORT _LNK_CONV _splitpath( char *, char *, char *, char *, char * );
         void           _IMPORT _LNK_CONV _searchenv( char *, char *, char *);
         void           _IMPORT _LNK_CONV  swab( char *, char *, int );
         void           _IMPORT _LNK_CONV _swab( char *, char *, int );

       #if (defined(__IBMC__) || defined(__IBMCPP__))
         #pragma map( onexit , "_onexit"  )
         #pragma map( swab   , "_swab"    )
       #else
         #pragma Alias( onexit , "_onexit"  )
         #pragma Alias( swab   , "_swab"    )
       #endif

         int           _IMPORT _LNK_CONV rpmatch(const char *);
         int           _IMPORT _LNK_CONV csid(const char *);
         int           _IMPORT _LNK_CONV wcsid(const wchar_t);

       #if (defined(__IBMC__) || defined(__IBMCPP__))
         #pragma map( rpmatch, "_rpmatch" )
         #pragma map( csid   , "_csid"    )
         #pragma map( wcsid  , "_wcsid"   )
       #else
         #pragma Alias( rpmatch, "_rpmatch" )
         #pragma Alias( csid   , "_csid"    )
         #pragma Alias( wcsid  , "_wcsid"   )
       #endif

         /* Define different memory model versions of memory management       */
         /* routines to the standard names.                                   */

         #define _ncalloc( x, y )  calloc( (x), (y) )
         #define _fcalloc( x, y )  calloc( (x), (y) )
         #define _nfree( x )       free( (x) )
         #define _ffree( x )       free( (x) )
         #define _nmalloc( x )     malloc( (x) )
         #define _fmalloc( x )     malloc( (x) )
         #define _nrealloc( x, y ) realloc( (x), (y) )
         #define _frealloc( x, y ) realloc( (x), (y) )
         #define _fheapmin( )      _heapmin( )
         #define _nheapmin( )      _heapmin( )

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

