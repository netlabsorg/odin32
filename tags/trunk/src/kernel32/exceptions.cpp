/* $Id: exceptions.cpp,v 1.50 2001-02-14 10:36:45 sandervl Exp $ */

/* WARNING: Compiling this module with ICC with optimizations turned on   */
/* currently breaks this module. To get correct code, it is not necessary */
/* to turn all optimizations off, just use the -Op- flag.                 */

/*
 * Win32 Exception functions for OS/2
 *
 * Ported Wine exception handling code
 *
 * Copyright 1998 Sander van Leeuwen (OS/2 port)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 * (dlls\ntdll\exception.c)
 *
 * Copyright 1999 Turchanov Sergey
 * Copyright 1999 Alexandre Julliard
 *
 * (win32\except.c)
 *
 * Win32 exception functions
 *
 * Copyright (c) 1996 Onno Hovers, (onno@stack.urc.tue.nl)
 *
 * Notes:
 *  What really happens behind the scenes of those new
 *  __try{...}__except(..){....}  and
 *  __try{...}__finally{...}
 *  statements is simply not documented by Microsoft. There could be different
 *  reasons for this:
 *  One reason could be that they try to hide the fact that exception
 *  handling in Win32 looks almost the same as in OS/2 2.x.
 *  Another reason could be that Microsoft does not want others to write
 *  binary compatible implementations of the Win32 API (like us).
 *
 *  Whatever the reason, THIS SUCKS!! Ensuring portabilty or future
 *  compatability may be valid reasons to keep some things undocumented.
 *  But exception handling is so basic to Win32 that it should be
 *  documented!
 *
 * Fixmes:
 *  -Most functions need better parameter checking.
 *  -I do not know how to handle exceptions within an exception handler.
 *   or what is done when ExceptionNestedException is returned from an
 *   exception handler
 *  -Real exceptions are not yet implemented. only the exception functions
 *   are implemented. A real implementation needs some new code in
 *   loader/signal.c. There would also be a need for showing debugging
 *   information in UnhandledExceptionFilter.
 *
 */
#define INCL_MISC
#define INCL_BASE
#define INCL_WINBUTTONS
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <builtin.h>
#include "exceptions.h"
#include "exceptutil.h"
#include <misc.h>
#include "mmap.h"
#include <wprocess.h>
#include "oslibexcept.h"
#include "exceptstackdump.h"

#define DBG_LOCALLOG    DBG_exceptions
#include "dbglocal.h"

/* Exception record for handling exceptions happening inside exception handlers */
typedef struct
{
    WINEXCEPTION_FRAME frame;
    WINEXCEPTION_FRAME *prevFrame;
} EXC_NESTED_FRAME;

//Global Process Unhandled exception filter
static LPTOP_LEVEL_EXCEPTION_FILTER CurrentUnhExceptionFlt = NULL;
static UINT                         CurrentErrorMode = SEM_FAILCRITICALERRORS;
static PEXCEPTION_HANDLER           StartupCodeHandler = NULL;

extern "C" PWINEXCEPTION_FRAME GetExceptionRecord(ULONG offset, ULONG segment);

LONG WIN32API UnhandledExceptionFilter(PWINEXCEPTION_POINTERS lpexpExceptionInfo);
void KillWin32Process(void);

#ifdef DEBUG
void PrintWin32ExceptionChain(PWINEXCEPTION_FRAME pframe);
#else
#define PrintWin32ExceptionChain(a)
#endif

/*****************************************************************************
 * Name      : UINT SetErrorMode
 * Purpose   :
 * Parameters: UINT fuErrorMode
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Tue, 1999/07/01 09:00]
 *****************************************************************************/

UINT WIN32API SetErrorMode(UINT fuErrorMode)
{
  UINT oldmode = CurrentErrorMode;

  dprintf(("KERNEL32: SetErrorMode(%08xh)\n",
           fuErrorMode));
  CurrentErrorMode = fuErrorMode;

  if(fuErrorMode & SEM_FAILCRITICALERRORS || fuErrorMode & SEM_NOOPENFILEERRORBOX)
    DosError(FERR_DISABLEHARDERR);
  else
    DosError(FERR_ENABLEHARDERR);

  // SEM_NOGPFAULTERRORBOX  and SEM_NOALIGNMENTFAULTEXCEPT --> UnhandledExceptionFilter()

  return(oldmode);
}

static inline WINEXCEPTION_FRAME * EXC_push_frame( WINEXCEPTION_FRAME *frame )
{
    // TODO: rewrite in assembly
    TEB *teb = GetThreadTEB();
    frame->Prev = (PWINEXCEPTION_FRAME)teb->except;
    teb->except = frame;
    return frame->Prev;
}

static inline WINEXCEPTION_FRAME * EXC_pop_frame( WINEXCEPTION_FRAME *frame )
{
    // TODO: rewrite in assembly
    TEB *teb = GetThreadTEB();
    teb->except = frame->Prev;
    return frame->Prev;
}

/*****************************************************************************
 * Name      : VOID _Pascal OS2RaiseException
 * Purpose   : Unwinds exception handlers (heavily influenced by Wine)
 * Parameters: ...
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Tue, 1999/07/01 09:00]
 *****************************************************************************/

void WIN32API RtlRaiseException(WINEXCEPTION_RECORD *rec, WINCONTEXT *context);

