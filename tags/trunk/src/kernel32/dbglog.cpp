/* $Id: dbglog.cpp,v 1.13 2004-12-25 16:39:00 sao2l02 Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Debug Logging procedures
 *
 * Copyright 1998-2002 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Joel Troster
 * Copyright 1998 Peter FitzSimmons
 *
 */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/

#define INCL_BASE
#define INCL_WIN
#define INCL_WINERRORS
#define INCL_DOSFILEMGR
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <win32type.h>
#include <win32api.h>
#include <dbglog.h>
#include "initterm.h"
#include "logging.h"
#include "exceptutil.h"
#include <wprocess.h>
#include <versionos2.h>
#include "odinbuild.h"
#include <cpuhlp.h>

ULONG WIN32API dbg_GetThreadCallDepth();
/*****************************************************************************
 * PMPRINTF Version                                                          *
 *****************************************************************************/

#ifdef PMPRINTF

/* ----- Customization variables ----- */
#define PRINTFID      ""
#define PRINTFMAXLEN  300
#define PRINTFLINELEN 100
#define PRINTFTHREADS  54
#define PRINTFQNAME   "\\QUEUES\\PRINTF32"

/* ----- Includes and externals ----- */
#include <stddef.h>                /* .. */
#include <time.h>                  /* .. */

extern ULONG flAllocMem;    /*Tue 03.03.1998: knut */

/* ----- Local defines ----- */
#define PRINTFIDSIZE sizeof(PRINTFID)
#define PRINTFMAXBUF PRINTFIDSIZE+PRINTFLINELEN


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

/* ----- Per-thread output buffer and current indices into line ---- */
struct perthread {
  LONG   lineindex;                /* where next char */
  LONG   tidemark;                 /* rightmost char */
  int    bell;                     /* TRUE if line has bell */
  UCHAR  line[PRINTFMAXBUF];       /* accumulator */
  };

/* ----- Local static variables ----- */
static ULONG  ourpid=0;            /* our process ID */
static ULONG  servepid=0;          /* process IDs of the server */
static HQUEUE qhandle=0;           /* handle for the queue */
static struct perthread *tps[PRINTFTHREADS+1]; /* -> per-thread data */

/* ----- Local subroutine ----- */
static int printf_(struct perthread *);


