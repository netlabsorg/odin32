/* $Id: registry.cpp,v 1.4 1999-10-14 17:21:14 phaller Exp $ */

/*
 * Win32 registry API functions for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include "misc.h"
#include "advapi32.h"
#include "unicode.h"
#include <winreg.h>


ODINDEBUGCHANNEL(ADVAPI32-REGISTRY)


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1


/*****************************************************************************
 * Name      : Convert Key
 * Purpose   : convert key handle values between win32 and os/2
 * Parameters: HKEY winkey - the win32 key handle value
 * Variables :
 * Result    : the os/2 warp registry key handle value
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

static HKEY ConvertKey(HKEY winkey)
{
  switch((int)winkey)
  {
    case HKEY_CLASSES_ROOT:   return HKEY_CLASSES_ROOT_O32;
    case HKEY_CURRENT_USER:   return HKEY_CURRENT_USER_O32;
    case HKEY_LOCAL_MACHINE:  return HKEY_LOCAL_MACHINE_O32;
    case HKEY_USERS:          return HKEY_USERS_O32;
  }
  return(winkey);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION1(DWORD,RegCloseKey,HKEY,arg1)
{
  return _O32_RegCloseKey(ConvertKey(arg1));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(DWORD,RegCreateKeyA,HKEY,  arg1,
                                  LPCSTR,arg2,
                                  PHKEY, arg3)
{
  return _O32_RegCreateKey(ConvertKey(arg1),
                          arg2,
                          arg3);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(DWORD,RegCreateKeyW,HKEY,   arg1,
                                  LPCWSTR,arg2,
                                  PHKEY,  arg3)
{
  char *astring = UnicodeToAsciiString((LPWSTR)arg2);
  LONG  rc;

  rc = _O32_RegCreateKey(ConvertKey(arg1),
                        astring,
                        arg3);

  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION9(DWORD,RegCreateKeyExA,HKEY,                 arg1,
                                    LPCSTR,               arg2,
                                    DWORD,                arg3,
                                    LPSTR,                arg4,
                                    DWORD,                arg5,
                                    REGSAM,               arg6,
                                    LPSECURITY_ATTRIBUTES,arg7,
                                    PHKEY,                arg8,
                                    LPDWORD,              arg9)
{
  return _O32_RegCreateKeyEx(ConvertKey(arg1),
                            arg2,
                            arg3,
                            arg4,
                            arg5,
                            arg6 | KEY_READ,
                            arg7,
                            arg8,
                            arg9);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION9(DWORD,RegCreateKeyExW,HKEY,                 arg1,
                                    LPCWSTR,              arg2,
                                    DWORD,                arg3,
                                    LPWSTR,               arg4,
                                    DWORD,                arg5,
                                    REGSAM,               arg6,
                                    LPSECURITY_ATTRIBUTES,arg7,
                                    PHKEY,                arg8,
                                    LPDWORD,              arg9)
{
  char *astring1 = UnicodeToAsciiString((LPWSTR)arg2);
  char *astring2 = UnicodeToAsciiString(arg4);
  LONG  rc;

  rc = _O32_RegCreateKeyEx(ConvertKey(arg1),
                          astring1,
                          arg3,
                          astring2,
                          arg5,
                          arg6 | KEY_READ,
                          arg7,
                          arg8,
                          arg9);

  FreeAsciiString(astring1);
  FreeAsciiString(astring2);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION2(DWORD,RegDeleteKeyW,HKEY,  arg1,
                                  LPWSTR,arg2)
{
  char *astring = UnicodeToAsciiString(arg2);
  LONG  rc;

  rc = _O32_RegDeleteKey(ConvertKey(arg1),
                        astring);
  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION2(DWORD,RegDeleteKeyA,HKEY,  arg1,
                                  LPCSTR,arg2)
{
  return _O32_RegDeleteKey(ConvertKey(arg1),
                          arg2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION2(DWORD,RegDeleteValueA,HKEY, arg1,
                                    LPSTR,arg2)
{
  return _O32_RegDeleteValue(ConvertKey(arg1),
                            arg2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION2(DWORD,RegDeleteValueW,HKEY,  arg1,
                                    LPWSTR,arg2)
{
  char *astring = UnicodeToAsciiString(arg2);
  LONG  rc;

  rc = _O32_RegDeleteValue(ConvertKey(arg1),
                          astring);
  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION4(DWORD,RegEnumKeyA,HKEY, arg1,
                                DWORD,arg2,
                                LPSTR,arg3,
                                DWORD,arg4)
{
  return _O32_RegEnumKey(ConvertKey(arg1),
                        arg2,
                        arg3,
                        arg4);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION4(DWORD,RegEnumKeyW,HKEY,  arg1,
                                DWORD, arg2,
                                LPWSTR,arg3,
                                DWORD, arg4)
{
  char *astring;
  LONG  rc;

  rc = _O32_RegEnumKey(ConvertKey(arg1),
                      arg2,
                      (char *)arg3,
                      arg4);
  if(rc == ERROR_SUCCESS)
  {
    astring = (char *)malloc(arg4);
    strcpy(astring, (char *)arg3);
    AsciiToUnicode(astring, arg3);
    free(astring);
  }
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION8(DWORD,RegEnumKeyExA,HKEY,      arg1,
                                  DWORD,     arg2,
                                  LPSTR,     arg3,
                                  LPDWORD,   arg4,
                                  LPDWORD,   arg5,
                                  LPSTR,     arg6,
                                  LPDWORD,   arg7,
                                  LPFILETIME,arg8)
{
  return _O32_RegEnumKeyEx(ConvertKey(arg1),
                          arg2,
                          arg3,
                          arg4,
                          arg5,
                          arg6,
                          arg7,
                          arg8);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION8(DWORD,RegEnumKeyExW,HKEY,      arg1,
                                  DWORD,     arg2,
                                  LPWSTR,    arg3,
                                  LPDWORD,   arg4,
                                  LPDWORD,   arg5,
                                  LPWSTR,    arg6,
                                  LPDWORD,   arg7,
                                  LPFILETIME,arg8)
{
  char *astring;
  LONG  rc;

  rc = _O32_RegEnumKeyEx(ConvertKey(arg1),
                        arg2,
                        (char *)arg3,
                        arg4,
                        arg5,
                        (char *)arg6,
                        arg7,
                        arg8);
  if(rc == ERROR_SUCCESS)
  {
    astring = (char *)malloc(max(*arg4, *arg7));   //class & keyname
    strcpy(astring, (char *)arg3);
    AsciiToUnicode(astring, arg3);
    if(arg6 != NULL)
    {
      strcpy(astring, (char *)arg6);
      AsciiToUnicode(astring, arg6);
    }
    free(astring);
  }
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION8(DWORD,RegEnumValueA,HKEY,   arg1,
                                  DWORD,  arg2,
                                  LPSTR,  arg3,
                                  LPDWORD,arg4,
                                  LPDWORD,arg5,
                                  LPDWORD,arg6,
                                  LPBYTE, arg7,
                                  LPDWORD,arg8)
{
  return _O32_RegEnumValue(ConvertKey(arg1),
                          arg2,
                          arg3,
                          arg4,
                          arg5,
                          arg6,
                          arg7,
                          arg8);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION8(DWORD,RegEnumValueW,HKEY,   arg1,
                                  DWORD,  arg2,
                                  LPWSTR, arg3,
                                  LPDWORD,arg4,
                                  LPDWORD,arg5,
                                  LPDWORD,arg6,
                                  LPBYTE, arg7,
                                  LPDWORD,arg8)
{
  char *astring;
  LONG  rc;

  rc = _O32_RegEnumValue(ConvertKey(arg1),
                        arg2,
                        (char *)arg3,
                        arg4,
                        arg5,
                        arg6,
                        arg7,
                        arg8);
  if(rc == ERROR_SUCCESS)
  {
    astring = (char *)malloc(*arg4);
    strcpy(astring, (char *)arg3);
    AsciiToUnicode(astring, arg3);
    free(astring);
  }
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(DWORD,RegOpenKeyA,HKEY,  arg1,
                                LPCSTR,arg2,
                                PHKEY, arg3)
{
  LONG rc;

  rc = _O32_RegOpenKey(ConvertKey(arg1),
                      arg2,
                      arg3);
  if(rc)
    *arg3 = 0;

  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(DWORD,RegOpenKeyW,HKEY,   arg1,
                                LPCWSTR,arg2,
                                PHKEY,  arg3)
{
  char *astring = UnicodeToAsciiString((LPWSTR)arg2);
  LONG  rc;

  rc = _O32_RegOpenKey(ConvertKey(arg1),
                      astring,
                      arg3);
  if(rc)
    *arg3 = 0;

  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION5(DWORD,RegOpenKeyExA,HKEY,  arg1,
                                  LPCSTR,arg2,
                                  DWORD, arg3,
                                  REGSAM,arg4,
                                  PHKEY, arg5)
{
  LONG rc;

  rc = _O32_RegOpenKeyEx(ConvertKey(arg1),
                        arg2,
                        arg3,
                        arg4,
                        arg5);

  //SvL: This fixes crashes in pmwinx.dll. (if an app doesn't check the
  //     return value and uses the whatever *arg5 contains)
  if(rc)
    *arg5 = 0;

  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION5(DWORD,RegOpenKeyExW,HKEY,   arg1,
                                  LPCWSTR,arg2,
                                  DWORD,  arg3,
                                  REGSAM, arg4,
                                  PHKEY,  arg5)
{
  char *astring = UnicodeToAsciiString((LPWSTR)arg2);
  LONG  rc;

  rc = _O32_RegOpenKeyEx(ConvertKey(arg1),
                        astring,
                        arg3,
                        arg4,
                        arg5);
  //SvL: This fixes crashes in pmwinx.dll. (if an app doesn't check the
  //     return value and uses the whatever *arg5 contains)
  if(rc)
    *arg5 = 0;

  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION12(DWORD,RegQueryInfoKeyA,HKEY,       arg1,
                                      LPSTR,      arg2,
                                      LPDWORD,    arg3,
                                      LPDWORD,    arg4,
                                      LPDWORD,    arg5,
                                      LPDWORD,    arg6,
                                      LPDWORD,    arg7,
                                      LPDWORD,    arg8,
                                      LPDWORD,    arg9,
                                      LPDWORD,    arg10,
                                      LPDWORD,    arg11,
                                      LPFILETIME, arg12)
{
  return _O32_RegQueryInfoKey(ConvertKey(arg1),
                             arg2,
                             arg3,
                             arg4,
                             arg5,
                             arg6,
                             arg7,
                             arg8,
                             arg9,
                             arg10,
                             arg11,
                             arg12);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION12(DWORD,RegQueryInfoKeyW,HKEY,       arg1,
                                      LPWSTR,     arg2,
                                      LPDWORD,    arg3,
                                      LPDWORD,    arg4,
                                      LPDWORD,    arg5,
                                      LPDWORD,    arg6,
                                      LPDWORD,    arg7,
                                      LPDWORD,    arg8,
                                      LPDWORD,    arg9,
                                      LPDWORD,    arg10,
                                      LPDWORD,    arg11,
                                      LPFILETIME, arg12)
{
  char *astring;
  LONG  rc;

  rc = _O32_RegQueryInfoKey(ConvertKey(arg1),
                           (char *)arg2,
                           arg3,
                           arg4,
                           arg5,
                           arg6,
                           arg7,
                           arg8,
                           arg9,
                           arg10,
                           arg11,
                           arg12);
  if(rc == ERROR_SUCCESS)
  {
    astring = (char *)malloc(*arg3);
    strcpy(astring, (char *)arg2);
    AsciiToUnicode(astring, arg2);
    free(astring);
  }
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION4(DWORD,RegQueryValueA,HKEY,  arg1,
                                   LPCSTR,arg2,
                                   LPSTR, arg3,
                                   PLONG, arg4)
{
  dprintf(("ADVAPI32:Registry key=%s\n",
           arg2));
  return _O32_RegQueryValue(ConvertKey(arg1),
                           arg2,
                           arg3,
                           arg4);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION4(DWORD,RegQueryValueW,HKEY,   hkey,
                                   LPCWSTR,lpszSubKey,
                                   LPWSTR, lpszValue,
                                   PLONG,  pcbValue)
{
  char *astring1 = UnicodeToAsciiString((LPWSTR)lpszSubKey);
  char *astring2;
  LONG  rc;

  rc = _O32_RegQueryValue(ConvertKey(hkey),
                         astring1,
                         (char *)lpszValue,
                         pcbValue);
  if(rc == ERROR_SUCCESS)
  {
    astring2 = (char *)malloc(*pcbValue);
    strcpy(astring2, (char *)lpszValue);
    AsciiToUnicode(astring2, lpszValue);
    free(astring2);
  }
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION6(LONG,RegQueryValueExA,HKEY,   arg1,
                                    LPSTR,  arg2,
                                    LPDWORD,arg3,
                                    LPDWORD,arg4,
                                    LPBYTE, arg5,
                                    LPDWORD,arg6)
{
  dprintf(("ADVAPI32:Registry key=%s\n",
           arg2));
  return _O32_RegQueryValueEx(ConvertKey(arg1),
                             arg2,
                             arg3,
                             arg4,
                             arg5,
                             arg6);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : TODO: DOESN'T WORK FOR STRING DATA!!
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION6(LONG,RegQueryValueExW,HKEY,   arg1,
                                    LPWSTR, arg2,
                                    LPDWORD,arg3,
                                    LPDWORD,arg4,
                                    LPBYTE, arg5,
                                    LPDWORD,arg6)
{
  char *astring = UnicodeToAsciiString(arg2);
  LONG  rc;

  rc = _O32_RegQueryValueEx(ConvertKey(arg1),
                           astring,
                           arg3,
                           arg4,
                           arg5,
                           arg6);
  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION5(DWORD,RegSetValueA,HKEY,  hkey,
                                 LPCSTR,lpSubKey,
                                 DWORD, dwType,
                                 LPCSTR,lpData,
                                 DWORD, cbData)
{
  //SvL: 8-11-'97: Bugfix: crash in pmwinx if size == 0 and string is large
  if(cbData == 0)
    cbData = strlen(lpData);

  return(_O32_RegSetValue(ConvertKey(hkey),
                         lpSubKey,
                         dwType,
                         lpData,
                         cbData));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION5(DWORD,RegSetValueW,HKEY,   hkey,
                                 LPCWSTR,lpSubKey,
                                 DWORD,  dwType,
                                 LPCWSTR,lpData,
                                 DWORD,  cbData)
{
  char *astring1 = UnicodeToAsciiString((LPWSTR)lpSubKey);
  char *astring2 = UnicodeToAsciiString((LPWSTR)lpData);
  LONG  rc;

  //SvL: 8-11-'97: Bugfix: crash in pmwinx if size == 0 and string is large
  if(cbData == 0)
    cbData = strlen(astring2);

  rc = _O32_RegSetValue(ConvertKey(hkey),
                       astring1,
                       dwType,
                       astring2,
                       cbData);

  FreeAsciiString(astring1);
  FreeAsciiString(astring2);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : TODO:Check for string length here too?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION6(DWORD,RegSetValueExA,HKEY,  arg1,
                                   LPSTR, arg2,
                                   DWORD, arg3,
                                   DWORD, arg4,
                                   BYTE*, arg5,
                                   DWORD, arg6)
{
  return _O32_RegSetValueEx(ConvertKey(arg1),
                           arg2,
                           arg3,
                           arg4,
                           arg5,
                           arg6);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : TODO:Check for string length here too?
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION6(DWORD,RegSetValueExW,HKEY,  arg1,
                                   LPWSTR,arg2,
                                   DWORD, arg3,
                                   DWORD, arg4,
                                   BYTE*, arg5,
                                   DWORD, arg6)
{
  char *astring = UnicodeToAsciiString(arg2);
  LONG  rc;

  dprintf(("ADVAPI32: RegSetValueExW(%08xh,%s,%08xh,%08xh,%08xh,%08xh)\n",
           arg1,
           astring,
           arg3,
           arg4,
           arg5,
           arg6));

  rc = _O32_RegSetValueEx(ConvertKey(arg1),
                         astring,
                         arg3,
                         arg4,
                         arg5,
                         arg6);
  FreeAsciiString(astring);
  return(rc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION1(DWORD,RegFlushKey,HKEY,hkey)
{
  dprintf(("ADVAPI32: RegFlushKey not implemented yet."));

  return(ERROR_SUCCESS);
}


/*****************************************************************************
 * Name      : RegConnectRegistryA
 * Purpose   : The RegConnectRegistry function establishes a connection to a
 *             predefined registry handle on another computer.
 * Parameters: LPTSTR lpszComputerName address of name of remote computer
 *             HKEY   hKey             predefined registry handle
 *             PHKEY  phkResult        address of buffer for remote registry handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegConnectRegistryA,LPCSTR,lpszComputerName,
                                       HKEY,  hKey,
                                       PHKEY, phkResult)
{
  char szLocalName[256];
  DWORD dwNameLength = sizeof(szLocalName)-2;

  szLocalName[0] = '\\';
  szLocalName[1] = '\\';
  GetComputerNameA(szLocalName+2, &dwNameLength);

  dprintf(("ADVAPI32: RegConnectRegistryA(%s,local %s) not implemented yet.\n",
           lpszComputerName,
           szLocalName));

  /* local registry ? */
  if ( (                    lpszComputerName  == NULL) ||
       (strcmp(szLocalName, lpszComputerName) == 0   ) )
  {
    /* @@@PH experimental !!! */
    *phkResult = hKey;

    return (NO_ERROR);
  }

  return (ERROR_ACCESS_DENIED);
}


