#define INCL_BASE
#define INCL_WIN
#define INCL_WINERRORS
#define INCL_DOSFILEMGR
#include <os2wrap.h>                     // Odin32 OS/2 api wrappers

#include <_ras.h>
#include <umalloc.h>

#include <custombuild.h>
#include <odincrt.h>
#ifdef __GNUC__
#include <float.h>
#else
#include <libc/float.h>
#endif

#include "initterm.h"

#define DBG_LOCALLOG    DBG__ras
#include "dbglocal.h"

/* RAS functions to isolate all RAS related services:
 *
 * - Common statistic functions to track objects allocations/deallocations.
 *
 * - RAS event logging.
 *
 * - RAS plugin support.
 *
 * Is to be used by all Odin subsystems by calling the KERNEL32 exports.
 *
 * Note: RAS subsystem does not use any other Odin subsystem, and IMO must not.
 *       That is RAS has its own heap, serialization, logging.
 *       External stuff that is used:
 *           - from Odin: asm helpers from interlock.asm
 *           - from libc: uheap functions
 *
 *       The following has been borrowed from other Odin parts and adapted:
 *           - critical section
 *           - shared heap
 *
 * Note2: Currently RAS subsystem works on per process basis.
 *        This can be changed if there will be a need to have
 *        shared RAS subsystem for different processes.
 *        The code has some support for this.
 *
 * Note3: RAS subsystem works with OS/2 fs value. Current fs is saved
 *        and restored at each public function.
 */

typedef struct _RAS_TRACK RAS_TRACK;

typedef struct _RAS_OBJECT_INFO
{
    struct _RAS_OBJECT_INFO *next;
    struct _RAS_OBJECT_INFO *prev;

    RAS_TRACK_HANDLE h;

    ULONG objident;
    ULONG usecount;

    ULONG objhandle;

    void *objdata;
    ULONG cbobjdata;

    char userdata[1];
} RAS_OBJECT_INFO;

struct _RAS_TRACK
{
    struct _RAS_TRACK *next;
    struct _RAS_TRACK *prev;

    RAS_OBJECT_INFO *objfirst;
    RAS_OBJECT_INFO *objlast;

    char objname[80];

    ULONG cbuserdata;

    ULONG fLogObjectContent: 1;
    ULONG fMemory: 1;
    ULONG fLogAtExit: 1;
    ULONG fLogObjectsAtExit: 1;

    FNLOC *pfnLogObjectContent;
    FNCOC *pfnCompareObjectContent;

    /* Used when fMemory = 1 for RasTrackAlloc, Realloc and Free calls */
    ULONG cAllocs;
    ULONG cFrees;
    ULONG cbTotalAllocated;

};

typedef struct _CRITICAL_SECTION_RAS
{
    LONG    LockCount;
    LONG    RecursionCount;
    ULONG   OwningThread;
    HEV     hevLock;
} CRITICAL_SECTION_RAS;


typedef struct _RAS_CONFIG
{
    ULONG ulTimeout;        // RAS critical section timeout
    ULONG ulInitHeapSize;
    ULONG fDumpHeapObjects: 1;
    ULONG fUseHighMem: 1;
    ULONG fRasEnable: 1;
    ULONG fRasBreakPoint: 1;
} RAS_CONFIG;

typedef struct _RasLogChannel
{
    FNWL *pfnWriteLog;      // function to write ras string to log
    FNOLF *pfnOpenLogFile;
    FNCLF *pfnCloseLogFile;

    ULONG hlogfile;         // filehandle if default logging functions are used

    HMODULE hmod;
} RasLogChannel;

typedef struct _RAS_DATA
{
    HMODULE hmod;           // handle of this dll

    RasLogChannel rlc;

    Heap_t rasheap;
    void *pHeapMem;
    ULONG ulRefCount;
    ULONG flAllocMem;

    HMODULE hmodPlugin;

    RasEntryTable ret;
    RasPluginEntryTable pet;

    struct _RAS_TRACK *firsttrack;
    struct _RAS_TRACK *lasttrack;

    RAS_TRACK_HANDLE h;     // current handle, set by ENTER_RAS

    ULONG fNoEOL: 1;
} RAS_DATA;


int rasInitialized = 0;

CRITICAL_SECTION_RAS csras = { -1, 0, 0, 0 };
RAS_CONFIG rascfg;
RAS_DATA rasdata;


/* Private functions */

typedef struct _RASCONTEXT_I
{
    USHORT fs;
    USHORT cw;
} RASCONTEXT_I;

