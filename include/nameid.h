/* $Id: nameid.h,v 1.6 1999-09-01 19:14:30 sandervl Exp $ */

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

#ifndef ULONG
#define ULONG unsigned long
#endif

#ifdef __cplusplus
        }
#endif

#endif
