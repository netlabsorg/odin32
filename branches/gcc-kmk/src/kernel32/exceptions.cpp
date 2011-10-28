/* $Id: exceptions.cpp,v 1.74 2004-01-20 13:41:10 sandervl Exp $ */

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
#define INCL_WIN
#define INCL_WINBUTTONS
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <pmscan.h>
#include "exceptions.h"
#include "exceptutil.h"
#include <misc.h>
#include "mmap.h"
#include <wprocess.h>
#include <win32api.h>
#include "oslibexcept.h"
#include "oslibmem.h"
#include "exceptstackdump.h"
#include "hmthread.h"

#include "WinImageBase.h"
#include "WinDllBase.h"
#include "WinExeBase.h"

/* Really lazy! But, including wincon.h means lot's of missing COORD. */
#define CTRL_C_EVENT     0     //#include <wincon.h>
#define CTRL_BREAK_EVENT 1     //#include <wincon.h>
#include "console.h"
#include "initterm.h"


#define DBG_LOCALLOG    DBG_exceptions
#include "dbglocal.h"

#include <_ras.h>

#ifdef WITH_KLIB
/* quick and dirty - don't wanna mess with includes. */
typedef enum { enmRead, enmWrite, enmUnknown } ENMACCESS;
BOOL  _Optlink      kHeapDbgException(void *    pvAccess,
                                      ENMACCESS enmAccess,
                                      void *    pvIP,
                                      void *    pvOS);
#endif

/* Exception record for handling exceptions happening inside exception handlers */
typedef struct
{
    WINEXCEPTION_FRAME frame;
    WINEXCEPTION_FRAME *prevFrame;
} EXC_NESTED_FRAME;

//Global Process Unhandled exception filter
static LPTOP_LEVEL_EXCEPTION_FILTER CurrentUnhExceptionFlt = NULL;
static UINT                         CurrentErrorMode = 0;
static PEXCEPTION_HANDLER           StartupCodeHandler = NULL;