VOID _Pascal OS2RaiseException(DWORD dwExceptionCode,
                               DWORD dwExceptionFlags,
                               DWORD cArguments,
                               DWORD *lpArguments,
                               DWORD eip, DWORD esp,  DWORD ebp, DWORD flags,
                               DWORD eax, DWORD ebx,  DWORD ecx, DWORD edx,
                               DWORD edi, DWORD esi,  DWORD cs,  DWORD ds,
                               DWORD es,  DWORD fs,   DWORD gs,  DWORD ss)
{
  WINEXCEPTION_RECORD   record;
  WINEXCEPTION_POINTERS ExceptionInfo;
  WINCONTEXT            context;
  int                   rc;
  int                   i;

  dprintf(("KERNEL32: RaiseException(%08xh)\n",
           dwExceptionCode));

  memset(&record, 0, sizeof(record));

  /* compose an exception record */
  record.ExceptionCode       = dwExceptionCode;
  record.ExceptionFlags      = dwExceptionFlags;
  record.ExceptionRecord     = NULL;
  record.NumberParameters    = cArguments;
  record.ExceptionAddress    = (LPVOID)eip;

  memset(&context, 0, sizeof(context));
  context.ContextFlags = WINCONTEXT_FULL; //segments, integer, control
  context.SegGs  = gs;
  context.SegFs  = fs;
  context.SegEs  = es;
  context.SegDs  = ds;
  context.Edi    = edi;
  context.Esi    = esi;
  context.Ebx    = ebx;
  context.Edx    = edx;
  context.Ecx    = ecx;
  context.Eax    = eax;
  context.Ebp    = ebp;
  context.Eip    = eip;
  context.SegCs  = cs;
  context.EFlags = flags;
  context.Esp    = esp;
  context.SegSs  = ss;

  if(lpArguments)
  {
    for(i=0;
        i<cArguments;
        i++)
      record.ExceptionInformation[i] = lpArguments[i];
  }

  rc = RtlDispatchException(&record, &context);

  // and finally, the unhandled exception filter
  if(rc == ExceptionContinueSearch && UnhandledExceptionFilter != NULL)
  {
    dprintf(("KERNEL32: RaiseException calling UnhandledExceptionFilter.\n"));

    ExceptionInfo.ExceptionRecord = &record;
    ExceptionInfo.ContextRecord   = &context;

    rc = UnhandledExceptionFilter(&ExceptionInfo);
    //FIXME: UnhandledExceptionFilter does NOT return the same values as
    //       other filters!!
  }

  // terminate the process
  if(rc != ExceptionContinueExecution ||
     record.ExceptionFlags & EH_NONCONTINUABLE)
  {
    dprintf(("KERNEL32: RaiseException terminating process.\n"));
    DosExit(EXIT_PROCESS, 0);
  }

  dprintf(("KERNEL32: RaiseException returns.\n"));
  return;
}

/*******************************************************************
 *         EXC_RaiseHandler
 *
 * Handler for exceptions happening inside a handler.
 */
static DWORD WIN32API EXC_RaiseHandler( WINEXCEPTION_RECORD *rec, WINEXCEPTION_FRAME *frame,
//                             WINCONTEXT *context, WINEXCEPTION_FRAME **dispatcher )
                               WINCONTEXT *context, LPVOID dispatcher )
{
    if (rec->ExceptionFlags & (EH_UNWINDING | EH_EXIT_UNWIND))
        return ExceptionContinueSearch;
    /* We shouldn't get here so we store faulty frame in dispatcher */
    *(PWINEXCEPTION_FRAME*)dispatcher = ((EXC_NESTED_FRAME*)frame)->prevFrame;
    return ExceptionNestedException;
}

/*******************************************************************
 *         EXC_UnwindHandler
 *
 * Handler for exceptions happening inside an unwind handler.
 */
static DWORD WIN32API EXC_UnwindHandler( WINEXCEPTION_RECORD *rec, WINEXCEPTION_FRAME *frame,
//                              WINCONTEXT *context, WINEXCEPTION_FRAME **dispatcher )
                                WINCONTEXT *context, LPVOID dispatcher )
{
    if (!(rec->ExceptionFlags & (EH_UNWINDING | EH_EXIT_UNWIND)))
        return ExceptionContinueSearch;
    /* We shouldn't get here so we store faulty frame in dispatcher */
    *(PWINEXCEPTION_FRAME*)dispatcher = ((EXC_NESTED_FRAME*)frame)->prevFrame;
    return ExceptionCollidedUnwind;
}

/*******************************************************************
 *         EXC_CallHandler
 *
 * Call an exception handler, setting up an exception frame to catch exceptions
 * happening during the handler execution.
 * WARNING:
 * Please do not change the first 4 parameters order in any way - some exceptions handlers
 * rely on Base Pointer (EBP) to have a fixed position related to the exception frame
 */
static DWORD EXC_CallHandler( WINEXCEPTION_RECORD *record, WINEXCEPTION_FRAME *frame,
                              WINCONTEXT *context, WINEXCEPTION_FRAME **dispatcher,
                              PEXCEPTION_HANDLER handler, PEXCEPTION_HANDLER nested_handler)
{
    EXC_NESTED_FRAME newframe;
    DWORD ret;

    newframe.frame.Handler = nested_handler;
    newframe.prevFrame     = frame;
    EXC_push_frame( &newframe.frame );
    dprintf(("KERNEL32: Calling handler at %p code=%lx flags=%lx\n",
           handler, record->ExceptionCode, record->ExceptionFlags));
    ret = handler( record, frame, context, dispatcher );
    dprintf(("KERNEL32: Handler returned %lx\n", ret));
    EXC_pop_frame( &newframe.frame );
    return ret;
}

