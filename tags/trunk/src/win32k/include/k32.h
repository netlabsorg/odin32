/* $Id: k32.h,v 1.2 2000-02-15 23:39:19 bird Exp $
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


#ifdef __cplusplus
}
#endif
#endif
