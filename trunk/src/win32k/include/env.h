/* $Id: env.h,v 1.1 2000-04-17 01:56:49 bird Exp $
 *
 * Enviroment access functions
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


#ifdef __cplusplus
}
#endif

#endif