//******************************************************************************
//******************************************************************************
DWORD RtlDispatchException(WINEXCEPTION_RECORD *pRecord, WINCONTEXT *pContext)
{
  PWINEXCEPTION_FRAME   pFrame, dispatch, nested_frame;
  int                   rc;

  // get chain of exception frames
  rc  = ExceptionContinueSearch;

  nested_frame = NULL;
  TEB *winteb = GetThreadTEB();
  pFrame      = (PWINEXCEPTION_FRAME)winteb->except;

  dprintf(("KERNEL32: RtlDispatchException entered"));

  PrintWin32ExceptionChain(pFrame);

  // walk the exception chain
  while( (pFrame != NULL) && (pFrame != ((void *)0xFFFFFFFF)) )
  {
        dprintf(("KERNEL32: RtlDispatchException - pframe=%08X, pframe->Prev=%08X", pFrame, pFrame->Prev));
        if (pFrame == pFrame->Prev) {
            dprintf(("KERNEL32: RtlDispatchException - Invalid exception frame!!"));
            return 0;
        }

        dispatch=0;

        /* Check frame address */
        if (((void*)pFrame < winteb->stack_low) ||
            ((void*)(pFrame+1) > winteb->stack_top) ||
            (int)pFrame & 3)
        {
            dprintf(("Invalid stack! low=%08X, top=%08X, pframe = %08X",
                    winteb->stack_low, winteb->stack_top, pFrame));

            pRecord->ExceptionFlags |= EH_STACK_INVALID;
            break;
        }


        /* call handler */
        rc = EXC_CallHandler(pRecord, pFrame, pContext, &dispatch, pFrame->Handler, EXC_RaiseHandler );

        PrintWin32ExceptionChain(pFrame);

        if (pFrame == nested_frame)
        {
            /* no longer nested */
            nested_frame = NULL;
            pRecord->ExceptionFlags &= ~EH_NESTED_CALL;
        }


        switch(rc)
        {
        case ExceptionContinueExecution:
            if (!(pRecord->ExceptionFlags & EH_NONCONTINUABLE)) {
                dprintf(("KERNEL32: RtlDispatchException returns %#x (ContinueExecution)", rc));
                return rc;
            }
            break;
        case ExceptionContinueSearch:
            break;
        case ExceptionNestedException:
            if (nested_frame < dispatch) nested_frame = dispatch;
            pRecord->ExceptionFlags |= EH_NESTED_CALL;
            break;
        default:
            break;
        }

        dprintf(("KERNEL32: RtlDispatchException - going from frame %08X to previous frame %08X", pFrame, pFrame->Prev));
        if (pFrame == pFrame->Prev) {
            dprintf(("KERNEL32: RtlDispatchException - Invalid exception frame!!"));
            break;
        }
        pFrame = pFrame->Prev;
  }
  dprintf(("KERNEL32: RtlDispatchException returns %#x", rc));
  PrintWin32ExceptionChain(pFrame);
  return rc;
}
/*****************************************************************************
 * Name      : int _Pascal OS2RtlUnwind
 * Purpose   : Unwinds exception handlers (heavily influenced by Wine)
 * Parameters: ...
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Tue, 1999/07/01 09:00]
 *****************************************************************************/

int _Pascal OS2RtlUnwind(PWINEXCEPTION_FRAME  pEndFrame,
                         LPVOID unusedEip,
                         PWINEXCEPTION_RECORD pRecord,
                         DWORD  returnEax,
                         DWORD eip, DWORD esp,  DWORD ebp, DWORD flags,
                         DWORD eax, DWORD ebx,  DWORD ecx, DWORD edx,
                         DWORD edi, DWORD esi,  DWORD cs,  DWORD ds,
                         DWORD es,  DWORD fs,   DWORD gs,  DWORD ss)
{
  PWINEXCEPTION_FRAME frame, dispatch;
  WINEXCEPTION_RECORD record, newrec;
  WINCONTEXT          context;
  DWORD               rc;

  dprintf(("KERNEL32: RtlUnwind pEndFrame=%08X, unusedEip=%08X, pRecord=%08X, returnEax=%#x\n", pEndFrame, unusedEip, pRecord, returnEax));


  memset(&context, 0, sizeof(context));
  context.ContextFlags = WINCONTEXT_FULL;   //segments, integer, control
  context.SegGs  = gs;
  context.SegFs  = fs;
  context.SegEs  = es;
  context.SegDs  = ds;
  context.Edi    = edi;
  context.Esi    = esi;
  context.Ebx    = ebx;
  context.Edx    = edx;
  context.Ecx    = ecx;
  context.Eax    = returnEax;
  context.Ebp    = ebp;
  context.Eip    = eip;
  context.SegCs  = cs;
  context.EFlags = flags;
  context.Esp    = esp;
  context.SegSs  = ss;

  /* build an exception record, if we do not have one */
  if(!pRecord)
  {
    memset(&record, 0, sizeof(record));
    record.ExceptionCode    = STATUS_UNWIND;
    record.ExceptionFlags   = 0;
    record.ExceptionRecord  = NULL;
    record.ExceptionAddress = (LPVOID)eip;
    record.NumberParameters = 0;
    pRecord = &record;
  }

  if(pEndFrame) pRecord->ExceptionFlags |= EH_UNWINDING;
  else          pRecord->ExceptionFlags |= EH_UNWINDING | EH_EXIT_UNWIND;

  /* get chain of exception frames */
  TEB *winteb = GetThreadTEB();
  frame       = (PWINEXCEPTION_FRAME)winteb->except;

  PrintWin32ExceptionChain(frame);

  while ((frame != (PWINEXCEPTION_FRAME)0xffffffff) && (frame != pEndFrame))
  {
        /* Check frame address */
        if (pEndFrame && (frame > pEndFrame))
        {
            newrec.ExceptionCode    = STATUS_INVALID_UNWIND_TARGET;
            newrec.ExceptionFlags   = EH_NONCONTINUABLE;
            newrec.ExceptionRecord  = pRecord;
            newrec.NumberParameters = 0;
            dprintf(("KERNEL32: RtlUnwind terminating thread.\n"));
            DosExit(EXIT_THREAD, 0);
        }
        if (((void*)frame < winteb->stack_low) ||
            ((void*)(frame+1) > winteb->stack_top) ||
            (int)frame & 3)
        {
            newrec.ExceptionCode    = STATUS_BAD_STACK;
            newrec.ExceptionFlags   = EH_NONCONTINUABLE;
            newrec.ExceptionRecord  = pRecord;
            newrec.NumberParameters = 0;
            dprintf(("KERNEL32: RtlUnwind terminating thread.\n"));
            DosExit(EXIT_THREAD, 0);
        }

        /* Call handler */
        dprintf(("KERNEL32: RtlUnwind - calling exception handler %08X", frame->Handler));
        rc = EXC_CallHandler(pRecord, frame, &context, &dispatch, frame->Handler, EXC_UnwindHandler );
        dprintf(("KERNEL32: RtlUnwind - handler returned %#x", rc));
        switch (rc)
        {
        case ExceptionContinueSearch:
            break;
        case ExceptionCollidedUnwind:
            frame = dispatch;
            break;
        default:
            newrec.ExceptionCode    = STATUS_INVALID_DISPOSITION;
            newrec.ExceptionFlags   = EH_NONCONTINUABLE;
            newrec.ExceptionRecord  = pRecord;
            newrec.NumberParameters = 0;
            dprintf(("KERNEL32: RtlUnwind terminating thread.\n"));
            DosExit(EXIT_THREAD, 0);
            break;
        }
        dprintf(("KERNEL32: RtlUnwind (before)- frame=%08X, frame->Prev=%08X", frame, frame->Prev));
        SetExceptionChain((DWORD)frame->Prev);
        frame = frame->Prev;
        dprintf(("KERNEL32: RtlUnwind (after) - frame=%08X, frame->Prev=%08X", frame, frame->Prev));
  }

  dprintf(("KERNEL32: RtlUnwind returning.\n"));
  PrintWin32ExceptionChain(frame);
  return(0);
}


