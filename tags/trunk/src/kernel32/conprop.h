/* $Id: conprop.h,v 1.3 1999-06-10 19:11:30 phaller Exp $ */

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
 * Resources                                                                 *
 *****************************************************************************/

#include "consolerc.h"


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

//#include <win32type.h>
#include "handlemanager.h"
#include "console2.h"
#include <console.h>


/*****************************************************************************
 * Defines & Macros                                                          *
 *****************************************************************************/

          /* this is sent after properties have been read properly from disk */
#define UM_PROPERTY_APPLY   WM_USER + 0x10
#define UM_PROPERTY_UNDO    WM_USER + 0x11


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/


typedef struct _Options
{
  HMODULE hmodResources;                   /* module handle for PM resources */

  BOOL  fTerminateAutomatically;       /* wait for window termination or not */
  BOOL  fSpeakerEnabled;               /* whether we process BEL 0x07 or not */
  ULONG ulSpeakerDuration;        /* duration and frequency for speaker beep */
  ULONG ulSpeakerFrequency;
  ULONG ulUpdateLimit;        /* when exceeding this line counter, automatic */
                                               /* screen update is performed */

  BOOL  fSetWindowPosition;           /* TRUE if window has to be positioned */

  COORD coordDefaultPosition;      /* default position of the console window */
  COORD coordDefaultSize;          /* default size     of the console window */
  COORD coordBufferSize;           /* size of the console buffer             */

  BOOL  fQuickInsert;                         /* like NT's quick insert mode */
  BOOL  fInsertMode;                                          /* insert mode */
  BOOL  fMouseActions;                  /* like Warp 4's "mouse action" mode */
  BOOL  fToolbarActive;                   /* windows95-like toolbar on / off */

  ULONG ulTabSize;                                         /* tabulator size */

  UCHAR ucDefaultAttribute;                    /* the default text attribute */

  UCHAR ucCursorDivisor;                /* cursor timer divisor for blinking */

  ULONG ulConsoleThreadPriorityClass;       /* priority settings for message */
  ULONG ulConsoleThreadPriorityDelta;                              /* thread */
  ULONG ulAppThreadPriorityClass;       /* priority settings for application */
  ULONG ulAppThreadPriorityDelta;                                  /* thread */

} CONSOLEOPTIONS, *PCONSOLEOPTIONS;


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


                                         /* read console properties from EAs */
APIRET EXPENTRY ConsolePropertyRead(PCONSOLEOPTIONS pConsoleOptions);


                                          /* write console properties to EAs */
APIRET EXPENTRY ConsolePropertyWrite(PCONSOLEOPTIONS pConsoleOptions);


#ifdef __cplusplus
}
#endif


#endif /* _CONSOLEPROPERTY_H_ */
