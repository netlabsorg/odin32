/* $Id: oslibutil.h,v 1.1 1999-07-17 09:17:57 sandervl Exp $ */
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

#endif //__OSLIBUTIL_H__
