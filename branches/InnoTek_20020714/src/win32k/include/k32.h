/* $Id: k32.h,v 1.11 2001-07-10 05:24:33 bird Exp $
 *
 * k32 API - definitions.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _k32_h_
#define _k32_h_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Exported Functions                                                         *
*******************************************************************************/
APIRET _System k32AllocMemEx(PPVOID ppb, ULONG cb, ULONG flFlags, ULONG ulCS, ULONG ulEIP);
APIRET _System k32KillProcessEx(ULONG flAction, PID pid);
APIRET _System k32HandleSystemEvent(ULONG ulEvent, HEV hev, BOOL fHandle);
APIRET _System k32QueryCallGate(PUSHORT pusCGSelector);
APIRET _System k32QuerySystemMemInfo(PK32SYSTEMMEMINFO pMemInfo);
APIRET _System k32QueryOTEs(HMTE hMTE, PQOTEBUFFER pQOte, ULONG cbOte);
APIRET _System k32QueryOptionsStatus(PK32OPTIONS pOptions, PK32STATUS pStatus);
APIRET _System k32SetOptions(PK32OPTIONS pOptions);
APIRET _System k32ProcessReadWrite(PID pid, ULONG cb, PVOID pvSource, PVOID pvTarget, BOOL fRead);
APIRET _System k32SetEnvironment(PSZ pszzEnvironment, ULONG cchEnvironment, PID pid);


#ifdef __cplusplus
}
#endif
#endif
