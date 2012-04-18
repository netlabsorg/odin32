/* $Id: oslibexcept.cpp,v 1.7 2001-06-04 21:18:40 sandervl Exp $ */
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
#include <exceptions.h>
#include <wprocess.h>

#include "oslibexcept.h"

#define DBG_LOCALLOG    DBG_oslibexcept
#include "dbglocal.h"

/**
 * Converts the OS/2 exception information to the Win32 exception information.
 *
 * Returns TRUE on succes and FALSE otherwise.
 */
BOOL APIENTRY OSLibConvertExceptionInfo(PEXCEPTIONREPORTRECORD pReportRec,
                                        PCONTEXTRECORD pContextRec,
                                        PWINEXCEPTION_RECORD pWinReportRec,
                                        PWINCONTEXT pWinContextRec,
                                        TEB *pWinTEB)
{
    memset(pWinReportRec, 0, sizeof(*pWinReportRec));
    memcpy(pWinReportRec, pReportRec, sizeof(*pReportRec));

    switch (pReportRec->ExceptionNum)
    {
    case XCPT_FLOAT_DENORMAL_OPERAND:
        pWinReportRec->ExceptionCode = EXCEPTION_FLT_DENORMAL_OPERAND;
        break;
    case XCPT_FLOAT_DIVIDE_BY_ZERO:
        pWinReportRec->ExceptionCode = EXCEPTION_FLT_DIVIDE_BY_ZERO;
        break;
    case XCPT_FLOAT_INEXACT_RESULT:
        pWinReportRec->ExceptionCode = EXCEPTION_FLT_INEXACT_RESULT;
        break;
    case XCPT_FLOAT_INVALID_OPERATION:
        pWinReportRec->ExceptionCode = EXCEPTION_FLT_INVALID_OPERATION;
        break;
    case XCPT_FLOAT_OVERFLOW:
        pWinReportRec->ExceptionCode = EXCEPTION_FLT_OVERFLOW;
        break;
    case XCPT_FLOAT_STACK_CHECK:
        pWinReportRec->ExceptionCode = EXCEPTION_FLT_STACK_CHECK;
        break;
    case XCPT_FLOAT_UNDERFLOW:
        pWinReportRec->ExceptionCode = EXCEPTION_FLT_UNDERFLOW;
        break;
    case XCPT_INTEGER_DIVIDE_BY_ZERO:
        pWinReportRec->ExceptionCode = EXCEPTION_INT_DIVIDE_BY_ZERO;
        break;
    case XCPT_INTEGER_OVERFLOW:
        pWinReportRec->ExceptionCode = EXCEPTION_INT_OVERFLOW;
        break;
    case XCPT_PRIVILEGED_INSTRUCTION:
        pWinReportRec->ExceptionCode = EXCEPTION_PRIV_INSTRUCTION;
        break;
    case XCPT_BREAKPOINT:
        pWinReportRec->ExceptionCode = EXCEPTION_BREAKPOINT;
        break;
    case XCPT_SINGLE_STEP:
        pWinReportRec->ExceptionCode = EXCEPTION_SINGLE_STEP;
        break;
    case XCPT_ARRAY_BOUNDS_EXCEEDED:
        pWinReportRec->ExceptionCode = EXCEPTION_ARRAY_BOUNDS_EXCEEDED;
        break;
    case XCPT_DATATYPE_MISALIGNMENT:
        pWinReportRec->ExceptionCode = EXCEPTION_DATATYPE_MISALIGNMENT;
        break;
    case XCPT_ILLEGAL_INSTRUCTION:
        pWinReportRec->ExceptionCode = EXCEPTION_ILLEGAL_INSTRUCTION;
        break;
    case XCPT_INVALID_LOCK_SEQUENCE:
        pWinReportRec->ExceptionCode = EXCEPTION_ILLEGAL_INSTRUCTION;
        break;
    case XCPT_GUARD_PAGE_VIOLATION:
        pWinReportRec->ExceptionCode = EXCEPTION_GUARD_PAGE;
        break;
    case XCPT_UNABLE_TO_GROW_STACK:
        pWinReportRec->ExceptionCode = EXCEPTION_STACK_OVERFLOW;
        break;
    case XCPT_IN_PAGE_ERROR:
        pWinReportRec->ExceptionCode = EXCEPTION_IN_PAGE_ERROR;
        break;
    case XCPT_ACCESS_VIOLATION:
        pWinReportRec->ExceptionCode = EXCEPTION_ACCESS_VIOLATION;
        break;
    default: // no other exceptions should be dispatched to win32 apps
        return FALSE;
    }

    // TODO:
    // According to the Wine folks the flags are the same in OS/2 and win32
    // Let's assume for now the rest is identical as well

    // copy context record info
    memset(pWinContextRec, 0, sizeof(*pWinContextRec));
    if (pContextRec->ContextFlags & CONTEXT_CONTROL)
    {
        pWinContextRec->ContextFlags |= WINCONTEXT_CONTROL;
        pWinContextRec->Ebp     = pContextRec->ctx_RegEbp;
        pWinContextRec->Eip     = pContextRec->ctx_RegEip;
        pWinContextRec->SegCs   = pContextRec->ctx_SegCs;
        pWinContextRec->EFlags  = pContextRec->ctx_EFlags;
        pWinContextRec->Esp     = pContextRec->ctx_RegEsp;
        pWinContextRec->SegSs   = pContextRec->ctx_SegSs;
    }
    if (pContextRec->ContextFlags & CONTEXT_INTEGER)
    {
        pWinContextRec->ContextFlags |= WINCONTEXT_INTEGER;
        pWinContextRec->Edi     = pContextRec->ctx_RegEdi;
        pWinContextRec->Esi     = pContextRec->ctx_RegEsi;
        pWinContextRec->Ebx     = pContextRec->ctx_RegEbx;
        pWinContextRec->Edx     = pContextRec->ctx_RegEdx;
        pWinContextRec->Ecx     = pContextRec->ctx_RegEcx;
        pWinContextRec->Eax     = pContextRec->ctx_RegEax;
    }

    if (pContextRec->ContextFlags & CONTEXT_SEGMENTS)
    {
        pWinContextRec->ContextFlags |= WINCONTEXT_SEGMENTS;
        pWinContextRec->SegGs   = pContextRec->ctx_SegGs;
        // set Wn32 TEB if we run in switch FS mode
        if (pWinTEB)
            pWinContextRec->SegFs   = pWinTEB->teb_sel;
        else
            pWinContextRec->SegFs   = pContextRec->ctx_SegFs;
        pWinContextRec->SegEs   = pContextRec->ctx_SegEs;
        pWinContextRec->SegDs   = pContextRec->ctx_SegDs;
    }

    if (pContextRec->ContextFlags & CONTEXT_FLOATING_POINT)
    {
        pWinContextRec->ContextFlags |= WINCONTEXT_FLOATING_POINT;
        //TODO: First 7 dwords the same?
        memcpy(&pWinContextRec->FloatSave,
               pContextRec->ctx_env, sizeof(pContextRec->ctx_env));
        memcpy(&pWinContextRec->FloatSave.RegisterArea,
               pContextRec->ctx_stack, sizeof(pContextRec->ctx_stack));
    }

    return TRUE;
}