extern "C" {

void rasSaveContext(RASCONTEXT_I *pcontext)
{
    pcontext->fs = RestoreOS2FS ();
    pcontext->cw = _control87(0, 0);
}

void rasRestoreContext(RASCONTEXT_I *pcontext)
{
    _control87(pcontext->cw, 0xFFFF);
    SetFS (pcontext->fs);
}

ULONG rasGetCurrentThreadId (void)
{
    PTIB ptib;
    PPIB ppib;

    DosGetInfoBlocks(&ptib, &ppib);

    return ppib->pib_ulpid << 16 | ptib->tib_ptib2->tib2_ultid;
}

ULONG rasInitializeCriticalSection (CRITICAL_SECTION_RAS *crit)
{
    int rc = NO_ERROR;

    rc = DosCreateEventSem (NULL, &crit->hevLock, DC_SEM_SHARED, 0);

    if (rc != NO_ERROR)
    {
        crit->hevLock = 0;
    }

    return rc;
}

void rasUninitializeCriticalSection (CRITICAL_SECTION_RAS *crit)
{
    DosCloseEventSem (crit->hevLock);
}

ULONG rasEnterCriticalSection (CRITICAL_SECTION_RAS *crit, ULONG ulTimeout)
{
    APIRET rc = NO_ERROR;

    ULONG threadid = rasGetCurrentThreadId ();

    // We want to acquire the section, count the entering
    DosInterlockedIncrement (&crit->LockCount);

    if (crit->OwningThread == threadid)
    {
        // This thread already owns the section
        crit->RecursionCount++;
        return NO_ERROR;
    }

    // try to acquire the section
    for (;;)
    {
        // try to assign owning thread id atomically
        if (DosInterlockedCompareExchange((PLONG)&crit->OwningThread, threadid, 0) == 0)
        {
            break;
        }

        rc = DosWaitEventSem (crit->hevLock, ulTimeout);

        if (rc != NO_ERROR)
        {
            // We fail, deregister itself
            DosInterlockedDecrement (&crit->LockCount);
            return rc;
        }
    }

    // the section was successfully aquired
    crit->RecursionCount = 1;

    return NO_ERROR;
}


ULONG rasLeaveCriticalSection (CRITICAL_SECTION_RAS *crit)
{
    if (crit->OwningThread != rasGetCurrentThreadId ())
    {
        return ERROR_INVALID_PARAMETER;
    }

    if (--crit->RecursionCount)
    {
        DosInterlockedDecrement (&crit->LockCount);

        return NO_ERROR;
    }

    crit->OwningThread = 0;

    if (DosInterlockedDecrement (&crit->LockCount) >= 0)
    {
        ULONG ulnrposts = 0;

        DosPostEventSem (crit->hevLock);

        DosResetEventSem (crit->hevLock, &ulnrposts);
    }

    return NO_ERROR;
}

void EnterSerializeRAS (void)
{
    rasEnterCriticalSection (&csras, rascfg.ulTimeout);
}

void ExitSerializeRAS (void)
{
    rasLeaveCriticalSection (&csras);
}

/* Private logging functions */
void ulong2string (unsigned long number, char *string, int n, int base)
{
    static const char *digits = "0123456789ABCDEF";

    unsigned long tmp = number;
    char *s = string;
    int len = 0;
    int l = 0;
    int i;

    if (n <= 0)
    {
        return;
    }

    if (tmp == 0)
    {
        s[l++] = digits[0];
    }

    while (tmp != 0)
    {
        if (l >= n)
        {
            break;
        }
        s[l++] = digits[tmp%base];
        len++;
        tmp /= base;
    }
    if (l < n)
    {
        s[l++] = '\0';
    }

    s = string;

    for (i = 0; i < len/2; i++)
    {
        tmp = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = tmp;
    }

    return;
}

void long2string (long number, char *string, int n, int base)
{
    if (n <= 0)
    {
        return;
    }

    if (number < 0)
    {
        *string++ = '-';
        number = -number;
        n--;
    }

    ulong2string (number, string, n, base);
}

int string2ulong (const char *string, char **pstring2, unsigned long *pvalue, int base)
{
    unsigned long value = 0;
    int sign = 1;

    const char *p = string;

    if (p[0] == '-')
    {
        sign = -1;
        p++;
    }

    if (base == 0)
    {
        if (p[0] == 0 && (p[1] == 'x' || p[1] == 'X'))
        {
            base = 16;
            p += 2;
        }
        else if (p[0] == 0)
        {
            base = 8;
            p += 1;
        }
        else
        {
            base = 10;
        }
    }

    while (*p)
    {
        int digit = 0;

        if ('0' <= *p && *p <= '9')
        {
            digit = *p - '0';
        }
        else if ('a' <= *p && *p <= 'f')
        {
            digit = *p - 'a' + 0xa;
        }
        else if ('A' <= *p && *p <= 'F')
        {
            digit = *p - 'A' + 0xa;
        }
        else
        {
            break;
        }

        if (digit >= base)
        {
            break;
        }

        value = value*base + digit;

        p++;
    }

    if (pstring2)
    {
        *pstring2 = (char *)p;
    }

    *pvalue = sign*value;

    return 1;
}

#ifndef __GNUC__

int vsnprintf (char *buf, int n, const char *fmt, va_list args)
{
    int count = 0;
    char *s = (char *)fmt;
    char *d = buf;

    if (n <= 0)
    {
        return 0;
    }

    n--;

    while (*s && count < n)
    {
        char tmpstr[16];

        char *str = NULL;

        int width = 0;
        int precision = 0;

        if (*s == '%')
        {
            s++;

            if ('0' <= *s && *s <= '9' || *s == '-')
            {
                char setprec = (*s == '0');
                string2ulong (s, &s, (unsigned long *)&width, 10);
                if (setprec)
                {
                    precision = width;
                }
            }

            if (*s == '.')
            {
                s++;
                string2ulong (s, &s, (unsigned long *)&precision, 10);
            }

            if (*s == 's')
            {
                str = va_arg(args, char *);
                s++;
                precision = 0;
            }
            else if (*s == 'c')
            {
                tmpstr[0] = va_arg(args, int);
                tmpstr[1] = 0;
                str = &tmpstr[0];
                s++;
                precision = 0;
            }
            else if (*s == 'p' || *s == 'P')
            {
                int num = va_arg(args, int);

                ulong2string (num, tmpstr, sizeof (tmpstr), 16);

                str = &tmpstr[0];
                s++;
                width = 8;
                precision = 8;
            }
            else
            {
                if (*s == 'l')
                {
                    s++;
                }

                if (*s == 'd' || *s == 'i')
                {
                    int num = va_arg(args, int);

                    long2string (num, tmpstr, sizeof (tmpstr), 10);

                    str = &tmpstr[0];
                    s++;
                }
                else if (*s == 'u')
                {
                    int num = va_arg(args, int);

                    ulong2string (num, tmpstr, sizeof (tmpstr), 10);

                    str = &tmpstr[0];
                    s++;
                }
                else if (*s == 'x' || *s == 'X')
                {
                    int num = va_arg(args, int);

                    ulong2string (num, tmpstr, sizeof (tmpstr), 16);

                    str = &tmpstr[0];
                    s++;
                }
            }
        }

        if (str != NULL)
        {
            int i;
            char numstr[16];
            int len = strlen (str);
            int leftalign = 0;

            if (width < 0)
            {
                width = -width;
                leftalign = 1;
            }

            if (precision)
            {
                i = 0;
                while (precision > len)
                {
                    numstr[i++] = '0';
                    precision--;
                }

                memcpy (&numstr[i], str, len);

                str = &numstr[0];
                len += i;
            }

            if (len < width && !leftalign)
            {
                while (len < width && count < n)
                {
                    *d++ = ' ';
                    width--;
                    count++;
                }

                if (count >= n)
                {
                    break;
                }
            }

            i = 0;
            while (i < len && count < n)
            {
                *d++ = str[i++];
                count++;
            }

            if (count >= n)
            {
                break;
            }

            if (len < width && leftalign)
            {
                while (len < width && count < n)
                {
                    *d++ = ' ';
                    width--;
                    count++;
                }

                if (count >= n)
                {
                    break;
                }
            }
        }
        else
        {
            *d++ = *s++;
            count++;
        }
    }

    *d = '\0';

    return count + 1;
}

#endif // ifndef __GNUC__

#ifdef __GNUC__
int WIN32API_VA ras_snprintf (char *buf, int n, const char *fmt, ...)
#else
int WIN32API snprintf (char *buf, int n, const char *fmt, ...)
#endif
{
    va_list args;

    int rc = 0;

    va_start (args, fmt);

    rc = vsnprintf (buf, n, fmt, args);

    va_end (args);

    return rc;
}

int WIN32API rasOpenLogFile (ULONG *ph, const char *logfilename)
{
    ULONG ulAction = 0;

    int rc = DosOpen (logfilename, ph, &ulAction, 0L, FILE_ARCHIVED,
                      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                      OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYNONE |
                      OPEN_ACCESS_READWRITE, 0L);

    if (rc == ERROR_TOO_MANY_OPEN_FILES)
    {
        LONG lReqCount = 10l;
        ULONG ulCurMaxFH = 0ul;

        DosSetRelMaxFH (&lReqCount, &ulCurMaxFH);

        rc = DosOpen (logfilename, ph, &ulAction, 0L, FILE_ARCHIVED,
                      OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                      OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYNONE |
                      OPEN_ACCESS_READWRITE, 0L);
    }

    if (rc != NO_ERROR)
    {
        *ph = -1;
    }

    return rc;
}

void WIN32API rasCloseLogFile (ULONG h)
{
    DosClose ((HFILE)h);
}

void WIN32API rasWriteLog (ULONG h, char *msg, ULONG len)
{
    ULONG ulActual = 0;

    DosWrite ((HFILE)h, msg, len, &ulActual);
}

void rasCloseLogChannel (RasLogChannel *prlc)
{
    if (prlc->hlogfile != -1)
    {
        prlc->pfnCloseLogFile (prlc->hlogfile);
        prlc->hlogfile = -1;
    }

    if (prlc->hmod)
    {
        DosFreeModule (prlc->hmod);
        prlc->hmod            = NULLHANDLE;
    }

    prlc->pfnWriteLog     = NULL;
    prlc->pfnOpenLogFile  = NULL;
    prlc->pfnCloseLogFile = NULL;
}

int rasOpenLogChannel (const char *env_loghandler, RasLogChannel *prlc,
                       const char *filename)
{
    int rc = NO_ERROR;

    const char *env = NULL;

    DosScanEnv (env_loghandler, &env);

    HMODULE hmod = NULLHANDLE;

    PFN popenlogfile = NULL;
    PFN pcloselogfile = NULL;
    PFN pwritelog = NULL;

    if (env)
    {
        rc = DosLoadModule (NULL, 0, env, &hmod);

        if (rc == NO_ERROR)
        {
            rc = DosQueryProcAddr (hmod, 0, "WIN32RAS_OPENLOGFILE", &popenlogfile);
        }

        if (rc == NO_ERROR)
        {
            rc = DosQueryProcAddr (hmod, 0, "WIN32RAS_CLOSELOGFILE", &pcloselogfile);
        }

        if (rc == NO_ERROR)
        {
            rc = DosQueryProcAddr (hmod, 0, "WIN32RAS_WRITELOG", &pwritelog);
        }

        if (rc != NO_ERROR && hmod)
        {
            DosFreeModule (hmod);
            hmod = NULLHANDLE;
        }
    }

    if (rc == NO_ERROR && hmod && popenlogfile && pcloselogfile && pwritelog)
    {
        prlc->pfnWriteLog     = (FNWL *)pwritelog;
        prlc->pfnOpenLogFile  = (FNOLF *)popenlogfile;
        prlc->pfnCloseLogFile = (FNCLF *)pcloselogfile;
        prlc->hlogfile        = -1;
        prlc->hmod            = hmod;
    }
    else
    {
        prlc->pfnWriteLog     = rasWriteLog;
        prlc->pfnOpenLogFile  = rasOpenLogFile;
        prlc->pfnCloseLogFile = rasCloseLogFile;
        prlc->hlogfile        = -1;
        prlc->hmod            = NULLHANDLE;

        rc = NO_ERROR;
    }

    rc = prlc->pfnOpenLogFile (&prlc->hlogfile, filename);

    if (rc != NO_ERROR)
    {
        prlc->hlogfile = -1;
        rasCloseLogChannel (prlc);
    }

    return rc;
}

int rasInitializeLog (void)
{
    int rc = NO_ERROR;

    const char *filename = "win32ras.log";

    const char *env = NULL;

    DosScanEnv ("WIN32RAS_LOG_FILENAME", &env);

    if (env)
    {
        filename = env;
    }

    char uniqueLogFileName[260];

    snprintf (uniqueLogFileName, sizeof(uniqueLogFileName),
              "%s.%d", filename, loadNr);

    if (rasdata.rlc.hlogfile == -1)
    {
        rc = rasOpenLogChannel ("WIN32RAS_LOGHANDLER", &rasdata.rlc, uniqueLogFileName);
    }

    return rc;
}

void rasUninitializeLog (void)
{
    rasCloseLogChannel (&rasdata.rlc);
}

void rasLogInternalV (RAS_LOG_CHANNEL_H hchannel, const char *fmt, va_list args)
{
    static char szOutMsg[4096];

    ULONG ulHdrLen = snprintf (szOutMsg, sizeof (szOutMsg), "%s", "");

    ulHdrLen -= 1;

    ULONG ulMsgLen = vsnprintf (&szOutMsg[ulHdrLen], sizeof (szOutMsg) - ulHdrLen, fmt, args);

    ulMsgLen -= 1;

    if (ulMsgLen > 0)
    {
        if (!rasdata.fNoEOL)
        {
            if (szOutMsg[ulMsgLen + ulHdrLen - 1] != '\n')
            {
                szOutMsg[ulMsgLen + ulHdrLen] = '\n';
                szOutMsg[ulMsgLen + ulHdrLen + 1] = '\0';
                ulMsgLen++;
            }
        }

        RasLogChannel *prlc = hchannel? (RasLogChannel *)hchannel: &rasdata.rlc;

        prlc->pfnWriteLog (prlc->hlogfile, szOutMsg, ulMsgLen + ulHdrLen);
    }
}

void rasLog (const char *fmt, ...)
{
    va_list args;

    va_start (args, fmt);

    rasLogInternalV (NULL, fmt, args);

    va_end (args);
}

void rasLogInternal (const char *fmt, ...)
{
    va_list args;

    va_start (args, fmt);

    rasLogInternalV (NULL, fmt, args);

    va_end (args);
}

void WIN32API_VA rasLogExternal (const char *fmt, ...)
{
    va_list args;

    USHORT  sel = RestoreOS2FS();

    va_start (args, fmt);

    rasLogInternalV (NULL, fmt, args);

    va_end (args);

    SetFS (sel);
}

/* Private heap functions */
#ifndef PAG_ANY
    #define PAG_ANY    0x00000400
#endif

#ifndef QSV_VIRTUALADDRESSLIMIT
    #define QSV_VIRTUALADDRESSLIMIT 30
#endif

static int privateRefCount = 0;

void * _LNK_CONV getmore_fn (Heap_t heap, size_t *size, int *clean)
{
    APIRET rc;
    void *p;

    rasLog ("RAS heap: getmore_fn(%08xh, %08xh, %08xh)\n", heap, *size, *clean);

    /* round the size up to a multiple of 64K */
    *size = (*size + 0x10000) & 0xFFFF0000ul;

    *clean = _BLOCK_CLEAN;

    rc = DosAllocSharedMem (&p, NULL, *size, rasdata.flAllocMem | OBJ_GETTABLE);

    if (rc != NO_ERROR)
    {
        rasLog ("RAS heap: getmore_fn: DosAllocSharedMem failed, rc = %d\n", rc);
    }

    return p;
}

void _LNK_CONV release_fn (Heap_t heap, void *block, size_t size)
{
    DosFreeMem (block);
}

int rasInitializeHeap (void)
{
    int rc = NO_ERROR;

    // necessary until next WGSS update
    if (++privateRefCount > 1)
    {
        rasLog ("RAS heap initialization: privateRefCount = %d\n", privateRefCount);
        return NO_ERROR;
    }

    if (rasdata.pHeapMem == NULL)
    {
        rasdata.flAllocMem = PAG_READ | PAG_WRITE | PAG_COMMIT;

        if (rascfg.fUseHighMem)
        {
            ULONG ulSysinfo = 0;

            rc = DosQuerySysInfo (QSV_VIRTUALADDRESSLIMIT, QSV_VIRTUALADDRESSLIMIT, &ulSysinfo, sizeof (ulSysinfo));

            if (rc == NO_ERROR && ulSysinfo > 512)   // VirtualAddresslimit is in MB
            {
                rasdata.flAllocMem |= PAG_ANY;

                rasLog ("RAS heap initialization: will use high memory\n");
            }
        }

        rc = DosAllocSharedMem (&rasdata.pHeapMem, NULL, rascfg.ulInitHeapSize,
                                rasdata.flAllocMem | OBJ_GETTABLE);
        if (rc != NO_ERROR)
        {
            rasLog ("RAS heap initialization: DosAllocSharedMem failed %d\n", rc);
            return NO_ERROR;
        }

        rasdata.rasheap = _ucreate (rasdata.pHeapMem, rascfg.ulInitHeapSize,
                                    _BLOCK_CLEAN, _HEAP_REGULAR | _HEAP_SHARED,
                                    getmore_fn, release_fn);

        if (rasdata.rasheap == NULL)
        {
            rasLog ("RAS heap initialization: _ucreate failed\n");

            DosFreeMem (rasdata.pHeapMem);
            rasdata.pHeapMem = NULL;

            return ERROR_NOT_ENOUGH_MEMORY;
        }
    }
    else
    {
        /* Not sure that this is really necessary, VAC heap probably
         * takes care of this in _uopen.
         */
        rc = DosGetSharedMem (rasdata.pHeapMem, rasdata.flAllocMem);

        if (rc != NO_ERROR)
        {
            rasLog ("RAS heap initialization: DosGetSharedMem failed %d\n", rc);

            return rc;
        }

        if (_uopen (rasdata.rasheap) != 0)
        {
            rasLog ("RAS heap initialization: _uopen failed\n");

            DosFreeMem (rasdata.pHeapMem);

            return ERROR_NOT_ENOUGH_MEMORY;
	}
    }

    rasdata.ulRefCount++;

    return NO_ERROR;
}

int _LNK_CONV callback_fn (const void *pentry, size_t sz, int useflag, int status,
                           const char *filename, size_t line)
{
    if (_HEAPOK != status)
    {
       rasLog ("status is not _HEAPOK: %d\n", status);
       return 1;
    }

    if (useflag == _USEDENTRY)
    {
        if (filename)
        {
            rasLog ("%08x %u at %s %d\n", pentry, sz, filename, line);
        }
        else
        {
            rasLog ("%08x %u\n", pentry, sz);
        }
    }

    return 0;
}

void rasUninitializeHeap (void)
{
    if (--privateRefCount > 0)
    {
        rasLog ("RAS heap uninitialization: privateRefCount = %d\n", privateRefCount);

        return;
    }

    if (--rasdata.ulRefCount == 0)
    {
        if (rasdata.rasheap)
        {
            if (rascfg.fDumpHeapObjects)
            {
                rasLog ("Dumping used RAS heap objects:\n");
                _uheap_walk (rasdata.rasheap, callback_fn);
                rasLog ("Used RAS heap objects dump completed.\n");
            }

            _uclose (rasdata.rasheap);

            _udestroy (rasdata.rasheap, _FORCE);
            rasdata.rasheap = NULL;
        }

        if (rasdata.pHeapMem)
        {
            DosFreeMem (rasdata.pHeapMem);
            rasdata.pHeapMem = NULL;
        }
    }
    else
    {
        _uclose (rasdata.rasheap);
    }
}

void *rasAlloc (ULONG size)
{
    void *p = _umalloc(rasdata.rasheap, size);

    if (p)
    {
        memset (p, 0, size);
    }
    else
    {
        rasLog ("RAS heap: allocation failed, %d bytes\n", size);
    }

    return p;
}

void rasFree (void *p)
{
    free (p);
}

/* Private object manipulation functions */

RAS_TRACK_HANDLE rasVerifyTrackHandle (RAS_TRACK_HANDLE h)
{
    RAS_TRACK_HANDLE iter = rasdata.firsttrack;

    while (iter)
    {
        if (iter == h)
        {
            break;
        }

        iter = iter->next;
    }

    return iter;
}

ULONG rasGenObjIdent (void)
{
    static ULONG objident = 0;

    objident++;

    if (objident == 0)
    {
        objident++;
    }

    return objident;
}

int rasAddObjectInfo (RAS_OBJECT_INFO *pinfo, RAS_TRACK_HANDLE h, RAS_OBJECT_INFO *pinfo_next)
{
    if (h->objfirst)
    {
        pinfo->next = h->objfirst;
        h->objfirst->prev = pinfo;
        h->objfirst = pinfo;
    }
    else
    {
        h->objlast = h->objfirst = pinfo;
    }

    return NO_ERROR;
}

void rasRemoveObjectInfo (RAS_OBJECT_INFO *pinfo)
{
    if (pinfo->next)
    {
        pinfo->next->prev = pinfo->prev;
    }
    else
    {
        // this was last object
        pinfo->h->objlast = pinfo->prev;
    }

    if (pinfo->prev)
    {
        pinfo->prev->next = pinfo->next;
    }
    else
    {
        // This was the first object.
        pinfo->h->objfirst = pinfo->next;
    }
}

void rasInitObjectInfo (RAS_OBJECT_INFO *pinfo, RAS_TRACK_HANDLE h,
                       ULONG objhandle, void *objdata, ULONG cbobjdata)
{
    pinfo->objhandle = objhandle;

    pinfo->h = h;

    pinfo->usecount = 1;

    pinfo->objident = rasGenObjIdent ();

    if (h->fLogObjectContent)
    {
        if (objdata && cbobjdata > 0)
        {
            pinfo->objdata = &pinfo->userdata[h->cbuserdata];
            memcpy (pinfo->objdata, objdata, cbobjdata);
        }
    }

    pinfo->cbobjdata = cbobjdata;
}

struct _RAS_OBJECT_INFO *rasSearchObject2 (RAS_TRACK_HANDLE h, ULONG objident)
{
    if (!h)
    {
        return NULL;
    }

    RAS_OBJECT_INFO *iter = h->objfirst;

    while (iter)
    {
        if (iter->objident == objident)
        {
            break;
        }
        iter = iter->next;
    }

    return iter;
}

/* Fast object search function that WILL use some kind ob binary search
 * in object linked list. To achieve this the list of cource must be sorted
 * and this function also WILL return proposed place for a new object
 * in ppinfo_next (object next to searched one, would the serached object
 * in the list already). rasAddObjectInfo WILL then takes use the *ppinfo_next
 * to insert the object in right place in the list.
 */

struct _RAS_OBJECT_INFO *rasSearchObject (RAS_TRACK_HANDLE h, ULONG objhandle, RAS_OBJECT_INFO **ppinfo_next)
{
    if (!h)
    {
        return NULL;
    }

    RAS_OBJECT_INFO *iter = h->objfirst;

    while (iter)
    {
        if (iter->objhandle == objhandle)
        {
            break;
        }

        iter = iter->next;
    }

    if (iter && ppinfo_next)
    {
        *ppinfo_next = iter->next;
    }

    return iter;
}

void rasIncUseCount (RAS_OBJECT_INFO *pinfo)
{
    pinfo->usecount++;
}

int rasDecUseCount (RAS_OBJECT_INFO *pinfo)
{
    pinfo->usecount--;

    return pinfo->usecount;
}

ULONG WIN32API rasLogObjectContent (ULONG objident, ULONG objhandle, void *objdata, ULONG cbobjdata, FNRASLOG_EXTERNAL *pRasLog)
{
    int i;
    char buf[128];
    char *p = &buf[0];

    if (!objdata)
    {
        return NO_ERROR;
    }

    for (i = 0; i < cbobjdata; i++)
    {
        if (i % 16 == 0)
        {
             if (i > 0)
             {
                 pRasLog ("%s\n", buf);
                 p = &buf[0];
             }

             snprintf (p, sizeof(buf) - (p - &buf[0]), "%8.8x:", i / 16);
             p += strlen (p);
        }

        snprintf (p, sizeof(buf) - (p - &buf[0]), " %2.2x", ((char *)objdata)[i]);
        p += strlen (p);
    }

    pRasLog ("%s\n", buf);

    return NO_ERROR;
}

ULONG WIN32API rasCompareObjectContent (ULONG objhandle, void *objdata1, ULONG cbobjdata1, void *objdata2, ULONG cbobjdata2)
{
    if (cbobjdata1 != cbobjdata2)
    {
        return 1;
    }

    if (objdata1 == NULL || objdata2 == NULL)
    {
        // can't compare, assume they are not equal
        return 1;
    }

    return memcmp (objdata1, objdata2, cbobjdata1);
}

ULONG rasCallLogObjectContent (RAS_TRACK_HANDLE h, RASCONTEXT_I *pctx,
                               ULONG objident, ULONG objhandle,
                               void *objdata, ULONG cbobjdata, FNRASLOG_EXTERNAL *rasLogExternal)
{
    rasRestoreContext (pctx);

    ULONG rc = h->pfnLogObjectContent (objident, objhandle, objdata, cbobjdata, rasLogExternal);

    rasSaveContext (pctx);

    return rc;
}

ULONG WIN32API rasCallCompareObjectContent (RAS_TRACK_HANDLE h, RASCONTEXT_I *pctx,
                                            ULONG objhandle, void *objdata1, ULONG cbobjdata1, void *objdata2, ULONG cbobjdata2)
{
    rasRestoreContext (pctx);

    ULONG rc = h->pfnCompareObjectContent (objhandle, objdata1, cbobjdata1, objdata2, cbobjdata2);

    rasSaveContext (pctx);

    return rc;
}

void WIN32API _RasEntry (ULONG ulEvent, void *p, ULONG cb)
{
    return;
}

void rasQueryEnvUlong (const char *name, ULONG *pul, ULONG min, ULONG max, ULONG mult)
{
   const char *env = NULL;

   DosScanEnv (name, &env);

   if (env)
   {
       ULONG ul = 0;
       char *e = NULL;

       string2ulong (env, &e, &ul, 10);

       if (e && *e == '\0')
       {
           if (min <= ul && ul <= max)
           {
               *pul = ul * mult;
           }
       }
   }
}

int rasInitializePlugin (void)
{
    int rc = NO_ERROR;

    if (!rasdata.hmodPlugin)
    {
        rasdata.ret.cb = sizeof (RasEntryTable);

        rasdata.ret.RasRegisterObjectTracking   = RasRegisterObjectTracking;
        rasdata.ret.RasDeregisterObjectTracking = RasDeregisterObjectTracking;
        rasdata.ret.RasAddObject                = RasAddObject;
        rasdata.ret.RasRemoveObject             = RasRemoveObject;
        rasdata.ret.RasSetObjectUserData        = RasSetObjectUserData;
        rasdata.ret.RasQueryObjectUserData      = RasQueryObjectUserData;
        rasdata.ret.RasEnterSerialize           = RasEnterSerialize;
        rasdata.ret.RasExitSerialize            = RasExitSerialize;
        rasdata.ret.RasOpenLogChannel           = RasOpenLogChannel;
        rasdata.ret.RasWriteLogChannel          = RasWriteLogChannel;
        rasdata.ret.RasCloseLogChannel          = RasCloseLogChannel;
        rasdata.ret.RasLog                      = RasLog2;
        rasdata.ret.RasLogNoEOL                 = RasLogNoEOL2;
        rasdata.ret.RasLogMsg                   = RasLogMsg2;
#ifdef __GNUC__
        rasdata.ret.snprintf                    = ras_snprintf;
#else
        rasdata.ret.snprintf                    = snprintf;
#endif
        rasdata.ret.RasSaveContext              = RasSaveContext;
        rasdata.ret.RasRestoreContext           = RasRestoreContext;
        rasdata.ret.RasSetProcAddr              = RasSetProcAddr;
        rasdata.ret.RasGetModuleHandle          = RasGetModuleHandle;
        rasdata.ret.RasCountObjects             = RasCountObjects;
        rasdata.ret.RasTrackMemAlloc            = RasTrackMemAlloc;
        rasdata.ret.RasTrackMemRealloc          = RasTrackMemRealloc;
        rasdata.ret.RasTrackMemFree             = RasTrackMemFree;
        rasdata.ret.RasGetTrackHandle           = RasGetTrackHandle;

        rasdata.pet.cb = sizeof (RasPluginEntryTable);

        rasdata.pet.RasEntry                    = _RasEntry;

        const char *env = NULL;

        DosScanEnv ("WIN32RAS_PLUGIN", &env);

        if (env)
        {
            HMODULE hmod = NULLHANDLE;
            FNPI *pfnPluginInit  = NULL;

            rc = DosLoadModule (NULL, 0, env, &hmod);

            if (rc == NO_ERROR)
            {
                rc = DosQueryProcAddr (hmod, 0, "WIN32RAS_PLUGIN_INIT", (PFN *)&pfnPluginInit);
            }

            if (rc != NO_ERROR)
            {
                rasLogInternal ("Could not load RAS plugin %s rc = %d", env, rc);
            }

            if (rc != NO_ERROR && hmod)
            {
                DosFreeModule (hmod);
            }
            else
            {
                rasdata.hmodPlugin = hmod;

                pfnPluginInit (rasdata.hmod, &rasdata.ret, &rasdata.pet);
            }
        }
    }

    return rc;
}

int rasUninitializePlugin (void)
{
    int rc = NO_ERROR;

    if (rasdata.hmodPlugin)
    {
        HMODULE hmod = rasdata.hmodPlugin;
        FNPE *pfnPluginExit  = NULL;

        rc = DosQueryProcAddr (hmod, 0, "WIN32RAS_PLUGIN_EXIT", (PFN *)&pfnPluginExit);

        if (rc == NO_ERROR)
        {
            pfnPluginExit (rasdata.hmod);
        }

        DosFreeModule (hmod);
    }

    return rc;
}

/* Public RAS functions entry/exit sequences */

#define NO_HANDLE ((RAS_TRACK_HANDLE)-1)

#define ENTER_RAS_RET(a, b)                     \
    if (!a || !rascfg.fRasEnable) return b;     \
    RAS_TRACK_HANDLE old_rasdata_h = rasdata.h; \
    RASCONTEXT_I ctx;                           \
    rasSaveContext (&ctx);                      \
    do {                                        \
        EnterSerializeRAS ();                   \
        if (a != NO_HANDLE) rasdata.h = a;      \
    } while (0)

#define ENTER_RAS(a)                            \
    if (!a || !rascfg.fRasEnable) return;       \
    RAS_TRACK_HANDLE old_rasdata_h = rasdata.h; \
    RASCONTEXT_I ctx;                           \
    rasSaveContext (&ctx);                      \
    do {                                        \
        EnterSerializeRAS ();                   \
        if (a != NO_HANDLE) rasdata.h = a;      \
    } while (0)

#define ENTER_RAS_NO_RESTORE()                      \
    do {                                            \
        if (rascfg.fRasEnable) EnterSerializeRAS ();\
    } while (0)

#define EXIT_RAS() do {                   \
        rasRestoreContext (&ctx);         \
        rasdata.h = old_rasdata_h;        \
        ExitSerializeRAS ();              \
    } while (0)

