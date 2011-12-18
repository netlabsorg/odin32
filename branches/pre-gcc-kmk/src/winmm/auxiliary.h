/* $Id: auxiliary.h,v 1.1 1999-10-05 19:23:59 phaller Exp $ */

/*
 * Auxilary multimedia apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __AUX_H__
#define __AUX_H__

#ifdef __cplusplus
extern "C" {
#endif

BOOL  auxOS2Open(void);
void  auxOS2Close(void);
ULONG auxOS2SetVolume(ULONG dwVolume);
ULONG auxOS2GetVolume(ULONG *dwVolume);

extern ULONG auxDeviceId;

#ifdef __cplusplus
}
#endif

#endif
