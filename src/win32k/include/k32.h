/* $Id: k32.h,v 1.7 2001-02-19 05:50:38 bird Exp $
 *
 * k32 - definitions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
APIRET k32AllocMemEx(PPVOID ppb, ULONG cb, ULONG flFlags, ULONG ulCS, ULONG ulEIP);
APIRET k32QuerySystemMemInfo(PK32SYSTEMMEMINFO pMemInfo);
APIRET k32QueryOTEs(HMTE hMTE, PQOTEBUFFER pQOte, ULONG cbOte);
APIRET k32QueryOptionsStatus(PK32OPTIONS pOptions, PK32STATUS pStatus);
APIRET k32SetOptions(PK32OPTIONS pOptions);
APIRET k32ProcessReadWrite(PID pid, ULONG cb, PVOID pvSource, PVOID pvTarget, BOOL fRead);


#ifdef __cplusplus
}
#endif
#endif