/*****************************************************************************
 * Name      : LONG WIN32API UnhandledExceptionFilter
 * Purpose   :
 * Parameters: ...
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Tue, 1999/07/01 09:00]
 *****************************************************************************/

LONG WIN32API UnhandledExceptionFilter(PWINEXCEPTION_POINTERS lpexpExceptionInfo)
{
  char  message[72];
  DWORD rc;

  // @@@PH: experimental change to have more control over exception handling
#pragma pack(4)
  typedef struct
   {
      ULONG    cb;                  /* Size of fixed part of structure          */
      HPOINTER hIcon;               /* Icon handle                              */
      ULONG    cButtons;            /* Number of buttons                        */
      ULONG    flStyle;             /* Icon style flags (MB_ICONQUESTION, etc...)*/
      HWND     hwndNotify;          /* Reserved                                 */
      MB2D     mb2d[4];             /* Array of button definitions              */
   } myMB2INFO;
#pragma pack()

  myMB2INFO mb2InfoExceptionBox = { 20,             // size of structure
                                    NULLHANDLE,     // icon handle
                                    4,              // number of buttons
                                    MB_ICONHAND,    // icon style
                                    NULLHANDLE,     // reserved
                                    { {"continue ~search",    100, BS_PUSHBUTTON | BS_TEXT | BS_AUTOSIZE},
                                      {"continue ~execution", 101, BS_PUSHBUTTON | BS_TEXT | BS_AUTOSIZE},
                                      {"execute ~handler",    102, BS_PUSHBUTTON | BS_TEXT | BS_AUTOSIZE | BS_DEFAULT},
                                      {"~terminate process",  103, BS_PUSHBUTTON | BS_TEXT | BS_AUTOSIZE} }
                                  };

  dprintf(("KERNEL32: Default UnhandledExceptionFilter, CurrentErrorMode=%X", CurrentErrorMode));

  // We must not care about ErrorMode here!! The app expects that its own
  // UnhandledExceptionFilter will be cared even if it never touched ErrorMode.
  if(CurrentUnhExceptionFlt) // && !(CurrentErrorMode & (SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX)))
  {
    dprintf(("KERNEL32: Calling user UnhandledExceptionFilter"));
    rc = CurrentUnhExceptionFlt(lpexpExceptionInfo);
    if(rc != WINEXCEPTION_CONTINUE_SEARCH)
      return rc;
  }

  sprintf(message,
          "Unhandled exception 0x%08lx at address 0x%08lx.",
          lpexpExceptionInfo->ExceptionRecord->ExceptionCode,
          lpexpExceptionInfo->ExceptionRecord->ExceptionAddress);
/*
  rc = WinMessageBox2(HWND_DESKTOP,
                     HWND_DESKTOP,
                     message,
                     "Oh, nooo!",
                     0,
                     (PMB2INFO)&mb2InfoExceptionBox);
   switch (rc)
   {
     case 100:
       return WINEXCEPTION_CONTINUE_SEARCH;

     case 101:
       return WINEXCEPTION_CONTINUE_EXECUTION;

     case 102:
       return WINEXCEPTION_EXECUTE_HANDLER;

     case 103:
       KillWin32Process();
       // fall-through

     default:
       return WINEXCEPTION_EXECUTE_HANDLER;
   }
*/
  rc = WinMessageBox(HWND_DESKTOP,
                     HWND_DESKTOP,
                     message,
                     "Oh, nooo!",
                     0,
                     MB_ABORTRETRYIGNORE | MB_ERROR);
   switch (rc)
   {
     case MBID_IGNORE:
       return WINEXCEPTION_CONTINUE_EXECUTION;

     case MBID_ABORT:
       KillWin32Process();
       // fall-through

     case MBID_RETRY:
     default:
       return WINEXCEPTION_EXECUTE_HANDLER;
   }
}


/*****************************************************************************
 * Name      : LPTOP_LEVEL_EXCEPTION_FILTER WIN32API SetUnhandledExceptionFilter
 * Purpose   :
 * Parameters: ...
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Tue, 1999/07/01 09:00]
 *****************************************************************************/

LPTOP_LEVEL_EXCEPTION_FILTER WIN32API SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
  LPTOP_LEVEL_EXCEPTION_FILTER old = CurrentUnhExceptionFlt;

  dprintf(("KERNEL32: SetUnhandledExceptionFilter to %08X\n",
           lpTopLevelExceptionFilter));

  CurrentUnhExceptionFlt = lpTopLevelExceptionFilter;

  return(old);
}


/*****************************************************************************
 * Name      : KillWin32Process
 * Purpose   :
 * Parameters: ...
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Tue, 1999/07/01 09:00]
 *****************************************************************************/


//******************************************************************************
extern "C" ULONG getEAX();
extern "C" ULONG getEBX();
VOID WIN32API ExitProcess(DWORD exitcode);
//******************************************************************************
void KillWin32Process(void)
{
 static BOOL fEntry = FALSE;

  if(fEntry == FALSE) {
        fEntry = TRUE;
        ExitProcess(666);
        return;
  }
  //Restore original OS/2 TIB selector
  RestoreOS2FS();

  SetExceptionChain((ULONG)-1);
  DosExit(EXIT_PROCESS, 666);
}


/*****************************************************************************
 * Name      : void static dprintfException
 * Purpose   : log the exception to win32os2.log
 * Parameters: ...
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/01 09:00]
 *****************************************************************************/

