/* $Id: oslibtime.h,v 1.2 2003-01-05 12:31:24 sandervl Exp $ */
/*
 * OS/2 time procedures
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBTIME_H__
#define __OSLIBTIME_H__

#ifdef __cplusplus
extern "C" {
#endif

ULONG OSLibDosGetTickCount();

#ifdef __cplusplus
}
#endif

#endif //__OSLIBTIME_H__
