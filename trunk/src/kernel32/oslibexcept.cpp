/* $Id: oslibexcept.cpp,v 1.6 2001-05-19 11:14:38 sandervl Exp $ */
/*
 * Exception handler util. procedures
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <win32type.h>
#include <misc.h>
#include "oslibexcept.h"
#include <exceptions.h>
#include <wprocess.h>

#define DBG_LOCALLOG    DBG_oslibexcept
#include "dbglocal.h"

//******************************************************************************
//Dispatches OS/2 exception to win32 handler
//Returns: TRUE, win32 exception handler returned continue execution
//         FALSE, otherwise
//******************************************************************************
BOOL APIENTRY OSLibDispatchException(PEXCEPTIONREPORTRECORD pReportRec,
                                     PEXCEPTIONREGISTRATIONRECORD pRegistrationRec,
                                     PCONTEXTRECORD pContextRec, PVOID p)
{
 WINEXCEPTION_RECORD winreportrec;
 WINCONTEXT          wincontextrec;
 ULONG               rc;

  memset(&winreportrec, 0, sizeof(winreportrec));
  memcpy(&winreportrec, pReportRec, sizeof(*pReportRec));

  switch(pReportRec->ExceptionNum) {
  case XCPT_FLOAT_DENORMAL_OPERAND:
        winreportrec.ExceptionCode = EXCEPTION_FLT_DENORMAL_OPERAND;
        break;
  case XCPT_FLOAT_DIVIDE_BY_ZERO:
        winreportrec.ExceptionCode = EXCEPTION_FLT_DIVIDE_BY_ZERO;
        break;
  case XCPT_FLOAT_INEXACT_RESULT:
        winreportrec.ExceptionCode = EXCEPTION_FLT_INEXACT_RESULT;
        break;
  case XCPT_FLOAT_INVALID_OPERATION:
        winreportrec.ExceptionCode = EXCEPTION_FLT_INVALID_OPERATION;
        break;
  case XCPT_FLOAT_OVERFLOW:
        winreportrec.ExceptionCode = EXCEPTION_FLT_OVERFLOW;
        break;
  case XCPT_FLOAT_STACK_CHECK:
        winreportrec.ExceptionCode = EXCEPTION_FLT_STACK_CHECK;
        break;
  case XCPT_FLOAT_UNDERFLOW:
        winreportrec.ExceptionCode = EXCEPTION_FLT_UNDERFLOW;
        break;
  case XCPT_INTEGER_DIVIDE_BY_ZERO:
        winreportrec.ExceptionCode = EXCEPTION_INT_DIVIDE_BY_ZERO;
        break;
  case XCPT_INTEGER_OVERFLOW:
        winreportrec.ExceptionCode = EXCEPTION_INT_OVERFLOW;
        break;
  case XCPT_PRIVILEGED_INSTRUCTION:
        winreportrec.ExceptionCode = EXCEPTION_PRIV_INSTRUCTION;
        break;
  case XCPT_BREAKPOINT:
        winreportrec.ExceptionCode = EXCEPTION_BREAKPOINT;
        break;
  case XCPT_SINGLE_STEP:
        winreportrec.ExceptionCode = EXCEPTION_SINGLE_STEP;
        break;
  case XCPT_ARRAY_BOUNDS_EXCEEDED:
        winreportrec.ExceptionCode = EXCEPTION_ARRAY_BOUNDS_EXCEEDED;
        break;
  case XCPT_DATATYPE_MISALIGNMENT:
        winreportrec.ExceptionCode = EXCEPTION_DATATYPE_MISALIGNMENT;
        break;
  case XCPT_ILLEGAL_INSTRUCTION:
        winreportrec.ExceptionCode = EXCEPTION_ILLEGAL_INSTRUCTION;
        break;
  case XCPT_INVALID_LOCK_SEQUENCE:
        winreportrec.ExceptionCode = EXCEPTION_ILLEGAL_INSTRUCTION;
        break;
  case XCPT_GUARD_PAGE_VIOLATION:
        winreportrec.ExceptionCode = EXCEPTION_GUARD_PAGE;
        break;
  case XCPT_UNABLE_TO_GROW_STACK:
        winreportrec.ExceptionCode = EXCEPTION_STACK_OVERFLOW;
        break;
  case XCPT_IN_PAGE_ERROR:
        winreportrec.ExceptionCode = EXCEPTION_IN_PAGE_ERROR;
        break;
  case XCPT_ACCESS_VIOLATION:
        winreportrec.ExceptionCode = EXCEPTION_ACCESS_VIOLATION;
        break;
  default: //no other exceptions should be dispatched to win32 apps
        return FALSE;
  }
  //TODO:
  //According to the Wine folks the flags are the same in OS/2 and win32
  //Let's assume for now the rest is identical as well

  //copy context record info
  memset(&wincontextrec, 0, sizeof(wincontextrec));
  if(pContextRec->ContextFlags & CONTEXT_CONTROL) {
        wincontextrec.ContextFlags |= WINCONTEXT_CONTROL;
        wincontextrec.Ebp     = pContextRec->ctx_RegEbp;
        wincontextrec.Eip     = pContextRec->ctx_RegEip;
        wincontextrec.SegCs   = pContextRec->ctx_SegCs;
        wincontextrec.EFlags  = pContextRec->ctx_EFlags;
        wincontextrec.Esp     = pContextRec->ctx_RegEsp;
        wincontextrec.SegSs   = pContextRec->ctx_SegSs;
  }
  if(pContextRec->ContextFlags & CONTEXT_INTEGER) {
        wincontextrec.ContextFlags |= WINCONTEXT_INTEGER;
        wincontextrec.Edi     = pContextRec->ctx_RegEdi;
        wincontextrec.Esi     = pContextRec->ctx_RegEsi;
        wincontextrec.Ebx     = pContextRec->ctx_RegEbx;
        wincontextrec.Edx     = pContextRec->ctx_RegEdx;
        wincontextrec.Ecx     = pContextRec->ctx_RegEcx;
        wincontextrec.Eax     = pContextRec->ctx_RegEax;
  }

  TEB *winteb = GetThreadTEB();

  if(pContextRec->ContextFlags & CONTEXT_SEGMENTS) {
        wincontextrec.ContextFlags |= WINCONTEXT_SEGMENTS;
        wincontextrec.SegGs   = pContextRec->ctx_SegGs;
//   This resets FS to 0x150B - we DON'T want that!!
//      wincontextrec.SegFs   = pContextRec->ctx_SegFs;
        wincontextrec.SegFs   = winteb->teb_sel;
        wincontextrec.SegEs   = pContextRec->ctx_SegEs;
        wincontextrec.SegDs   = pContextRec->ctx_SegDs;
  }
  if(pContextRec->ContextFlags & CONTEXT_FLOATING_POINT) {
        wincontextrec.ContextFlags |= WINCONTEXT_FLOATING_POINT;
        //TODO: First 7 dwords the same?
        memcpy(&wincontextrec.FloatSave, pContextRec->ctx_env, sizeof(pContextRec->ctx_env));
        memcpy(&wincontextrec.FloatSave.RegisterArea, pContextRec->ctx_stack, sizeof(pContextRec->ctx_stack));
  }
  //It doesn't seem correct if we dispatch real exceptions to win32 apps
  //Some just call RtlUnwind and continue as if they were processing an
  //exception thrown by C++ code. (instead of real OS exception)
#if 1
  // We need to reset FS to its original (Win32) value, otherwise we'll likely
  // fuck up the Win32 exception handlers. They could end up using the wrong
  // exception chain if they access FS:[0] directly.
  DWORD oldsel = SetReturnFS(winteb->teb_sel);

  switch(pReportRec->ExceptionNum) {
  case XCPT_FLOAT_DENORMAL_OPERAND:
  case XCPT_FLOAT_DIVIDE_BY_ZERO:
  case XCPT_FLOAT_INEXACT_RESULT:
  case XCPT_FLOAT_INVALID_OPERATION:
  case XCPT_FLOAT_OVERFLOW:
  case XCPT_FLOAT_STACK_CHECK:
  case XCPT_FLOAT_UNDERFLOW:
        rc = RtlDispatchException(&winreportrec, &wincontextrec);
        break;

  case XCPT_ACCESS_VIOLATION:
        rc = RtlDispatchException(&winreportrec, &wincontextrec);
        break;

  case XCPT_ILLEGAL_INSTRUCTION:
  case XCPT_PRIVILEGED_INSTRUCTION:
        rc = RtlDispatchException(&winreportrec, &wincontextrec);
        break;

  case XCPT_INTEGER_DIVIDE_BY_ZERO:
  case XCPT_INTEGER_OVERFLOW:
  case XCPT_BREAKPOINT:
  case XCPT_SINGLE_STEP:
  case XCPT_ARRAY_BOUNDS_EXCEEDED:
  case XCPT_DATATYPE_MISALIGNMENT:
  case XCPT_INVALID_LOCK_SEQUENCE:
  case XCPT_GUARD_PAGE_VIOLATION:
  case XCPT_UNABLE_TO_GROW_STACK:
  case XCPT_IN_PAGE_ERROR:
  default:
	SetFS(oldsel);	//restore FS
        return FALSE; //let's not dispatch those for now
  }
  SetFS(oldsel);	//restore FS

  if(rc == ExceptionContinueExecution) {
        dprintf(("Win32 exception handler returned ExceptionContinueExecution"));
        if(wincontextrec.ContextFlags & WINCONTEXT_CONTROL) {
                pContextRec->ctx_RegEbp = wincontextrec.Ebp;
                pContextRec->ctx_RegEip = wincontextrec.Eip;
                pContextRec->ctx_SegCs  = wincontextrec.SegCs;
                pContextRec->ctx_EFlags = wincontextrec.EFlags;
                pContextRec->ctx_RegEsp = wincontextrec.Esp;
                pContextRec->ctx_SegSs  = wincontextrec.SegSs;
        }
        if(wincontextrec.ContextFlags & WINCONTEXT_INTEGER) {
                pContextRec->ctx_RegEdi = wincontextrec.Edi;
                pContextRec->ctx_RegEsi = wincontextrec.Esi;
                pContextRec->ctx_RegEbx = wincontextrec.Ebx;
                pContextRec->ctx_RegEdx = wincontextrec.Edx;
                pContextRec->ctx_RegEcx = wincontextrec.Ecx;
                pContextRec->ctx_RegEax = wincontextrec.Eax;
        }
#if 0
        //This is not a good idea
        if(wincontextrec.ContextFlags & WINCONTEXT_SEGMENTS) {
                pContextRec->ctx_SegGs = wincontextrec.SegGs;
                pContextRec->ctx_SegFs = wincontextrec.SegFs;
                pContextRec->ctx_SegEs = wincontextrec.SegEs;
                pContextRec->ctx_SegDs = wincontextrec.SegDs;
        }
#endif
        if(wincontextrec.ContextFlags & WINCONTEXT_FLOATING_POINT) {
                //TODO: First 7 dwords the same?
                memcpy(pContextRec->ctx_env, &wincontextrec.FloatSave, sizeof(pContextRec->ctx_env));
                memcpy(pContextRec->ctx_stack, &wincontextrec.FloatSave.RegisterArea, sizeof(pContextRec->ctx_stack));
        }
        if (pContextRec->ContextFlags & CONTEXT_CONTROL)         /* check flags */
                dprintf(("   SS:ESP=%04x:%08x EFLAGS=%08x\n",
                             pContextRec->ctx_SegSs,
                             pContextRec->ctx_RegEsp,
                             pContextRec->ctx_EFlags));
                dprintf(("   CS:EIP=%04x:%08x EBP   =%08x\n",
                             pContextRec->ctx_SegCs,
                             pContextRec->ctx_RegEip,
                             pContextRec->ctx_RegEbp));

        if (pContextRec->ContextFlags & CONTEXT_INTEGER)         /* check flags */
                    dprintf(("   EAX=%08x EBX=%08x ESI=%08x\n",
                             pContextRec->ctx_RegEax,
                             pContextRec->ctx_RegEbx,
                             pContextRec->ctx_RegEsi));
                    dprintf(("   ECX=%08x EDX=%08x EDI=%08x\n",
                             pContextRec->ctx_RegEcx,
                             pContextRec->ctx_RegEdx,
                             pContextRec->ctx_RegEdi));

        if (pContextRec->ContextFlags & CONTEXT_SEGMENTS)        /* check flags */
                    dprintf(("   DS=%04x     ES=%08x"
                             "   FS=%04x     GS=%04x\n",
                              pContextRec->ctx_SegDs,
                              pContextRec->ctx_SegEs,
                              pContextRec->ctx_SegFs,
                              pContextRec->ctx_SegGs));

        if (pContextRec->ContextFlags & CONTEXT_FLOATING_POINT)  /* check flags */
        {
            ULONG ulCounter;                 /* temporary local counter for fp stack */

                dprintf(("   Env[0]=%08x Env[1]=%08x Env[2]=%08x Env[3]=%08x\n",
                             pContextRec->ctx_env[0],
                             pContextRec->ctx_env[1],
                             pContextRec->ctx_env[2],
                             pContextRec->ctx_env[3]));

                dprintf(("   Env[4]=%08x Env[5]=%08x Env[6]=%08x\n",
                             pContextRec->ctx_env[4],
                             pContextRec->ctx_env[5],
                             pContextRec->ctx_env[6]));

                for (ulCounter = 0;
                     ulCounter < 8; /* see TOOLKIT\INCLUDE\BSEEXPT.H, _CONTEXT structure */
                     ulCounter ++)
                      dprintf(("   FP-Stack[%u] losig=%08x hisig=%08x signexp=%04x\n",
                               ulCounter,
                               pContextRec->ctx_stack[0].losig,
                               pContextRec->ctx_stack[0].hisig,
                               pContextRec->ctx_stack[0].signexp));
        }

        return TRUE;
  }
  dprintf(("Win32 exception handler returned %x", rc));
#endif
  return FALSE;
}
//******************************************************************************
//******************************************************************************
