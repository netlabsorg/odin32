/* $Id: oslibthread.cpp,v 1.1 2003-03-27 14:00:53 sandervl Exp $ */
/*
 * Wrappers for OS/2 Dos* API (Thread)
 *
 * Copyright 1998-2002 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSDEVIOCTL
#define INCL_DOSDEVICES
#define INCL_NPIPES
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <win32api.h>
#include <winconst.h>
#include <dbglog.h>
#include "oslibdos.h"
#include "oslibthread.h"
#include "exceptions.h"

#define DBG_LOCALLOG    DBG_oslibthread
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetPriority(ULONG tid, int priority)
{
    DWORD  ret, os2priorityclass;
    LONG   os2prioritydelta;
    APIRET rc;

    switch(priority)
    {
    case THREAD_PRIORITY_IDLE_W:
        os2priorityclass = PRTYC_IDLETIME;
        os2prioritydelta = 0;
        break;
    case THREAD_PRIORITY_LOWEST_W:
        os2priorityclass = PRTYC_REGULAR;
        os2prioritydelta = PRTYD_MINIMUM;
        break;
    case THREAD_PRIORITY_BELOW_NORMAL_W:
        os2priorityclass = PRTYC_REGULAR;
        os2prioritydelta = -15;
        break;
    case THREAD_PRIORITY_NORMAL_W:
        os2priorityclass = PRTYC_REGULAR;
        os2prioritydelta = 0;
        break;
    case THREAD_PRIORITY_ABOVE_NORMAL_W:
        os2priorityclass = PRTYC_REGULAR;
        os2prioritydelta = 15;
        break;
    case THREAD_PRIORITY_HIGHEST_W:
        os2priorityclass = PRTYC_REGULAR;
        os2prioritydelta = PRTYD_MAXIMUM;
        break;
    case THREAD_PRIORITY_TIME_CRITICAL_W:
        //NOTE: There is code relying on the fact that win32 threads can never
        //      have the maximum priority available in OS/2!!
        //      (e.g. SetThreadContext)
        dprintf(("PRTYC_TIMECRITICAL!!"));
        os2priorityclass = PRTYC_TIMECRITICAL;
        os2prioritydelta = 0;
        break;
    default:
        dprintf(("!WARNING!: Invalid priority!!"));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return ERROR_INVALID_PARAMETER_W;
    }
    rc = DosSetPriority(PRTYS_THREAD, os2priorityclass, os2prioritydelta, tid);
    ret = error2WinError(rc, ERROR_INVALID_PARAMETER);
    SetLastError(ret);
    return ret;
}
//******************************************************************************
//Useful to temporarily boost the priority to max to ensure it's scheduled first
//Windows threads never receive such a high priority (time critical, delta 0 = max)
//******************************************************************************
void OSLibDosSetMaxPriority(ULONG tid)
{
    APIRET rc;

    dprintf(("OSLibDosSetMaxPriority for %x", tid));
    rc = DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, tid);
    if(rc != NO_ERROR) {
        DebugInt3();
    }
}
//******************************************************************************
//******************************************************************************
void OSLibDosExitThread(ULONG retcode)
{
    DosExit(EXIT_THREAD, retcode);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibQueryThreadContext(ULONG ulThreadId, ULONG teb_sel, CONTEXT *lpContext)
{
    CONTEXTRECORD ctxrec;
    APIRET        rc;
    int           nrtries = 0;

tryagain:
    rc = DosQueryThreadContext(ulThreadId, CONTEXT_FULL, &ctxrec);
    if(rc != NO_ERROR) {
        dprintf(("ERROR: DosQueryThreadContext failed with rc %d!!", rc));
        if(rc == ERROR_NOT_FROZEN && ++nrtries < 5) {
            DosSleep(50);
            goto tryagain;
        }
        SetLastError(error2WinError(rc, ERROR_INVALID_PARAMETER));
        return FALSE;
    }
    if(lpContext->ContextFlags & WINCONTEXT_CONTROL) {
        lpContext->Ebp     = ctxrec.ctx_RegEbp;
        lpContext->Eip     = ctxrec.ctx_RegEip;
        lpContext->SegCs   = ctxrec.ctx_SegCs;
        lpContext->EFlags  = ctxrec.ctx_EFlags;
        lpContext->Esp     = ctxrec.ctx_RegEsp;
        lpContext->SegSs   = ctxrec.ctx_SegSs;
    }
    if(lpContext->ContextFlags & WINCONTEXT_INTEGER) {
        lpContext->Edi     = ctxrec.ctx_RegEdi;
        lpContext->Esi     = ctxrec.ctx_RegEsi;
        lpContext->Ebx     = ctxrec.ctx_RegEbx;
        lpContext->Edx     = ctxrec.ctx_RegEdx;
        lpContext->Ecx     = ctxrec.ctx_RegEcx;
        lpContext->Eax     = ctxrec.ctx_RegEax;
    }
    if(lpContext->ContextFlags & WINCONTEXT_SEGMENTS) {
        lpContext->SegGs   = ctxrec.ctx_SegGs;
//   This resets FS to 0x150B - we DON'T want that!!
//      lpContext->SegFs   = ctxrec.ctx_SegFs;
        lpContext->SegFs   = teb_sel;
        lpContext->SegEs   = ctxrec.ctx_SegEs;
        lpContext->SegDs   = ctxrec.ctx_SegDs;
    }
    if(lpContext->ContextFlags & WINCONTEXT_FLOATING_POINT) {
        //TODO: First 7 dwords the same?
        memcpy(&lpContext->FloatSave, ctxrec.ctx_env, sizeof(ctxrec.ctx_env));
        memcpy(&lpContext->FloatSave.RegisterArea, ctxrec.ctx_stack, sizeof(ctxrec.ctx_stack));
    }
    SetLastError(ERROR_SUCCESS_W);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
void OSLibDisableThreadSwitching()
{
    DosEnterCritSec();
}
//******************************************************************************
//******************************************************************************
void OSLibEnableThreadSwitching()
{
    DosExitCritSec();
}
//******************************************************************************
//******************************************************************************