#define EXIT_RAS_NO_RESTORE() do {                 \
        if (rascfg.fRasEnable) ExitSerializeRAS ();\
    } while (0)



/* Exported functions */

int WIN32API RasInitialize (HMODULE hmod)
{
   int rc = NO_ERROR;

   if (!rasInitialized)
   {
       rasInitialized = 1;

       memset (&rascfg, 0, sizeof (rascfg));

       rascfg.ulTimeout = 60000; // default 1 minute
       rasQueryEnvUlong ("WIN32RAS_TIMEOUT", &rascfg.ulTimeout, 1, 3600, 1000);

       rascfg.ulInitHeapSize = 128*1024;
       rasQueryEnvUlong ("WIN32RAS_INITHEAPSIZE", &rascfg.ulInitHeapSize, 64, 16*1024, 1024);

       ULONG ul = 0;
       rasQueryEnvUlong ("WIN32RAS_DUMPHEAPOBJECTS", &ul, 0, 1, 1);
       rascfg.fDumpHeapObjects = ul;

       ul = 1;
       rasQueryEnvUlong ("WIN32RAS_USEHIGHMEM", &ul, 0, 1, 1);
       rascfg.fUseHighMem = ul;

       ul = 0;
       rasQueryEnvUlong ("WIN32RAS_ENABLE", &ul, 0, 1, 1);
       rascfg.fRasEnable = ul;

       ul = 0;
       rasQueryEnvUlong ("WIN32RAS_BREAKPOINT", &ul, 0, 1, 1);
       rascfg.fRasBreakPoint = ul;

       memset (&rasdata, 0, sizeof (rasdata));
       rasdata.rlc.hlogfile = -1;
       rasdata.hmod = hmod;

       rc = NO_ERROR;
   }

   if (!rascfg.fRasEnable)
   {
       return rc;
   }

   if (rascfg.fRasBreakPoint)
   {
       _interrupt(3);
   }

   rc = rasInitializeLog ();

   if (rc == NO_ERROR)
   {
       rc = rasInitializeCriticalSection (&csras);
   }

   if (rc == NO_ERROR)
   {
       rc = rasInitializeHeap ();
   }

   if (rc == NO_ERROR)
   {
       rc = rasInitializePlugin ();
   }

   return rc;
}