/* ----------------------------------------------------------------- */
/* The "printf" function.  Note this has a variable number of        */
/* arguments.                                                        */
/* ----------------------------------------------------------------- */
int SYSTEM WriteLog(char *f, ...)
  {
  TIB    *ptib;                    /* process/thread id structures */
  PIB    *ppib;                    /* .. */
  TID    ourtid;                   /* thread ID */
  struct perthread *tp;            /* pointer to per-thread data */
  int    rc;                       /* returncode */
  ULONG  urc;                      /* returncode */

  urc=DosOpenQueue(&servepid, &qhandle, PRINTFQNAME);  /* Open the Q */
  /* Non-0 RC means Q does not exist or cannot be opened */
  if (urc==343) return 0;          /* queue does not exist, so quit */
  if (urc!=0)   return -1;         /* report any other error */

  /* First determine our thread ID (and hence get access to the      */
  /* correct per-thread data.  If the per-thread data has not been   */
  /* allocated, then allocate it now.  It is never freed, once       */
  /* allocated, as PRINTF is not notified of end-of-thread.          */
  DosGetInfoBlocks(&ptib,&ppib);   /* get process/thread info */
  ourtid=ptib->tib_ptib2->tib2_ultid;   /* .. and copy TID */
  if (ourtid>PRINTFTHREADS)        /* too many threads .. */
    return 0;                      /* .. so quit, quietly */
  tp=tps[ourtid];                  /* copy to local pointer */
  if (tp==NULL) {                  /* uninitialized (NULL=0) */
    /* allocate a per-thread structure */
    tp=(struct perthread *)malloc(sizeof(struct perthread));
    if (tp==NULL) return -1;       /* out of memory -- return error */
    tps[ourtid]=tp;                /* save for future calls */
    strcpy(tp->line,PRINTFID);     /* initialize: line.. */
    tp->lineindex=PRINTFIDSIZE-1;  /* ..where next char */
    tp->tidemark =PRINTFIDSIZE-2;  /* ..rightmost char */
    tp->bell=FALSE;                /* ..if line has bell */
    if (ourpid==0) ourpid=ppib->pib_ulpid;   /* save PID for all to use */
    }

  { /* Block for declarations -- only needed if queue exists, etc. */
    LONG  count;                   /* count of characters formatted */
    UCHAR buffer[PRINTFMAXLEN+1];  /* formatting area */
    LONG  i, newind;               /* work */
    UCHAR ch;                      /* .. */
    va_list argptr;                /* -> variable argument list */

    va_start(argptr, f);           /* get pointer to argument list */
    count=vsprintf(buffer, f, argptr);
    va_end(argptr);                /* done with variable arguments */

    if (count<0) return count-1000;/* bad start */

    if (count>PRINTFMAXLEN) {
      /* Disaster -- we are probably "dead", but just in case we */
      /* are not, carry on with truncated data. */
      count=PRINTFMAXLEN;
      }
    buffer[count]='\0';            /* ensure terminated */
    /* OK, ready to go with the data now in BUFFER                    */
    /* We copy from the formatted string to the output (line) buffer, */
    /* taking note of certain control characters and sending a line   */
    /* the queue whenever we see a LF control, or when the line       */
    /* fills (causing a forced break).                                */
    for (i=0; ; i++) {
      ch=buffer[i]; if (!ch) break;
      switch(ch) {
        case '\r':                 /* carriage return */
          tp->lineindex=PRINTFIDSIZE-1; /* back to start of line */
          break;
        case '\n':                 /* new line */
        case '\f':                 /* form feed */
          rc=printf_(tp);          /* print a line */
          if (rc!=0) return rc;    /* error */
          break;
        case '\t':                 /* tab */
          newind=tp->lineindex-PRINTFIDSIZE+1;   /* offset into data */
          newind=tp->lineindex+5-newind%5;    /* new index requested */
          if (newind>=PRINTFMAXBUF) newind=PRINTFMAXBUF;    /* clamp */
          for (; tp->lineindex<newind; tp->lineindex++) {
            if (tp->lineindex>tp->tidemark) {  /* beyond current end */
              tp->line[tp->lineindex]=' ';              /* add space */
              tp->tidemark=tp->lineindex;
              }
            }
          break;
        case '\v':                 /* vertical tab */
          /* ignore it */
          break;
        case '\b':                 /* backspace */
          tp->lineindex=max(tp->lineindex-1,PRINTFIDSIZE);
          break;
        case '\a':                 /* alert (bell) */
          tp->bell=TRUE;
          break;
        default:                   /* ordinary character */
          tp->line[tp->lineindex]=ch;
          if (tp->lineindex>tp->tidemark)  /* is rightmost.. */
            tp->tidemark=tp->lineindex;
          tp->lineindex++;                 /* step for next */
        } /* switch */
      if (tp->lineindex>=PRINTFMAXBUF) {
        rc=printf_(tp);            /* print a line */
        if (rc!=0) return rc;      /* error */
        }

      } /* copy loop */
    return count;                  /* all formatted data processed */
    } /* block */
  } /* printf */

/* ----- printf_(tp) -- Local subroutine to send a line ------------ */
/* A line has been completed (or overflowed): write it to the queue. */
int printf_(struct perthread *tp)  /* pointer to per-thread data */
  {
  ULONG   urc;                     /* unsigned returncode */
  PSZ     pszTo, pszFrom;          /* character pointers */
  PVOID   addr;                    /* address of output data */
  long    size;                    /* total size of output data */
  time_t  timenow;                 /* holds current time */

  tp->line[tp->tidemark+1]='\0';   /* add terminator */
  size=tp->tidemark+2;             /* total length of data */

  /* Get some shared memory that can be given away */
  urc=DosAllocSharedMem(&addr, NULL, (unsigned)size,
    OBJ_GIVEABLE|PAG_WRITE|PAG_COMMIT|flAllocMem);
                                   /*knut: added flAllocMem */

  if (urc!=0) return -2;           /* error */

  pszTo=addr;                      /* copy for clarity */
  pszFrom=&(tp->line[0]);          /* pointer to source */
  strcpy(pszTo,pszFrom);           /* copy the string to shared memory */

  if (ourpid!=servepid) {          /* (no giveaway needed if to self) */
    urc=DosGiveSharedMem(addr, servepid, PAG_READ); /* give access */
    if (urc!=0) return -3;}        /* error */

  /* Write the selector, size, and timestamp to the queue */
  if (tp->bell) size=-size;        /* BELL passed by negation */
  time(&timenow);                  /* optional - else use 0 */
  urc=DosWriteQueue(qhandle,       /* handle */
         (unsigned)timenow,        /* 'request' (timestamp) */
         (unsigned)size,           /* 'length'  (length/bell) */
                   addr,           /* 'address' (address) */
                   0);             /* priority (FIFO if enabled) */
  if (urc!=0) return -4;           /* error */
  if (ourpid!=servepid) {          /* if given away.. */
    urc=DosFreeMem(addr);          /* .. *we* are done with it */
    if (urc!=0) return -5;}        /* error */
  /* Reset the line buffer and indices */
  tp->lineindex=PRINTFIDSIZE-1;    /* where next char */
  tp->tidemark =PRINTFIDSIZE-2;    /* rightmost char */
  tp->bell     =FALSE;             /* true if line has bell */
  return 0;                        /* success! */
  } /* printf_ */
