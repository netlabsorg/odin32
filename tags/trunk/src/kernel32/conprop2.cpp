/* $Id: conprop2.cpp,v 1.7 2001-03-13 18:45:33 sandervl Exp $ */

/*
 * Win32 Console API Translation for OS/2
 *
 * 1998/03/06 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) conprop.cpp             1.0.0   1998/03/06 PH Start from scratch
 */


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 - save / load properties from EAs

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#define  INCL_GPI
#define  INCL_WIN
#define  INCL_DOSMEMMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#define  INCL_DOSPROCESS
#define  INCL_DOSMODULEMGR
#define  INCL_VIO
#define  INCL_AVIO
#include <os2wrap.h>    //Odin32 OS/2 api wrappers

#include <win32type.h>
#include <win32api.h>
#include <misc.h>
#include <stdio.h>

#include "console.h"
#include "console2.h"
#include "conprop.h"

#define DBG_LOCALLOG	DBG_conprop2
#include "dbglocal.h"


ODINDEBUGCHANNEL(KERNEL32-CONPROP2)

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

/*****************************************************************************
 * Name      : iQueryModuleKeyName
 * Funktion  : determine name of key to store the console properties for
 *             this process
 * Parameter :
 * Variablen :
 * Ergebnis  :
 * Bemerkung :
 *
 * Autor     : Patrick Haller [1998/06/13 23:20]
 *****************************************************************************/

static DWORD iQueryModuleKeyName(LPSTR lpstrProcessName,
                                 DWORD dwProcessNameLength)
{
  // Note: one might want to scan the string and replace illegal characters
  // or use a module name only instead of the fully qualified path!
  return(GetModuleFileNameA(NULL,
                            lpstrProcessName,
                            dwProcessNameLength));
}


/*****************************************************************************
 * Name      : APIRET EXPENTRY ConsolePropertyDefault
 * Funktion  : load default options
 * Parameter : PICONSOLEOPTIONS pConsoleOptions
 * Variablen :
 * Ergebnis  : APIRET
 * Bemerkung :
 *
 * Autor     : Patrick Haller [1998/06/13 23:20]
 *****************************************************************************/

DWORD ConsolePropertyDefault(PICONSOLEOPTIONS pConsoleOptions)
{
  /*************************************
   * Initialize Console Window Options *
   *************************************/

  pConsoleOptions->fTerminateAutomatically = FALSE;
  pConsoleOptions->fSpeakerEnabled         = TRUE;
  pConsoleOptions->fSetWindowPosition      = FALSE;
  pConsoleOptions->coordDefaultPosition.X  = 0;
  pConsoleOptions->coordDefaultPosition.Y  = 0;
  pConsoleOptions->coordDefaultSize.X      = 80;
  pConsoleOptions->coordDefaultSize.Y      = 25;
  pConsoleOptions->fQuickInsert            = FALSE;
  pConsoleOptions->fInsertMode             = FALSE;
  pConsoleOptions->fMouseActions           = FALSE;
  pConsoleOptions->fToolbarActive          = FALSE;
  pConsoleOptions->ucDefaultAttribute      = 0x0007;  /* 07 = grey on black */
  pConsoleOptions->ulTabSize               = 8;      /* tabulator size */
  pConsoleOptions->ulUpdateLimit           = 8; /* scroll max. n lines */

                                     /* priority settings for message thread */
  pConsoleOptions->ulConsoleThreadPriorityClass = PRTYC_REGULAR;
  pConsoleOptions->ulConsoleThreadPriorityDelta = +10;

  pConsoleOptions->ucCursorDivisor = 10; /* timer divisor for blinking */

  return NO_ERROR;
}


/*****************************************************************************
 * Name      : APIRET EXPENTRY ConsolePropertyLoad
 * Funktion  : load properties from registry
 * Parameter : PICONSOLEOPTIONS pConsoleOptions
 * Variablen :
 * Ergebnis  : APIRET
 * Bemerkung :
 *
 * Autor     : Patrick Haller [1998/06/13 23:20]
 *****************************************************************************/