static void dprintfException(PEXCEPTIONREPORTRECORD       pERepRec,
                             PEXCEPTIONREGISTRATIONRECORD pERegRec,
                             PCONTEXTRECORD               pCtxRec,
                             PVOID                        p)
{
  PSZ    pszExceptionName = "<unknown>";        /* points to name/type excpt */
  char   szData[128];          /* local storage for excpt dep.   information */
  char   szData2[128];         /* local storage for excpt dep.   information */
  APIRET rc               = XCPT_CONTINUE_SEARCH;        /* excpt-dep.  code */
  BOOL   fExcptSoftware   = FALSE;         /* software/hardware gen. exceptn */
  BOOL   fExcptFatal      = TRUE;                       /* fatal exception ? */
  BOOL   fExcptPortable   = TRUE;                /* portability of exception */
  PPIB   pPIB;                                  /* process information block */
  PTIB   pTIB;                                  /* thread  information block */
  ULONG  ulModule;                                          /* module number */
  ULONG  ulObject;                        /* object number within the module */
  CHAR   szModule[260];                        /* buffer for the module name */
  ULONG  ulOffset;             /* offset within the object within the module */

  szData[0]  = 0;                                              /* initialize */
  szData2[0] = 0;                                              /* initialize */
  switch(pERepRec->ExceptionNum)                    /* take according action */
  {
                        /* portable, non-fatal software-generated exceptions */
    case XCPT_GUARD_PAGE_VIOLATION:
      pszExceptionName = "Guard Page Violation";
      sprintf(szData,
              "R/W %08xh at %08xh.",
              pERepRec->ExceptionInfo[0],
              pERepRec->ExceptionInfo[1]);
      fExcptSoftware   = TRUE;
      fExcptFatal      = FALSE;
      rc               = XCPT_CONTINUE_EXECUTION;
      break;

    case XCPT_UNABLE_TO_GROW_STACK:
      pszExceptionName = "Unable To Grow Stack";
      fExcptSoftware   = TRUE;
      fExcptFatal      = FALSE;
      rc               = XCPT_CONTINUE_EXECUTION;
      break;

                           /* portable, fatal, hardware-generated exceptions */
    case XCPT_ACCESS_VIOLATION:
      pszExceptionName  = "Access Violation";
      /* sprintf (szData, "Access type %08x at %08x.", pERepRec->ExceptionInfo[0],
               pERepRec->ExceptionInfo[1]); */
      switch (pERepRec->ExceptionInfo[0])
      {
        case XCPT_READ_ACCESS:
           sprintf (szData,
                    "Read Access at address %08xh",
                    pERepRec->ExceptionInfo[1]);
           break;

        case XCPT_WRITE_ACCESS:
           sprintf (szData,
                    "Write Access at address %08x",
                    pERepRec->ExceptionInfo[1]);
           break;

        case XCPT_SPACE_ACCESS:
           sprintf (szData,
                    "Space Access at selector %08x",
                    pERepRec->ExceptionInfo[1]);
           break;

        case XCPT_LIMIT_ACCESS:
           strcpy (szData,
                   "Limit Access");
           break;

        case XCPT_UNKNOWN_ACCESS:
           strcpy (szData,
                   "Unknown Access");
           break;

        default:
           strcpy (szData,
                   "(Invalid Access Code)");
           break;
      }
      break;

    case XCPT_INTEGER_DIVIDE_BY_ZERO:
      pszExceptionName  = "Division By Zero (Integer)";
      break;

    case XCPT_FLOAT_DIVIDE_BY_ZERO:
      pszExceptionName  = "Division By Zero (Float)";
      break;

    case XCPT_FLOAT_INVALID_OPERATION:
      pszExceptionName  = "Invalid Floating Point Operation";
      break;

    case XCPT_ILLEGAL_INSTRUCTION:
      pszExceptionName  = "Illegal Instruction";
      break;

    case XCPT_PRIVILEGED_INSTRUCTION:
      pszExceptionName  = "Privileged Instruction";
      break;

    case XCPT_INTEGER_OVERFLOW:
      pszExceptionName  = "Integer Overflow";
      break;

    case XCPT_FLOAT_OVERFLOW:
      pszExceptionName  = "Floating Point Overflow";
      break;

    case XCPT_FLOAT_UNDERFLOW:
      pszExceptionName  = "Floating Point Underflow";
      break;

    case XCPT_FLOAT_DENORMAL_OPERAND:
      pszExceptionName  = "Floating Point Denormal Operand";
      break;

    case XCPT_FLOAT_INEXACT_RESULT:
      pszExceptionName  = "Floating Point Inexact Result";
      break;

    case XCPT_FLOAT_STACK_CHECK:
      pszExceptionName  = "Floating Point Stack Check";
      break;

    case XCPT_DATATYPE_MISALIGNMENT:
      pszExceptionName  = "Datatype Misalignment";
      sprintf(szData,
              "R/W %08x alignment %08x at %08x.",
              pERepRec->ExceptionInfo[0],
              pERepRec->ExceptionInfo[1],
              pERepRec->ExceptionInfo[2]);
      break;

    case XCPT_BREAKPOINT:
      pszExceptionName  = "Breakpoint (don't debug me! :)";
      break;

    case XCPT_SINGLE_STEP:
      pszExceptionName  = "Single Step (don't debug me! :)";
      break;

                           /* portable, fatal, software-generated exceptions */
    case XCPT_IN_PAGE_ERROR:
      pszExceptionName  = "In Page Error";
      sprintf(szData,
              "at %08x.",
              pERepRec->ExceptionInfo[0]);
      fExcptSoftware    = TRUE;
      break;

    case XCPT_PROCESS_TERMINATE:
      pszExceptionName  = "Process Termination";
      fExcptSoftware    = TRUE;
      break;

    case XCPT_ASYNC_PROCESS_TERMINATE:
      pszExceptionName  = "Process Termination (async)";
      sprintf(szData,
              "terminating thread TID=%u",
              pERepRec->ExceptionInfo[0]);
      fExcptSoftware    = TRUE;
      break;

    case XCPT_NONCONTINUABLE_EXCEPTION:
      pszExceptionName  = "Noncontinuable Exception";
      fExcptSoftware    = TRUE;
      break;

    case XCPT_INVALID_DISPOSITION:
      pszExceptionName  = "Invalid Disposition";
      fExcptSoftware    = TRUE;
      break;

                                           /* non-portable, fatal exceptions */
    case XCPT_INVALID_LOCK_SEQUENCE:
      pszExceptionName  = "Invalid Lock Sequence";
      fExcptSoftware    = TRUE;
      fExcptPortable    = FALSE;
      break;

    case XCPT_ARRAY_BOUNDS_EXCEEDED:
      pszExceptionName  = "Array Bounds Exceeded";
      fExcptSoftware    = TRUE;
      fExcptPortable    = FALSE;
      break;

                                              /* unwind operation exceptions */
    case XCPT_UNWIND:
      pszExceptionName  = "Unwind Exception";
      fExcptSoftware    = TRUE;
      fExcptPortable    = FALSE;
      break;

    case XCPT_BAD_STACK:
      pszExceptionName  = "Unwind Exception, Bad Stack";
      fExcptSoftware    = TRUE;
      fExcptPortable    = FALSE;
      break;

    case XCPT_INVALID_UNWIND_TARGET:
      pszExceptionName  = "Unwind Exception, Invalid Target";
      fExcptSoftware    = TRUE;
      fExcptPortable    = FALSE;
      break;

                                                  /* fatal signal exceptions */
    case XCPT_SIGNAL:
      pszExceptionName = "Signal";
      sprintf(szData,
              "Signal Number = %08x",
              pERepRec->ExceptionInfo[0]);
      fExcptSoftware    = TRUE;
      fExcptPortable    = FALSE;

      switch (pERepRec->ExceptionInfo[0])          /* resolve signal information */
      {
        case XCPT_SIGNAL_INTR:
           pszExceptionName = "Signal (Interrupt)";
           break;

        case XCPT_SIGNAL_KILLPROC:
           pszExceptionName = "Signal (Kill Process)";
           break;

        case XCPT_SIGNAL_BREAK:
           pszExceptionName = "Signal (Break)";
           break;
      }
      break;

    default:
       pszExceptionName = "(unknown exception code)";
       sprintf(szData,
               "Exception Code = %08x",
               pERepRec->ExceptionNum);
  }


                                  /* now dump the information to the logfile */
  dprintf(("---[Exception Information]------------\n"));
  sprintf(szData2,
          "   %s (",
          pszExceptionName);

  if (fExcptSoftware == TRUE)            /* software or hardware generated ? */
     strcat (szData2, "software generated,");
  else
     strcat (szData2, "hardware generated,");

  if (fExcptPortable == TRUE)                        /* portable exception ? */
     strcat (szData2, "portable,");
  else
     strcat (szData2, "non-portable,");

  if (fExcptFatal    == TRUE)                           /* fatal exception ? */
     strcat (szData2, "fatal");
  else
     strcat (szData2, "non-fatal");

  strcat (szData2,                                     /* add trailing brace */
          ")\n");


  dprintf((szData2));

  if (szData[0] != 0)                 /* see if there is an additional entry */
     dprintf(("   %s\n",
              szData));

  rc = DosQueryModFromEIP(&ulModule,
                          &ulObject,
                          sizeof(szModule),
                          szModule,
                          &ulOffset,
                          (ULONG)pERepRec->ExceptionAddress);

  dprintf(("   Exception Address = %08x ",
           pERepRec->ExceptionAddress));

  if(rc == NO_ERROR && ulObject != -1)
    dprintf(("<%s> (#%u), obj #%u:%08x\n",
             szModule,
             ulModule,
             ulObject,
             ulOffset));
  else
    dprintf(("<win32 app>\n"));


  rc = DosGetInfoBlocks (&pTIB,           /* query kernel information blocks */
                         &pPIB);
  if (rc == NO_ERROR)
  {
    dprintf(("   Thread:  Ordinal TID: %u, TID: %u, Priority: %04xh\n",
                pTIB->tib_ordinal,
                pTIB->tib_ptib2->tib2_ultid,
                pTIB->tib_ptib2->tib2_ulpri));

    dprintf(("   Process: PID: %u, Parent: %u, Status: %u\n",
              pPIB->pib_ulpid,
              pPIB->pib_ulppid,
              pPIB->pib_flstatus));
  }

  if (pCtxRec->ContextFlags & CONTEXT_CONTROL)         /* check flags */
    dprintf(("   SS:ESP=%04x:%08x EFLAGS=%08x\n",
             pCtxRec->ctx_SegSs,
             pCtxRec->ctx_RegEsp,
             pCtxRec->ctx_EFlags));
    dprintf(("   CS:EIP=%04x:%08x EBP   =%08x\n",
             pCtxRec->ctx_SegCs,
             pCtxRec->ctx_RegEip,
             pCtxRec->ctx_RegEbp));

  if (pCtxRec->ContextFlags & CONTEXT_INTEGER)         /* check flags */
    dprintf(("   EAX=%08x EBX=%08x ESI=%08x\n",
             pCtxRec->ctx_RegEax,
             pCtxRec->ctx_RegEbx,
             pCtxRec->ctx_RegEsi));
    dprintf(("   ECX=%08x EDX=%08x EDI=%08x\n",
             pCtxRec->ctx_RegEcx,
             pCtxRec->ctx_RegEdx,
             pCtxRec->ctx_RegEdi));

  if (pCtxRec->ContextFlags & CONTEXT_SEGMENTS)        /* check flags */
    dprintf(("   DS=%04x     ES=%08x",
              pCtxRec->ctx_SegDs,
              pCtxRec->ctx_SegEs));
    dprintf(("   FS=%04x     GS=%04x\n",
              pCtxRec->ctx_SegFs,
              pCtxRec->ctx_SegGs));

  if (pCtxRec->ContextFlags & CONTEXT_FLOATING_POINT)  /* check flags */
  {
    ULONG ulCounter;                 /* temporary local counter for fp stack */

    dprintf(("   Env[0]=%08x Env[1]=%08x Env[2]=%08x Env[3]=%08x\n",
             pCtxRec->ctx_env[0],
             pCtxRec->ctx_env[1],
             pCtxRec->ctx_env[2],
             pCtxRec->ctx_env[3]));

    dprintf(("   Env[4]=%08x Env[5]=%08x Env[6]=%08x\n",
             pCtxRec->ctx_env[4],
             pCtxRec->ctx_env[5],
             pCtxRec->ctx_env[6]));

    for (ulCounter = 0;
         ulCounter < 8; /* see TOOLKIT\INCLUDE\BSEEXPT.H, _CONTEXT structure */
         ulCounter ++)
      dprintf(("   FP-Stack[%u] losig=%08x hisig=%08x signexp=%04x\n",
               ulCounter,
               pCtxRec->ctx_stack[0].losig,
               pCtxRec->ctx_stack[0].hisig,
               pCtxRec->ctx_stack[0].signexp));
  }

  dprintf(("---[End Of Exception Information]-----\n"));
}

