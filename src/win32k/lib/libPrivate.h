/* $Id: libPrivate.h,v 1.1.2.1 2002-04-01 13:04:55 bird Exp $
 *
 * Private headerfile for the win32k library.
 *
 * Copyright (c) 2001-2002 knut st. osmundsen (bird@anduin.net)
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
