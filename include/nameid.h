/* $Id: nameid.h,v 1.1 1999-05-24 20:19:06 ktk Exp $ */

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

void SYSTEM UpCase(char *mixedcase);

#ifndef ULONG
#define ULONG unsigned long
#endif

ULONG GetOS2ModuleHandle(ULONG hmod);

#ifdef __cplusplus
        }
#endif

#endif