/*****************************************************************************
 * Name      : ERR _System OS2ExceptionHandler
 * Purpose   :
 * Parameters: ...
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1999/07/01 09:00]
 *****************************************************************************/
                                                             /* from PPC DDK */
#ifndef XCPT_CONTINUE_STOP
#define XCPT_CONTINUE_STOP 0x00716668
#endif

ULONG APIENTRY OS2ExceptionHandler(PEXCEPTIONREPORTRECORD       pERepRec,
                                   PEXCEPTIONREGISTRATIONRECORD pERegRec,
                                   PCONTEXTRECORD               pCtxRec,
                                   PVOID                        p)
{
  //SvL: Check if exception inside debug fprintf -> if so, clear lock so
  //     next dprintf won't wait forever
  LogException(ENTER_EXCEPTION);

  /* Access violation at a known location */
  switch(pERepRec->ExceptionNum)
  {
  case XCPT_FLOAT_DENORMAL_OPERAND:
  case XCPT_FLOAT_DIVIDE_BY_ZERO:
  case XCPT_FLOAT_INEXACT_RESULT:
//  case XCPT_FLOAT_INVALID_OPERATION:
  case XCPT_FLOAT_OVERFLOW:
  case XCPT_FLOAT_STACK_CHECK:
  case XCPT_FLOAT_UNDERFLOW:
        dprintfException(pERepRec, pERegRec, pCtxRec, p);
        dprintf(("KERNEL32: OS2ExceptionHandler: FPU exception\n"));
        if(!fIsOS2Image && !fExitProcess)  //Only for real win32 apps
        {
                if(OSLibDispatchException(pERepRec, pERegRec, pCtxRec, p) == FALSE)
                {
                        pCtxRec->ctx_env[0] |= 0x1F;
                        pCtxRec->ctx_stack[0].losig = 0;
                        pCtxRec->ctx_stack[0].hisig = 0;
                        pCtxRec->ctx_stack[0].signexp = 0;
                }
                dprintf(("KERNEL32: OS2ExceptionHandler: fix and continue\n"));
                goto continueexecution;
        }
        else
        {
                dprintf(("KERNEL32: OS2ExceptionHandler: continue search\n"));
                goto continuesearch;
        }

  case XCPT_PROCESS_TERMINATE:
  case XCPT_ASYNC_PROCESS_TERMINATE:
        dprintfException(pERepRec, pERegRec, pCtxRec, p);
        SetExceptionChain((ULONG)-1);
        goto continuesearch;

  case XCPT_ACCESS_VIOLATION:
  {
   Win32MemMap *map;
   BOOL  fWriteAccess = FALSE;
   ULONG offset, accessflag;

        if(pERepRec->ExceptionInfo[1] == 0 && pERepRec->ExceptionInfo[1] == XCPT_DATA_UNKNOWN) {
                goto continueFail;
        }
//------------->>> WARNING: potentially dangerous workaround!!
        /* Some apps set ES = FS and Odin doesn't like that!       */
        /* Note: maybe we could even check for ES != DS? But maybe */
        /* that might cause more harm than good...                 */
        if (pCtxRec->ContextFlags & CONTEXT_SEGMENTS)
            if (pCtxRec->ctx_SegEs == pCtxRec->ctx_SegFs) {
                /* Let's just reset ES to the DS value and hope it's okay */
                pCtxRec->ctx_SegEs = pCtxRec->ctx_SegDs;
                goto continueexecution;
        }

        switch(pERepRec->ExceptionInfo[0]) {
        case XCPT_READ_ACCESS:
                accessflag = MEMMAP_ACCESS_READ;
                break;
        case XCPT_WRITE_ACCESS:
                accessflag = MEMMAP_ACCESS_WRITE;
                fWriteAccess = TRUE;
                break;
        case XCPT_EXECUTE_ACCESS:
                accessflag = MEMMAP_ACCESS_EXECUTE;
                break;
        default:
                goto continueFail;
        }

        map = Win32MemMapView::findMapByView(pERepRec->ExceptionInfo[1], &offset, accessflag);
        if(map == NULL) {
                goto continueFail;
        }
        if(map->commitPage(offset, fWriteAccess) == TRUE)
                goto continueexecution;

        //no break;
  }
continueFail:

////#define DEBUGSTACK
#ifdef DEBUGSTACK
  if(pCtxRec->ContextFlags & CONTEXT_CONTROL) {
        ULONG *stackptr;
        APIRET rc;
        int    i;
        ULONG  ulOffset, ulModule, ulObject;
        CHAR   szModule[CCHMAXPATH];

        stackptr = (ULONG *)pCtxRec->ctx_RegEsp;
        dprintf(("Stack DUMP:"));
        for(i=0;i<16;i++) {
                rc = DosQueryModFromEIP(&ulModule,
                                        &ulObject,
                                        sizeof(szModule),
                                        szModule,
                                        &ulOffset,
                                        (ULONG)*stackptr);

                if (rc == NO_ERROR)
                        dprintf(("0x%8x: 0x%8x %s (#%u), obj #%u:%08x", stackptr, *stackptr, szModule, ulModule, ulObject, ulOffset));
                else    dprintf(("0x%8x: 0x%8x", stackptr, *stackptr));
                stackptr++;
        }
        dprintf(("Stack DUMP END"));
  }
#endif

  case XCPT_BREAKPOINT:
  case XCPT_ARRAY_BOUNDS_EXCEEDED:
  case XCPT_DATATYPE_MISALIGNMENT:
  case XCPT_ILLEGAL_INSTRUCTION:
  case XCPT_PRIVILEGED_INSTRUCTION:
  case XCPT_INVALID_LOCK_SEQUENCE:
  case XCPT_INTEGER_DIVIDE_BY_ZERO:
  case XCPT_INTEGER_OVERFLOW:
  case XCPT_SINGLE_STEP:
  case XCPT_UNABLE_TO_GROW_STACK:
  case XCPT_IN_PAGE_ERROR:
CrashAndBurn:
        //SvL: TODO: this may not always be the right thing to do
        //MN: If EH_NESTED_CALL is set, an exception occurred during the execution
        //    of this exception handler. We better bail out ASAP or we'll likely
        //    recurse infinitely until we run out of stack space!!
        if (pERepRec->fHandlerFlags & EH_NESTED_CALL)
                goto continuesearch;

#ifdef DEBUG
        dprintfException(pERepRec, pERegRec, pCtxRec, p);
        if(pCtxRec->ContextFlags & CONTEXT_CONTROL) {
                dbgPrintStack(pERepRec, pERegRec, pCtxRec, p);
        }
#endif
        if(!fIsOS2Image && !fExitProcess)  //Only for real win32 apps
        {
                if(OSLibDispatchException(pERepRec, pERegRec, pCtxRec, p) == TRUE)
                {
                        goto continueexecution;
                }
        }
        else    goto continuesearch; //pass on to OS/2 RTL or app exception handler

        dprintf(("KERNEL32: OS2ExceptionHandler: Continue and kill\n"));
        pCtxRec->ctx_RegEip = (ULONG)KillWin32Process;
        pCtxRec->ctx_RegEsp = pCtxRec->ctx_RegEsp + 0x10;
        pCtxRec->ctx_RegEax = pERepRec->ExceptionNum;
        pCtxRec->ctx_RegEbx = pCtxRec->ctx_RegEip;
        goto continueexecution;

  //@@@PH: growing thread stacks might need special treatment
  case XCPT_GUARD_PAGE_VIOLATION:
        //SvL: don't print anything here -> fatal hang if happens inside fprintf
        //dprintf(("KERNEL32: OS2ExceptionHandler: trying to grow stack (continue search)"));
        goto continuesearch;

  case XCPT_SIGNAL:
        if(pERepRec->ExceptionInfo[0] == XCPT_SIGNAL_KILLPROC)          /* resolve signal information */
        {
            SetExceptionChain((ULONG)-1);
            goto continuesearch;
        }
        goto CrashAndBurn;

  default: //non-continuable exceptions
        dprintfException(pERepRec, pERegRec, pCtxRec, p);
        goto continuesearch;
  }
continuesearch:
  LogException(LEAVE_EXCEPTION);
  return XCPT_CONTINUE_SEARCH;

continueexecution:
  LogException(LEAVE_EXCEPTION);
  return XCPT_CONTINUE_EXECUTION;
}