/*****************************************************************************
 * Name      : RegConnectRegistryW
 * Purpose   : The RegConnectRegistry function establishes a connection to a
 *             predefined registry handle on another computer.
 * Parameters: LPWSTR lpszComputerName address of name of remote computer
 *             HKEY   hKey             predefined registry handle
 *             PHKEY  phkResult        address of buffer for remote registry handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegConnectRegistryW,LPCWSTR,lpszComputerName,
                                       HKEY,   hKey,
                                       PHKEY,  phkResult)
{
                                               /* corresponding ascii string */
  LPSTR pszAscii;
  LONG rc;         /* returncode from call to ascii version of this function */

  dprintf(("ADVAPI32: RegConnectRegistryW not implemented yet."));

  if (lpszComputerName != NULL)
    pszAscii = UnicodeToAsciiString((LPWSTR)lpszComputerName);
  else
    pszAscii = NULL;

  rc = RegConnectRegistryA(pszAscii,
                           hKey,
                           phkResult);

  if (pszAscii != NULL)
    FreeAsciiString(pszAscii);

  return (rc);                                                         /* OK */
}


/*****************************************************************************
 * Name      : RegGetKeySecurity
 * Purpose   : The RegGetKeySecurity function retrieves a copy of the security
 *             descriptor protecting the specified open registry key.
 * Parameters: HKEY                 hKey        open handle of key to set
 *             SECURITY_INFORMATION SecInf      descriptor contents
 *             PSECURITY_DESCRIPTOR pSecDesc    address of descriptor for key
 *             LPDWORD              lpcbSecDesc address of size of buffer and descriptor
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION4(LONG,RegGetKeySecurity,HKEY,                 hKey,
                                     SECURITY_INFORMATION, SecInf,
                                     PSECURITY_DESCRIPTOR, pSecDesc,
                                     LPDWORD,              lpcbSecDesc)
{
  dprintf(("ADVAPI32: RegGetKeySecurity not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegLoadKeyA
 * Purpose   : The RegLoadKey function creates a subkey under HKEY_USER or
 *             HKEY_LOCAL_MACHINE and stores registration information from a
 *             specified file into that subkey. This registration information
 *             is in the form of a hive. A hive is a discrete body of keys,
 *             subkeys, and values that is rooted at the top of the registry
 *             hierarchy. A hive is backed by a single file and .LOG file.
 * Parameters: HKEY    hKey       handle of open key
 *             LPCSTR lpszSubKey address of name of subkey
 *             LPCSTR lpszFile   address of filename for registry information
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegLoadKeyA,HKEY,  hKey,
                               LPCSTR,lpszSubKey,
                               LPCSTR,lpszFile)
{
  dprintf(("ADVAPI32: RegLoadKeyA not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegLoadKeyW
 * Purpose   : The RegLoadKey function creates a subkey under HKEY_USER or
 *             HKEY_LOCAL_MACHINE and stores registration information from a
 *             specified file into that subkey. This registration information
 *             is in the form of a hive. A hive is a discrete body of keys,
 *             subkeys, and values that is rooted at the top of the registry
 *             hierarchy. A hive is backed by a single file and .LOG file.
 * Parameters: HKEY    hKey       handle of open key
 *             LPCWSTR lpszSubKey address of name of subkey
 *             LPCWSTR lpszFile   address of filename for registry information
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegLoadKeyW,HKEY,   hKey,
                               LPCWSTR,lpszSubKey,
                               LPCWSTR,lpszFile)
{
  dprintf(("ADVAPI32: RegLoadKeyW not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegQueryMultipleValuesA
 * Purpose   : The RegQueryMultipleValues function retrieves the type and data
 *             for a list of value names associated with an open registry key.
 * Parameters: HKEY    hKey        handle of key to query
 *             PVALENT val_list    address of array of value entry structures
 *             DWORD   num_vals    size of array of value entry structures
 *             LPTSTR  lpValueBuf  address of buffer for value information
 *             LPDWORD ldwTotsize  address of size of value buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION5(LONG,RegQueryMultipleValuesA,HKEY,    hKey,
                                           PVALENTA,val_list,
                                           DWORD,   num_vals,
                                           LPTSTR,  lpValueBuf,
                                           LPDWORD, ldwTotsize)
{
  dprintf(("ADVAPI32: RegQueryMultipleValuesA not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegQueryMultipleValuesW
 * Purpose   : The RegQueryMultipleValues function retrieves the type and data
 *             for a list of value names associated with an open registry key.
 * Parameters: HKEY    hKey        handle of key to query
 *             PVALENT val_list    address of array of value entry structures
 *             DWORD   num_vals    size of array of value entry structures
 *             LPWSTR  lpValueBuf  address of buffer for value information
 *             LPDWORD ldwTotsize  address of size of value buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION5(LONG,RegQueryMultipleValuesW,HKEY,    hKey,
                                           PVALENTW,val_list,
                                           DWORD,   num_vals,
                                           LPWSTR,  lpValueBuf,
                                           LPDWORD, ldwTotsize)
{
  dprintf(("ADVAPI32: RegQueryMultipleValuesW not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegRemapPreDefKey
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#if 0
ODINFUNCTION1(LONG,RegRemapPreDefKey,HKEY,hKey)
{
  dprintf(("ADVAPI32: RegRemapPreDefKey not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}
#endif


/*****************************************************************************
 * Name      : RegReplaceKeyA
 * Purpose   : The RegReplaceKey function replaces the file backing a key and
 *             all its subkeys with another file, so that when the system is
 *             next started, the key and subkeys will have the values stored in the new file.
 * Parameters: HKEY    hKey      handle of open key
 *             LPCSTR lpSubKey  address of name of subkey
 *             LPCSTR lpNewFile address of filename for file with new data
 *             LPCSTR lpOldFile address of filename for backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION4(LONG,RegReplaceKeyA,HKEY,   hKey,
                                  LPCSTR,lpSubKey,
                                  LPCSTR,lpNewFile,
                                  LPCSTR,lpOldFile)
{
  dprintf(("ADVAPI32: RegReplaceKeyA not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegReplaceKeyW
 * Purpose   : The RegReplaceKey function replaces the file backing a key and
 *             all its subkeys with another file, so that when the system is
 *             next started, the key and subkeys will have the values stored in the new file.
 * Parameters: HKEY    hKey      handle of open key
 *             LPCWSTR lpSubKey  address of name of subkey
 *             LPCWSTR lpNewFile address of filename for file with new data
 *             LPCWSTR lpOldFile address of filename for backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION4(LONG,RegReplaceKeyW,HKEY,   hKey,
                                  LPCWSTR,lpSubKey,
                                  LPCWSTR,lpNewFile,
                                  LPCWSTR,lpOldFile)
{
  dprintf(("ADVAPI32: RegReplaceKeyW not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegRestoreKeyA
 * Purpose   : The RegRestoreKey function reads the registry information in a
 *             specified file and copies it over the specified key. This
 *             registry information may be in the form of a key and multiple
 *             levels of subkeys.
 * Parameters: HKEY    hKey      handle of key where restore begins
 *             LPCSTR lpszFile  address of filename containing saved tree
 *             DWORD   fdw       optional flags
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegRestoreKeyA,HKEY,   hKey,
                                  LPCSTR,lpszFile,
                                  DWORD,  fdw)
{
  dprintf(("ADVAPI32: RegRestoreKeyA not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegRestoreKeyW
 * Purpose   : The RegRestoreKey function reads the registry information in a
 *             specified file and copies it over the specified key. This
 *             registry information may be in the form of a key and multiple
 *             levels of subkeys.
 * Parameters: HKEY    hKey      handle of key where restore begins
 *             LPCWSTR lpszFile  address of filename containing saved tree
 *             DWORD   fdw       optional flags
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegRestoreKeyW,HKEY,    hKey,
                                  LPCWSTR, lpszFile,
                                  DWORD,   fdw)
{
  dprintf(("ADVAPI32: RegRestoreKeyW not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegSaveKeyA
 * Purpose   : The RegSaveKey function saves the specified key and all of its
 *             subkeys and values to a new file.
 * Parameters: HKEY                  hKey      handle of key where save begins
 *             LPCSTR               lpszFile  address of filename to save to
 *             LPSECURITY_ATTRIBUTES lpsa      address of security structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegSaveKeyA,HKEY,                 hKey,
                               LPCSTR,               lpszFile,
                               LPSECURITY_ATTRIBUTES,lpsa)
{
  dprintf(("ADVAPI32: RegSaveKeyA not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegSaveKeyW
 * Purpose   : The RegSaveKey function saves the specified key and all of its
 *             subkeys and values to a new file.
 * Parameters: HKEY                  hKey      handle of key where save begins
 *             LPCWSTR               lpszFile  address of filename to save to
 *             LPSECURITY_ATTRIBUTES lpsa      address of security structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegSaveKeyW,HKEY,                 hKey,
                               LPCWSTR,              lpszFile,
                               LPSECURITY_ATTRIBUTES,lpsa)
{
  dprintf(("ADVAPI32: RegSaveKeyW not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegSetKeySecurity
 * Purpose   : The RegSetKeySecurity function sets the security of an open registry key.
 * Parameters: HKEY                 hKey open handle of key to set
 *             SECURITY_INFORMATION si   descriptor contents
 *             PSECURITY_DESCRIPTOR psd  address of descriptor for key
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION3(LONG,RegSetKeySecurity,HKEY,                hKey,
                                     SECURITY_INFORMATION,si,
                                     PSECURITY_DESCRIPTOR,psd)
{
  dprintf(("ADVAPI32: RegSetKeySecurity not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegUnLoadKeyA
 * Purpose   : The RegUnLoadKey function unloads the specified key and subkeys from the registry.
 * Parameters: HKEY    hKey       handle of open key
 *             LPCSTR lpszSubKey address of name of subkey to unload
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION2(LONG,RegUnLoadKeyA,HKEY,    hKey,
                                 LPCSTR, lpszSubKey)
{
  dprintf(("ADVAPI32: RegUnLoadKeyA not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegUnLoadKeyW
 * Purpose   : The RegUnLoadKey function unloads the specified key and subkeys from the registry.
 * Parameters: HKEY    hKey       handle of open key
 *             LPCWSTR lpszSubKey address of name of subkey to unload
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION2(LONG,RegUnLoadKeyW,HKEY,    hKey,
                                 LPCWSTR, lpszSubKey)
{
  dprintf(("ADVAPI32: RegUnLoadKeyW not implemented.\n"));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegNotifyChangeKeyValue
 * Purpose   :
 * Parameters: HKEY    hKey       handle of open key
 *             LPCWSTR lpszSubKey address of name of subkey to unload
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION5(LONG,RegNotifyChangeKeyValue,HKEY,  hKey,
                                           BOOL,  bWatchSubtree,
                                           DWORD, dwNotifyFilter,
                                           HANDLE,hEvent,
                                           BOOL,  fAsynchronus)
{
   dprintf(("ADVAPI32: RegNotifyChangeKeyValue() not implemented.\n"));

   return ERROR_ACCESS_DENIED;
}