DWORD ConsolePropertyLoad(PICONSOLEOPTIONS pConsoleOptions)
{
  dprintf (("KERNEL32: Console:ConsolePropertyLoad(%08xh)\n",
            pConsoleOptions));

  // HKEY_CURRENT_USER/SOFTWARE/ODIN/ConsoleProperties/<process name>/<option name>
  LONG  lRes = ERROR_SUCCESS_W;
  HKEY  hkConsole;
  char  szKey[256];
  char  szProcessName[256];
  DWORD dwRes;
  DWORD dwSize;
  DWORD dwType;

  // query process's name
  dwRes = iQueryModuleKeyName((LPSTR)&szProcessName,
                              sizeof(szProcessName));
  if (dwRes == 0)
  {
    dprintf(("KERNEL32: ConProp: ConsolePropertyLoad: GetModuleFileName failed\n"));
    lRes = ERROR_INVALID_PARAMETER;
  }
  else
  {
    // open process key
    sprintf (szKey,
             "Software\\ODIN\\ConsoleProperties\\%s",
             szProcessName);

    lRes = RegOpenKeyExA(HKEY_CURRENT_USER,
                         szKey,
                         0,
                         KEY_ALL_ACCESS,
                         &hkConsole);
  }

  // try to open DEFAULT
  if (lRes != ERROR_SUCCESS_W)
    lRes = RegOpenKeyExA(HKEY_CURRENT_USER,
                         "Software\\ODIN\\ConsoleProperties\\DEFAULT",
                         0,
                         KEY_ALL_ACCESS,
                         &hkConsole);

  // now it's time to retrieve information
  if (lRes != ERROR_SUCCESS_W)
  {
    // load hardcoded defaults instead
    ConsolePropertyDefault(pConsoleOptions);
    return ERROR_INVALID_PARAMETER;
  }


  // OK, finally retrieve tokens
#define REGQUERYVALUE(name,var)              \
  dwSize = sizeof(pConsoleOptions->var);             \
  lRes = RegQueryValueExA(hkConsole, name, NULL, &dwType, \
           (LPBYTE)&pConsoleOptions->var, &dwSize);

  REGQUERYVALUE("AutomaticTermination",     fTerminateAutomatically)
  REGQUERYVALUE("Speaker",                  fSpeakerEnabled)
  REGQUERYVALUE("SpeakerDuration",          ulSpeakerDuration)
  REGQUERYVALUE("SpeakerFrequency",         ulSpeakerFrequency)
  REGQUERYVALUE("UpdateLimit",              ulUpdateLimit)
  REGQUERYVALUE("SetWindowPosition",        fSetWindowPosition)
  REGQUERYVALUE("DefaultPosition_x",        coordDefaultPosition.X)
  REGQUERYVALUE("DefaultPosition_y",        coordDefaultPosition.Y)
  REGQUERYVALUE("DefaultSize_x",            coordDefaultSize.X)
  REGQUERYVALUE("DefaultSize_y",            coordDefaultSize.Y)
  REGQUERYVALUE("BufferSize_x",             coordBufferSize.X)
  REGQUERYVALUE("BufferSize_y",             coordBufferSize.Y)
  REGQUERYVALUE("QuickInsert",              fQuickInsert)
  REGQUERYVALUE("InsertMode",               fInsertMode)
  REGQUERYVALUE("MouseActions",             fMouseActions)
  REGQUERYVALUE("Toolbar",                  fToolbarActive)
  REGQUERYVALUE("TabSize",                  ulTabSize)
  REGQUERYVALUE("DefaultAttribute",         ucDefaultAttribute)
  REGQUERYVALUE("CursorDivisor",            ucCursorDivisor)
  REGQUERYVALUE("ConsolePriorityClass",     ulConsoleThreadPriorityClass)
  REGQUERYVALUE("ConsolePriorityDelta",     ulConsoleThreadPriorityDelta)
  REGQUERYVALUE("ApplicationPriorityClass", ulAppThreadPriorityClass)
  REGQUERYVALUE("ApplicationPriorityDelta", ulAppThreadPriorityDelta)
#undef REGQUERYVALUE

  RegCloseKey(hkConsole);

  return (NO_ERROR);
}