/*****************************************************************************
 * Name      : void OS2SetExceptionHandler
 * Purpose   : Sets the main thread exception handler in FS:[0] (original OS/2 FS selector)
 * Parameters: exceptframe: pointer to exception handler frame on stack (2 ULONGs)
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Sun, 1999/08/21 12:16]
 *****************************************************************************/
void OS2SetExceptionHandler(void *exceptframe)
{
 PEXCEPTIONREGISTRATIONRECORD pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)exceptframe;

  pExceptRec->prev_structure   = (PEXCEPTIONREGISTRATIONRECORD)0;
  pExceptRec->ExceptionHandler = OS2ExceptionHandler;

  /* disable trap popups */
//  DosError(FERR_DISABLEEXCEPTION | FERR_DISABLEHARDERR);

  DosSetExceptionHandler(pExceptRec);
  dprintf(("OS2SetExceptionHandler: exception chain %x", pExceptRec));
#ifdef DEBUG_ENABLELOG_LEVEL2
  PrintExceptionChain();
#endif
}

#ifdef DEBUG
void PrintExceptionChain()
{
 USHORT sel = RestoreOS2FS();
 PEXCEPTIONREGISTRATIONRECORD pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)QueryExceptionChain();

  dprintf(("Exception chain list:"));
  while(pExceptRec != 0 && (ULONG)pExceptRec != -1) {
        dprintf(("record %x", pExceptRec));
        pExceptRec = pExceptRec->prev_structure;
  }
  SetFS(sel);
}