#endif



/*****************************************************************************
 * Standard Version                                                          *
 *****************************************************************************/

static FILE *flog = NULL;   /*PLF Mon  97-09-08 20:00:15*/
static BOOL init = FALSE;
static BOOL fLogging = TRUE;
static int  dwEnableLogging = 1;
static int  oldcrtmsghandle = 0;

static BOOL fDisableThread[5] = {0};
static BOOL fFlushLines = FALSE;

static char *pszLastLogEntry = NULL;

//#define CHECK_ODINHEAP
#if defined(DEBUG) && defined(CHECK_ODINHEAP)
int checkOdinHeap = 1;
int checkingheap = 0;
#define ODIN_HEAPCHECK() \
        if(checkingheap) checkOdinHeap = 0; \
        checkingheap++; \
        if(checkOdinHeap)   _heap_check(); \
        checkingheap--;
#else
#define ODIN_HEAPCHECK()
#endif

//#define LOG_TIME
//#define SHOW_FPU_CONTROLREG
#define WIN32_IP_LOGGING
#define WIN32_IP_LOG_PORT	5001

#ifdef WIN32_IP_LOGGING
#include <types.h>
#include <netinet/in.h>
#include <sys/socket.h>

static int logSocket = -1;
static struct sockaddr_in servername;
#endif

