/* $Id: libPrivate.h,v 1.1 2001-02-21 07:47:58 bird Exp $
 *
 * Private headerfile for the win32k library.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _libPrivate_h_
#define _libPrivate_h_



/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern BOOL     fInited;
extern HFILE    hWin32k;
extern USHORT   usCGSelector;


/*******************************************************************************
*   Functions Prototypes                                                       *
*******************************************************************************/
APIRET APIENTRY libCallThruCallGate(ULONG ulFunction, PVOID pvParam);

#endif