void WIN32API RasUninitialize (void)
{
    ENTER_RAS(NO_HANDLE);

    /* Deregister all objects */
    while (rasdata.firsttrack)
    {
        RAS_TRACK_HANDLE iter = rasdata.firsttrack;

        if (iter->fLogAtExit)
        {
            RasLogObjects (iter, iter->fLogObjectsAtExit? RAS_FLAG_LOG_OBJECTS: 0);
        }

        RasDeregisterObjectTracking (iter);

        rasFree (iter);
    }

    rasUninitializePlugin ();

    rasUninitializeHeap ();

    EXIT_RAS();

    rasUninitializeCriticalSection (&csras);

    rasUninitializeLog ();
}

RAS_TRACK_HANDLE WIN32API RasGetTrackHandle (const char *objname)
{
    ENTER_RAS_RET(NO_HANDLE, NULL);

    RAS_TRACK_HANDLE iter = rasdata.firsttrack;

    while (iter)
    {
        if (stricmp (objname, iter->objname) == 0)
        {
            break;
        }

        iter = iter->next;
    }

    EXIT_RAS();

    return iter;
}

void WIN32API RasRegisterObjectTracking (RAS_TRACK_HANDLE *ph, const char *objname,
                                         ULONG cbuserdata,
                                         ULONG flags,
                                         FNLOC *pfnLogObjectContent,
                                         FNCOC *pfnCompareObjectContent)
{
    ENTER_RAS(NO_HANDLE);

    RAS_TRACK *prt = (RAS_TRACK *)rasAlloc (sizeof (RAS_TRACK));

    if (prt)
    {
        strcpy (prt->objname, objname);

        prt->cbuserdata = cbuserdata;

        if (flags & RAS_TRACK_FLAG_LOGOBJECTCONTENT)
        {
            prt->fLogObjectContent = 1;
        }

        if (flags & RAS_TRACK_FLAG_MEMORY)
        {
            prt->fMemory = 1;
        }

        if (flags & RAS_TRACK_FLAG_LOG_AT_EXIT)
        {
            prt->fLogAtExit = 1;
        }

        if (flags & RAS_TRACK_FLAG_LOG_OBJECTS_AT_EXIT)
        {
            prt->fLogObjectsAtExit = 1;
        }

        if (pfnLogObjectContent)
        {
            prt->pfnLogObjectContent = pfnLogObjectContent;
        }
        else
        {
            prt->pfnLogObjectContent = rasLogObjectContent;
        }

        if (pfnCompareObjectContent)
        {
            prt->pfnCompareObjectContent = pfnCompareObjectContent;
        }
        else
        {
            prt->pfnCompareObjectContent = pfnCompareObjectContent;
        }

        /* Insert the new tracking record in the list */
        if (rasdata.firsttrack)
        {
            prt->next = rasdata.firsttrack;
            rasdata.firsttrack->prev = prt;
            rasdata.firsttrack = prt;
        }
        else
        {
            rasdata.lasttrack = rasdata.firsttrack = prt;
        }

        *ph = prt;
    }

    EXIT_RAS();
}