void PrintWin32ExceptionChain(PWINEXCEPTION_FRAME pframe)
{
  dprintf(("Win32 exception chain:"));
  while ((pframe != NULL) && ((ULONG)pframe != 0xFFFFFFFF)) {
        dprintf(("Record at %08X, Prev at %08X, handler at %08X", pframe, pframe->Prev, pframe->Handler));
        if (pframe == pframe->Prev) {
            dprintf(("Chain corrupted! Record at %08X pointing to itself!", pframe));
            break;
        }
        pframe = pframe->Prev;
  }
}

#endif


/*****************************************************************************
 * Name      : void OS2UnsetExceptionHandler
 * Purpose   : Removes the main thread exception handler in FS:[0] (original OS/2 FS selector)
 * Parameters: exceptframe: pointer to exception handler frame on stack (2 ULONGs)
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Sun, 1999/08/21 12:16]
 *****************************************************************************/
void OS2UnsetExceptionHandler(void *exceptframe)
{
 PEXCEPTIONREGISTRATIONRECORD pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)exceptframe;

  DosUnsetExceptionHandler(pExceptRec);
  dprintf(("OS2UnsetExceptionHandler: exception chain %x", pExceptRec));
#ifdef DEBUG_ENABLELOG_LEVEL2
  PrintExceptionChain();
#endif
}

void SetOS2ExceptionChain(ULONG val)
{
 USHORT sel = GetFS();

    SetExceptionChain(val);
    SetFS(sel);
}

int _System CheckCurFS()
{
 USHORT sel = RestoreOS2FS();
 PEXCEPTIONREGISTRATIONRECORD pExceptRec;

    if(sel == 0x150b) {
        SetFS(sel);
        return FALSE;
    }
    pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)QueryExceptionChain();
    if(pExceptRec->ExceptionHandler != OS2ExceptionHandler) {
        SetFS(sel);
        return FALSE;
    }
    SetFS(sel);
    return TRUE;
}

