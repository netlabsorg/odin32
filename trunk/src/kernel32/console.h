/* $Id: console.h,v 1.5 2002-06-26 07:13:00 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 Console Subsystem for OS/2
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************
 */

/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define TABVIRTUALKEYCODES 0x6D
extern char tabVirtualKeyCodes[TABVIRTUALKEYCODES];

extern BOOL flVioConsole;

DWORD iConsoleInit(BOOL fVioConsole);       /* creation of the console subsystem */

DWORD iConsoleDevicesRegister(void);       /* create console devices */

void  iConsoleWaitClose(void);      /* wait for user to close console */

BOOL  iConsoleIsActive(void);  /* returns TRUE if console window open */

ULONG iConsoleTerminate(void);/* termination of the console subsystem */

BOOL InternalGenerateConsoleCtrlEvent(DWORD dwCtrlEvent, DWORD dwProcessGroupID);
BOOL WIN32API DefaultConsoleCtrlHandler(DWORD dwEvent);

#ifdef __cplusplus
  }
#endif

#endif /* _CONSOLE_H_ */
