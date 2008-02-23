/* $Id: oslibwps.h,v 1.1 2003-03-03 16:37:13 sandervl Exp $ */
/*
 * WPS object procedures
 *
 * Copyright 2003 Sander van Leeuwen (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBWPS_H__
#define __OSLIBWPS_H__

#ifdef __cplusplus
extern "C" {
#endif

BOOL WIN32API OSLibWinCreateObject(LPSTR pszPath, LPSTR pszArgs,
                                   LPSTR pszWorkDir, LPSTR pszLink,
                                   LPSTR pszDescription, LPSTR pszIcoPath,
                                   INT iIcoNdx, BOOL fDesktop);
BOOL WIN32API OSLibWinDeleteObject(LPSTR lpszLink);
BOOL WIN32API OSLibIsShellLink(LPSTR lpszLink);

#ifdef __cplusplus
}
#endif

#endif //__OSLIBWPS_H__
