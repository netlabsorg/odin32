
#ifndef __WINE_DEBUGTOOLS_H
#define __WINE_DEBUGTOOLS_H

#ifdef __WIN32OS2__
#include <odinwrap.h>
#include <odin.h>

#ifndef __MISC_H__

#ifdef DEBUG
#ifdef PRIVATE_LOGGING
  //To use private dll logging, define PRIVATE_LOGGING and
  //add Open/ClosePrivateLogFiles (see below) functions to the dll
  //to open close the private logfile. The logfile handle should
  //be stored in the _privateLogFile variable
  //dprintf can be called like this:
  //dprintf((LOG, "PE file           : %s", szFileName));
  #define LOG       (void*)_privateLogFile
  #define dprintf(a)      WritePrivateLog a
  #define dprintfGlobal(a)      WriteLog a
#else
  #define dprintf(a)      WriteLog a
#endif
  #define eprintf(a)      WriteLog a ; WriteLogError a
  #define dassert(a, b)   if(!(a)) WriteLogError b
  #define dbgCheckObj(a)   a->checkObject()
  #define DisableLogging  DecreaseLogCount
  #define EnableLogging   IncreaseLogCount

#ifdef DEBUG_ENABLELOG_LEVEL2
#ifdef PRIVATE_LOGGING
  #define dprintf2(a)      WritePrivateLog a
#else
  #define dprintf2(a)      WriteLog a
#endif
#else
  #define dprintf2(a)
#endif

#else
  #define dprintfGlobal(a)
  #define dprintf(a)
  #define dprintf2(a)
  #define eprintf(a)
  #define dassert(a, b)
  #define dbgCheckObj(a)
  #define DisableLogging
  #define EnableLogging
#endif

int  SYSTEM WriteLog(char *tekst, ...);
int  SYSTEM WritePrivateLog(void *logfile, char *tekst, ...);
int  SYSTEM WriteLogError(char *tekst, ...);

void SYSTEM DecreaseLogCount();
void SYSTEM IncreaseLogCount();

#endif //__MISC_H__

#endif

#ifdef __WINE__  /* Debugging interface is internal to Wine */

#include <stdio.h>
#include "config.h"
#include "debugstr.h"

#define DEBUG_RUNTIME

/* Internal definitions (do not use these directly) */

enum __DEBUG_CLASS { __DBCL_FIXME, __DBCL_ERR, __DBCL_WARN, __DBCL_TRACE, __DBCL_COUNT };

extern char __debug_msg_enabled[][__DBCL_COUNT];

extern const char * const debug_cl_name[__DBCL_COUNT];
extern const char * const debug_ch_name[];

#define __GET_DEBUGGING(dbcl,dbch)    (__debug_msg_enabled[(dbch)][(dbcl)])
#define __SET_DEBUGGING(dbcl,dbch,on) (__debug_msg_enabled[(dbch)][(dbcl)] = (on))

#define __DPRINTF(dbcl,dbch) \
  (!__GET_DEBUGGING(dbcl,dbch) || \
     (dprintf(("%s:%s:%s ", debug_cl_name[(dbcl)], debug_ch_name[(dbch)], __line__)),0)) \
    ? 0 : WriteLog

#define __DUMMY_DPRINTF 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)


/* Exported definitions and macros */

/* use configure to allow user to compile out debugging messages */
#ifndef NO_TRACE_MSGS
#define TRACE        __DPRINTF(__DBCL_TRACE,*DBCH_DEFAULT)
#define TRACE_(ch)   __DPRINTF(__DBCL_TRACE,dbch_##ch)
#define TRACE_ON(ch) __GET_DEBUGGING(__DBCL_TRACE,dbch_##ch)
#else
#define TRACE        __DUMMY_DPRINTF
#define TRACE_(ch)   __DUMMY_DPRINTF
#define TRACE_ON(ch) 0
#endif /* NO_TRACE_MSGS */

#ifndef NO_DEBUG_MSGS
#define WARN         __DPRINTF(__DBCL_WARN,*DBCH_DEFAULT)
#define WARN_(ch)    __DPRINTF(__DBCL_WARN,dbch_##ch)
#define WARN_ON(ch)  __GET_DEBUGGING(__DBCL_WARN,dbch_##ch)
#define FIXME        __DPRINTF(__DBCL_FIXME,*DBCH_DEFAULT)
#define FIXME_(ch)   __DPRINTF(__DBCL_FIXME,dbch_##ch)
#define FIXME_ON(ch) __GET_DEBUGGING(__DBCL_FIXME,dbch_##ch)
#else
#define WARN         __DUMMY_DPRINTF
#define WARN_(ch)    __DUMMY_DPRINTF
#define WARN_ON(ch)  0
#define FIXME        __DUMMY_DPRINTF
#define FIXME_(ch)   __DUMMY_DPRINTF
#define FIXME_ON(ch) 0
#endif /* NO_DEBUG_MSGS */