void WIN32API RasDeregisterObjectTracking (RAS_TRACK_HANDLE h)
{
    ENTER_RAS (h);

    h = rasVerifyTrackHandle (h);

    if (h)
    {
        /* Remove all objects */
        while (h->objfirst)
        {
            RAS_OBJECT_INFO *iter = h->objfirst;

            rasRemoveObjectInfo (iter);

            rasFree (iter);
        }

        /* Remove the track record */
        if (h->next)
        {
            h->next->prev = h->prev;
        }
        else
        {
            // this was last tracking record
            rasdata.lasttrack = h->prev;
        }

        if (h->prev)
        {
            h->prev->next = h->next;
        }
        else
        {
            // This was the first track record
            rasdata.firsttrack = h->next;
        }
    }

    EXIT_RAS ();
}

/* Add a new object in list of tracked ones. */
ULONG WIN32API RasAddObject (RAS_TRACK_HANDLE h, ULONG objhandle,
                            void *objdata, ULONG cbobjdata)
{
    ENTER_RAS_RET (h, 0);

    struct _RAS_OBJECT_INFO *pinfo_next = NULL;

    struct _RAS_OBJECT_INFO *pinfo = rasSearchObject (h, objhandle, &pinfo_next);

//    rasLog ("Object added: handle = %8.8X\n", objhandle);

    if (pinfo != NULL)
    {
        /* Object already in the list. Normally that should not happen and is
         * probably an indication of some problems with the caller's subsystem.
         * The subsystem will take this into account by incrementing use count
         * for this object.
         */
        rasIncUseCount (pinfo);

        /* log this event */
        rasLog ("Dublicate object added: handle = %8.8X\n", objhandle);
        if (h->fLogObjectContent)
        {
            rasLogInternal ("Added object content:\n");
            rasCallLogObjectContent (h, &ctx, pinfo->objident, objhandle, objdata, cbobjdata, rasLogExternal);

            rasLogInternal ("Existing object content:\n");
            rasCallLogObjectContent (h, &ctx, pinfo->objident, objhandle, pinfo->objdata, pinfo->cbobjdata, rasLogExternal);

            if (rasCallCompareObjectContent (h, &ctx, objhandle, objdata, cbobjdata, pinfo->objdata, pinfo->cbobjdata) != 0)
            {
                rasLogInternal ("Objects are different\n");
            }
        }
    }
    else
    {
        pinfo = (RAS_OBJECT_INFO *)rasAlloc (sizeof (RAS_OBJECT_INFO) - sizeof (RAS_OBJECT_INFO::userdata)
                                             + h->cbuserdata
                                             + (objdata? cbobjdata: 0));
        if (pinfo)
        {
            rasInitObjectInfo (pinfo, h, objhandle, objdata, cbobjdata);

            int rc = rasAddObjectInfo (pinfo, h, pinfo_next);

            if (rc != NO_ERROR)
            {
                rasFree (pinfo);
                pinfo = NULL;
            }
        }
    }

    EXIT_RAS ();

    if (pinfo)
    {
        return pinfo->objident;
    }

    return 0;
}

