/* $Id: asmutil.h,v 1.1 2001-05-30 17:43:38 sandervl Exp $ */

/*
 * asmutil.asm function definition
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __ASMUTIL_H__
#define __ASMUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

double CDECL invokeStdCallDouble(PVOID function, int paramsize, PVOID pStack);
DWORD  CDECL invokeStdCallDword(PVOID function, int paramsize, PVOID pStack);

#ifdef __cplusplus
}
#endif

#endif
