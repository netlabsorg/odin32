/* $Id: oslibutil.h,v 1.4 2000-01-18 20:08:12 sandervl Exp $ */
/*
 * Window API utility functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBUTIL_H__
#define __OSLIBUTIL_H__

#ifndef OS2_INCLUDED
#define HAB ULONG
#define HMQ ULONG
#endif

HAB   GetThreadHAB();
void  SetThreadHAB(HAB hab);

HMQ   GetThreadMessageQueue();
void  SetThreadMessageQueue(HMQ hmq);

DWORD GetThreadMessageExtraInfo();
DWORD SetThreadMessageExtraInfo(DWORD lParam);

#endif //__OSLIBUTIL_H__
