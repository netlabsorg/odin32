/* $Id: k32.h,v 1.2.4.2 2000-09-02 20:45:29 bird Exp $
 *
 * k32 - definitions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
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
APIRET k32AllocMemEx(PPVOID ppb, ULONG cb, ULONG flag, ULONG ulCS, ULONG ulEIP);
APIRET k32QueryOTEs(HMTE hMTE, PQOTEBUFFER pQOte, ULONG cbOte);
APIRET k32QueryOptionsStatus(PK32OPTIONS pOptions, PK32STATUS pStatus);
APIRET k32SetOptions(PK32OPTIONS pOptions);


#ifdef __cplusplus
}
#endif
#endif
