/* $Id: env.h,v 1.4 2000-12-11 06:53:51 bird Exp $
 *
 * Environment access functions
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
const char *GetEnv(BOOL fExecChild);


#ifdef __cplusplus
}
#endif

#endif
