#if (defined(__IBMC__) || defined(__IBMCPP__))
#pragma info( none )
#ifndef __CHKHDR__
   #pragma info( none )
#endif
#pragma info( restore )
#endif

#ifndef __string_h
   #define __string_h

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

   #ifndef __size_t
      #define __size_t
      typedef unsigned int size_t;
   #endif

   #ifndef NULL
      #if (defined(__EXTENDED__)  || defined( __cplusplus ))
         #define NULL 0
      #else
         #define NULL ((void *)0)
      #endif
   #endif

   #ifdef _M_I386
   char * _Builtin __strcat( char *, const char * );
   char * _Builtin __strchr( const char *, int );
   int    _Builtin __strcmp( const char *, const char * );
   char * _Builtin __strcpy( char*, const char * );
   size_t _Builtin __strlen( const char * );
   char * _Builtin __strncat( char *, const char *, size_t );
   int    _Builtin __strncmp( const char *, const char *, size_t );
   char * _LNK_CONV __strncpy( char *, const char *, size_t );
   char * _Builtin __strrchr( const char *, int );
   #endif

   int    _IMPORT _LNK_CONV strcoll( const char *, const char * );
   size_t _IMPORT _LNK_CONV strcspn( const char *, const char * );
   char * _IMPORT _LNK_CONV strerror( int );
   char * _IMPORT _LNK_CONV strpbrk( const char *, const char * );
   size_t _IMPORT _LNK_CONV strspn( const char *, const char * );
   char * _IMPORT _LNK_CONV strstr( const char *, const char * );
   char * _IMPORT _LNK_CONV strtok( char*, const char * );
   size_t _IMPORT _LNK_CONV strxfrm( char *, const char *, size_t );

   #ifdef _M_I386
   #ifdef __cplusplus
      inline char * _LNK_CONV strcat( char * s1, const char * s2 )
                               { return __strcat( s1, s2 ); }
      inline char * _LNK_CONV strchr( const char * s, int ch )
                               { return __strchr( s, ch ); }
      inline int    _LNK_CONV strcmp( const char * s1, const char * s2 )
                               { return __strcmp( s1, s2 ); }
      inline char * _LNK_CONV strcpy( char * s1, const char * s2 )
                               { return __strcpy( s1, s2 ); }
      inline size_t _LNK_CONV strlen( const char * s )
                               { return __strlen( s ); }
      inline char * _LNK_CONV strncat( char * s1, const char * s2, size_t len )
                               { return __strncat( s1, s2, len ); }
      inline int    _LNK_CONV strncmp( const char * s1, const char * s2, size_t len )
                               { return __strncmp( s1, s2, len ); }
      inline char * _LNK_CONV strncpy( char * s1, const char * s2, size_t len )
                               { return __strncpy( s1, s2, len ); }
      inline char * _LNK_CONV strrchr( const char * s1, int ch )
                               { return __strrchr( s1, ch ); }
   #else
      char * _IMPORT _LNK_CONV strcat( char *, const char * );
      char * _IMPORT _LNK_CONV strchr( const char *, int );
      int    _IMPORT _LNK_CONV strcmp( const char *, const char * );
      char * _IMPORT _LNK_CONV strcpy( char *, const char * );
      size_t _IMPORT _LNK_CONV strlen( const char * );
      char * _IMPORT _LNK_CONV strncat( char *, const char *, size_t );
      int    _IMPORT _LNK_CONV strncmp( const char *, const char *, size_t );
      char * _IMPORT _LNK_CONV strncpy( char *, const char *, size_t );
      char * _IMPORT _LNK_CONV strrchr( const char *, int );

      #pragma info( none )
      #define strcat( x, y )     __strcat( (x), (y) )
      #define strchr( x, y )     __strchr( (x), (y) )
      #define strcmp( x, y )     __strcmp( (x), (y) )
      #define strcpy( x, y )     __strcpy( (x), (y) )
      #define strlen( x )        __strlen( (x) )
      #define strncat( x, y, z ) __strncat( (x), (y), (z) )
      #define strncmp( x, y, z ) __strncmp( (x), (y), (z) )
      #define strncpy( x, y, z ) __strncpy( (x), (y), (z) )
      #define strrchr( x, y )    __strrchr( (x), (y) )
      #pragma info( restore )
   #endif
   #endif

