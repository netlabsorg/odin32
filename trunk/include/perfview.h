/* $Id: perfview.h,v 1.2 2001-10-12 00:49:51 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 performance measurement and self-profiling API functions
 *
 * Copyright 2001 Patrick Haller <patrick.haller@innotek.de>
 */


#ifndef _PERFVIEW_H_
#define _PERFVIEW_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  
// calibrate the subsystem
void _Optlink PerfView_Initialize(void);

// register a call to a function
void _Optlink PerfView_RegisterCall(char* pszFunctionName, 
                                    unsigned long int nTicks);

// dump the collected profile to the specified file
void _Optlink PerfView_DumpProfile(FILE *file);

// write the standard performance profile logfile
void _Optlink PerfView_Write();
  
  
#ifdef __cplusplus
}
#endif
  
#endif /* _PERFVIEW_H_ */