void WIN32API RasRemoveObject (RAS_TRACK_HANDLE h, ULONG objhandle)
{
    ENTER_RAS (h);

//    rasLog ("Object to remove: handle = %8.8X\n", objhandle);

    struct _RAS_OBJECT_INFO *pinfo = rasSearchObject (h, objhandle, NULL);

//    rasLog ("Objects pinfo = %8.8X\n", pinfo);

    if (pinfo != NULL)
    {
        if (rasDecUseCount (pinfo) == 0)
        {
            rasRemoveObjectInfo (pinfo);

            rasFree (pinfo);
        }
    }

    EXIT_RAS ();
}


void WIN32API RasQueryObjectUserData (RAS_TRACK_HANDLE h, ULONG objident, void *pdata, ULONG cbdata, ULONG *pcbdataret)
{
    ENTER_RAS (h);

    struct _RAS_OBJECT_INFO *pinfo = rasSearchObject2 (h, objident);

    if (pinfo)
    {
        if (cbdata > pinfo->h->cbuserdata)
        {
            cbdata = pinfo->h->cbuserdata;
        }

        memcpy (pdata, &pinfo->userdata, cbdata);
    }
    else
    {
        cbdata = 0;
    }

    EXIT_RAS ();

    if (pcbdataret)
    {
        *pcbdataret = cbdata;
    }

    return;
}

