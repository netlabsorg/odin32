/* $Id: trackbar.h,v 1.9 2002-10-29 13:15:16 sandervl Exp $ */

#ifndef __WINE_ASYNC_H
#define __WINE_ASYNC_H

extern void ASYNC_RegisterFD(int unixfd,void (*handler)(int fd,void *private),void *private);
extern void ASYNC_UnregisterFD(int unixfd,void (*handler)(int fd,void *private));
#endif