#ifdef __THW_PPC__
      char * strcat( char *, const char * );
      char * strchr( const char *, int );
      int    strcmp( const char *, const char * );
      char * strcpy( char *, const char * );
      size_t strlen( const char * );
      char * strncat( char *, const char *, size_t );
      int    strncmp( const char *, const char *, size_t );
      char * strncpy( char *, const char *, size_t );
      char * strrchr( const char *, int );
#endif

   #ifndef __memory_h
      #define __memory_h

      #ifdef _M_I386
      void * _Builtin __memcpy( void *, const void *, size_t );
      void * _Builtin __memchr( const void *, int, size_t );
      int    _Builtin __memcmp( const void *, const void *, size_t );
      void * _Builtin __memset( void *, int, size_t );
      void * _Builtin __memmove( void *, const void *, size_t );
      #endif

      #ifdef _M_I386
      #ifdef __cplusplus
         inline void * _LNK_CONV memcpy( void * m1, const void * m2, size_t len )
                                  { return __memcpy( m1, m2, len ); }
         inline void * _LNK_CONV memchr( const void * m, int ch, size_t len )
                                  { return __memchr( m, ch, len ); }
         inline int    _LNK_CONV memcmp( const void * m1, const void * m2, size_t len )
                                  { return __memcmp( m1, m2, len ); }
         inline void * _LNK_CONV memset( void * m, int ch, size_t len )
                                  { return __memset( m, ch, len ); }
         inline void * _LNK_CONV memmove( void * m1, const void * m2, size_t len )
                                  { return __memmove( m1, m2, len ); }
      #else
         void * _IMPORT _LNK_CONV memcpy( void *, const void *, size_t );
         void * _IMPORT _LNK_CONV memchr( const void *, int, size_t );
         int    _IMPORT _LNK_CONV memcmp( const void *, const void *, size_t );
         void * _IMPORT _LNK_CONV memset( void *, int, size_t );
         void * _IMPORT _LNK_CONV memmove( void *, const void *, size_t );

         #pragma info( none )
         #define memcpy( x, y, z )  __memcpy( (x), (y), (z) )
         #define memchr( x, y, z )  __memchr( (x), (y), (z) )
         #define memcmp( x, y, z )  __memcmp( (x), (y), (z) )
         #define memset( x, y, z )  __memset( (x), (y), (z) )
         #define memmove( x, y, z ) __memmove( (x), (y), (z) )
         #pragma info( restore )
      #endif
      #endif

      #ifdef __THW_PPC__
         void * memcpy( void *, const void *, size_t );
         void * memchr( const void *, int, size_t );
         int    memcmp( const void *, const void *, size_t );
         void * memset( void *, int, size_t );
         void * memmove( void *, const void *, size_t );
      #endif

      #if defined( __EXTENDED__ )

         void * _IMPORT _LNK_CONV memccpy( void *, void *, int, unsigned int );
         int    _IMPORT _LNK_CONV memicmp( void *, void *, unsigned int );

         #define _fmemcpy( x, y, z )     memcpy( (x), (y), (z) )
         #define _fmemchr( x, y, z )     memchr( (x), (y), (z) )
         #define _fmemcmp( x, y, z )     memcmp( (x), (y), (z) )
         #define _fmemset( x, y, z )     memset( (x), (y), (z) )
         #define _fmemccpy( w, x, y, z ) memccpy( (w), (x), (y), (z) )
         #define _fmemicmp( x, y, z )    memicmp( (x), (y), (z) )

      #endif

   #endif

   #if (defined(__EXTENDED__) || defined(__MW_EXT_ANSI__))

      char * _IMPORT _LNK_CONV strdup( const char * );
      int    _IMPORT _LNK_CONV stricmp( const char *, const char * );
      char * _IMPORT _LNK_CONV strlwr( char * );
      int    _IMPORT _LNK_CONV strnicmp( const char *, const char *, size_t );
      char * _IMPORT _LNK_CONV strupr( char * );

      int    _IMPORT _LNK_CONV strcmpi( const char *, const char * );
      char * _IMPORT _LNK_CONV strrev( char * );
      char * _IMPORT _LNK_CONV strset( char *, int );
      char * _IMPORT _LNK_CONV strnset( char *, int, size_t );
      char * _IMPORT _LNK_CONV _strerror( char * );

      #define _fmemmove( x, y, z )  memmove( (x), (y), (z) )
      #define _fstrcat( x, y )      strcat( (x), (y) )
      #define _fstrchr( x, y )      strchr( (x), (y) )
      #define _fstrcmp( x, y )      strcmp( (x), (y) )
      #define _fstrcpy( x, y )      strcpy( (x), (y) )
      #define _fstrlen( x )         strlen( (x) )
      #define _fstrncat( x, y, z )  strncat( (x), (y), (z) )
      #define _fstrncmp( x, y, z )  strncmp( (x), (y), (z) )
      #define _fstrncpy( x, y, z )  strncpy( (x), (y), (z) )
      #define _fstrrchr( x, y )     strrchr( (x), (y) )

      size_t _IMPORT _LNK_CONV _fstrcspn( const char *, const char * );
      char * _IMPORT _LNK_CONV _fstrdup( const char * );
      char * _IMPORT _LNK_CONV _nstrdup( const char * );
      int    _IMPORT _LNK_CONV _fstricmp( const char *, const char * );
      char * _IMPORT _LNK_CONV _fstrlwr( char * );
      int    _IMPORT _LNK_CONV _fstrnicmp( const char *, const char *, size_t );
      char * _IMPORT _LNK_CONV _fstrnset( char *, int, size_t );
      char * _IMPORT _LNK_CONV _fstrpbrk( const char *, const char * );
      char * _IMPORT _LNK_CONV _fstrrev( char * );
      char * _IMPORT _LNK_CONV _fstrset( char *, int );
      size_t _IMPORT _LNK_CONV _fstrspn( const char *, const char * );
      char * _IMPORT _LNK_CONV _fstrstr( const char *, const char * );
      char * _IMPORT _LNK_CONV _fstrtok( char*, const char * );
      char * _IMPORT _LNK_CONV _fstrupr( char * );

    #if (defined(__IBMC__) || defined(__IBMCPP__))
      #pragma map( _fstrcspn , "strcspn"  )
      #pragma map( _fstrdup  , "strdup"   )
      #pragma map( _nstrdup  , "strdup"   )
      #pragma map( _fstricmp , "stricmp"  )
      #pragma map( _fstrlwr  , "strlwr"   )
      #pragma map( _fstrnicmp, "strnicmp" )
      #pragma map( _fstrnset , "strnset"  )
      #pragma map( _fstrpbrk , "strpbrk"  )
      #pragma map( _fstrrev  , "strrev"   )
      #pragma map( _fstrset  , "strset"   )
      #pragma map( _fstrspn  , "strspn"   )
      #pragma map( _fstrstr  , "strstr"   )
      #pragma map( _fstrtok  , "strtok"   )
      #pragma map( _fstrupr  , "strupr"   )
    #else
      #pragma Alias( _fstrcspn , "strcspn"  )
      #pragma Alias( _fstrdup  , "strdup"   )
      #pragma Alias( _nstrdup  , "strdup"   )
      #pragma Alias( _fstricmp , "stricmp"  )
      #pragma Alias( _fstrlwr  , "strlwr"   )
      #pragma Alias( _fstrnicmp, "strnicmp" )
      #pragma Alias( _fstrnset , "strnset"  )
      #pragma Alias( _fstrpbrk , "strpbrk"  )
      #pragma Alias( _fstrrev  , "strrev"   )
      #pragma Alias( _fstrset  , "strset"   )
      #pragma Alias( _fstrspn  , "strspn"   )
      #pragma Alias( _fstrstr  , "strstr"   )
      #pragma Alias( _fstrtok  , "strtok"   )
      #pragma Alias( _fstrupr  , "strupr"   )
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