void WIN32API RasSetObjectUserData (RAS_TRACK_HANDLE h, ULONG objident, void *pdata, ULONG cbdata, ULONG *pcbdataret)
{
    ENTER_RAS (h);

    struct _RAS_OBJECT_INFO *pinfo = rasSearchObject2 (h, objident);

    if (pinfo)
    {
        if (cbdata > pinfo->h->cbuserdata)
        {
            cbdata = pinfo->h->cbuserdata;
        }

        memcpy (&pinfo->userdata, pdata, cbdata);
    }
    else
    {
        cbdata = 0;
    }

    EXIT_RAS ();

    if (pcbdataret)
    {
        *pcbdataret = cbdata;
    }

    return;
}


void WIN32API_VA RasLog (const char *fmt, ...)
{
    ENTER_RAS (NO_HANDLE);

    va_list args;

    va_start (args, fmt);

    rasLogInternalV (NULL, fmt, args);

    va_end (args);

    EXIT_RAS ();
}

void WIN32API_VA RasLogNoEOL (const char *fmt, ...)
{
    va_list args;

    ENTER_RAS (NO_HANDLE);

    va_start (args, fmt);

    ULONG noeolstate = rasdata.fNoEOL;

    rasdata.fNoEOL = 1;

    rasLogInternalV (NULL, fmt, args);

    rasdata.fNoEOL = noeolstate;

    va_end (args);

    EXIT_RAS ();
}

void WIN32API RasLogMsg (ULONG msg, ULONG parm1, ULONG parm2)
{
    ENTER_RAS (NO_HANDLE);

    EXIT_RAS ();
}

