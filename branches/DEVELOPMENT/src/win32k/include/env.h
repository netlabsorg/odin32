/* $Id: env.h,v 1.2 2000-04-17 02:26:04 bird Exp $
 *
 * Environment access functions
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _env_h_
#define _env_h_

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
*   Prototypes                                                                 *
*******************************************************************************/
const char *ScanEnv(const char *paszEnv, const char *pszVar);
const char *GetEnv(void);


#ifdef __cplusplus
}
#endif

#endif
