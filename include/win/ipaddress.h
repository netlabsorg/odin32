/* $Id: ipaddress.h,v 1.3 2001-03-31 13:23:33 sandervl Exp $ */

/*
 * IP Address class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1998 Alex Priem
 */

#ifndef __WINE_IPADDRESS_H
#define __WINE_IPADDRESS_H

typedef struct tagIPADDRESS_INFO
{
        COMCTL32_HEADER header;

        BYTE LowerLimit[4];
        BYTE UpperLimit[4];

        RECT    rcClient;
        INT     uFocus;
} IPADDRESS_INFO;

typedef struct tagIP_SUBCLASS_INFO
{
    WNDPROC wpOrigProc[4];
    HWND    hwndIP[4];
        IPADDRESS_INFO *infoPtr;
    HWND    hwnd;
    UINT    uRefCount;
} IP_SUBCLASS_INFO, *LPIP_SUBCLASS_INFO;



#endif  /* __WINE_IPADDRESS_H */
