/* $Id: device.h,v 1.1 1999-05-24 20:19:11 ktk Exp $ */

#ifndef __WINE_DEVICE_H
#define __WINE_DEVICE_H
extern HANDLE DEVICE_Open( LPCSTR filename, DWORD access,
                             LPSECURITY_ATTRIBUTES sa );
#endif
