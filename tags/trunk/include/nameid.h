/* $Id: nameid.h,v 1.5 1999-09-01 15:00:33 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __NAMEID_H__
#define __NAMEID_H__

#ifdef __cplusplus
      extern "C" {
#endif

#define RESID_CONVERTEDNAMES    63*1024

int SYSTEM ConvertNameId(ULONG hModule, char *);

//#ifndef ULONG
//#define ULONG unsigned long
//#endif
//PH: raises problems with WINE headers
typedef unsigned long ULONG;

#ifdef __cplusplus
        }
#endif

#endif
