/* $Id: w32sys.h,v 1.1 1999-05-24 20:19:21 ktk Exp $ */

/*
 * W32SYS
 * 
 * Copyright (c) 1996 Anand Kumria
 */

#ifndef __WINE__W32SYS_H
#define __WINE__W32SYS_H

typedef struct _WIN32SINFO {
    BYTE   bMajor;
    BYTE   bMinor;
    WORD   wBuildNumber;
    BOOL16 fDebug;
} WIN32SINFO, *LPWIN32SINFO;

#endif /* __WINE_W32SYS_H */
