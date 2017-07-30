/* $Id: exception.c,v 1.3 2003-04-08 12:47:05 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1999 Turchanov Sergey
 * Copyright 1999 Alexandre Julliard
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 *
 * NT basis DLL
 *
 */

/*
 * Note: All exception handling in ODIN has to be done in the KERNEL32 DLL.
 *       Therefore, we're just putting forwarders in here.
 */

#include <stdlib.h>
#include <string.h>

#include "ntdll.h"

#include "winuser.h"
#include "winerror.h"
#include "winreg.h"

/*
#include "winnt.h"
#include "ntddk.h"
#include "process.h"
#include "global.h"
#include "wine/exception.h"
#include "stackframe.h"
#include "sig_context.h"
#include "miscemu.h"
#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(seh)
*/


/***********************************************************************
 *            RtlRaiseException  (NTDLL.464)
 */
void WINAPI REGS_FUNC(RtlRaiseException)( EXCEPTION_RECORD *rec, CONTEXT *context )
{
  dprintf(("NTDLL: RtlRaiseException (%08xh,%08xh) not implemented.\n",
           rec,
           context));

  // forward to handler in KERNEL32
}


/*******************************************************************
 *         RtlUnwind  (KERNEL32.590) (NTDLL.518)
 */
void WINAPI REGS_FUNC(RtlUnwind)( PEXCEPTION_FRAME pEndFrame, LPVOID unusedEip,
                                  PEXCEPTION_RECORD pRecord, DWORD returnEax,
                                  CONTEXT *context )
{
  dprintf(("NTDLL: RtlUnwind(%08xh, %08xh, %08xh, %08xh, %08xh) not implemented.\n",
           pEndFrame,
           unusedEip,
           pRecord,
           returnEax,
           context));

  // forward to handler in KERNEL32
}


/*******************************************************************
 *         NtRaiseException    (NTDLL.175)
 *
 * Real prototype:
 *    DWORD WINAPI NtRaiseException( EXCEPTION_RECORD *rec, CONTEXT *ctx, BOOL first );
 */
void WINAPI REGS_FUNC(NtRaiseException)( EXCEPTION_RECORD *rec, CONTEXT *ctx,
                                         BOOL first, CONTEXT *context )
{
  dprintf(("NTDLL: NtRaiseException(%08xh, %08xh, %08xh, %08xh) not implemented.\n",
           rec,
           ctx,
           first,
           context));


    //REGS_FUNC(RtlRaiseException)( rec, ctx );
    *context = *ctx;
}


/***********************************************************************
 *            RtlRaiseStatus  (NTDLL.465)
 *
 * Raise an exception with ExceptionCode = status
 */
void WINAPI RtlRaiseStatus( NTSTATUS status )
{
  EXCEPTION_RECORD ExceptionRec;

  dprintf(("NTDLL: RtlRaiseStatus(%08xh) not implemented.\n",
           status));

  ExceptionRec.ExceptionCode    = status;
  ExceptionRec.ExceptionFlags   = EH_NONCONTINUABLE;
  ExceptionRec.ExceptionRecord  = NULL;
  ExceptionRec.NumberParameters = 0;
  //RtlRaiseException( &ExceptionRec );
}


/***********************************************************************
 *           DebugBreak   (KERNEL32.181)
 */
void WINAPI REGS_FUNC(DebugBreak)( CONTEXT *context )
{
    EXCEPTION_RECORD rec;

    rec.ExceptionCode    = EXCEPTION_BREAKPOINT;
    rec.ExceptionFlags   = EH_NONCONTINUABLE;
    rec.ExceptionRecord  = NULL;
    rec.NumberParameters = 0;
    //REGS_FUNC(RtlRaiseException)( &rec, context );
}