extern "C" {

PWINEXCEPTION_FRAME GetExceptionRecord(ULONG offset, ULONG segment);

LONG WIN32API UnhandledExceptionFilter(PWINEXCEPTION_POINTERS lpexpExceptionInfo);
void KillWin32Process(void);

static void sprintfException(PEXCEPTIONREPORTRECORD pERepRec,
                             PEXCEPTIONREGISTRATIONRECORD pERegRec,
                             PCONTEXTRECORD pCtxRec, PVOID p, PSZ szTrapDump);

int __cdecl __seh_handler(PWINEXCEPTION_RECORD pRec,
                          PWINEXCEPTION_FRAME pFrame,
                          PCONTEXTRECORD pContext, PVOID pVoid);

PWINEXCEPTION_FRAME __cdecl __seh_get_prev_frame(PWINEXCEPTION_FRAME pFrame);

#ifdef DEBUG
static void PrintWin32ExceptionChain(PWINEXCEPTION_FRAME pframe);
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

#if 0
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
#endif

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

#if 1
DWORD EXC_CallHandler( WINEXCEPTION_RECORD *record, WINEXCEPTION_FRAME *frame,
                       WINCONTEXT *context, WINEXCEPTION_FRAME **dispatcher,
                       PEXCEPTION_HANDLER handler, PEXCEPTION_HANDLER nested_handler);

#else
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
#endif
//******************************************************************************
//******************************************************************************
DWORD RtlDispatchException(WINEXCEPTION_RECORD *pRecord, WINCONTEXT *pContext)
{
    PWINEXCEPTION_FRAME   pFrame, pPrevFrame, dispatch, nested_frame;
    int                   rc;

    // get chain of exception frames
    rc  = ExceptionContinueSearch;

    nested_frame = NULL;
    TEB *winteb = GetThreadTEB();
    pFrame      = (PWINEXCEPTION_FRAME)winteb->except;

    dprintf(("KERNEL32: RtlDispatchException entered"));

    PrintWin32ExceptionChain(pFrame);

    // walk the exception chain
    while( (pFrame != NULL) && ((ULONG)((ULONG)pFrame & 0xFFFFF000) != 0xFFFFF000) )
    {
        pPrevFrame = __seh_get_prev_frame(pFrame);

        dprintf(("KERNEL32: RtlDispatchException - pframe=%08X, pframe->Prev=%08X",
                 pFrame, pPrevFrame));
        if (pFrame == pPrevFrame) {
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
        if (pFrame->Handler)
        {
            rc = EXC_CallHandler(pRecord, pFrame, pContext, &dispatch,
                                 pFrame->Handler, EXC_RaiseHandler);
        }
        else
        {
            dprintf(("pFrame->Handler is NULL!!!!!"));
            rc = ExceptionContinueSearch;
        }

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
            if (!(pRecord->ExceptionFlags & EH_NONCONTINUABLE))
            {
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

        dprintf(("KERNEL32: RtlDispatchException - going from frame %08X to previous frame %08X",
                 pFrame, pPrevFrame));
        if (pFrame == pPrevFrame)
        {
            dprintf(("KERNEL32: RtlDispatchException - Invalid exception frame!!"));
            break;
        }
        pFrame = pPrevFrame;
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
  PWINEXCEPTION_FRAME frame, prevFrame, dispatch;
  WINEXCEPTION_RECORD record; //, newrec;
  WINCONTEXT          context;
  DWORD               rc;

  dprintf(("KERNEL32: RtlUnwind pEndFrame=%08X, unusedEip=%08X, pRecord=%08X, returnEax=%#x\n", pEndFrame, unusedEip, pRecord, returnEax));

  TEB *winteb = GetThreadTEB();
  if (!winteb)
  {
      dprintf(("KERNEL32: RtlUnwind TEB is NULL\n"));
      DebugInt3();
      return 0;
  }

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
  frame       = (PWINEXCEPTION_FRAME)winteb->except;

  PrintWin32ExceptionChain(frame);

  while (((ULONG)((ULONG)frame & 0xFFFFF000) != 0xFFFFF000) && (frame != pEndFrame))
  {
        prevFrame = __seh_get_prev_frame(frame);

        /* Check frame address */
        if (pEndFrame && (frame > pEndFrame))
        {
#if 0
            // TODO
            newrec.ExceptionCode    = STATUS_INVALID_UNWIND_TARGET;
            newrec.ExceptionFlags   = EH_NONCONTINUABLE;
            newrec.ExceptionRecord  = pRecord;
            newrec.NumberParameters = 0;
            RtlRaiseException(&newrec, NULL);
#else
            dprintf(("KERNEL32: RtlUnwind terminating thread (invalid target).\n"));
            DosExit(EXIT_THREAD, 0);
#endif
        }
        if (((void*)frame < winteb->stack_low) ||
            ((void*)(frame+1) > winteb->stack_top) ||
            (int)frame & 3)
        {
#if 0
            // TODO
            newrec.ExceptionCode    = STATUS_BAD_STACK;
            newrec.ExceptionFlags   = EH_NONCONTINUABLE;
            newrec.ExceptionRecord  = pRecord;
            newrec.NumberParameters = 0;
#else
            dprintf(("KERNEL32: RtlUnwind terminating thread (bad stack).\n"));
            DosExit(EXIT_THREAD, 0);
#endif
        }

        /* Call handler */
        dprintf(("KERNEL32: RtlUnwind - calling exception handler %08X", frame->Handler));
        if(frame->Handler) {
            // ensure the Win32 FS (may be accessed directly in the handler)
            DWORD oldsel = SetReturnFS(winteb->teb_sel);
            rc = EXC_CallHandler(pRecord, frame, &context, &dispatch, frame->Handler, EXC_UnwindHandler );
            // restore FS
            SetFS(oldsel);
        }
        else {
            dprintf(("pFrame->Handler is NULL!!!!!"));
            rc = ExceptionContinueSearch;
        }
        dprintf(("KERNEL32: RtlUnwind - handler returned %#x", rc));
        switch (rc)
        {
        case ExceptionContinueSearch:
            break;
        case ExceptionCollidedUnwind:
            frame = dispatch;
            break;
        default:
#if 0
            // TODO
            newrec.ExceptionCode    = STATUS_INVALID_DISPOSITION;
            newrec.ExceptionFlags   = EH_NONCONTINUABLE;
            newrec.ExceptionRecord  = pRecord;
            newrec.NumberParameters = 0;
#else
            dprintf(("KERNEL32: RtlUnwind terminating thread (invalid disposition).\n"));
            DosExit(EXIT_THREAD, 0);
#endif
            break;
        }
        dprintf(("KERNEL32: RtlUnwind (before)- frame=%08X, frame->Prev=%08X", frame, prevFrame));
        winteb->except = (void*)prevFrame;
        frame = prevFrame;
        dprintf(("KERNEL32: RtlUnwind (after) - frame=%08X, frame->Prev=%08X", frame,
                 ((ULONG)((ULONG)frame & 0xFFFFF000) != 0xFFFFF000) ?
                 __seh_get_prev_frame(frame) : (void*)0xFFFFFFFF));
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
    char      szModName[16];
    char      message[128];
    ULONG     iObj;
    ULONG     offObj;
    HMODULE   hmod;
    DWORD     rc;

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

    if (DosQueryModFromEIP(&hmod, &iObj, sizeof(szModName), szModName, &offObj, (ULONG)lpexpExceptionInfo->ExceptionRecord->ExceptionAddress))
        sprintf(message, "Unhandled exception 0x%08lx at address 0x%08lx. (DQMFEIP rc=%d)",
                lpexpExceptionInfo->ExceptionRecord->ExceptionCode,
                lpexpExceptionInfo->ExceptionRecord->ExceptionAddress);
    else
    {
        if (iObj == -1)
        {   /* fault in DosAllocMem allocated memory, hence PE loader.. */
            Win32ImageBase * pMod;
            if (WinExe && WinExe->insideModule((ULONG)lpexpExceptionInfo->ExceptionRecord->ExceptionAddress))
                pMod = WinExe;
            else
                pMod = Win32DllBase::findModuleByAddr((ULONG)lpexpExceptionInfo->ExceptionRecord->ExceptionAddress);
            if (pMod != NULL)
            {
                szModName[0] = '\0';
                strncat(szModName, pMod->getModuleName(), sizeof(szModName) - 1);
                iObj = 0xFF;
                offObj = (ULONG)lpexpExceptionInfo->ExceptionRecord->ExceptionAddress
                        - (ULONG)pMod->getInstanceHandle();
            }
        }
        sprintf(message,
                "Unhandled exception 0x%08lx at address 0x%08lx.\r"
                "Mod: %s obj: 0x%2lx off:0x%08lx",
                lpexpExceptionInfo->ExceptionRecord->ExceptionCode,
                lpexpExceptionInfo->ExceptionRecord->ExceptionAddress,
                szModName, iObj, offObj);
    }

/*  This is very dangerous. Can hang PM.
    rc = WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, message, "Application Error",
                       0, MB_ABORTRETRYIGNORE | MB_ERROR);
    switch (rc)
    {
    case MBID_IGNORE:
       return WINEXCEPTION_CONTINUE_EXECUTION;

    case MBID_ABORT:
    case MBID_RETRY:
    default:
       return WINEXCEPTION_EXECUTE_HANDLER;
    }
*/
    return WINEXCEPTION_EXECUTE_HANDLER;
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
VOID WIN32API ExitProcess(DWORD exitcode);
//******************************************************************************
void KillWin32Process(void)
{
    static BOOL fEntry = FALSE;

    if(!fExitProcess && fEntry == FALSE) {
        fEntry = TRUE;
        ExitProcess(666);
        return;
    }
    //Restore original OS/2 TIB selector
    RestoreOS2FS();

    DosExit(EXIT_PROCESS, 666);
}
//*****************************************************************************
//*****************************************************************************
void KillWin32Thread(void)
{
//    ExitThread(666);
    //Restore original OS/2 TIB selector
    RestoreOS2FS();

    DosExit(EXIT_THREAD, 666);
}
//*****************************************************************************
//*****************************************************************************
static void sprintfException(PEXCEPTIONREPORTRECORD       pERepRec,
                             PEXCEPTIONREGISTRATIONRECORD pERegRec,
                             PCONTEXTRECORD               pCtxRec,
                             PVOID                        p,
                             PSZ                          szTrapDump)
{
    if(pERepRec->ExceptionNum == XCPT_GUARD_PAGE_VIOLATION)
    {
        strcpy(szTrapDump, "Guard Page Violation");
        return;
    }

    PCSZ   pszExceptionName = "<unknown>";        /* points to name/type excpt */
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
    char   szLineException[512];
    char   szLineExceptionType[128];

    szLineException[0]  = 0;                                              /* initialize */
    szLineExceptionType[0] = 0;                                              /* initialize */
    switch(pERepRec->ExceptionNum)                    /* take according action */
    {
    /* portable, non-fatal software-generated exceptions */
    case XCPT_GUARD_PAGE_VIOLATION:
        strcpy(szLineException, "Guard Page Violation");
        sprintf(szLineExceptionType, "R/W %08xh at %08xh.", pERepRec->ExceptionInfo[0], pERepRec->ExceptionInfo[1]);
        fExcptSoftware   = TRUE;
        fExcptFatal      = FALSE;
        rc               = XCPT_CONTINUE_EXECUTION;
        break;

    case XCPT_UNABLE_TO_GROW_STACK:
        strcpy(szLineException, "Unable To Grow Stack");
        fExcptSoftware   = TRUE;
        fExcptFatal      = FALSE;
        rc               = XCPT_CONTINUE_EXECUTION;
        break;

    /* portable, fatal, hardware-generated exceptions */
    case XCPT_ACCESS_VIOLATION:
        strcpy(szLineException, "Access Violation");
        switch (pERepRec->ExceptionInfo[0])
        {
        case XCPT_READ_ACCESS:
            sprintf (szLineExceptionType, "Read Access at address %08xh", pERepRec->ExceptionInfo[1]);
            break;

        case XCPT_WRITE_ACCESS:
            sprintf (szLineExceptionType, "Write Access at address %08x", pERepRec->ExceptionInfo[1]);
            break;

        case XCPT_SPACE_ACCESS:
            sprintf (szLineExceptionType, "Space Access at selector %08x", pERepRec->ExceptionInfo[1]);
            break;

        case XCPT_LIMIT_ACCESS:
            strcpy (szLineExceptionType, "Limit Access");
            break;

        case XCPT_UNKNOWN_ACCESS:
            strcpy (szLineExceptionType, "Unknown Access");
            break;

        default:
            strcpy (szLineExceptionType, "(Invalid Access Code)");
            break;
        }
        break;

    case XCPT_INTEGER_DIVIDE_BY_ZERO:
        strcpy(szLineException, "Division By Zero (Integer)");
        break;

    case XCPT_FLOAT_DIVIDE_BY_ZERO:
        strcpy(szLineException, "Division By Zero (Float)");
        break;

    case XCPT_FLOAT_INVALID_OPERATION:
        strcpy(szLineException, "Invalid Floating Point Operation");
        break;

    case XCPT_ILLEGAL_INSTRUCTION:
        strcpy(szLineException, "Illegal Instruction");
        break;

    case XCPT_PRIVILEGED_INSTRUCTION:
        strcpy(szLineException, "Privileged Instruction");
        break;

    case XCPT_INTEGER_OVERFLOW:
        strcpy(szLineException, "Integer Overflow");
        break;

    case XCPT_FLOAT_OVERFLOW:
        strcpy(szLineException, "Floating Point Overflow");
        break;

    case XCPT_FLOAT_UNDERFLOW:
        strcpy(szLineException, "Floating Point Underflow");
        break;

    case XCPT_FLOAT_DENORMAL_OPERAND:
        strcpy(szLineException, "Floating Point Denormal Operand");
        break;

    case XCPT_FLOAT_INEXACT_RESULT:
        strcpy(szLineException, "Floating Point Inexact Result");
        break;

    case XCPT_FLOAT_STACK_CHECK:
        strcpy(szLineException, "Floating Point Stack Check");
        break;

    case XCPT_DATATYPE_MISALIGNMENT:
        strcpy(szLineException, "Datatype Misalignment");
        sprintf(szLineExceptionType, "R/W %08x alignment %08x at %08x.", pERepRec->ExceptionInfo[0],
                pERepRec->ExceptionInfo[1], pERepRec->ExceptionInfo[2]);
        break;

    case XCPT_BREAKPOINT:
        strcpy(szLineException, "Breakpoint (DEBUG)");
        break;

    case XCPT_SINGLE_STEP:
        strcpy(szLineException, "Single Step (DEBUG)");
        break;

    /* portable, fatal, software-generated exceptions */
    case XCPT_IN_PAGE_ERROR:
        strcpy(szLineException, "In Page Error");
        sprintf(szLineExceptionType, "at %08x.", pERepRec->ExceptionInfo[0]);
        fExcptSoftware    = TRUE;
        break;

    case XCPT_PROCESS_TERMINATE:
        strcpy(szLineException, "Process Termination");
        fExcptSoftware    = TRUE;
        break;

    case XCPT_ASYNC_PROCESS_TERMINATE:
        strcpy(szLineException, "Process Termination (async)");
        sprintf(szLineExceptionType, "terminating thread TID=%u", pERepRec->ExceptionInfo[0]);
        fExcptSoftware    = TRUE;
        break;

    case XCPT_NONCONTINUABLE_EXCEPTION:
        strcpy(szLineException, "Noncontinuable Exception");
        fExcptSoftware    = TRUE;
        break;

    case XCPT_INVALID_DISPOSITION:
        strcpy(szLineException, "Invalid Disposition");
        fExcptSoftware    = TRUE;
        break;

    /* non-portable, fatal exceptions */
    case XCPT_INVALID_LOCK_SEQUENCE:
        strcpy(szLineException, "Invalid Lock Sequence");
        fExcptSoftware    = TRUE;
        fExcptPortable    = FALSE;
        break;

    case XCPT_ARRAY_BOUNDS_EXCEEDED:
        strcpy(szLineException, "Array Bounds Exceeded");
        fExcptSoftware    = TRUE;
        fExcptPortable    = FALSE;
        break;

    /* unwind operation exceptions */
    case XCPT_UNWIND:
        strcpy(szLineException, "Unwind Exception");
        fExcptSoftware    = TRUE;
        fExcptPortable    = FALSE;
        break;

    case XCPT_BAD_STACK:
        strcpy(szLineException, "Unwind Exception, Bad Stack");
        fExcptSoftware    = TRUE;
        fExcptPortable    = FALSE;
        break;

    case XCPT_INVALID_UNWIND_TARGET:
        strcpy(szLineException, "Unwind Exception, Invalid Target");
        fExcptSoftware    = TRUE;
        fExcptPortable    = FALSE;
        break;

    /* fatal signal exceptions */
    case XCPT_SIGNAL:
        strcpy(szLineException, "Signal");
        sprintf(szLineExceptionType, "Signal Number = %08x", pERepRec->ExceptionInfo[0]);
        fExcptSoftware    = TRUE;
        fExcptPortable    = FALSE;

        switch (pERepRec->ExceptionInfo[0])          /* resolve signal information */
        {
        case XCPT_SIGNAL_INTR:
            strcpy(szLineException, "Signal (Interrupt)");
            break;

        case XCPT_SIGNAL_KILLPROC:
            strcpy(szLineException, "Signal (Kill Process)");
            break;

        case XCPT_SIGNAL_BREAK:
            strcpy(szLineException, "Signal (Break)");
            break;
        }
        break;

    default:
        strcpy(szLineException,  "(unknown exception code)");
        sprintf(szLineExceptionType, "Exception Code = %08x", pERepRec->ExceptionNum);
        break;
    }

    sprintf(szTrapDump, "---[Exception Information]------------\n   %s (", szLineException);

    if (fExcptSoftware == TRUE)            /* software or hardware generated ? */
        strcat (szTrapDump, "software generated,");
    else
        strcat (szTrapDump, "hardware generated,");

    if (fExcptPortable == TRUE)                        /* portable exception ? */
        strcat (szTrapDump, "portable,");
    else
        strcat (szTrapDump, "non-portable,");

    if (fExcptFatal    == TRUE)                           /* fatal exception ? */
        strcat (szTrapDump, "fatal");
    else
        strcat (szTrapDump, "non-fatal");

    strcat(szTrapDump, ")\n");                                    /* add trailing brace */

    if (szLineExceptionType[0])
        sprintf(szTrapDump + strlen(szTrapDump), "   %s\n", szLineExceptionType);

    sprintf(szLineException, "   Exception Address = %08x ", pERepRec->ExceptionAddress);
    strcat(szTrapDump, szLineException);

    rc = DosQueryModFromEIP(&ulModule, &ulObject, sizeof(szModule),
                            szModule, &ulOffset, (ULONG)pERepRec->ExceptionAddress);

    if(rc == NO_ERROR && ulObject != -1)
    {
        sprintf(szLineException, "<%8.8s> (%04X) obj %04X:%08X", szModule, ulModule, ulObject + 1, ulOffset);
#ifdef RAS
        char szSYMInfo[260];

     	DosQueryModuleName(ulModule, sizeof(szModule), szModule);

        int namelen = strlen(szModule);
       	if(namelen > 3)
       	{
	        strcpy(szModule + namelen - 3, "SYM");
	        dbgGetSYMInfo(szModule, ulObject, ulOffset, szSYMInfo, sizeof (szSYMInfo),
                          FALSE);
            strcat(szLineException, " ");
            strcat(szLineException, szSYMInfo);
        }
#else
        strcat(szLineException, "\n");
#endif
        strcat(szTrapDump, szLineException);
    }
    else
    {   /* fault in DosAllocMem allocated memory, hence PE loader.. */
        Win32ImageBase * pMod;
        if (WinExe && WinExe->insideModule((ULONG)pERepRec->ExceptionAddress))
            pMod = WinExe;
        else
            pMod = Win32DllBase::findModuleByAddr((ULONG)pERepRec->ExceptionAddress);
        if (pMod != NULL)
        {
            szModule[0] = '\0';
            strncat(szModule, pMod->getModuleName(), sizeof(szModule) - 1);
            ulObject = 0xFF;
            ulOffset = (ULONG)pERepRec->ExceptionAddress - (ULONG)pMod->getInstanceHandle();
            sprintf(szLineException, "<%8.8s> (%04X) obj %04X:%08X", szModule, ulModule, ulObject, ulOffset);
        }
        else sprintf(szLineException, "<unknown win32 module>\n");

        strcat(szTrapDump, szLineException);
    }

    rc = DosGetInfoBlocks (&pTIB, &pPIB);
    if (rc == NO_ERROR)
    {
        sprintf(szLineException, "   Thread:  Ordinal TID: %u, TID: %u, Priority: %04xh\n",
                pTIB->tib_ordinal, pTIB->tib_ptib2->tib2_ultid, pTIB->tib_ptib2->tib2_ulpri);
        strcat(szTrapDump, szLineException);

        sprintf(szLineException, "   Process: PID: %u, Parent: %u, Status: %u\n", pPIB->pib_ulpid,
                pPIB->pib_ulppid, pPIB->pib_flstatus);
        strcat(szTrapDump, szLineException);
    }

    if (pCtxRec->ContextFlags & CONTEXT_CONTROL) {        /* check flags */
        sprintf(szLineException, "   SS:ESP=%04x:%08x  EFLAGS=%08x\n", pCtxRec->ctx_SegSs, pCtxRec->ctx_RegEsp,
                 pCtxRec->ctx_EFlags);
        strcat(szTrapDump, szLineException);
        sprintf(szLineException, "   CS:EIP=%04x:%08x  EBP   =%08x\n", pCtxRec->ctx_SegCs, pCtxRec->ctx_RegEip,
                 pCtxRec->ctx_RegEbp);
        strcat(szTrapDump, szLineException);
    }

    if (pCtxRec->ContextFlags & CONTEXT_INTEGER) {        /* check flags */
        sprintf(szLineException, "   EAX=%08x EBX=%08x ESI=%08x\n", pCtxRec->ctx_RegEax, pCtxRec->ctx_RegEbx,
                pCtxRec->ctx_RegEsi);
        strcat(szTrapDump, szLineException);
        sprintf(szLineException, "   ECX=%08x EDX=%08x EDI=%08x\n", pCtxRec->ctx_RegEcx, pCtxRec->ctx_RegEdx,
             pCtxRec->ctx_RegEdi);
        strcat(szTrapDump, szLineException);
    }

    if (pCtxRec->ContextFlags & CONTEXT_SEGMENTS) {       /* check flags */
        sprintf(szLineException, "   DS=%04x      ES=%08x  FS=%04x     GS=%04x\n",   pCtxRec->ctx_SegDs, pCtxRec->ctx_SegEs, pCtxRec->ctx_SegFs, pCtxRec->ctx_SegGs);
        strcat(szTrapDump, szLineException);
    }

    if (pCtxRec->ContextFlags & CONTEXT_FLOATING_POINT)  /* check flags */
    {
        ULONG ulCounter;                 /* temporary local counter for fp stack */

        sprintf(szLineException, "   Env[0]=%08x Env[1]=%08x Env[2]=%08x Env[3]=%08x\n",
                 pCtxRec->ctx_env[0], pCtxRec->ctx_env[1],
                 pCtxRec->ctx_env[2], pCtxRec->ctx_env[3]);
        strcat(szTrapDump, szLineException);

        sprintf(szLineException, "   Env[4]=%08x Env[5]=%08x Env[6]=%08x\n",
                 pCtxRec->ctx_env[4], pCtxRec->ctx_env[5], pCtxRec->ctx_env[6]);
        strcat(szTrapDump, szLineException);

        for (ulCounter = 0; ulCounter < 8; /* see TOOLKIT\INCLUDE\BSEEXPT.H, _CONTEXT structure */
             ulCounter ++)
        {
            sprintf(szLineException, "   FP-Stack[%u] losig=%08x hisig=%08x signexp=%04x\n",
                     ulCounter, pCtxRec->ctx_stack[0].losig, pCtxRec->ctx_stack[0].hisig,
                     pCtxRec->ctx_stack[0].signexp);
            strcat(szTrapDump, szLineException);
        }
    }
    sprintf(szLineException, "---[End Of Exception Information]-----\n");
    strcat(szTrapDump, szLineException);
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

#ifdef DEBUG

static void dprintfException(PEXCEPTIONREPORTRECORD       pERepRec,
                             PEXCEPTIONREGISTRATIONRECORD pERegRec,
                             PCONTEXTRECORD               pCtxRec,
                             PVOID                        p)
{
    char szTrapDump[2048];
    szTrapDump[0] = '\0';
    sprintfException(pERepRec, pERegRec, pCtxRec, p, szTrapDump);
#ifdef RAS
    RasLog (szTrapDump);
#endif
    /* now dump the information to the logfile */
    dprintf(("\n%s", szTrapDump));
}

#else // DEBUG
#define dprintfException(a,b,c,d) do {} while (0)
#endif // DEBUG

//*****************************************************************************
static char szExceptionLogFileName[CCHMAXPATH] = "";
static BOOL fExceptionLoggging = TRUE;
//*****************************************************************************
//Override filename of exception log (expects full path)
//*****************************************************************************
void WIN32API SetCustomExceptionLog(LPSTR lpszLogName)
{
    strcpy(szExceptionLogFileName, lpszLogName);
}
//*****************************************************************************
//*****************************************************************************
void WIN32API SetExceptionLogging(BOOL fEnable)
{
    fExceptionLoggging = fEnable;
}
//*****************************************************************************
//*****************************************************************************
static void logException(PEXCEPTIONREPORTRECORD pERepRec, PEXCEPTIONREGISTRATIONRECORD pERegRec, PCONTEXTRECORD pCtxRec, PVOID p)
{
    APIRET rc;
    HFILE  hFile;
    ULONG  ulAction, ulBytesWritten;

    if(fExceptionLoggging == FALSE) {
        return;
    }

    if(szExceptionLogFileName[0] == 0) {
        strcpy(szExceptionLogFileName, kernel32Path);
        strcat(szExceptionLogFileName, "\\except.log");
    }
    rc = DosOpen(szExceptionLogFileName,         /* File path name */
                 &hFile,                         /* File handle */
                 &ulAction,                      /* Action taken */
                 0L,                             /* File primary allocation */
                 0L,                             /* File attribute */
                 OPEN_ACTION_CREATE_IF_NEW |
                 OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
                 OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,
                 0L);                            /* No extended attribute */

    if(rc == NO_ERROR) {
        DosSetFilePtr(hFile, 0, FILE_END, &ulBytesWritten);
        if(WinExe) {
            LPSTR lpszExeName;

            lpszExeName = WinExe->getModuleName();

            if(lpszExeName) {
                DosWrite(hFile, (PVOID)"\n", 2, &ulBytesWritten);
                DosWrite(hFile, lpszExeName, strlen(lpszExeName), &ulBytesWritten);
                DosWrite(hFile, (PVOID)"\n", 2, &ulBytesWritten);
            }
        }
        LPSTR lpszTime;
        time_t curtime;

        curtime = time(NULL);
        lpszTime = asctime(localtime(&curtime));
        if(lpszTime) {
            DosWrite(hFile, lpszTime, strlen(lpszTime), &ulBytesWritten);
        }

        char szTrapDump[2048];
        szTrapDump[0] = '\0';
        sprintfException(pERepRec, pERegRec, pCtxRec, p, szTrapDump);
#ifdef RAS
        RasLog (szTrapDump);
#endif
        DosWrite(hFile, szTrapDump, strlen(szTrapDump), &ulBytesWritten);
        DosClose(hFile);
    }
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

// borrowed from ntddk.h
extern "C"
void WIN32API RtlUnwind(
	LPVOID,
	LPVOID,
	LPVOID,DWORD);

// Assembly wrapper for clearing the direction flag before calling our real
// exception handler
ULONG APIENTRY OS2ExceptionHandler(PEXCEPTIONREPORTRECORD       pERepRec,
                                   PEXCEPTIONREGISTRATIONRECORD pERegRec,
                                   PCONTEXTRECORD               pCtxRec,
                                   PVOID                        p);

ULONG APIENTRY OS2ExceptionHandler2ndLevel(PEXCEPTIONREPORTRECORD       pERepRec,
                                           PEXCEPTIONREGISTRATIONRECORD pERegRec,
                                           PCONTEXTRECORD               pCtxRec,
                                           PVOID                        p)
{
#ifdef DEBUG
    //SvL: Check if exception inside debug fprintf -> if so, clear lock so
    //     next dprintf won't wait forever
    int prevlock = LogException(ENTER_EXCEPTION);
#endif

// @@VP20040507: no need to sprintf every exception
//    //Print exception name & exception type
//    //Not for a guard page exception as sprintfException uses a lot of stack
//    //and can trigger nested guard page exceptions (crash)
//    if(pERepRec->ExceptionNum != XCPT_GUARD_PAGE_VIOLATION) {
//        sprintfException(pERepRec, pERegRec, pCtxRec, p, szTrapDump);
//    }

    // We have to disable unwinding of the Win32 exception handlers because
    // experiments have shown that when running under SMP kernel the stack
    // becomes corrupt at the time when unwinding takes place: attempts to
    // to follow the exception chain crash when accessing one of the the
    // previous frame. Although it may seem that performing unwinding here
    // (when we are definitely above any Win32 exception frames installed by
    // the application so that the OS could theoretically already discard lower
    // stack areas) is wrong, it's not the case of the crash. First, doing the
    // very same thing from the SEH handler (see comments in sehutil.s), i.e.
    // when the given Win32 stack frame (and all previous ones) is definitely
    // alive, crahes due to the same stack corruption too. Second, when running
    // under UNI kernel, BOTH approaches don't crash (i.e. the stack is fine).
    // This looks like the SMP kernel doesn't manage the stack right during
    // exception handling :( See also http://svn.netlabs.org/odin32/ticket/37.
    //
    // Note that disabling unwinding also breaks support for performing
    // setjmp()/lonjmp() that crosses the bounds of the __try {} block.
#if 0
    if (pERepRec->fHandlerFlags & EH_UNWINDING)
    {
        // unwind the appropriate portion of the Win32 exception chain
        if (pERepRec->fHandlerFlags & EH_EXIT_UNWIND)
        {
            dprintf(("KERNEL32: OS2ExceptionHandler: EH_EXIT_UNWIND, "
                     "unwinding all the Win32 exception chain"));
            RtlUnwind(NULL, 0, 0, 0);
        }
        else
        {
            dprintf(("KERNEL32: OS2ExceptionHandler: EH_UNWINDING, "
                     "unwinding the Win32 exception chain up to 0x%p", pERegRec));

            // find a Win32 exception frame closest to the OS/2 one (pERegRec)
            // and unwind up to the previous one (to unwind the closest frame
            // itself too as we are definitely jumping out of it)
            TEB *winteb = GetThreadTEB();
            PWINEXCEPTION_FRAME frame = (PWINEXCEPTION_FRAME)winteb->except;
            while (frame != NULL && ((ULONG)frame)!= 0xFFFFFFFF &&
                   ((ULONG)frame) <= ((ULONG)pERegRec))
                frame = __seh_get_prev_frame(frame);
            if (((ULONG)frame) == 0xFFFFFFFF)
                frame = NULL;

            RtlUnwind(frame, 0, 0, 0);
        }
        goto continuesearch;
    }
#endif

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
        if((!fIsOS2Image || fSEHEnabled) && !fExitProcess)  //Only for real win32 apps or if SEH enabled
        {
            if(OSLibDispatchException(pERepRec, pERegRec, pCtxRec, p) == FALSE)
            {
                pCtxRec->ctx_env[0] |= 0x1F;
                pCtxRec->ctx_stack[0].losig = 0;
                pCtxRec->ctx_stack[0].hisig = 0;
                pCtxRec->ctx_stack[0].signexp = 0;
            }
            else
            {
                dprintf(("KERNEL32: OS2ExceptionHandler: fix and continue\n"));
                goto continueexecution;
            }
        }
        dprintf(("KERNEL32: OS2ExceptionHandler: continue search\n"));
        goto continuesearch;

    case XCPT_PROCESS_TERMINATE:
    case XCPT_ASYNC_PROCESS_TERMINATE:
#if 0
        dprintfException(pERepRec, pERegRec, pCtxRec, p);
        PrintExceptionChain();
        // fall through
#endif
    case XCPT_UNWIND:
        // Note that printing through ODINCRT (VACRT) when handling XCPT_UNWIND
        // is known to create deadlocks
        goto continuesearch;

    case XCPT_ACCESS_VIOLATION:
    {
        Win32MemMap *map;
        BOOL  fWriteAccess = FALSE;
        ULONG offset, accessflag;

#ifdef WITH_KLIB
        if (     pERepRec->ExceptionInfo[0] == XCPT_READ_ACCESS
              || pERepRec->ExceptionInfo[0] == XCPT_WRITE_ACCESS
              || pERepRec->ExceptionInfo[0] == XCPT_EXECUTE_ACCESS
              || pERepRec->ExceptionInfo[0] == XCPT_UNKNOWN_ACCESS
                )
        {
            ENMACCESS enmAccess = enmRead;
            switch (pERepRec->ExceptionInfo[0])
            {
                case XCPT_WRITE_ACCESS:     enmAccess = enmWrite; break;
                case XCPT_UNKNOWN_ACCESS:   enmAccess = enmUnknown; break;
            }

            if (kHeapDbgException((void*)pERepRec->ExceptionInfo[1],
                                  enmAccess,
                                  pERepRec->ExceptionAddress,
                                  pERepRec))
                goto continueexecution;
        }
#endif

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
            Win32MemMapNotify *map;

            map = Win32MemMapNotify::findMapByView(pERepRec->ExceptionInfo[1], &offset, accessflag);
            if(!map)
                goto continueFail;

            BOOL ret = map->notify(pERepRec->ExceptionInfo[1], offset, fWriteAccess);
            if(ret == TRUE) goto continueexecution;
            goto continueFail;
        }
        BOOL ret = map->commitPage(pERepRec->ExceptionInfo[1], offset, fWriteAccess);
        map->Release();
        if(ret == TRUE)
            goto continueexecution;

        //no break;
    }
continueFail:

    /*
     * vladest: OK, try to implement write AUTOCOMMIT
     * last chance after MMAP commit is failed
     */
    if (XCPT_ACCESS_VIOLATION == pERepRec->ExceptionNum &&
        (/*pERepRec->ExceptionInfo[0] == XCPT_READ_ACCESS ||*/
         pERepRec->ExceptionInfo[0] == XCPT_WRITE_ACCESS) &&
        pERepRec->ExceptionInfo[1] != XCPT_DATA_UNKNOWN)
    {
        ULONG offset, accessflag;

        DosQueryMem((PVOID) pERepRec->ExceptionInfo[1],
                    &offset, &accessflag);
        dprintf(("KERNEL32: OS2ExceptionHandler: failed address info 0x%X size 0x%X. flag %X\n",
                 pERepRec->ExceptionInfo[1], offset, accessflag));
        /* check for valid address */
        if (!pERepRec->ExceptionInfo[1] ||
            pERepRec->ExceptionInfo[1] == 0xAAAAAAAA ||
            !offset || offset == 0xAAAAAAAA)
            goto CrashAndBurn;
        /* memory committed, but no write access */
        if (accessflag & PAG_GUARD)
            accessflag &=~PAG_GUARD;

        /* set memory aligned on page size and with size counting alignment */
        ULONG rc = DosSetMem((PVOID) (pERepRec->ExceptionInfo[1] & 0xFFFFF000),
                             offset + (pERepRec->ExceptionInfo[1] - (pERepRec->ExceptionInfo[1] & 0xFFFFF000)),
                  accessflag | PAG_WRITE | PAG_COMMIT);
        dprintf(("KERNEL32: OS2ExceptionHandler: commiting 0x%X size 0x%X. RC: %i\n",
                 pERepRec->ExceptionInfo[1] & 0xFFFFF000,
                 offset + (pERepRec->ExceptionInfo[1] - (pERepRec->ExceptionInfo[1] & 0xFFFFF000)),
                           rc));
        if (NO_ERROR == rc)
            goto continueexecution;
        else
            goto CrashAndBurn;
    }


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
    goto CrashAndBurn;

    case XCPT_INVALID_LOCK_SEQUENCE:
    {
        TEB *teb = GetThreadTEB();
        USHORT *eip = (USHORT *)pCtxRec->ctx_RegEip;

        if(teb && eip && *eip == SETTHREADCONTEXT_INVALID_LOCKOPCODE)
        {
            //Is this a pending SetThreadContext exception?
            //(see detailed description in the HMDeviceThreadClass::SetThreadContext method)
            if(teb->o.odin.context.ContextFlags)
            {
                dprintfException(pERepRec, pERegRec, pCtxRec, p);

                //NOTE: This will not work properly in case multiple threads execute this code
                dprintf(("Changing thread registers (SetThreadContext)!!"));

                if(teb->o.odin.context.ContextFlags & WINCONTEXT_CONTROL) {
                    pCtxRec->ctx_RegEbp = teb->o.odin.context.Ebp;
                    pCtxRec->ctx_RegEip = teb->o.odin.context.Eip;
////                    pCtxRec->ctx_SegCs  = teb->o.odin.context.SegCs;
                    pCtxRec->ctx_EFlags = teb->o.odin.context.EFlags;
                    pCtxRec->ctx_RegEsp = teb->o.odin.context.Esp;
////                    pCtxRec->ctx_SegSs  = teb->o.odin.context.SegSs;
                }
                if(teb->o.odin.context.ContextFlags & WINCONTEXT_INTEGER) {
                    pCtxRec->ctx_RegEdi = teb->o.odin.context.Edi;
                    pCtxRec->ctx_RegEsi = teb->o.odin.context.Esi;
                    pCtxRec->ctx_RegEbx = teb->o.odin.context.Ebx;
                    pCtxRec->ctx_RegEdx = teb->o.odin.context.Edx;
                    pCtxRec->ctx_RegEcx = teb->o.odin.context.Ecx;
                    pCtxRec->ctx_RegEax = teb->o.odin.context.Eax;
                }
                if(teb->o.odin.context.ContextFlags & WINCONTEXT_SEGMENTS) {
                    pCtxRec->ctx_SegGs  = teb->o.odin.context.SegGs;
////                    pCtxRec->ctx_SegFs  = teb->o.odin.context.SegFs;
                    pCtxRec->ctx_SegEs  = teb->o.odin.context.SegEs;
                    pCtxRec->ctx_SegDs  = teb->o.odin.context.SegDs;
                }
                if(teb->o.odin.context.ContextFlags & WINCONTEXT_FLOATING_POINT) {
                    //TODO: First 7 dwords the same?
                    memcpy(pCtxRec->ctx_env, &teb->o.odin.context.FloatSave, sizeof(pCtxRec->ctx_env));
                    memcpy(pCtxRec->ctx_stack, &teb->o.odin.context.FloatSave.RegisterArea, sizeof(pCtxRec->ctx_stack));
                }
                USHORT *lpAlias = (USHORT *)((char *)teb->o.odin.lpAlias + teb->o.odin.dwAliasOffset);
                *lpAlias = teb->o.odin.savedopcode;

                //Clear SetThreadContext markers
                teb->o.odin.context.ContextFlags = 0;

                OSLibDosFreeMem(teb->o.odin.lpAlias);

                teb->o.odin.lpAlias              = NULL;
                teb->o.odin.dwAliasOffset        = 0;

                //restore the original priority (we boosted it to ensure this thread was scheduled first)
                SetThreadPriority(teb->o.odin.hThread, GetThreadPriority(teb->o.odin.hThread));
                goto continueexecution;
            }
            else DebugInt3(); //oh, oh!!!!!

        }
        //no break;
    }

    case XCPT_PRIVILEGED_INSTRUCTION:
    case XCPT_ILLEGAL_INSTRUCTION:
    case XCPT_BREAKPOINT:
    case XCPT_ARRAY_BOUNDS_EXCEEDED:
    case XCPT_DATATYPE_MISALIGNMENT:
    case XCPT_INTEGER_DIVIDE_BY_ZERO:
    case XCPT_INTEGER_OVERFLOW:
    case XCPT_SINGLE_STEP:
    case XCPT_IN_PAGE_ERROR:
CrashAndBurn:
        //SvL: TODO: this may not always be the right thing to do
        //MN: If EH_NESTED_CALL is set, an exception occurred during the execution
        //    of this exception handler. We better bail out ASAP or we'll likely
        //    recurse infinitely until we run out of stack space!!
        if (pERepRec->fHandlerFlags & EH_NESTED_CALL)
                goto continuesearch;

#if defined(DEBUG)
        dprintfException(pERepRec, pERegRec, pCtxRec, p);

        if(!fExitProcess && (pCtxRec->ContextFlags & CONTEXT_CONTROL)) {
                dbgPrintStack(pERepRec, pERegRec, pCtxRec, p);
        }
#endif

        if((!fIsOS2Image || fSEHEnabled) && !fExitProcess)  //Only for real win32 apps or if SEH enabled
        {
            if(OSLibDispatchException(pERepRec, pERegRec, pCtxRec, p) == TRUE)
            {
                dprintf(("KERNEL32: OS2ExceptionHandler: fix and continue\n"));
                goto continueexecution;
            }
            else
            {
                dprintf(("KERNEL32: OS2ExceptionHandler: continue search\n"));
                goto continuesearch;
            }
        }
        else {
            if(fExitProcess) {
                PPIB   pPIB;
                PTIB   pTIB;
                APIRET rc;

                rc = DosGetInfoBlocks (&pTIB, &pPIB);
                if(rc == NO_ERROR)
                {
                    dprintf(("KERNEL32: OS2ExceptionHandler: Continue and kill thread"));

                    pCtxRec->ctx_RegEip = (pTIB->tib_ptib2->tib2_ultid != 1) ? (ULONG)KillWin32Thread : (ULONG)KillWin32Process;
                    pCtxRec->ctx_RegEsp = pCtxRec->ctx_RegEsp + 0x10;
                    pCtxRec->ctx_RegEax = pERepRec->ExceptionNum;
                    pCtxRec->ctx_RegEbx = pCtxRec->ctx_RegEip;
                    goto continueexecution;
                }
            }
            goto continuesearch; //pass on to OS/2 RTL or app exception handler
        }

        //Log fatal exception here
        logException(pERepRec, pERegRec, pCtxRec, p);

        dprintf(("KERNEL32: OS2ExceptionHandler: Continue and kill\n"));

        pCtxRec->ctx_RegEip = (ULONG)KillWin32Process;
        pCtxRec->ctx_RegEsp = pCtxRec->ctx_RegEsp + 0x10;
        pCtxRec->ctx_RegEax = pERepRec->ExceptionNum;
        pCtxRec->ctx_RegEbx = pCtxRec->ctx_RegEip;
        goto continueexecution;

    case XCPT_GUARD_PAGE_VIOLATION:
    {
        //NOTE:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //Don't print anything here -> fatal hang if exception occurred
        //inside fprintf
        //NOTE:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        TEB *teb = GetThreadTEB();
        DWORD stacktop, stackbottom;

        if(teb == NULL) {
            goto continueGuardException;
        }

        stacktop    = (DWORD)teb->stack_top;
        stackbottom = (DWORD)teb->stack_low;

        stackbottom = stackbottom & ~0xFFF;   //round down to page boundary
        stacktop    = stacktop & ~0xFFF;

        //Make sure we detect a stack overflow condition before the system does
        if(!fIsOS2Image &&
            pERepRec->ExceptionInfo[1]  >= stackbottom &&
            pERepRec->ExceptionInfo[1]  <  stacktop
           )
        {//this is a guard page exception for the thread stack
            APIRET rc;
            ULONG ulAddress, cbSize, ulMemFlags;

            //round down to page boundary
            ulAddress = pERepRec->ExceptionInfo[1] & ~0xFFF;

#if 0
            rc = DosQueryMem((PVOID)ulAddress, &cbSize, &ulMemFlags);
            if(rc) {
                dprintf(("ERROR: DosQueryMem old guard page failed with rc %d", rc));
                goto continueGuardException;
            }
#endif

            if(ulAddress == stackbottom + PAGE_SIZE)
            {//we don't have any stack left, throw an XCPT_UNABLE_TO_GROW_STACK
             //exception
                if(!fExitProcess)  //Only for real win32 apps
                {
                    EXCEPTIONREPORTRECORD recoutofstack;

                    recoutofstack               = *pERepRec;
                    recoutofstack.ExceptionNum  = XCPT_UNABLE_TO_GROW_STACK;
                    recoutofstack.fHandlerFlags = 0;
                    recoutofstack.NestedExceptionReportRecord = NULL;
                    recoutofstack.cParameters   = 0;

                    if(OSLibDispatchException(&recoutofstack, pERegRec, pCtxRec, p) == TRUE)
                    {
                        goto continueexecution;
                    }
                }
            }
        }
        else
        {//check for memory map guard page exception
        Win32MemMap *map;
        BOOL  fWriteAccess = FALSE, ret;
        ULONG offset, accessflag;

        switch(pERepRec->ExceptionInfo[0]) {
        case XCPT_READ_ACCESS:
                accessflag = MEMMAP_ACCESS_READ;
                break;
        case XCPT_WRITE_ACCESS:
                accessflag = MEMMAP_ACCESS_WRITE;
                fWriteAccess = TRUE;
                break;
        default:
                goto continueGuardException;
        }

        map = Win32MemMapView::findMapByView(pERepRec->ExceptionInfo[1], &offset, accessflag);
            if(map) {
        ret = map->commitGuardPage(pERepRec->ExceptionInfo[1], offset, fWriteAccess);
        map->Release();
        if(ret == TRUE)
            goto continueexecution;
            }
        }

continueGuardException:
        goto continuesearch;
    }

    case XCPT_UNABLE_TO_GROW_STACK:
    {
        //SvL: XCPT_UNABLE_TO_GROW_STACK is typically nested (failed guard page
        //     exception), so don't ignore them
        // We should no longer receive those!!
// @@VP20040507: Isn't this a bit dangerous to call dprintfon such exception
//#ifdef DEBUG
//        dprintfException(pERepRec, pERegRec, pCtxRec, p);
//#endif
        goto continuesearch;
    }


    /*
     * In OS/2 VIO Ctrl-C and Ctrl-Break is special stuff which comes in
     * thru the exception handler. In Win32 CUI they are handled by a
     * ControlCtrlEvent procedure. So, if we receive one of those signals
     * we assume that this is a VIO program and let the handlers handle this.
     * (If they want to.)
     */
    case XCPT_SIGNAL:
    {
        //This is not a reliable way to distinguish between Ctrl-C & Ctrl-Break
        BOOL breakPressed = WinGetKeyState(HWND_DESKTOP,VK_BREAK) & 0x8000;

        switch (pERepRec->ExceptionInfo[0])
        {
            case XCPT_SIGNAL_BREAK:
                breakPressed = TRUE;
                //no break

            case XCPT_SIGNAL_INTR:
                dprintfException(pERepRec, pERegRec, pCtxRec, p);
                if (InternalGenerateConsoleCtrlEvent((breakPressed) ? CTRL_BREAK_EVENT : CTRL_C_EVENT, 0))
                {
                    DosAcknowledgeSignalException(pERepRec->ExceptionInfo[0]);
                    goto continueexecution;
                }
                goto continuesearch;

            case XCPT_SIGNAL_KILLPROC:  /* resolve signal information */
                goto continuesearch;
        }
        goto CrashAndBurn;
    }

    default: //non-continuable exceptions
        dprintfException(pERepRec, pERegRec, pCtxRec, p);
        goto continuesearch;
    }
continuesearch:
#ifdef DEBUG
    LogException(LEAVE_EXCEPTION, prevlock);
#endif
    return XCPT_CONTINUE_SEARCH;

continueexecution:
#ifdef DEBUG
    LogException(LEAVE_EXCEPTION, prevlock);
#endif
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
void SYSTEM OS2SetExceptionHandler(void *exceptframe)
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
//*****************************************************************************
// Set exception handler if our handler has not yet been registered
//*****************************************************************************
void WIN32API ODIN_SetExceptionHandler(void *pExceptionRegRec)
{
  BOOL   fFound = FALSE;
  USHORT sel = RestoreOS2FS();
  PEXCEPTIONREGISTRATIONRECORD pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)QueryExceptionChain();

  while(pExceptRec != 0 && (ULONG)pExceptRec != -1)
  {
        if(pExceptRec->ExceptionHandler == OS2ExceptionHandler)
        {
            fFound = TRUE;
            break;
        }
        pExceptRec = pExceptRec->prev_structure;
  }
  if(!fFound)
  {
      OS2SetExceptionHandler(pExceptionRegRec);
  }
  SetFS(sel);
}
//*****************************************************************************
// Remove exception handler if it was registered previously
//
//*****************************************************************************
void WIN32API ODIN_UnsetExceptionHandler(void *pExceptionRegRec)
{
  USHORT sel = RestoreOS2FS();
  PEXCEPTIONREGISTRATIONRECORD pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)QueryExceptionChain();
  BOOL   fFound = FALSE;

  while(pExceptRec != 0 && (ULONG)pExceptRec != -1)
  {
        if(pExceptRec == pExceptionRegRec)
        {
            fFound = TRUE;
            break;
        }
        pExceptRec = pExceptRec->prev_structure;
  }

#ifdef DEBUG
  pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)QueryExceptionChain();

  if(fFound && pExceptRec != (PEXCEPTIONREGISTRATIONRECORD)pExceptionRegRec)
  {
      dprintf(("ERROR: ODIN_UnsetExceptionHandler: INSIDE!!!: exc rec %p, head %p\n", pExceptionRegRec, pExceptRec));
      PrintExceptionChain ();
  }
#endif
  if(fFound) {
      OS2UnsetExceptionHandler(pExceptionRegRec);
  }
  SetFS(sel);
}
//*****************************************************************************
//*****************************************************************************
#ifdef DEBUG
void PrintExceptionChain()
{
 USHORT sel = RestoreOS2FS();
 PEXCEPTIONREGISTRATIONRECORD pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)QueryExceptionChain();

  dprintf(("OS/2 Exception chain:"));
  while(pExceptRec != 0 && (ULONG)pExceptRec != -1)
  {
      char szBuf[512];
      ULONG ulModule;
      ULONG ulObject, ulOffset;
      CHAR szModule[260];

      *szBuf = '\0';
      APIRET rc = DosQueryModFromEIP(&ulModule, &ulObject, sizeof(szModule),
                                     szModule, &ulOffset, (ULONG)pExceptRec->ExceptionHandler);
      if(rc == NO_ERROR && ulObject != -1)
      {
        sprintf(szBuf, " <%8.8s> (%04X) obj %04X:%08X", szModule, ulModule, ulObject + 1, ulOffset);
#ifdef RAS
        char szSYMInfo[260];

        DosQueryModuleName(ulModule, sizeof(szModule), szModule);

        int namelen = strlen(szModule);
        if(namelen > 3)
        {
            strcpy(szModule + namelen - 3, "SYM");
            dbgGetSYMInfo(szModule, ulObject, ulOffset,
                          szSYMInfo, sizeof (szSYMInfo), TRUE);
            strcat(szBuf, " ");
            strcat(szBuf, szSYMInfo);
            // remove trailing \n or space
            szBuf[strlen(szBuf) - 1] = '\0';
        }
#endif
      }
      else
      {
          *szBuf = '\0';
      }

        dprintf(("  record %08X, prev %08X, handler %08X%s",
                 pExceptRec, pExceptRec->prev_structure, pExceptRec->ExceptionHandler,
                 szBuf));

        pExceptRec = pExceptRec->prev_structure;
  }
  dprintf(("END of OS/2 Exception chain."));
  SetFS(sel);
}
//*****************************************************************************
//*****************************************************************************
void PrintWin32ExceptionChain(PWINEXCEPTION_FRAME pFrame)
{
    dprintf(("Win32 exception chain:"));
    while ((pFrame != NULL) && ((ULONG)pFrame != 0xFFFFFFFF))
    {
        PWINEXCEPTION_FRAME pPrevFrame = __seh_get_prev_frame(pFrame);
        dprintf(("  Record at %08X, Prev at %08X, handler at %08X%s",
                 pFrame, pPrevFrame, pFrame->Handler,
                 pFrame->Handler == (PEXCEPTION_HANDLER)__seh_handler ?
                 " (SEH)" : ""));
        if (pFrame == pPrevFrame)
        {
            dprintf(("Chain corrupted! Record at %08X pointing to itself!",
                     pFrame));
            break;
        }
        pFrame = pPrevFrame;
    }
    dprintf(("END of Win32 exception chain."));
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
void SYSTEM OS2UnsetExceptionHandler(void *exceptframe)
{
 PEXCEPTIONREGISTRATIONRECORD pExceptRec = (PEXCEPTIONREGISTRATIONRECORD)exceptframe;

  DosUnsetExceptionHandler(pExceptRec);
  dprintf(("OS2UnsetExceptionHandler: exception chain %x", pExceptRec));
#ifdef DEBUG_ENABLELOG_LEVEL2
  PrintExceptionChain();
#endif
}
//*****************************************************************************
//*****************************************************************************
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
//*****************************************************************************
//*****************************************************************************

} // extern "C"