int SYSTEM WriteLog(char *tekst, ...)
{
  USHORT  sel = RestoreOS2FS();
  va_list argptr;
  TEB *teb = GetThreadTEB();

  pszLastLogEntry = tekst;

  ODIN_HEAPCHECK();
  if(!init)
  {
    init = TRUE;

    if(getenv("WIN32LOG_FLUSHLINES")) {
        fFlushLines = TRUE;
    }
#ifdef DEFAULT_LOGGING_OFF
    if(getenv("WIN32LOG_ENABLED")) {
#else
    if(!getenv("NOWIN32LOG")) {
#endif

#ifdef WIN32_IP_LOGGING
        char *logserver = getenv("WIN32LOG_IPSERVER");
        if(logserver && loadNr == 0) {
             sock_init();

             memset(&servername, 0, sizeof(servername));
             servername.sin_family      = AF_INET;
             servername.sin_addr.s_addr = inet_addr(logserver);
             servername.sin_port        = WIN32_IP_LOG_PORT;

             logSocket = socket(PF_INET, SOCK_DGRAM, 0);
        }
#endif
        char szLogFile[CCHMAXPATH];
        const char *pszLogBase = getenv("WIN32LOG_FILEBASE");
        if (!pszLogBase)
            pszLogBase = "odin32_";

        sprintf(szLogFile, "%s%d.log", pszLogBase, loadNr);
        flog = fopen(szLogFile, "w");
        if(flog == NULL)
        {//probably running exe on readonly device
            sprintf(szLogFile, "%sodin32_%d.log", kernel32Path, loadNr);
            flog = fopen(szLogFile, "w");
        }
        oldcrtmsghandle = _set_crt_msg_handle(fileno(flog));
    }
    else
      fLogging = FALSE;

    fDisableThread[0] = getenv("DISABLE_THREAD1") != NULL;
    fDisableThread[1] = getenv("DISABLE_THREAD2") != NULL;
    fDisableThread[2] = getenv("DISABLE_THREAD3") != NULL;
    fDisableThread[3] = getenv("DISABLE_THREAD4") != NULL;
    fDisableThread[4] = getenv("DISABLE_THREAD5") != NULL;
  }

  if(teb) {
      if(teb->o.odin.threadId < 5 && fDisableThread[teb->o.odin.threadId-1] == 1) {
          SetFS(sel);
          return 1;
      }
  }

  if(!tekst) {
    if(flog) fflush( flog);
    SetFS(sel);
    return 1;
  }

  if(fLogging && flog && (dwEnableLogging > 0))
  {
    va_start(argptr, tekst);

#ifdef WIN32_IP_LOGGING
    if(logSocket == -1) {
#endif
    if(teb)
    {
      ULONG ulCallDepth = dbg_GetThreadCallDepth();

      teb->o.odin.logfile = (DWORD)flog;

#ifdef LOG_TIME
      if(sel == 0x150b && fSwitchTIBSel)
        fprintf(flog,
                "t%02d (%3d): (%x) (FS=150B) ",
                LOWORD(teb->o.odin.threadId),
                ulCallDepth,
                GetTickCount());
      else
        fprintf(flog,
                "t%02d (%3d): (%x) ",
                LOWORD(teb->o.odin.threadId),
                ulCallDepth,
                GetTickCount());
#else
      if(sel == 0x150b && fSwitchTIBSel)
        fprintf(flog,
#ifdef SHOW_FPU_CONTROLREG
                "t%02d (%3d)(%3x): ",
                LOWORD(teb->o.odin.threadId),
                ulCallDepth,
                CONTROL87(0,0));
#else
                "t%02d (%3d): (FS=150B) ",
                LOWORD(teb->o.odin.threadId),
                ulCallDepth);
#endif
      else
        fprintf(flog,
#ifdef SHOW_FPU_CONTROLREG
                "t%02d (%3d)(%3x): ",
                LOWORD(teb->o.odin.threadId),
                ulCallDepth,
                CONTROL87(0,0));
#else
                "t%02d (%3d): ",
                LOWORD(teb->o.odin.threadId),
                ulCallDepth);
#endif
#endif
    }
#ifdef LOG_TIME
    else {
        fprintf(flog, "tX: (%x) ", GetTickCount());
    }
#endif
#ifdef WIN32_IP_LOGGING
    }
#endif

#ifdef WIN32_IP_LOGGING
    if(logSocket != -1) {
        char logbuffer[1024];
        int  prefixlen = 0;

        if(teb)
        {
            ULONG ulCallDepth = dbg_GetThreadCallDepth();
#ifdef LOG_TIME
            if(sel == 0x150b && fSwitchTIBSel)
                sprintf(logbuffer, "t%02d (%3d): %x (FS=150B) ",
                        LOWORD(teb->o.odin.threadId), ulCallDepth, GetTickCount());
            else
                sprintf(logbuffer, "t%02d (%3d): %x ",
                        LOWORD(teb->o.odin.threadId), ulCallDepth, GetTickCount());
#else
            if(sel == 0x150b && fSwitchTIBSel)
                sprintf(logbuffer, "t%02d (%3d): (FS=150B) ",
                        LOWORD(teb->o.odin.threadId), ulCallDepth);
            else
                sprintf(logbuffer, "t%02d (%3d): ",
                        LOWORD(teb->o.odin.threadId), ulCallDepth);
#endif
            prefixlen = strlen(logbuffer);
        }

        vsprintf(&logbuffer[prefixlen], tekst, argptr);

        int rc;

        servername.sin_family      = AF_INET;
        servername.sin_port        = WIN32_IP_LOG_PORT;

        rc = sendto(logSocket, logbuffer, strlen(logbuffer)+1, 0, (struct sockaddr *)&servername, sizeof(servername));
        if(rc == -1) {
            rc = sock_errno();
        }
        if(teb) teb->o.odin.logfile = 0;
        va_end(argptr);
    }
    else {
        vfprintf(flog, tekst, argptr);
        if(teb) teb->o.odin.logfile = 0;
        va_end(argptr);

        if(tekst[strlen(tekst)-1] != '\n')
            fprintf(flog, "\n");

        if(fFlushLines)
            fflush(flog);
    }
#else
    vfprintf(flog, tekst, argptr);
    if(teb) teb->o.odin.logfile = 0;
    va_end(argptr);

    if(tekst[strlen(tekst)-1] != '\n')
      fprintf(flog, "\n");

    if(fFlushLines)
      fflush(flog);
#endif
  }
  SetFS(sel);
  return 1;
}
//******************************************************************************
//******************************************************************************
int SYSTEM WriteLogNoEOL(char *tekst, ...)
{
  USHORT  sel = RestoreOS2FS();
  va_list argptr;

  ODIN_HEAPCHECK();

  if(!init)
  {
    init = TRUE;

#ifdef DEFAULT_LOGGING_OFF
    if(getenv("WIN32LOG_ENABLED")) {
#else
    if(!getenv("NOWIN32LOG")) {
#endif
        char logname[CCHMAXPATH];

        sprintf(logname, "odin32_%d.log", loadNr);
        flog = fopen(logname, "w");
        if(flog == NULL) {//probably running exe on readonly device
            sprintf(logname, "%sodin32_%d.log", kernel32Path, loadNr);
            flog = fopen(logname, "w");
        }
    }
    else
      fLogging = FALSE;
  }

  if(fLogging && flog && (dwEnableLogging > 0))
  {
    TEB *teb = GetThreadTEB();

    va_start(argptr, tekst);
    if(teb) {
        teb->o.odin.logfile = (DWORD)flog;
    }
    vfprintf(flog, tekst, argptr);
    if(teb) teb->o.odin.logfile = 0;
    va_end(argptr);
  }
  SetFS(sel);
  return 1;
}
//******************************************************************************
//******************************************************************************
void SYSTEM DecreaseLogCount()
{
  dwEnableLogging--;
}
//******************************************************************************
//******************************************************************************
void SYSTEM IncreaseLogCount()
{
  dwEnableLogging++;
}
//******************************************************************************
//******************************************************************************
int SYSTEM WritePrivateLog(void *logfile, char *tekst, ...)
{
  USHORT  sel = RestoreOS2FS();
  va_list argptr;

  if(fLogging && logfile)
  {
    TEB *teb = GetThreadTEB();

    va_start(argptr, tekst);
    if(teb) {
        teb->o.odin.logfile = (DWORD)flog;
    }
    vfprintf((FILE *)logfile, tekst, argptr);
    if(teb) teb->o.odin.logfile = 0;
    va_end(argptr);

    if(tekst[strlen(tekst)-1] != '\n')
      fprintf((FILE *)logfile, "\n");
  }

  SetFS(sel);
  return 1;
}
//******************************************************************************
//WriteLog has to take special care to handle dprintfs inside our os/2 exception
//handler; if an exception occurs inside a dprintf, using dprintf in the exception
//handler will hang the process
//******************************************************************************
int LogException(int state, int prevlock)
{
    TEB *teb = GetThreadTEB();
    int  ret = 0;

    if (!teb) return 0;

#if !defined(__EMX__)
    if (teb->o.odin.logfile)
    {
#if (__IBMCPP__ == 300) || (__IBMC__ == 300)
        PUSHORT lock = (USHORT *)(teb->o.odin.logfile+0x1C);
#else
#if __IBMC__ >= 360 || __IBMCPP__ >= 360
//TODO: test this!!!!!!!
        PUSHORT lock = (USHORT *)(teb->o.odin.logfile+0x1C);
#else
#error Check the offset of the lock count word in the file stream structure for this compiler revision!!!!!
#endif
#endif
        ret = (*lock);
        if (state == ENTER_EXCEPTION)
        {
            if((*lock) > 0) (*lock)--;
        }
        else
        { //LEAVE_EXCEPTION
            if(prevlock) (*lock)++;
        }
    }
#else
//kso 2001-01-29: EMX/GCC
// we maybe should do something with the _more->rsem (_rmutex) structure but
// I wanna have this compile, so we'll address problems later.
#endif
    return ret;
}
//******************************************************************************
//Check if the exception occurred inside a fprintf (logging THDB member set)
//If true, decrease the lock count for that file stream
//NOTE: HACK: DEPENDS ON COMPILER VERSION!!!!
//******************************************************************************
void CheckLogException()
{
  TEB *teb = GetThreadTEB();
  PUSHORT lock;

  if(!teb) return;

#if !defined(__EMX__)
  if(teb->o.odin.logfile) {
    //oops, exception in vfprintf; let's clear the lock count
#if (__IBMCPP__ == 300) || (__IBMC__ == 300)
    lock = (PUSHORT)(teb->o.odin.logfile+0x1C);
#else
#if __IBMC__ >= 360 || __IBMCPP__ >= 360
//TODO: test this!!!!!!!
        PUSHORT lock = (USHORT *)(teb->o.odin.logfile+0x1C);
#else
#error Check the offset of the lock count word in the file stream structure for this compiler revision!!!!!
#endif
#endif
    (*lock)--;
  }
#else
//kso 2001-01-29: EMX/GCC
// we maybe should do something with the _more->rsem (_rmutex) structure but
// I wanna have this compile, so we'll address problems later.
#endif
}
//******************************************************************************
//NOTE: No need to save/restore FS, as our FS selectors have already been
//      destroyed and FS == 0x150B.
//******************************************************************************
void CloseLogFile()
{
  if(oldcrtmsghandle)
    _set_crt_msg_handle(oldcrtmsghandle);

#ifdef WIN32_IP_LOGGING
  if(logSocket != -1) {
      soclose(logSocket);
  }
#endif

  if(flog) fclose(flog);
  flog = 0;
}
//******************************************************************************
//Used to open any private logfiles used in kernel32 (for now only in winimagepeldr.cpp)
//******************************************************************************
void OpenPrivateLogFiles()
{
#ifdef DEFAULT_LOGGING_OFF
    if(getenv("WIN32LOG_ENABLED")) {
#else
    if(!getenv("NOWIN32LOG")) {
#endif
    OpenPrivateLogFilePE();
    }
}
//******************************************************************************
//Used to close all private logfiles used in kernel32 (for now only in winimagepeldr.cpp)
//******************************************************************************
void ClosePrivateLogFiles()
{
#ifdef DEFAULT_LOGGING_OFF
    if(getenv("WIN32LOG_ENABLED")) {
#else
    if(!getenv("NOWIN32LOG")) {
#endif
        ClosePrivateLogFilePE();
    }
}
//******************************************************************************
//******************************************************************************
void SYSTEM CheckVersion(ULONG version, char *modname)
{
    dprintf(("CheckVersion of %s, %d\n", modname, version));
    if(version != PE2LX_VERSION){
        static char msg[300];
        int r;
        dprintf(("Version mismatch! %d, %d: %s\n", version, PE2LX_VERSION, modname));
        sprintf(msg, "%s is intended for use with a different release of Odin.\n", modname);
        do{
            r = WinMessageBox(HWND_DESKTOP, NULLHANDLE, msg, "Version Mismatch!", 0, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_MOVEABLE);
        }while(r == MBID_RETRY);   // giggle
        if( r != MBID_IGNORE )
            exit(987);
    }
}
//******************************************************************************
//******************************************************************************
void SYSTEM CheckVersionFromHMOD(ULONG version, HMODULE hModule)
{
    char name[_MAX_PATH];

    // query name of dll.
    if(!DosQueryModuleName(hModule, sizeof(name), name))
        CheckVersion(version, name);
}
//******************************************************************************
//******************************************************************************
#ifdef __WATCOMC__  /*PLF Sat  97-06-21 17:12:36*/
    extern void interrupt3( void );
    #pragma aux interrupt3= \
              "int 3"
#endif
void WIN32API DebugBreak()
{
  dprintf(("DebugBreak\n"));

  LPSTR lpstrEnv = getenv("WIN32.DEBUGBREAK");              /* query environment */
  if (lpstrEnv == NULL)      /* if environment is not set, don't call debugger ! */
     return;

#ifdef __WATCOMC__
  interrupt3();
#else
  _interrupt(3);
#endif
}
//******************************************************************************
//******************************************************************************


/*****************************************************************************
 * Name      : DebugErrorBox
 * Purpose   : display an apprioriate error box with detailed information
 *             about the error cause
 * Parameters: APIRET iErrorCode - OS/2 error code
 *             PSZ    pszFormat  - printf-format string
 *             ...
 * Variables :
 * Result    : return code of the message box
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/09/13 19:55]
 *****************************************************************************/

int SYSTEM DebugErrorBox(ULONG  iErrorCode,
                         char*  pszFormat,
                         ...)
{
  char   szMessageBuffer[1024];              /* buffer for the text message */
  char   szErrorBuffer[1024];       /* buffer for the operating system text */
  ULONG  bc;                                                       /* dummy */
  APIRET rc2;                                             /* API returncode */
  int    iRC;                                    /* message box return code */

  USHORT  sel = RestoreOS2FS();
  va_list argptr;

  // clear memory
  memset (szMessageBuffer, 0, sizeof(szMessageBuffer));
  memset (szErrorBuffer, 0, sizeof(szErrorBuffer));

  // build message string
  va_start(argptr, pszFormat);
  vsprintf(szMessageBuffer, pszFormat, argptr);
  va_end(argptr);

  // query error string
  rc2 = DosGetMessage(NULL,
                      0,
                      szErrorBuffer,
                      sizeof(szErrorBuffer),
                      iErrorCode,
                      "OSO001.MSG",
                      &bc);

  // display message box
  iRC = WinMessageBox(HWND_DESKTOP,
                      NULLHANDLE,
                      szMessageBuffer,
                      szErrorBuffer,
                      0,
                      MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
  SetFS(sel);
  return iRC;
}


/**
 * Query Odin32 build number.
 * @returns Build number.
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int     WIN32API Odin32GetBuildNumber(void)
{
    dprintf(("Odin32GetBuildNumber returned %d\n", ODIN32_BUILD_NR));
    return ODIN32_BUILD_NR;
}

/*****************************************************************************
 * Name      : debug_c
 * Parameters: LPSTR targetpointer, int W\Achar, BOOL WCHAR yes/not
 *             ...
 * Result    : modified targetpointer points after converted (W)char
 * Remark    :
 * Status    :
 *
 * Author    : source debugtools modified: Dietrich Teickner
 *****************************************************************************/

LPSTR debug_c (LPSTR dst, unsigned int c, BOOL LongChar)
{
   *dst = '\\';
   switch (c)
   {
   case '\n': dst++ ; c = 'n'; break;
   case '\r': dst++ ; c = 'r'; break;
   case '\t': dst++ ; c = 't'; break;
   case '"': dst++ ; /* c = '"'; */ break;
   case '\\': dst++ ; /* c = '\\';*/ break;
   default:
     if (c < ' ' || c > 126)
     {
       dst++;
       if (LongChar)
/* different format for non ascii-wchars */
       {
         sprintf(dst,"%04x",c);
         dst+=3;
         c = *dst;
       }
       else
       {
         *dst++ = '0' + ((c >> 6) & 7);
         *dst++ = '0' + ((c >> 3) & 7);
         c = '0' + ((c >> 0) & 7);
       }
     }
   }
   *dst++ = c;
  return dst;
}

/* ------------------------------------------------------------------------- *
 * CallBackHelperfunction for Init the Debugstring Pointer Strukture         *
 * ------------------------------------------------------------------------- */
debugstr_data* WIN32API GetDebugStrDataInit(debugstr_data* retData) {
  if (retData) {
    retData->pToRes[0] = retData->pToRes[1] = &retData->res[0];
    retData->pToRes[2] = &retData->res[sizeof(retData->res)];
  }
  return retData;
}
/* ------------------------------------------------------------------------- *
 * CallBackHelperfunction for Alloc + Init the Debugstring Pointer Strukture *
 * ------------------------------------------------------------------------- */
debugstr_data* WIN32API GetDebugStrPtr(debugstr_data* (WIN32API *pInit)(debugstr_data* retData), int size);
/* ------------------------------------------------------------------------- *
 * Helperfunction for semistatic strings                                     *
 * returns as a funktion of the current threadid the debugstr-structure      *
 * ------------------------------------------------------------------------- */
debugstr_data* GetDebugStrData() {
  debugstr_data* retData = NULL;
#ifdef DEBUG
  retData = GetDebugStrPtr(GetDebugStrDataInit,sizeof(debugstr_data));
#endif
  return retData;
}
/* ------------------------------------------------------------------------- *
 * Helperfunction for semistatic strings                                     *
 * returns as a temporary reserved string with contence of parm-string       *
 * ------------------------------------------------------------------------- */
LPCSTR debugstr_x(LPCSTR src)
{
  debugstr_data *ad = GetDebugStrData();
  LPSTR retdst = "(error)";
  if (ad)
  {
    LPSTR dst;
    LPSTR dstend;
    int iLen = strlen(src)+1;
    dstend = ad->pToRes[2] - iLen;
    retdst = dst = ad->pToRes[0];
    if ((dst < ad->pToRes[1]) || (dst > dstend))
    {
       if (dst == ad->pToRes[1])
    /* loop, if iLen larger then (ad->pToRes[2] - ad->pToRes[1]) */
          iLen = (ad->pToRes[2] - ad->pToRes[1]);
       retdst = ad->pToRes[1];
    }
    ad->pToRes[0] = retdst + iLen;
    memcpy(retdst,src,iLen);
    retdst[iLen-1] = '\0';
  }
  return retdst;
}
/* ---------------------------------------------------------------------- */
/*****************************************************************************
 * Name      : debugstr_an, debugstr_wn
 * Parameters: LPC(W)STR soucepointer, int max-len
 *             ...
 * Variables : soucepointer, max-len, 3-pointer bufferstructure
 * Result    : pointer to resultstringbuffer
 * Remark    : do not free the string, it is contence of a buffer
 * Status    :
 *
 * Author    : Dietrich Teickner
 *****************************************************************************/

LPCSTR SYSTEM debugstr_an(LPCSTR src, int n)
{
  LPCSTR retdst = "(null)";
  if (n < 0) n = -1;
  if (src)
  {
    char temp[92];
    LPSTR pFree = NULL;
    LPSTR dst = NULL;
    LPSTR dststart = &temp[0];
    LPSTR dstend = &temp[sizeof(temp)];
#undef debugstr_reserve
#define debugstr_reserve ((sizeof("\\999") - 1) + sizeof("\"..."))
#undef debugstr_start
#define debugstr_start (sizeof('"'))
    if (HIWORD(src))
    {
      int iDstLen = n + (debugstr_start + debugstr_reserve);
      if (iDstLen > (_debugstr_data_res_len/2)) iDstLen = (_debugstr_data_res_len/2);
      if (iDstLen > sizeof(temp))
        pFree = (CHAR*)malloc(iDstLen);
      if (pFree)
      {
        dststart = pFree;
        dstend = pFree + iDstLen;
      }
      dst = dststart + debugstr_start;
      dstend -= debugstr_reserve;
      dststart[0] = '"';

      while (n-- != 0 && *src && (dst < dstend))
      {
        dst = debug_c (dst, (BYTE) *src++, FALSE);
      }
      strcpy(dst,"\"...");
      if (!*src)
        dst[1] = '\0';

    }
    else
    {
      sprintf(dststart, "#%04x", LOWORD(src));
    }
    retdst = debugstr_x(dststart);
    if (pFree)
      free(pFree);
  } /* endif (src != NULL) */
  return retdst;
}

/* ---------------------------------------------------------------------- */

LPCSTR SYSTEM debugstr_wn(LPCWSTR src, int n)
{
  LPCSTR retdst = "(null)";
  if (n < 0) n = -1;
  if (src)
  {
    char temp[92];
    LPSTR pFree = NULL;
    LPSTR dst = NULL;
    LPSTR dststart = &temp[0];
    LPSTR dstend = &temp[sizeof(temp)];
#undef debugstr_reserve
#define debugstr_reserve ((sizeof("\\xxxx") - 1) + sizeof("\"..."))
#undef debugstr_start
#define debugstr_start (sizeof('L') + sizeof('"'))
    if (HIWORD(src))
    {
      int iDstLen = n + (debugstr_start + debugstr_reserve);
      if (iDstLen > (_debugstr_data_res_len/2)) iDstLen = (_debugstr_data_res_len/2);
      if (iDstLen > sizeof(temp))
        pFree = (CHAR*)malloc(iDstLen);
      if (pFree)
      {
        dststart = pFree;
        dstend = pFree + iDstLen;
      }
      dst = dststart + debugstr_start;
      dstend -= debugstr_reserve;
      dststart[0] = 'L';
      dststart[1] = '"';

      while (n-- != 0 && *src && (dst < dstend))
      {
        dst = debug_c (dst, (WORD) *src++, TRUE);
      }
      strcpy(dst,"\"...");
      if (!*src)
        dst[1] = '\0';

    }
    else
    {
      sprintf(dststart, "#%04x", LOWORD(src));
    }
    retdst = debugstr_x(dststart);
    if (pFree)
      free(pFree);
  } /* endif (src != NULL) */
  return retdst;
}

typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;

LPCSTR SYSTEM debugstr_guid( const GUID *id )
{
    LPCSTR str = "(null)";
    char temp[64];
    if (id)
    {
      if (HIWORD(id))
      {
        sprintf( temp, "{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                 id->Data1, id->Data2, id->Data3,
                 id->Data4[0], id->Data4[1], id->Data4[2], id->Data4[3],
                 id->Data4[4], id->Data4[5], id->Data4[6], id->Data4[7] );
      }
      else
      {
        sprintf( temp, "<guid-0x%04x>", LOWORD(id));
      }
      str = debugstr_x(temp);
    }
    return str;
}