/*****************************************************************************
 * Name      : APIRET EXPENTRY ConsolePropertySave
 * Funktion  : save properties from registry
 * Parameter : PICONSOLEOPTIONS pConsoleOptions
 * Variablen :
 * Ergebnis  : APIRET
 * Bemerkung :
 *
 * Autor     : Patrick Haller [1998/06/13 23:20]
 *****************************************************************************/

DWORD ConsolePropertySave(PICONSOLEOPTIONS pConsoleOptions)
{
  dprintf (("KERNEL32: Console:ConsolePropertySave(%08xh)\n",
            pConsoleOptions));

  // HKEY_CURRENT_USER/SOFTWARE/ODIN/ConsoleProperties/<process name>/<option name>
  LONG  lRes = ERROR_SUCCESS_W;
  HKEY  hkConsole;
  char  szKey[256];
  char  szProcessName[256];
  DWORD dwRes;
  DWORD dwSize;
  DWORD dwType;
  DWORD dwDisposition = 0;

  // query process's name
  dwRes = iQueryModuleKeyName((LPSTR)&szProcessName,
                              sizeof(szProcessName));
  if (dwRes == 0)
  {
    dprintf(("KERNEL32: ConProp: ConsolePropertyLoad: GetModuleFileName failed\n"));
    lRes = ERROR_INVALID_PARAMETER;
  }
  else
  {
    // open process key
    sprintf (szKey,
             "Software\\ODIN\\ConsoleProperties\\%s",
             szProcessName);

    lRes = RegCreateKeyExA(HKEY_CURRENT_USER,
                               szKey,
                               0,
                               "",
                               0,
                               KEY_ALL_ACCESS,
                               NULL,
                               &hkConsole,
                               &dwDisposition);
    if (lRes != ERROR_SUCCESS_W)
       return lRes;
  }


#define REGSAVEVALUE(name,var) \
  lRes = RegSetValueExA(hkConsole, name, 0, REG_DWORD, \
                 (LPBYTE)&pConsoleOptions->var, sizeof(pConsoleOptions->var));

  REGSAVEVALUE("AutomaticTermination",     fTerminateAutomatically)
  REGSAVEVALUE("Speaker",                  fSpeakerEnabled)
  REGSAVEVALUE("SpeakerDuration",          ulSpeakerDuration)
  REGSAVEVALUE("SpeakerFrequency",         ulSpeakerFrequency)
  REGSAVEVALUE("UpdateLimit",              ulUpdateLimit)
  REGSAVEVALUE("SetWindowPosition",        fSetWindowPosition)
  REGSAVEVALUE("DefaultPosition_x",        coordDefaultPosition.X)
  REGSAVEVALUE("DefaultPosition_y",        coordDefaultPosition.Y)
  REGSAVEVALUE("DefaultSize_x",            coordDefaultSize.X)
  REGSAVEVALUE("DefaultSize_y",            coordDefaultSize.Y)
  REGSAVEVALUE("BufferSize_x",             coordBufferSize.X)
  REGSAVEVALUE("BufferSize_y",             coordBufferSize.Y)
  REGSAVEVALUE("QuickInsert",              fQuickInsert)
  REGSAVEVALUE("InsertMode",               fInsertMode)
  REGSAVEVALUE("MouseActions",             fMouseActions)
  REGSAVEVALUE("Toolbar",                  fToolbarActive)
  REGSAVEVALUE("TabSize",                  ulTabSize)
  REGSAVEVALUE("DefaultAttribute",         ucDefaultAttribute)
  REGSAVEVALUE("CursorDivisor",            ucCursorDivisor)
  REGSAVEVALUE("ConsolePriorityClass",     ulConsoleThreadPriorityClass)
  REGSAVEVALUE("ConsolePriorityDelta",     ulConsoleThreadPriorityDelta)
  REGSAVEVALUE("ApplicationPriorityClass", ulAppThreadPriorityClass)
  REGSAVEVALUE("ApplicationPriorityDelta", ulAppThreadPriorityDelta)
#undef REGSAVEVALUE

  RegCloseKey(hkConsole);

  return (NO_ERROR);

}

