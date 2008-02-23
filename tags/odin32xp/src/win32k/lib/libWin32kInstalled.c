/* $Id: libWin32kInstalled.c,v 1.3 2001-02-21 07:47:59 bird Exp $
 *
 * libWin32kInstalled - checks if Win32k is installed or not.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSERRORS


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include "win32k.h"
#include "libPrivate.h"


/**
 * Checks if Win32k is installed or not.
 * @returns     TRUE:   Win32k is installed.
 *              FALSE:  Win32k is not installed. (or not inited yet)
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      The caller is reponsible for having called the libInit
 *              function before this call to get a decent result.
 */
BOOL   APIENTRY  libWin32kInstalled(void)
{
    return fInited;
}

