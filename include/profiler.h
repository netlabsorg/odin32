/* $Id: profiler.h,v 1.1 2001-11-21 16:01:48 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * ODINCRT performance measurement
 *
 * Copyright 2001 Patrick Haller <patrick.haller@innotek.de>
 */


#ifndef _PROFILE_H_
#define _PROFILE_H_

#ifdef __cplusplus
extern "C" {
#endif
  
BOOL _System ProfilerIsEnabled(void);
void _System ProfilerEnable(BOOL fState);
void _System ProfilerInitialize(void);
void _System ProfilerTerminate(void);
void _System ProfilerWrite();

#ifdef __cplusplus
}
#endif
  
#endif /* _PROFILE_H_ */
