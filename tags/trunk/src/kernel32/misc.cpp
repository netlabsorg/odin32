/* $Id: misc.cpp,v 1.7 1999-08-21 19:47:30 sandervl Exp $ */

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
#include "misc.h"


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
int SYSTEM EXPORT WriteLog(char *f, ...)
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

#if 1   /*PLF Mon  97-09-08 20:04:28*/
static FILE *flog = NULL;   /*PLF Mon  97-09-08 20:00:15*/
static BOOL init = FALSE;
static BOOL fLogging = TRUE;

int SYSTEM EXPORT WriteLog(char *tekst, ...)
{
  USHORT  sel = RestoreOS2FS();
  va_list argptr;

  if(!init)
  {
    init = TRUE;

    if(!getenv("NOWIN32LOG"))
      flog = fopen("win32os2.log", "w");
    else
      fLogging = FALSE;
  }

  if(fLogging && flog)
  {
    va_start(argptr, tekst);
    vfprintf(flog, tekst, argptr);
    va_end(argptr);

    if(tekst[strlen(tekst)-1] != '\n')
      fprintf(flog, "\n");
  }

  SetFS(sel);
  return 1;
}

void CloseLogFile()
{
  USHORT  sel = RestoreOS2FS();
  fclose(flog);
  SetFS(sel);
}


int SYSTEM EXPORT WriteLogError(char *tekst, ...)
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
#endif


/*****************************************************************************
 * Modified Standard Version                                                 *
 *****************************************************************************/

#if 0   /*PLF Mon  97-09-08 20:04:26*/
/******************************************************************************/
static BOOL init = FALSE;
static BOOL fLog = TRUE;
/******************************************************************************/
int SYSTEM EXPORT WriteLog(char *tekst, ...)
{
ULONG Action, Wrote;
HFILE log;
APIRET rc;
char message[4096];
va_list argptr;
ULONG openFlags = OPEN_ACTION_CREATE_IF_NEW;

 if(fLog == FALSE)
    return(FALSE);

 if(!init) {
    init       = TRUE;
    openFlags |= OPEN_ACTION_REPLACE_IF_EXISTS;
        if(getenv("NOWIN32LOG"))
            fLog = FALSE;
 }
 else   openFlags |= OPEN_ACTION_OPEN_IF_EXISTS;

 rc = DosOpen(
              "win32os2.log",
              &log,           /* file handle returned */
              &Action,
              0L,
              FILE_NORMAL,
              openFlags,
              OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYWRITE,
              (PEAOP2)NULL);

 rc = DosSetFilePtr(log, 0, FILE_END, &Wrote);
 va_start(argptr, tekst);
 vsprintf(message, tekst, argptr);
 va_end(argptr);

 rc = DosWrite(log, message, strlen(message), &Wrote);

 DosClose(log);   /*PLF Mon  97-09-08 20:01:43*/
 return(TRUE);
}
#endif  /*PLF Mon  97-09-08 20:04:23*/