/* define error macro regardless of what is configured */
/* Solaris got an 'ERR' define in <sys/reg.h> */
#undef ERR
#define ERR        __DPRINTF(__DBCL_ERR,*DBCH_DEFAULT)
#define ERR_(ch)   __DPRINTF(__DBCL_ERR,dbch_##ch)
#define ERR_ON(ch) __GET_DEBUGGING(__DBCL_ERR,dbch_##ch)

#define DECLARE_DEBUG_CHANNEL(ch) \
    extern const int dbch_##ch;
#define DEFAULT_DEBUG_CHANNEL(ch) \
    static const int *const DBCH_DEFAULT = &dbch_##ch;

#define DPRINTF dbg_printf
#define MESSAGE dbg_printf

#endif  /* __WINE__ */

#ifdef __WIN32OS2__
#  undef DECLARE_DEBUG_CHANNEL
#  define DECLARE_DEBUG_CHANNEL ODINDEBUGCHANNEL
#  undef DEFAULT_DEBUG_CHANNEL
#  define DEFAULT_DEBUG_CHANNEL ODINDEBUGCHANNEL1
#  undef TRACE
#  undef TRACE_
#  undef TRACE_ON
#  define TRACE_ON(ch) 0
#  undef FIXME
#  undef FIXME_
#  undef FIXME_ON
#  define FIXME_ON(ch) 0
#  undef WARN
#  undef WARN_
#  undef WARN_ON
#  define WARN_ON(ch)  0
#  undef ERR
#  undef ERR_
#  undef ERR_ON
#  define ERR_ON(ch)   0
#ifdef DEBUG
#  define TRACE WriteLog
#  define TRACE_(ch) WriteLog
#  define FIXME WriteLog
#  define FIXME_(ch) WriteLog
#  define WARN  WriteLog
#  define WARN_(ch) WriteLog
#  define ERR_(ch) WriteLog
#  define ERR  WriteLog
#else
#  define TRACE 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#  define TRACE_(ch) 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#  define FIXME 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#  define FIXME_(ch) 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#  define WARN  1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#  define WARN_(ch) 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#  define ERR_(ch) 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#  define ERR  1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#endif
#undef __GET_DEBUGGING
#define __GET_DEBUGGING(dbcl,dbch)
#undef  __SET_DEBUGGING
#define __SET_DEBUGGING(dbcl,dbch,on)

#undef DPRINTF
#define DPRINTF WriteLog
#undef MESSAGE
#define MESSAGE WriteLog

#endif

#ifdef DEBUG
#ifdef __cplusplus
extern "C" {
#endif
LPCSTR debugstr_guid1( void *id );
#define debugstr_guid(a) debugstr_guid1((void *)a)

#ifdef __cplusplus
}
#endif

static LPCSTR debugstr_an (LPCSTR src, int n)
{
  LPSTR dst;
  static char res[128];

  if (!src) return "(null)";
  if (n > sizeof(res)) return "(null)";

  if (n < 0) n = 0;
  dst = res;
  *dst++ = '"';
  while (n-- > 0 && *src)
    {
      BYTE c = *src++;
      switch (c)
   {
   case '\n': *dst++ = '\\'; *dst++ = 'n'; break;
   case '\r': *dst++ = '\\'; *dst++ = 'r'; break;
   case '\t': *dst++ = '\\'; *dst++ = 't'; break;
   case '"': *dst++ = '\\'; *dst++ = '"'; break;
   case '\\': *dst++ = '\\'; *dst++ = '\\'; break;
   default:
     if (c >= ' ' && c <= 126)
       *dst++ = c;
     else
       {
         *dst++ = '\\';
         *dst++ = '0' + ((c >> 6) & 7);
         *dst++ = '0' + ((c >> 3) & 7);
         *dst++ = '0' + ((c >> 0) & 7);
       }
   }
    }
  *dst++ = '"';
  if (*src)
    {
      *dst++ = '.';
      *dst++ = '.';
      *dst++ = '.';
    }
  *dst++ = '\0';
  return res;
}

/* ---------------------------------------------------------------------- */

static LPCSTR debugstr_wn (LPCWSTR src, int n)
{
  LPSTR dst;
  static char res[128];

  if (!src) return "(null)";
  if (n > sizeof(res)) return "(null)";
  if (n < 0) n = 0;

  dst = res;
  *dst++ = 'L';
  *dst++ = '"';
  while (n-- > 0 && *src)
    {
      WORD c = *src++;
      switch (c)
   {
   case '\n': *dst++ = '\\'; *dst++ = 'n'; break;
   case '\r': *dst++ = '\\'; *dst++ = 'r'; break;
   case '\t': *dst++ = '\\'; *dst++ = 't'; break;
   case '"': *dst++ = '\\'; *dst++ = '"'; break;
   case '\\': *dst++ = '\\'; *dst++ = '\\'; break;
   default:
     if (c >= ' ' && c <= 126)
       *dst++ = (char)c;
     else
       {
         *dst++ = '\\';
          sprintf(dst,"%04x",c);
          dst+=4;
       }
   }
    }
  *dst++ = '"';
  if (*src)
    {
      *dst++ = '.';
      *dst++ = '.';
      *dst++ = '.';
    }
  *dst++ = '\0';
  return res;
}

#endif

#endif  /* __WINE_DEBUGTOOLS_H */
