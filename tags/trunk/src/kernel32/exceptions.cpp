/* $Id: exceptions.cpp,v 1.3 1999-06-10 20:47:51 phaller Exp $ */

/*
 * Win32 Device IOCTL API functions for OS/2
 *
 * Ported Wine exception handling code
 *
 * Copyright 1998 Sander van Leeuwen (OS/2 port)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
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
#define INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <builtin.h>
#include "exceptions.h"
#include "except.h"
#include "misc.h"

//Global Process Unhandled exception filter
static LPTOP_LEVEL_EXCEPTION_FILTER CurrentUnhExceptionFlt = NULL;
static UINT                         CurrentErrorMode = 0;
static PEXCEPTION_HANDLER           StartupCodeHandler = NULL;

extern "C" PWINEXCEPTION_FRAME QueryExceptionChain();
extern "C" PWINEXCEPTION_FRAME GetExceptionRecord(ULONG offset, ULONG segment);

LONG WIN32API UnhandledExceptionFilter(PWINEXCEPTION_POINTERS lpexpExceptionInfo);

//******************************************************************************
//******************************************************************************
UINT WIN32API SetErrorMode(UINT fuErrorMode)
{
 UINT oldmode = CurrentErrorMode;

  dprintf(("OS2SetErrorMode to %d\n", fuErrorMode));
  CurrentErrorMode = fuErrorMode;

  return(oldmode);
}
//******************************************************************************
//Unwinds exception handlers (heavily influenced by Wine)
//******************************************************************************
VOID _Pascal OS2RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags,
                   DWORD cArguments,      DWORD *lpArguments,
                   DWORD eip, DWORD esp,  DWORD ebp, DWORD flags,
                   DWORD eax, DWORD ebx,  DWORD ecx, DWORD edx,
                   DWORD edi, DWORD esi,  DWORD cs,  DWORD ds,
                   DWORD es,  DWORD fs,   DWORD gs,  DWORD ss)
{
 PWINEXCEPTION_FRAME   pframe;
 WINEXCEPTION_RECORD   record;
 WINEXCEPTION_POINTERS ExceptionInfo;
 WINCONTEXT            context;
 DWORD                 dispatch;
 int                   rc;
 int                   i;

   dprintf(("RaiseException %d\n", dwExceptionCode));

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

    if(lpArguments) {
    for(i=0;i<cArguments;i++)
        record.ExceptionInformation[i] = lpArguments[i];
    }

    // get chain of exception frames
    rc     = ExceptionContinueSearch;
    pframe = QueryExceptionChain();
    while( (pframe != NULL) && (pframe != ((void *)0xFFFFFFFF)) ) {
       dispatch=0;
       rc = pframe->Handler(&record, pframe, &context, &dispatch);

       if(rc == ExceptionContinueExecution)
            break;

       pframe = pframe->Prev;
    }
    if(rc == ExceptionContinueSearch && UnhandledExceptionFilter != NULL) {
	ExceptionInfo.ExceptionRecord = &record;
	ExceptionInfo.ContextRecord   = &context;

    	rc = UnhandledExceptionFilter(&ExceptionInfo);
    }

    if(rc != ExceptionContinueExecution)
    {
       DosExit(EXIT_PROCESS, 0);
    }
    return;
}
//******************************************************************************
//Unwinds exception handlers (heavily influenced by Wine)
//******************************************************************************
int _Pascal OS2RtlUnwind(PWINEXCEPTION_FRAME  pEndFrame, LPVOID unusedEip,
                 PWINEXCEPTION_RECORD pRecord,   DWORD  returnEax,
                 DWORD eip, DWORD esp,  DWORD ebp, DWORD flags,
                 DWORD eax, DWORD ebx,  DWORD ecx, DWORD edx,
                 DWORD edi, DWORD esi,  DWORD cs,  DWORD ds,
                 DWORD es,  DWORD fs,   DWORD gs,  DWORD ss)
{
 WINEXCEPTION_RECORD record;
 WINCONTEXT          context;
 DWORD               dispatch;
 int                 rc;

  dprintf(("RtlUnwind\n"));

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
  if(!pRecord) {
     record.ExceptionCode    = STATUS_INVALID_DISPOSITION;
     record.ExceptionFlags   = 0;
     record.ExceptionRecord  = NULL;
     record.ExceptionAddress = (LPVOID)eip;
     record.NumberParameters = 0;
     pRecord = &record;
  }

  if(pEndFrame)
        pRecord->ExceptionFlags |= EH_UNWINDING;
  else  pRecord->ExceptionFlags |= EH_UNWINDING | EH_EXIT_UNWIND;

  /* get chain of exception frames */
  while((QueryExceptionChain() != NULL) && (QueryExceptionChain() != (void *)0xffffffff) &&
    (QueryExceptionChain() != pEndFrame)) {
       dispatch=0;
       rc = QueryExceptionChain()->Handler(pRecord,
                                           QueryExceptionChain(),
                                           &context, &dispatch);

       if((rc == ExceptionCollidedUnwind) && (QueryExceptionChain() != (LPVOID)dispatch)) {
        SetExceptionChain(dispatch);
       }
       else
       if((QueryExceptionChain() != pEndFrame) && (QueryExceptionChain() != QueryExceptionChain()->Prev))
        SetExceptionChain((DWORD)QueryExceptionChain()->Prev);
       else break;
  }
  if(pEndFrame == UNWIND_ALL) {
    DosExit(EXIT_THREAD, 0);
  }
  return(0);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API UnhandledExceptionFilter(PWINEXCEPTION_POINTERS lpexpExceptionInfo)
{
 char  message[72];
 DWORD rc;

  dprintf(("UnhandledExceptionFilter\n"));

  if(CurrentUnhExceptionFlt && !(CurrentErrorMode & (SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX))) {
    rc = CurrentUnhExceptionFlt(lpexpExceptionInfo);
        if(rc != WINEXCEPTION_CONTINUE_SEARCH)
        return rc;
  }

  sprintf(message, "Unhandled exception 0x%08lx at address 0x%08lx.",
          lpexpExceptionInfo->ExceptionRecord->ExceptionCode,
          lpexpExceptionInfo->ExceptionRecord->ExceptionAddress);

  WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, message, "Oh, nooo!", 0, MB_OK | MB_ERROR);
  return WINEXCEPTION_EXECUTE_HANDLER;
}
//******************************************************************************
//******************************************************************************
LPTOP_LEVEL_EXCEPTION_FILTER WIN32API SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
 LPTOP_LEVEL_EXCEPTION_FILTER old = CurrentUnhExceptionFlt;
  dprintf(("SetUnhandledExceptionFilter to %X\n", lpTopLevelExceptionFilter));

  CurrentUnhExceptionFlt = lpTopLevelExceptionFilter;

  return(old);
}
//******************************************************************************
extern "C" ULONG getEAX();
extern "C" ULONG getEBX();
//******************************************************************************
void KillWin32Process(void)
{
 char  excptmsg[64];
 ULONG excptaddr, excptnr;

  excptnr   = getEAX();
  excptaddr = getEBX();

  dprintf(("KillWin32Process: Do you feel lucky, punk?!\n"));
//  sprintf(excptmsg, "Fatal Exception %X at %X", excptnr, excptaddr);
//  WinMessageBox(HWND_DESKTOP, NULL, excptmsg, "Win32 for OS/2", 0, MB_ERROR | MB_OK);
  SetExceptionChain((ULONG)0);
  DosExit(EXIT_PROCESS, 666);
}
//******************************************************************************
APIRET APIENTRY DosQueryModFromEIP (PULONG pulModule,
                                    PULONG pulObject,
                                    ULONG  ulBufferLength,
                                    PSZ    pszBuffer,
                                    PULONG pulOffset,
                                    ULONG  ulEIP);