void WIN32API RasLogObjects (RAS_TRACK_HANDLE h, ULONG flags)
{
    ENTER_RAS (h);

    rasLogInternal ("[%s] objects", h->objname);

    if (h->fMemory)
    {
        if (h->cAllocs)
        {
            // RasTrackMem APIs were used
            rasLogInternal ("  allocations = %d, frees = %d", h->cAllocs, h->cFrees);
            rasLogInternal ("  allocated %d bytes", h->cbTotalAllocated);
        }
    }

    RAS_OBJECT_INFO *iter = h->objfirst;

    int count = 0;
    ULONG cb = 0; // count total memory allocated if fMemory is set

    while (iter)
    {
        if (h->fMemory)
        {
            cb += iter->cbobjdata;
        }

        if (flags & RAS_FLAG_LOG_OBJECTS)
        {
            if (h->fMemory)
            {
                rasLogInternal ("  address = %8.8X size = %d", iter->objhandle, iter->cbobjdata);
            }
            else
            {
                rasLogInternal ("  handle = %8.8X\n", iter->objhandle);

                if (h->fLogObjectContent)
                {
                    rasCallLogObjectContent (h, &ctx, iter->objident, iter->objhandle, iter->objdata, iter->cbobjdata, rasLogExternal);
                }
            }
        }

        count++;

        iter = iter->next;
    }

    rasLogInternal ("%d [%s] objects", count, h->objname);

    if (h->fMemory && count > 0)
    {
        rasLogInternal ("%d bytes allocated", cb);
    }

    EXIT_RAS ();

    return;
}

void WIN32API RasCountObjects (RAS_TRACK_HANDLE h, ULONG *pcount, ULONG *pallocated)
{
    ENTER_RAS (h);

    RAS_OBJECT_INFO *iter = h->objfirst;

    int count = 0;
    ULONG cb = 0; // count total memory allocated if fMemory is set

    while (iter)
    {
        if (h->fMemory)
        {
            cb += iter->cbobjdata;
        }

        count++;

        iter = iter->next;
    }

    if (h->fMemory)
    {
        if (pallocated)
        {
            *pallocated = cb + h->cbTotalAllocated;
        }
    }

    if (pcount)
    {
        *pcount = count;
    }

    EXIT_RAS ();

    return;
}

void WIN32API_VA RasLog2 (RAS_LOG_CHANNEL_H hchannel, char *fmt, ...)
{
    ENTER_RAS (NO_HANDLE);

    va_list args;

    va_start (args, fmt);

    rasLogInternalV (hchannel, fmt, args);

    va_end (args);

    EXIT_RAS ();
}

void WIN32API_VA RasLogNoEOL2 (RAS_LOG_CHANNEL_H hchannel, char *fmt, ...)
{
    va_list args;

    ENTER_RAS (NO_HANDLE);

    va_start (args, fmt);

    ULONG noeolstate = rasdata.fNoEOL;

    rasdata.fNoEOL = 1;

    rasLogInternalV (hchannel, fmt, args);

    rasdata.fNoEOL = noeolstate;

    va_end (args);

    EXIT_RAS ();
}

void WIN32API RasLogMsg2 (RAS_LOG_CHANNEL_H hchannel, ULONG msg, ULONG parm1, ULONG parm2)
{
    ENTER_RAS (NO_HANDLE);

    EXIT_RAS ();
}

void WIN32API RasEnterSerialize (void)
{
    ENTER_RAS_NO_RESTORE ();
}

void WIN32API RasExitSerialize (void)
{
    EXIT_RAS_NO_RESTORE ();
}

int WIN32API RasOpenLogChannel (RAS_LOG_CHANNEL_H *phchannel, const char *env_loghandler,
                                const char *filename)
{
    ENTER_RAS_RET (NO_HANDLE, ERROR_GEN_FAILURE);

    int rc = NO_ERROR;

    RasLogChannel *prlc = (RasLogChannel *)rasAlloc (sizeof (RasLogChannel));

    if (!prlc)
    {
        rc = ERROR_NOT_ENOUGH_MEMORY;
    }
    else
    {
        rc = rasOpenLogChannel (env_loghandler, prlc, filename);

        if (rc != NO_ERROR)
        {
            rasFree (prlc);
        }
        else
        {
            *phchannel = (RAS_LOG_CHANNEL_H)prlc;
        }
    }

    EXIT_RAS ();

    return rc;
}

void WIN32API RasWriteLogChannel (RAS_LOG_CHANNEL_H hchannel, const char *msg, ULONG length)
{
    ENTER_RAS (NO_HANDLE);

    if (length > 0)
    {
        RasLogChannel *prlc = (RasLogChannel *)hchannel;

        prlc->pfnWriteLog (prlc->hlogfile, (char *)msg, length);
    }

    EXIT_RAS ();
}

void WIN32API RasCloseLogChannel (RAS_LOG_CHANNEL_H hchannel)
{
    ENTER_RAS (NO_HANDLE);

    RasLogChannel *prlc = (RasLogChannel *)hchannel;

    rasCloseLogChannel (prlc);

    rasFree (prlc);

    EXIT_RAS ();
}


void WIN32API RasEntry (ULONG ulEvent, void *p, ULONG cb)
{
    ENTER_RAS (NO_HANDLE);

    if (rasdata.pet.RasEntry)
    {
        rasdata.pet.RasEntry (ulEvent, p, cb);
    }

    EXIT_RAS ();
}

void WIN32API RasSaveContext(RASCONTEXT *pcontext)
{
    rasSaveContext ((RASCONTEXT_I *)pcontext);
}

void WIN32API RasRestoreContext(RASCONTEXT *pcontext)
{
    rasRestoreContext ((RASCONTEXT_I *)pcontext);
}

FARPROC WIN32API RasSetProcAddr (ULONG hModule, LPCSTR lpszProc, FARPROC pfnNewProc)
{
    return ODIN_SetProcAddress(hModule, lpszProc, pfnNewProc);
}

HMODULE WIN32API GetModuleHandleA(LPCSTR);

ULONG WIN32API RasGetModuleHandle (LPCTSTR lpszModule)
{
    return GetModuleHandleA (lpszModule);
}

void WIN32API RasTrackMemAlloc (RAS_TRACK_HANDLE h, ULONG size)
{
    ENTER_RAS (h);

    if (h->fMemory && size > 0)
    {
        h->cAllocs++;
        h->cbTotalAllocated += size;
    }

    EXIT_RAS ();
}

void WIN32API RasTrackMemRealloc (RAS_TRACK_HANDLE h, ULONG oldsize, ULONG newsize)
{
    ENTER_RAS (h);

    if (h->fMemory)
    {
        h->cbTotalAllocated += newsize - oldsize;
        if (newsize == 0)
        {
            rasLog ("WARNING: RasTrackMemRealloc: newsize = 0");
        }
    }

    EXIT_RAS ();
}

void WIN32API RasTrackMemFree (RAS_TRACK_HANDLE h, ULONG size)
{
    ENTER_RAS (h);

    if (h->fMemory && size > 0)
    {
        h->cFrees++;
        h->cbTotalAllocated -= size;
    }

    EXIT_RAS ();
}

} // extern "C"