/**
 * Converts the Win32 exception handler result to the OS/2 exception handler
 * result.
 *
 * Returns TRUE if the OS/2 exception handler result should be
 * XCPT_CONTINUE_EXECUTION and FALSE otherwise.
 */
BOOL APIENTRY OSLibConvertExceptionResult(ULONG rc,
                                          PWINCONTEXT pWinContextRec,
                                          PCONTEXTRECORD pContextRec)
{
    if (rc != ExceptionContinueExecution)
    {
        dprintf(("Win32 exception handler returned %x", rc));
        return FALSE;
    }

    dprintf(("Win32 exception handler returned ExceptionContinueExecution"));

    if (pWinContextRec->ContextFlags & WINCONTEXT_CONTROL)
    {
        pContextRec->ctx_RegEbp = pWinContextRec->Ebp;
        pContextRec->ctx_RegEip = pWinContextRec->Eip;
        pContextRec->ctx_SegCs  = pWinContextRec->SegCs;
        pContextRec->ctx_EFlags = pWinContextRec->EFlags;
        pContextRec->ctx_RegEsp = pWinContextRec->Esp;
        pContextRec->ctx_SegSs  = pWinContextRec->SegSs;
    }

    if (pWinContextRec->ContextFlags & WINCONTEXT_INTEGER)
    {
        pContextRec->ctx_RegEdi = pWinContextRec->Edi;
        pContextRec->ctx_RegEsi = pWinContextRec->Esi;
        pContextRec->ctx_RegEbx = pWinContextRec->Ebx;
        pContextRec->ctx_RegEdx = pWinContextRec->Edx;
        pContextRec->ctx_RegEcx = pWinContextRec->Ecx;
        pContextRec->ctx_RegEax = pWinContextRec->Eax;
    }

#if 0
    // This is not a good idea
    if (pWinContextRec->ContextFlags & WINCONTEXT_SEGMENTS)
    {
        pContextRec->ctx_SegGs = pWinContextRec->SegGs;
        pContextRec->ctx_SegFs = pWinContextRec->SegFs;
        pContextRec->ctx_SegEs = pWinContextRec->SegEs;
        pContextRec->ctx_SegDs = pWinContextRec->SegDs;
    }
#endif

    if (pWinContextRec->ContextFlags & WINCONTEXT_FLOATING_POINT)
    {
        //TODO: First 7 dwords the same?
        memcpy(pContextRec->ctx_env, &pWinContextRec->FloatSave,
               sizeof(pContextRec->ctx_env));
        memcpy(pContextRec->ctx_stack, &pWinContextRec->FloatSave.RegisterArea,
               sizeof(pContextRec->ctx_stack));
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

/**
 * Dispatches OS/2 exception to win32 handler.
 *
 * Returns TRUE if the Win32 exception handler returned
 * ExceptionContinueExecution and FALSE otherwise.
 */
BOOL APIENTRY OSLibDispatchExceptionWin32(PEXCEPTIONREPORTRECORD pReportRec,
                                          PEXCEPTIONREGISTRATIONRECORD pRegistrationRec,
                                          PCONTEXTRECORD pContextRec, PVOID p)
{
    WINEXCEPTION_RECORD winReportRec;
    WINCONTEXT          winContextRec;

    ULONG rc;

    TEB *pWinTEB = GetThreadTEB();

    OSLibConvertExceptionInfo(pReportRec, pContextRec,
                              &winReportRec, &winContextRec, pWinTEB);

    // It doesn't seem correct if we dispatch real exceptions to win32 apps
    // Some just call RtlUnwind and continue as if they were processing an
    // exception thrown by C++ code. (instead of real OS exception)

    // We need to reset FS to its original (Win32) value, otherwise we'll likely
    // fuck up the Win32 exception handlers. They could end up using the wrong
    // exception chain if they access FS:[0] directly.
    DWORD oldsel = SetReturnFS(pWinTEB->teb_sel);

    switch(pReportRec->ExceptionNum)
    {
    case XCPT_FLOAT_DENORMAL_OPERAND:
    case XCPT_FLOAT_DIVIDE_BY_ZERO:
    case XCPT_FLOAT_INEXACT_RESULT:
    case XCPT_FLOAT_INVALID_OPERATION:
    case XCPT_FLOAT_OVERFLOW:
    case XCPT_FLOAT_STACK_CHECK:
    case XCPT_FLOAT_UNDERFLOW:
        rc = RtlDispatchException(&winReportRec, &winContextRec);
        break;

    case XCPT_ACCESS_VIOLATION:
        rc = RtlDispatchException(&winReportRec, &winContextRec);
        break;

    case XCPT_ILLEGAL_INSTRUCTION:
    case XCPT_PRIVILEGED_INSTRUCTION:
    case XCPT_INTEGER_DIVIDE_BY_ZERO:
    case XCPT_UNABLE_TO_GROW_STACK:
    case XCPT_GUARD_PAGE_VIOLATION:
#ifndef DEBUG
    case XCPT_BREAKPOINT:
#endif
        rc = RtlDispatchException(&winReportRec, &winContextRec);
        break;

#ifdef DEBUG
    case XCPT_BREAKPOINT:
#endif
    case XCPT_INTEGER_OVERFLOW:
    case XCPT_SINGLE_STEP:
    case XCPT_ARRAY_BOUNDS_EXCEEDED:
    case XCPT_DATATYPE_MISALIGNMENT:
    case XCPT_INVALID_LOCK_SEQUENCE:
    case XCPT_IN_PAGE_ERROR:
    default:
        SetFS(oldsel);	//restore FS
        return FALSE; //let's not dispatch those for now
    }

    SetFS(oldsel);	//restore FS

    return OSLibConvertExceptionResult(rc, &winContextRec, pContextRec);
}
