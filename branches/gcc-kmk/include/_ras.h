#ifndef __RAS__H
#define __RAS__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <odin.h>
#include <win32type.h>

#define RAS


#ifdef RAS

#ifdef __cplusplus
extern "C" {
#endif

/* The RAS subsystem initialization/deinitialization */

int WIN32API RasInitialize (
                 HMODULE hmod                    /* the custom dll handle */
             );

void WIN32API RasUninitialize (
                  void
              );


/* Generic RAS entry to be called from various places in Odin code.
 */
void WIN32API RasEntry (
                  ULONG ulEvent,                 /* RAS event number */
                  void *p,                       /* pointer to event specific data */
                  ULONG cb                       /* size of event specific data */
              );

/* RAS events */
#define RAS_EVENT_Kernel32InitComplete  (1)
#define RAS_EVENT_User32InitComplete    (2)
#define RAS_EVENT_Gdi32InitComplete     (3)

/* External logging library functions typedefs */
typedef int WIN32API FNOLF (ULONG *ph, const char *logfilename);
typedef void WIN32API FNCLF (ULONG h);
typedef void WIN32API FNWL (ULONG h, char *buf, ULONG buflen);

/* Tracked objects logging functions typedefs */
typedef void WIN32API FNRASLOG_EXTERNAL (const char *fmt, ...);

typedef ULONG WIN32API FNLOC (ULONG objident, ULONG objhandle, void *objdata, ULONG cbobjdata, FNRASLOG_EXTERNAL *pRasLog);
typedef ULONG WIN32API FNCOC (ULONG objhandle, void *objdata1, ULONG cbobjdata1, void *objdata2, ULONG cbobjdata);

/* Object tracking handle */
typedef struct _RAS_TRACK RAS_TRACK;
typedef RAS_TRACK *RAS_TRACK_HANDLE;

/* Object tracking flags */
#define RAS_TRACK_FLAG_LOGOBJECTCONTENT         (0x1)
#define RAS_TRACK_FLAG_MEMORY                   (0x2)
#define RAS_TRACK_FLAG_LOG_AT_EXIT              (0x4)
#define RAS_TRACK_FLAG_LOG_OBJECTS_AT_EXIT      (0x8 | RAS_TRACK_FLAG_LOG_AT_EXIT)

void WIN32API RasRegisterObjectTracking (
                  RAS_TRACK_HANDLE *ph,           /* returned handle */
                  const char *objname,            /* arbitrary distinguishable object name */
                  ULONG cbuserdata,               /* extra data size */
                  ULONG flags,                    /* object tracking flags */
                  FNLOC *pfnLogObjectContent,     /* custom function to log the object content */
                  FNCOC *pfnCompareObjectContent  /* custom function to compare two objects */
              );

void WIN32API RasDeregisterObjectTracking (
                  RAS_TRACK_HANDLE h              /* handle previously returned by RasRegisterObjectTracking */
              );

ULONG WIN32API RasAddObject (                     /* returns unique object ident */
                  RAS_TRACK_HANDLE h,             /* handle previously returned by RasRegisterObjectTracking */
                  ULONG objhandle,                /* distinctive handle of the object */
                  void *objdata,                  /* actual object pointer */
                  ULONG cbobjdata                 /* size of object */
              );

void WIN32API RasTrackMemAlloc (
                  RAS_TRACK_HANDLE h,             /* handle previously returned by RasRegisterObjectTracking */
                  ULONG size
              );

void WIN32API RasTrackMemRealloc (
                  RAS_TRACK_HANDLE h,             /* handle previously returned by RasRegisterObjectTracking */
                  ULONG oldsize,
                  ULONG newsize
              );

void WIN32API RasTrackMemFree (
                  RAS_TRACK_HANDLE h,             /* handle previously returned by RasRegisterObjectTracking */
                  ULONG size
              );

void WIN32API RasRemoveObject (
                  RAS_TRACK_HANDLE h,             /* handle previously returned by RasRegisterObjectTracking */
                  ULONG objhandle                 /* distinctive handle of the object */
              );

void WIN32API RasSetObjectUserData (
                  RAS_TRACK_HANDLE h,             /* handle previously returned by RasRegisterObjectTracking */
                  ULONG objident,                 /* unique object ident returned */
                  void *pdata,                    /* arbitrary data to be saved */
                  ULONG cbdata,                   /* size of data */
                  ULONG *pcbdataret               /* returned size of data actually saved */
              );

void WIN32API RasQueryObjectUserData (
                  RAS_TRACK_HANDLE h,             /* handle previously returned by RasRegisterObjectTracking */
                  ULONG objident,                 /* unique object ident returned */
                  void *pdata,                    /* data buffer for queried data */
                  ULONG cbdata,                   /* size of data buffer */
                  ULONG *pcbdataret               /* returned size of data actually queried */
              );

/* RAS serialization */
void WIN32API RasEnterSerialize (
                  void
              );

void WIN32API RasExitSerialize (
                  void
              );

/* RAS logging channels */
typedef struct _RAS_LOG_CHANNEL *RAS_LOG_CHANNEL_H;

int WIN32API RasOpenLogChannel (                   /* returns system error code */
                  RAS_LOG_CHANNEL_H *phchannel,    /* returned channel handle */
                  const char *env_loghandler,      /* name of environment variable that is equal to name of external logging dll */
                  const char *filename             /* file name to log to */
              );

void WIN32API RasWriteLogChannel (
                  RAS_LOG_CHANNEL_H hchannel,      /* log channel handle returned by RasOpenLogChannel */
                  const char *msg,                 /* string to log */
                  ULONG length                     /* length of string */
              );

void WIN32API RasCloseLogChannel (
                  RAS_LOG_CHANNEL_H hchannel       /* log channel handle returned by RasOpenLogChannel */
              );

/* RAS logging functions */
void WIN32API RasLog (
                  const char *fmt,                 /* 'printf' style format string */
                  ...
              );

void WIN32API RasLogNoEOL (
                  const char *fmt,                 /* 'printf' style format string */
                  ...
              );

void WIN32API RasLogMsg (
                  ULONG msg,                       /* RAS message number */
                  ULONG parm1,                     /* message parameter 1 */
                  ULONG parm2                      /* message parameter 2 */
              );

#define RAS_FLAG_LOG_OBJECTS (0x1)

void WIN32API RasLogObjects (
                  RAS_TRACK_HANDLE h,              /* handle previously returned by RasRegisterObjectTracking */
                  ULONG flags                      /* logging mode */
              );

void WIN32API RasLog2 (
                  RAS_LOG_CHANNEL_H hchannel,      /* log channel to log to */
                  char *fmt,                       /* 'printf' style format string */
                  ...
              );

void WIN32API RasLogNoEOL2 (
                  RAS_LOG_CHANNEL_H hchannel,      /* log channel to log to */
                  char *fmt,                       /* 'printf' style format string */
                  ...
              );

void WIN32API RasLogMsg2 (
                  RAS_LOG_CHANNEL_H hchannel,      /* log channel to log to */
                  ULONG msg,                       /* RAS message number */
                  ULONG parm1,                     /* message parameter 1 */
                  ULONG parm2                      /* message parameter 2 */
              );

/* RAS replacement for C runtime sprintf function */
#ifdef __GNUC__
int WIN32API ras_snprintf (
#else
int WIN32API snprintf (
#endif
                  char *buf,                       /* memory buffer for formatted string */
                  int n,                           /* length of memeory buffer */
                  const char *fmt,                 /* 'printf' style format string */
                  ...
             );

/* Tracked object counting function */
void WIN32API RasCountObjects (
                  RAS_TRACK_HANDLE h,              /* handle previously returned by RasRegisterObjectTracking */
                  ULONG *pcount,                   /* pointer to returned count of objects */
                  ULONG *pallocated                /* pointer to returned number of bytes allocated for memory trackings */
              );

/* Obtain tracking handle for particular objects */
RAS_TRACK_HANDLE WIN32API RasGetTrackHandle (
                  const char *objname              /* object name use in RasRegisterObjectTracking */
              );



/* Odin context save and restore functions.
 * The context has to be saved before calling
 * any external API (OS/2 and or C runtime functions).
 */

typedef struct _RASCONTEXT
{
    ULONG data[8];
} RASCONTEXT;

void WIN32API RasSaveContext (
                  RASCONTEXT *pcontext              /* memory buffer for context to save */
              );
void WIN32API RasRestoreContext (
                  RASCONTEXT *pcontext              /* memory buffer for context to restore */
              );

FARPROC WIN32API RasSetProcAddr (                   /* returns old procedure address */
                  ULONG hModule,                    /* Odin32 module handle */
                  LPCSTR lpszProc,                  /* name of procedure */
                  FARPROC pfnNewProc                /* new procedure address */
              );

ULONG WIN32API RasGetModuleHandle (
                  LPCTSTR lpszModule
              );

#ifdef __cplusplus
} // extern "C"
#endif

/* RAS entries that are passed to plugin to use
 */
typedef struct _RasEntryTable
{
    ULONG cb;

    void  (* WIN32API RasRegisterObjectTracking) (RAS_TRACK_HANDLE *ph, const char *objname, ULONG cbuserdata, ULONG flags, FNLOC *pfnLogObjectContent, FNCOC *pfnCompareObjectContent);
    void  (* WIN32API RasDeregisterObjectTracking) (RAS_TRACK_HANDLE h);
    ULONG (* WIN32API RasAddObject) (RAS_TRACK_HANDLE h, ULONG objhandle, void *objdata, ULONG cbobjdata);
    void  (* WIN32API RasRemoveObject) (RAS_TRACK_HANDLE h, ULONG objhandle);
    void  (* WIN32API RasSetObjectUserData) (RAS_TRACK_HANDLE h, ULONG objident, void *pdata, ULONG cbdata, ULONG *pcbdataret);
    void  (* WIN32API RasQueryObjectUserData) (RAS_TRACK_HANDLE h, ULONG objident, void *pdata, ULONG cbdata, ULONG *pcbdataret);
    void  (* WIN32API RasEnterSerialize) (void);
    void  (* WIN32API RasExitSerialize) (void);
    int   (* WIN32API RasOpenLogChannel) (RAS_LOG_CHANNEL_H *phchannel, const char *env_loghandler, const char *filename);
    void  (* WIN32API RasWriteLogChannel) (RAS_LOG_CHANNEL_H hchannel, const char *msg, ULONG length);
    void  (* WIN32API RasCloseLogChannel) (RAS_LOG_CHANNEL_H hchannel);
    void  (* WIN32API RasLog) (RAS_LOG_CHANNEL_H hchannel, char *fmt, ...);
    void  (* WIN32API RasLogNoEOL) (RAS_LOG_CHANNEL_H hchannel, char *fmt, ...);
    void  (* WIN32API RasLogMsg) (RAS_LOG_CHANNEL_H hchannel, ULONG msg, ULONG parm1, ULONG parm2);
    int   (* WIN32API snprintf) (char *buf, int n, const char *fmt, ...);
    void  (* WIN32API RasSaveContext) (RASCONTEXT *pcontext);
    void  (* WIN32API RasRestoreContext) (RASCONTEXT *pcontext);
    FARPROC (* WIN32API RasSetProcAddr) (HMODULE hModule, LPCSTR lpszProc, FARPROC pfnNewProc);
    ULONG (* WIN32API RasGetModuleHandle) (LPCTSTR lpszModule);
    void  (* WIN32API RasCountObjects) (RAS_TRACK_HANDLE h, ULONG *pcount, ULONG *pallocated);
    void  (* WIN32API RasTrackMemAlloc) (RAS_TRACK_HANDLE h, ULONG size);
    void  (* WIN32API RasTrackMemRealloc) (RAS_TRACK_HANDLE h, ULONG oldsize,ULONG newsize);
    void  (* WIN32API RasTrackMemFree) (RAS_TRACK_HANDLE h, ULONG size);
    RAS_TRACK_HANDLE (* WIN32API RasGetTrackHandle) (const char *objname);

} RasEntryTable;

/* RAS entries those can be replaced by plugin
 */
typedef struct _RasPluginEntryTable
{
    ULONG cb;

    void  (* WIN32API RasEntry) (ULONG ulEvent, void *p, ULONG cb);
} RasPluginEntryTable;

/* RAS plugin functions typedefs */
typedef void WIN32API FNPI (HMODULE hmod, RasEntryTable *pret, RasPluginEntryTable *ppet);
typedef void WIN32API FNPE (HMODULE hmod);

/* RAS breakpoint support */
#if defined(__GNUC__)
#include <sys/builtin.h>
#else
#include <builtin.h>
#endif

#ifdef __cplusplus
#define __INLINE inline
#else
#define __INLINE _Inline
#endif

// Note: no need to generate int 3 in RAS build, just log the event.

#define RasDebugInt3() RasBreakPoint(__FILE__, __FUNCTION__, __LINE__, 0, 0, 0)
#define RasDebugInt3_x(a, b, c) RasBreakPoint(__FILE__, __FUNCTION__, __LINE__, a, b, c)

void __INLINE RasBreakPoint (const char *szFile, const char *szFunction, int iLine, ULONG msg, ULONG parm1, ULONG parm2)
{
    RasLog ("BreakPoint at %s(%d)::%s", szFile, iLine, szFunction);
    if (msg)
    {
        RasLogMsg (msg, parm1, parm2);
    }
}

/* Replace old DebugInt3 with RAS version */
#ifdef DebugInt3
#undef DebugInt3
#endif

#define DebugInt3 RasDebugInt3

#else

// non RAS version, that does nothing
#define RasRegisterObjectTracking(a, b, c, d, e, f)
#define RasDeregisterObjectTracking(a)
#define RasAddObject(a, b, c, d)
#define RasRemoveObject(a, b)
#define RasSetObjectUserData(a, b, c, d, e)
#define RasQueryObjectUserData(a, b, c, d, e)
#define RasInitialize()
#define RasUninitialize()
#define RasLogObjects(a, b)
#define RasEntry (a, b, c)
#define RasTrackMemAlloc(a, b)
#define RasTrackMemRealloc(a, b, c)
#define RasTrackMemFree(a, b)


#endif /* RAS */

#endif /* __RAS__H */
