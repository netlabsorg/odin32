#ifndef _EMXSTDDEF_H
#define _EMXSTDDEF_H

#if !defined (_SIZE_T)
#define _SIZE_T
typedef unsigned long size_t;
#endif

   #ifndef __time_t
      #define __time_t
      typedef long time_t;
   #endif

   #ifndef __tm_t
      #define __tm_t
      struct tm
         {
         int tm_sec;      /* seconds after the minute [0-61]        */
         int tm_min;      /* minutes after the hour [0-59]          */
         int tm_hour;     /* hours since midnight [0-23]            */
         int tm_mday;     /* day of the month [1-31]                */
         int tm_mon;      /* months since January [0-11]            */
         int tm_year;     /* years since 1900                       */
         int tm_wday;     /* days since Sunday [0-6]                */
         int tm_yday;     /* days since January 1 [0-365]           */
         int tm_isdst;    /* Daylight Saving Time flag              */
      };
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

    div_t    div( int, int );
    ldiv_t   ldiv( long int, long int );

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* more file._flag flags, but these conflict with Unix */
#define _IOFBF    0x0000
#define _IONBF    0x0004
#define _IOLBF    0x0040

#define EOF       (-1)
#define FILENAME_MAX 260
#define FOPEN_MAX 20
#define L_tmpnam  260

#define BUFSIZ    512

#ifndef SEEK_SET
#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2
#endif

#define __ASM_NAME(name) "_" name
#define __ASM_FUNC(name) ".type " __ASM_NAME(name) ",@function"

#ifdef __GNUC__
# define __ASM_GLOBAL_FUNC(name,code) \
      __asm__( ".align 4\n\t" \
               ".globl " __ASM_NAME(#name) "\n\t" \
               __ASM_FUNC(#name) "\n" \
               __ASM_NAME(#name) ":\n\t" \
               code );
#else  /* __GNUC__ */
# define __ASM_GLOBAL_FUNC(name,code) \
      void __asm_dummy_##name(void) { \
          asm( ".align 4\n\t" \
               ".globl " __ASM_NAME(#name) "\n\t" \
               __ASM_FUNC(#name) "\n" \
               __ASM_NAME(#name) ":\n\t" \
               code ); \
      }
#endif  /* __GNUC__ */

#endif
