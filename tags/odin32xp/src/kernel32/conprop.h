/* $Id: conprop.h,v 1.6 1999-10-27 12:38:46 phaller Exp $ */

/*
 * Win32 Console Subsystem for OS/2
 *
 * 1998/03/06 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) ConProp.Cpp 1.0.0 1998/03/06 PH start
 */

#ifndef _CONSOLEPROPERTY_H_
#define _CONSOLEPROPERTY_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include "handlemanager.h"
#include "console2.h"
#include "consolerc.h"


/*****************************************************************************
 * Defines & Macros                                                          *
 *****************************************************************************/

          /* this is sent after properties have been read properly from disk */
#define UM_PROPERTY_APPLY   WM_USER + 0x10
#define UM_PROPERTY_UNDO    WM_USER + 0x11


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/


 /*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


MRESULT EXPENTRY ConsolePropertyDlgProc(HWND   hwnd,
                                        ULONG  ulMessage,
                                        MPARAM mp1,
                                        MPARAM mp2);

                                               /* default console properties */
DWORD ConsolePropertyDefault(PICONSOLEOPTIONS pConsoleOptions);
                                                  /* read console properties */
DWORD ConsolePropertyLoad(PICONSOLEOPTIONS pConsoleOptions);

                                                 /* write console properties */
DWORD ConsolePropertySave(PICONSOLEOPTIONS pConsoleOptions);

                                                 /* apply console properties */
DWORD ConsolePropertyApply(PICONSOLEOPTIONS pConsoleOptions);

#ifdef __cplusplus
}
#endif


#endif /* _CONSOLEPROPERTY_H_ */
