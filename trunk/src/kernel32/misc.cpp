/* $Id: misc.cpp,v 1.30 2001-02-11 10:34:44 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Logging procedures
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Joel Troster
 * Copyright 1998 Peter FitzSimmons
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

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
#include <misc.h>
#include "initterm.h"
#include "logging.h"
#include "exceptutil.h"
#include <wprocess.h>
#include <versionos2.h>

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

//#define CHECK_ODINHEAP
#if defined(DEBUG) && defined(CHECK_ODINHEAP)
int checkOdinHeap = 1;
#define ODIN_HEAPCHECK()    if(checkOdinHeap)   _heap_check();
#else
#define ODIN_HEAPCHECK()
#endif

int SYSTEM WriteLog(char *tekst, ...)
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
        oldcrtmsghandle = _set_crt_msg_handle(fileno(flog));
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
        if(sel == 0x150b && !fIsOS2Image) {
        fprintf(flog, "t%d: (FS=150B) ", teb->o.odin.threadId);
    }
    else    fprintf(flog, "t%d: ", teb->o.odin.threadId);
    }
    vfprintf(flog, tekst, argptr);
    if(teb) teb->o.odin.logfile = 0;
    va_end(argptr);

    if(tekst[strlen(tekst)-1] != '\n')
      fprintf(flog, "\n");
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
void LogException(int state)
{
    TEB *teb = GetThreadTEB();

    if (!teb) return;

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
        if (state == ENTER_EXCEPTION)
        {
            (*lock)--;
        }
        else
        { //LEAVE_EXCEPTION
            (*lock)++;
        }
    }
#else
//kso 2001-01-29: EMX/GCC
// we maybe should do something with the _more->rsem (_rmutex) structure but
// I wanna have this compile, so we'll address problems later.
#endif
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

  fclose(flog);
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
int SYSTEM WriteLogError(char *tekst, ...)
{
  USHORT  sel = RestoreOS2FS();
  va_list argptr;

  va_start(argptr, tekst);
  printf("ERROR: ");
  vprintf(tekst, argptr);
  va_end(argptr);
  if(tekst[strlen(tekst)-1] != '\n')
    printf("\n");

  SetFS(sel);
  return 1;
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
        sprintf(msg, "%s is intended for use with a different release of PE2LX.\n", modname);
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