//******************************************************************************
void dprintfException(PEXCEPTIONREPORTRECORD pERepRec,
                  PEXCEPTIONREGISTRATIONRECORD pERegRec,
                      PCONTEXTRECORD pCtxRec,
                      PVOID p)
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
              "R/W %08x at %08x.",
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
                    "Read Access at address %08x",
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

  if (rc == NO_ERROR)
    dprintf(("%s (#%u), Obj #%u, Offset %08x\n",
             szModule,
             ulModule,
             ulObject,
             ulOffset));
  else
    dprintf(("\n"));


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
dprintf(("   SS:ESP=%04x:%08x EFLAGS=%08x\n"
         "   CS:EIP=%04x:%08x EBP   =%08x\n",
             pCtxRec->ctx_SegSs,
             pCtxRec->ctx_RegEsp,
             pCtxRec->ctx_EFlags,
             pCtxRec->ctx_SegCs,
             pCtxRec->ctx_RegEip,
             pCtxRec->ctx_RegEbp));

  if (pCtxRec->ContextFlags & CONTEXT_INTEGER)         /* check flags */
    dprintf(("   EAX=%08x ESI=%08x\n"
             "   EBX=%08x EDI=%08x\n"
             "   ECX=%08x\n"
             "   EDX=%08x\n",
             pCtxRec->ctx_RegEax,
             pCtxRec->ctx_RegEsi,
             pCtxRec->ctx_RegEbx,
             pCtxRec->ctx_RegEdi,
             pCtxRec->ctx_RegEcx,
             pCtxRec->ctx_RegEdx));

  if (pCtxRec->ContextFlags & CONTEXT_SEGMENTS)        /* check flags */
    dprintf(("   DS=%08x\n"
             "   ES=%08x\n"
             "   FS=%08x\n"
             "   GS=%08x\n",
              pCtxRec->ctx_SegDs,
              pCtxRec->ctx_SegEs,
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

                                                             /* from PPC DDK */
#ifndef XCPT_CONTINUE_STOP
#define XCPT_CONTINUE_STOP 0x00716668
#endif

//  return (XCPT_CONTINUE_STOP);
//  return (XCPT_CONTINUE_SEARCH);
}
//******************************************************************************
//******************************************************************************
ERR _System OS2ExceptionHandler(PEXCEPTIONREPORTRECORD pERepRec,
                        PEXCEPTIONREGISTRATIONRECORD pERegRec,
                        PCONTEXTRECORD pCtxRec,
                        PVOID p)
{
//  pERegRec->prev_structure = 0;
  dprintfException(pERepRec, pERegRec, pCtxRec, p);

  /* Access violation at a known location */
  switch(pERepRec->ExceptionNum) {
  case XCPT_FLOAT_DENORMAL_OPERAND:
  case XCPT_FLOAT_DIVIDE_BY_ZERO:
  case XCPT_FLOAT_INEXACT_RESULT:
  case XCPT_FLOAT_INVALID_OPERATION:
  case XCPT_FLOAT_OVERFLOW:
  case XCPT_FLOAT_STACK_CHECK:
  case XCPT_FLOAT_UNDERFLOW:
    dprintf(("FPU exception, fix and continue\n"));
    pCtxRec->ctx_env[0] |= 0x1F;
    pCtxRec->ctx_stack[0].losig = 0;
    pCtxRec->ctx_stack[0].hisig = 0;
    pCtxRec->ctx_stack[0].signexp = 0;

    return (ERR)(XCPT_CONTINUE_EXECUTION);

  case XCPT_PROCESS_TERMINATE:
  case XCPT_ASYNC_PROCESS_TERMINATE:
    SetExceptionChain((ULONG)0);
    return (XCPT_CONTINUE_SEARCH);

  case XCPT_ACCESS_VIOLATION:
  case XCPT_BREAKPOINT:
  case XCPT_ARRAY_BOUNDS_EXCEEDED:
  case XCPT_DATATYPE_MISALIGNMENT:
  case XCPT_ILLEGAL_INSTRUCTION:
  case XCPT_PRIVILEGED_INSTRUCTION:
  case XCPT_INVALID_LOCK_SEQUENCE:
  case XCPT_INTEGER_DIVIDE_BY_ZERO:
  case XCPT_INTEGER_OVERFLOW:
  case XCPT_SINGLE_STEP:
  case XCPT_GUARD_PAGE_VIOLATION:
  case XCPT_UNABLE_TO_GROW_STACK:
  case XCPT_IN_PAGE_ERROR:
  case XCPT_SIGNAL:
    dprintf(("Continue and kill\n"));
    pCtxRec->ctx_RegEip = (ULONG)KillWin32Process;
    pCtxRec->ctx_RegEsp = pCtxRec->ctx_RegEsp + 0x10;
    pCtxRec->ctx_RegEax = pERepRec->ExceptionNum;
    pCtxRec->ctx_RegEbx = pCtxRec->ctx_RegEip;
    return (ERR)(XCPT_CONTINUE_EXECUTION);

  default: //non-continuable exceptions
        return (XCPT_CONTINUE_SEARCH);
  }
  return (XCPT_CONTINUE_SEARCH);
}
//******************************************************************************
//SvL: Replace original startup code exception handler
extern BOOL fExeStarted;
//******************************************************************************
void ReplaceExceptionHandler()
{
 static BOOL         fRegistered = FALSE;
 PWINEXCEPTION_FRAME orgframe;
 APIRET          rc;
 ULONGULONG            timerval;

  DisableFPUExceptions();

  if(fExeStarted == FALSE)
    return;

  orgframe = QueryExceptionChain();
  if((int)orgframe == 0 || (int)orgframe == -1)
    return;

  dprintf(("ReplaceExceptionHandler\n"));

  StartupCodeHandler = orgframe->Handler;
  orgframe->Handler = (PEXCEPTION_HANDLER)OS2ExceptionHandler;
  orgframe->Prev    = (_WINEXCEPTION_FRAME *)0;
  SetExceptionChain((ULONG)-1);
#if 0
  if(fRegistered == FALSE) {
#endif
    dprintf(("New exception frame at %X\n", orgframe));
    rc = DosSetExceptionHandler((PEXCEPTIONREGISTRATIONRECORD)orgframe);
    fRegistered = TRUE;
#if 0
  }
  else {
    while(orgframe && orgframe->Handler != (PEXCEPTION_HANDLER)OS2ExceptionHandler) {
        orgframe = orgframe->Prev;
    }
    dprintf(("Restore old exception frame %X\n", orgframe));
    if(orgframe) {
        orgframe->Prev  = (_WINEXCEPTION_FRAME *)0;
        SetExceptionChain((ULONG)orgframe);
    }
  }
#endif
}
//******************************************************************************
//******************************************************************************

