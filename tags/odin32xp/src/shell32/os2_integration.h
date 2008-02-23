/* $Id: os2_integration.h,v 1.1 2002-03-08 11:00:59 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 2002 Patrick Haller (patrick.haller@innotek.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _OS2_INTEGRATION_H_
#define _OS2_INTEGRATION_H_


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

DWORD ShellExecuteOS2(HWND   hwndWin32,
                      LPCSTR pszOperation,
                      LPCSTR pszFile,
                      LPCSTR pszParameters,
                      LPCSTR pszDirectory,
                      INT    iShowCmd);

#endif /* _OS2_INTEGRATION_H_ */
