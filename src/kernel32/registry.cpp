/* $Id: registry.cpp,v 1.21 2003-04-02 12:58:30 sandervl Exp $ */

/*
 * Win32 registry API functions for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 *
 * TODO: Many unicode apis are still wrong (missing unicode conversions and wrong sizes)!!!!!
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
#include <misc.h>
#include "unicode.h"
#include <winreg.h>
#include <heapstring.h>

#define DBG_LOCALLOG    DBG_registry
#include "dbglocal.h"


ODINDEBUGCHANNEL(ADVAPI32-REGISTRY)


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1


static HKEY hKeyClassesRoot  = HKEY_CLASSES_ROOT_O32;
static HKEY hKeyCurrentUser  = HKEY_CURRENT_USER_O32;
static HKEY hKeyLocalMachine = HKEY_LOCAL_MACHINE_O32;
static HKEY hKeyUsers        = HKEY_USERS_O32;

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
  switch((DWORD)winkey)
  {
    case HKEY_CLASSES_ROOT:   return hKeyClassesRoot;
    case HKEY_CURRENT_USER:   return hKeyCurrentUser;
    case HKEY_LOCAL_MACHINE:  return hKeyLocalMachine;
    case HKEY_USERS:          return hKeyUsers;
  }
  return(winkey);
}

void WIN32API SetRegistryRootKey(HKEY hRootkey, HKEY hKey)
{
  switch((DWORD)hRootkey)
  {
    case HKEY_CLASSES_ROOT:   
        hKeyClassesRoot = hKey;
        break;
    case HKEY_CURRENT_USER:
        hKeyCurrentUser = hKey;
        break;
    case HKEY_LOCAL_MACHINE:
        hKeyLocalMachine = hKey;
        break;
    case HKEY_USERS:
        hKeyUsers = hKey;
        break;
  }
}

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegCloseKey(HKEY hKey)
{
    switch(hKey) 
    {//Closing a root key should just return success (matters for custom builds)
    case HKEY_CLASSES_ROOT:
    case HKEY_CURRENT_USER:
    case HKEY_LOCAL_MACHINE:
    case HKEY_USERS:
        return 0;
    }
    return O32_RegCloseKey(ConvertKey(hKey));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegCreateKeyA(HKEY hKey, LPCSTR lpszSubKey, PHKEY phkResult)
{
  dprintf(("RegCreateKeyA %x %s", hKey, lpszSubKey));
  return O32_RegCreateKey(ConvertKey(hKey),
                          lpszSubKey,
                          phkResult);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegCreateKeyW(HKEY hKey, LPCWSTR lpszSubKey, PHKEY phkResult)
{
  char *astring = UnicodeToAsciiString((LPWSTR)lpszSubKey);
  LONG  rc;

  dprintf(("RegCreateKeyW %x %s", hKey, astring));
  rc = O32_RegCreateKey(ConvertKey(hKey),
                        astring,
                        phkResult);
  
  if (NULL != astring)
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
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegCreateKeyExA(HKEY                  hKey,
                              LPCSTR                lpszSubKey,
                              DWORD                 dwReserved,
                              LPSTR                 lpszClass,
                              DWORD                 fdwOptions,
                              REGSAM                samDesired,
                              LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                              PHKEY                 phkResult,
                              LPDWORD               lpdwDisposition)
{
  dprintf(("RegCreateKeyExA %x %s", hKey, lpszSubKey));

  return O32_RegCreateKeyEx(ConvertKey(hKey),
                             lpszSubKey,
                             dwReserved,
                             lpszClass,
                             fdwOptions,
                             samDesired, //  | KEY_READ
                             lpSecurityAttributes,
                             phkResult,
                             lpdwDisposition);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegCreateKeyExW(HKEY                  hKey,
                              LPCWSTR               lpszSubKey,
                              DWORD                 dwReserved,
                              LPWSTR                lpszClass,
                              DWORD                 fdwOptions,
                              REGSAM                samDesired,
                              LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                              PHKEY                 phkResult,
                              LPDWORD               lpdwDisposition)
{
  char *astring1 = UnicodeToAsciiString((LPWSTR)lpszSubKey);
  char *astring2 = UnicodeToAsciiString(lpszClass);
  LONG  rc;

  dprintf(("RegCreateKeyExW %x %s", hKey, astring1));

  rc = O32_RegCreateKeyEx(ConvertKey(hKey),
                          astring1,
                          dwReserved,
                          astring2,
                          fdwOptions,
                          samDesired, //  | KEY_READ
                          lpSecurityAttributes,
                          phkResult,
                          lpdwDisposition);
  
  if (NULL != astring1)
    FreeAsciiString(astring1);
  
  if (NULL != astring2)
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
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegDeleteKeyW(HKEY hKey, LPCWSTR lpszSubKey)
{
  char *astring = UnicodeToAsciiString(lpszSubKey);
  LONG  rc;

  dprintf(("RegDeleteKeyW %s", astring));
  rc = O32_RegDeleteKey(ConvertKey(hKey),
                        astring);
  
  if (NULL != astring)
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
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegDeleteKeyA(HKEY hKey, LPCSTR lpszSubKey)
{
  dprintf(("RegDeleteKeyW %s", lpszSubKey));
  return O32_RegDeleteKey(ConvertKey(hKey), lpszSubKey);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegDeleteValueA(HKEY hKey, LPSTR lpszValue)
{
  return O32_RegDeleteValue(ConvertKey(hKey),
                            lpszValue);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : CORRECTED UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LONG WIN32API RegDeleteValueW(HKEY hKey, LPWSTR lpszValue)
{
  char *astring = UnicodeToAsciiString(lpszValue);
  LONG  rc;

  rc = O32_RegDeleteValue(ConvertKey(hKey),
                          astring);
  
  if (NULL != astring)
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

LONG WIN32API RegEnumKeyA(HKEY hKey, DWORD iSubKey, LPSTR lpszName,
                          DWORD cchName)
{
  return O32_RegEnumKey(ConvertKey(hKey),
                        iSubKey,
                        lpszName,
                        cchName);
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

LONG WIN32API RegEnumKeyW(HKEY  hKey, DWORD  iSubKey, LPWSTR lpszName,                          
                          DWORD cchName)
{
  char *astring;
  LONG  rc;

  rc = O32_RegEnumKey(ConvertKey(hKey),
                      iSubKey,
                      (char *)lpszName,
                      cchName);
  if(rc == ERROR_SUCCESS)
  {
    astring = (char *)malloc(cchName);
    strcpy(astring, (char *)lpszName);
    AsciiToUnicode(astring, lpszName);
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

LONG WIN32API RegEnumKeyExA(HKEY       arg1,
                            DWORD      arg2,
                            LPSTR      arg3,
                            LPDWORD    arg4,
                            LPDWORD    arg5,
                            LPSTR      arg6,
                            LPDWORD    arg7,
                            LPFILETIME arg8)
{
  return O32_RegEnumKeyEx(ConvertKey(arg1),
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

LONG WIN32API RegEnumKeyExW(HKEY       hKey,
                            DWORD      iSubkey,
                            LPWSTR     lpszName,
                            LPDWORD    lpcchName,
                            LPDWORD    lpdwReserved,
                            LPWSTR     lpszClass,
                            LPDWORD    lpcchClass,
                            LPFILETIME lpffLastWrite)
{
  char *astring;
  LONG  rc;

  rc = O32_RegEnumKeyEx(ConvertKey(hKey),
                        iSubkey,
                        (char *)lpszName,
                        lpcchName,
                        lpdwReserved,
                        (char *)lpszClass,
                        lpcchClass,
                        lpffLastWrite);
  if(rc == ERROR_SUCCESS)
  {
    astring = (char *)malloc(max(*lpcchName+1, (lpcchClass) ? (*lpcchClass+1) : 0));   //class & keyname
    strcpy(astring, (char *)lpszName);
    AsciiToUnicode(astring, lpszName);
    if(lpszClass != NULL)
    {
      strcpy(astring, (char *)lpszClass);
      AsciiToUnicode(astring, lpszClass);
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

LONG WIN32API RegEnumValueA(HKEY    arg1,
                            DWORD   arg2,
                            LPSTR   arg3,
                            LPDWORD arg4,
                            LPDWORD arg5,
                            LPDWORD arg6,
                            LPBYTE  arg7,
                            LPDWORD arg8)
{
  return O32_RegEnumValue(ConvertKey(arg1),
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
 
LONG WIN32API RegEnumValueW(HKEY    hkey,
                            DWORD   iValue,
                            LPWSTR  lpszValue,
                            LPDWORD lpcchValue,
                            LPDWORD lpdwReserved,
                            LPDWORD lpdwType,
                            LPBYTE  lpbData,
                            LPDWORD lpcbData)
{
  char *astring;
  LONG  rc, oldsize = 0;

  if(lpcbData) {
    oldsize = *lpcbData;
  }
  rc = O32_RegEnumValue(ConvertKey(hkey),
                        iValue,
                        (char *)lpszValue,
                        lpcchValue,
                        lpdwReserved,
                        lpdwType,
                        lpbData,
                        lpcbData);
  if(rc == ERROR_SUCCESS)
  {
    astring = (char *)malloc(*lpcchValue+1); //+ 0 terminator
    strcpy(astring, (char *)lpszValue);
    AsciiToUnicode(astring, lpszValue);
    free(astring);

    if(lpcbData && lpbData)
    {
        switch(*lpdwType) {
        case REG_SZ:
        case REG_EXPAND_SZ:
            if(oldsize < *lpcbData*sizeof(WCHAR)) {
                *lpcbData = *lpcbData*sizeof(WCHAR);
                return ERROR_MORE_DATA;
            }
            astring = (char *)malloc(*lpcbData);
            strcpy(astring, (char *)lpbData);
            lstrcpyAtoW((LPWSTR)lpbData, astring);
            free(astring);
            break;
        case REG_MULTI_SZ:
        case REG_LINK: //???
            dprintf(("ERROR: key data must be translated from Unicode to Ascii!!"));
            break;
        default:
            break;
        }
    }

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

LONG WIN32API RegOpenKeyA(HKEY hKey, LPCSTR arg2, PHKEY arg3)
{
  LONG rc;

  dprintf(("RegOpenKey %s", arg2));

  rc = O32_RegOpenKey(ConvertKey(hKey),
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

LONG WIN32API RegOpenKeyW(HKEY hKey, LPCWSTR arg2, PHKEY arg3)
{
  char *astring = UnicodeToAsciiString((LPWSTR)arg2);
  LONG  rc;

  rc = O32_RegOpenKey(ConvertKey(hKey),
                      astring,
                      arg3);
  if(rc)
    *arg3 = 0;
  
  if (NULL != astring)
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

LONG WIN32API RegOpenKeyExA(HKEY hKey, LPCSTR arg2, DWORD arg3,
                            REGSAM arg4, PHKEY arg5)
{
  LONG rc;

  dprintf(("RegOpenKeyEx %s", arg2));
  rc = O32_RegOpenKeyEx(ConvertKey(hKey),
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

LONG WIN32API RegOpenKeyExW(HKEY    arg1,
                            LPCWSTR arg2,
                            DWORD   arg3,
                            REGSAM  arg4,
                            PHKEY   arg5)
{
  char *astring = UnicodeToAsciiString((LPWSTR)arg2);
  LONG  rc;

  rc = RegOpenKeyExA(arg1, astring, arg3, arg4, arg5);
  //SvL: This fixes crashes in pmwinx.dll. (if an app doesn't check the
  //     return value and uses the whatever *arg5 contains)
  if(rc)
    *arg5 = 0;
  
  if (NULL != astring)
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

LONG WIN32API RegQueryInfoKeyA(HKEY        hkey,
                               LPSTR       lpszClass,
                               LPDWORD     lpcchClass,
                               LPDWORD     lpdwReserved,
                               LPDWORD     lpcSubKeys,
                               LPDWORD     lpcchMaxSubKey,
                               LPDWORD     lpcchMaxClass,
                               LPDWORD     lpcValues,
                               LPDWORD     lpcchMaxValueName,
                               LPDWORD     lpcbMaxValueData,
                               LPDWORD     lpcbSecurityDescriptor,
                               LPFILETIME  lpftLastWriteTime)
{
    return O32_RegQueryInfoKey(ConvertKey(hkey), lpszClass,
                               lpcchClass, lpdwReserved, lpcSubKeys,
                               lpcchMaxSubKey, lpcchMaxClass, lpcValues,
                               lpcchMaxValueName, lpcbMaxValueData, 
                               lpcbSecurityDescriptor,lpftLastWriteTime);
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

LONG WIN32API RegQueryInfoKeyW(HKEY        hkey,
                               LPWSTR      lpszClass,
                               LPDWORD     lpcchClass,
                               LPDWORD     lpdwReserved,
                               LPDWORD     lpcSubKeys,
                               LPDWORD     lpcchMaxSubKey,
                               LPDWORD     lpcchMaxClass,
                               LPDWORD     lpcValues,
                               LPDWORD     lpcchMaxValueName,
                               LPDWORD     lpcbMaxValueData,
                               LPDWORD     lpcbSecurityDescriptor,
                               LPFILETIME  lpftLastWriteTime)
{
    LONG  rc;

    rc = O32_RegQueryInfoKey(ConvertKey(hkey), (char *)lpszClass,
                             lpcchClass, lpdwReserved, lpcSubKeys,
                             lpcchMaxSubKey, lpcchMaxClass, lpcValues,
                             lpcchMaxValueName, lpcbMaxValueData, 
                             lpcbSecurityDescriptor,lpftLastWriteTime);
    if(rc == ERROR_SUCCESS)
    {
        if(lpcchClass && *lpcchClass) {
            char *astring = (char *)malloc(*lpcchClass+1); //returned length does NOT include 0 terminator
            strcpy(astring, (char *)lpszClass);
            AsciiToUnicode(astring, lpszClass);
            free(astring);
        }
        else 
        if(lpszClass) *lpszClass = 0;
    }
    //TODO: lpcbMaxValueData could be wrong for string key values!!! (as it's in bytes)
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

LONG WIN32API RegQueryValueA(HKEY  arg1,
                             LPCSTR arg2,
                             LPSTR  arg3,
                             PLONG  arg4)
{
  dprintf(("ADVAPI32:Registry key=%s\n",
           arg2));
  return O32_RegQueryValue(ConvertKey(arg1),
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

LONG WIN32API RegQueryValueW(HKEY    hkey,
                             LPCWSTR lpszSubKey,
                             LPWSTR  lpszValue,
                             PLONG   pcbValue)
{
  char *astring1 = UnicodeToAsciiString((LPWSTR)lpszSubKey);
  char *astring2;
  LONG  rc;

  rc = RegQueryValueA(hkey, astring1, (char *)lpszValue, pcbValue);
  if(rc == ERROR_SUCCESS)
  {
    if(pcbValue) {
            astring2 = (char *)malloc(*pcbValue);   //includes 0 terminator
            strcpy(astring2, (char *)lpszValue);
            AsciiToUnicode(astring2, lpszValue);
            free(astring2);
    }
  }

  if((rc == ERROR_SUCCESS || rc == ERROR_MORE_DATA) && 
     pcbValue) 
  {
      *pcbValue = *pcbValue * sizeof(WCHAR); //size in bytes!!
  }
  
  if (NULL != astring1)
    FreeAsciiString(astring1);
  
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

LONG WIN32API RegQueryValueExA(HKEY   hkey,
                               LPCSTR   lpszValueName,
                               LPDWORD lpdwReserved,
                               LPDWORD lpdwType,
                               LPBYTE  lpbData,
                               LPDWORD lpcbData)
{
  LONG ret;
  DWORD dwType = 0;

  dprintf(("ADVAPI32:Registry key=%s", lpszValueName));

  if(lpdwType == NULL) {
      lpdwType = &dwType;
  }
  ret = O32_RegQueryValueEx(ConvertKey(hkey),
                             lpszValueName,
                             lpdwReserved,
                             lpdwType,
                             lpbData,
                             lpcbData);

  if(ret == 0) {
      if(lpdwType)  dprintf(("key type:   %x", *lpdwType));
      if(lpcbData)  dprintf(("key length: %d", *lpcbData));
  }
  if(ret == 0 || ret == ERROR_MORE_DATA) {
      //TODO:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // Probably the same bug for some other key types (binary, multi_sz, REG_EXPAND_SZ)
      if(*lpdwType == REG_SZ && (lpbData == NULL || ret == ERROR_MORE_DATA) && lpcbData) {
          dprintf(("Get the real size of the string key data"));
          //Get the real size of the string key data
          //not a nice fix; hope this is enough (there is no clear connection
          //between returned length and real string length (not linear for different
          //string sizes))
          *lpcbData = 4096;
          lpbData   = (LPBYTE)malloc(*lpcbData);
          if(lpcbData) {
              //don't overwrite return value (in case it was ERROR_MODE_DATA)
              O32_RegQueryValueEx(ConvertKey(hkey),
                                  lpszValueName,
                                  lpdwReserved,
                                  lpdwType,
                                  lpbData,
                                  lpcbData);
              if(lpcbData)  dprintf(("real key length: %d", *lpcbData));
              free(lpbData);
          }
      }
  }
  return ret;
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

LONG WIN32API RegQueryValueExW(HKEY   hkey,
                               LPCWSTR  lpszValueName,
                               LPDWORD lpdwReserved,
                               LPDWORD lpdwType,
                               LPBYTE  lpbData,
                               LPDWORD lpcbData)
{
  char *astring = UnicodeToAsciiString(lpszValueName);
  char *akeydata = NULL;
  LONG  rc;
  DWORD dwType;

  if(lpbData && lpcbData)
  {
    akeydata = (char *)malloc(*lpcbData+1);
    akeydata[*lpcbData] = 0;
  }

  if(lpdwType == NULL) {
    lpdwType = &dwType;
  }

  rc = RegQueryValueExA(hkey, astring, lpdwReserved, lpdwType,
                        (LPBYTE)akeydata, lpcbData);
  //could also query key type (without returning data), call it again and only allocate translation
  //buffer if string type
  if(rc == ERROR_SUCCESS && lpbData && lpcbData)
  {
      switch(*lpdwType) {
      case REG_SZ:
      case REG_EXPAND_SZ:
          lstrcpyAtoW((LPWSTR)lpbData, akeydata);
          break;
      case REG_MULTI_SZ:
      case REG_LINK: //???
          dprintf(("ERROR: key data must be translated from Unicode to Ascii!!"));
          break;
      default:
          memcpy(lpbData, akeydata, *lpcbData);
          break;
      }
  }

  if((rc == ERROR_SUCCESS || rc == ERROR_MORE_DATA) && 
     lpcbData) 
  {
      switch(*lpdwType) {
      case REG_SZ:
      case REG_EXPAND_SZ:
      case REG_MULTI_SZ:
      case REG_LINK: //???
          *lpcbData = *lpcbData * sizeof(WCHAR); //size in bytes!!
          break;
      }
  }

  if (NULL != astring)
    FreeAsciiString(astring);
  
  if(akeydata)
    free(akeydata);

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

LONG WIN32API RegSetValueA(HKEY  hkey,
                           LPCSTR lpSubKey,
                           DWORD  dwType,
                           LPCSTR lpData,
                           DWORD  cbData)
{
 LONG rc;

  //SvL: 8-11-'97: Bugfix: crash in pmwinx if size == 0 and string is large
  if(cbData == 0)
    cbData = strlen(lpData);

  rc = O32_RegSetValue(ConvertKey(hkey), lpSubKey,
                       dwType, lpData, cbData);

  if(rc == ERROR_NOT_ENOUGH_MEMORY && cbData == 0 && dwType == REG_SZ)
  {
    char regdata = 0;
    //SvL: Netscape sets an empty string key this way; Open32 doesn't like it
    rc = O32_RegSetValue(ConvertKey(hkey),
                         lpSubKey,
                         dwType,
                         &regdata,
                         1);
  }
  return rc;
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

LONG WIN32API RegSetValueW(HKEY   hkey,
                           LPCWSTR lpSubKey,
                           DWORD   dwType,
                           LPCWSTR lpData,
                           DWORD   cbData)
{
  char *astring1 = UnicodeToAsciiString((LPWSTR)lpSubKey);
  char *astring2 = UnicodeToAsciiString((LPWSTR)lpData);
  LONG  rc;

  rc = RegSetValueA(hkey, astring1, dwType, astring2, cbData);
  
  if (NULL != astring1)
    FreeAsciiString(astring1);
  
  if (NULL != astring2)
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

LONG WIN32API RegSetValueExA(HKEY   hkey,
                             LPCSTR lpszValueName,
                             DWORD  dwReserved,
                             DWORD  fdwType,
                             BYTE*  lpbData,
                             DWORD  cbData)
{
  LPSTR lpszExpandedString = NULL;
  LONG  ret;

  if(fdwType == REG_SZ || fdwType == REG_EXPAND_SZ) {
    dprintf(("ADVAPI32: RegSetValueExA)%08xh,%s,%08xh,%08xh,%s,%08xh)",
               hkey,
               lpszValueName,
               dwReserved,
               fdwType,
               lpbData,
               cbData));
  }
  else {
    dprintf(("ADVAPI32: RegSetValueExA)%08xh,%s,%08xh,%08xh,%08xh,%08xh)",
               hkey,
               lpszValueName,
               dwReserved,
               fdwType,
               lpbData,
               cbData));
  }

  if(fdwType == REG_EXPAND_SZ) {
      dprintf(("!WARNING!: REG_EXPAND_SZ converted to REG_SZ"));
      fdwType = REG_SZ; //registry.dll doesn't like this type

      //Expand string
      lpszExpandedString = (LPSTR)malloc(cbData);
      if(lpszExpandedString == NULL) {
          DebugInt3();
          return ERROR_NOT_ENOUGH_MEMORY;
      }
      ExpandEnvironmentStringsA((LPSTR)lpbData, lpszExpandedString, cbData);
      lpbData = (BYTE *)lpszExpandedString;
      cbData  = strlen(lpszExpandedString)+1;
      dprintf(("Expanded to: %s", lpszExpandedString));
  }
  ret = O32_RegSetValueEx(ConvertKey(hkey), lpszValueName, dwReserved,
                          fdwType, lpbData, cbData);

  if(lpszExpandedString) free(lpszExpandedString);

  return ret;
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

LONG WIN32API RegSetValueExW(HKEY  hkey,
                             LPCWSTR lpszValueName,
                             DWORD  dwReserved,
                             DWORD  fdwType,
                             BYTE*  lpbData,
                             DWORD  cbData)
{
  char *astring = UnicodeToAsciiString(lpszValueName);
  char *akeydata = NULL;
  LONG  rc;

  switch(fdwType) {
  case REG_SZ:
  case REG_EXPAND_SZ:
        akeydata = UnicodeToAsciiString((LPWSTR)lpbData);
        lpbData = (BYTE *)akeydata;
        break;
  case REG_MULTI_SZ:
  case REG_LINK: //???
        dprintf(("ERROR: key data must be translated from Unicode to Ascii!!"));
        break;
  }
  rc = RegSetValueExA(hkey, astring, dwReserved, fdwType, lpbData, cbData);
  
  if(akeydata)
    FreeAsciiString(akeydata);
  
  if (NULL != astring)
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

LONG WIN32API RegFlushKey(HKEY hkey)
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

LONG WIN32API RegConnectRegistryA(LPCSTR lpszComputerName,
                                  HKEY   hKey,
                                  PHKEY  phkResult)
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

LONG WIN32API RegConnectRegistryW(LPCWSTR lpszComputerName,
                                  HKEY   hKey,
                                  PHKEY  phkResult)
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

LONG WIN32API RegGetKeySecurity(HKEY                 hKey,
                                SECURITY_INFORMATION SecInf,
                                PSECURITY_DESCRIPTOR pSecDesc,
                                LPDWORD              lpcbSecDesc)
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

LONG WIN32API RegLoadKeyA(HKEY  hKey,
                          LPCSTR lpszSubKey,
                          LPCSTR lpszFile)
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

LONG WIN32API RegLoadKeyW(HKEY   hKey,
                          LPCWSTR lpszSubKey,
                          LPCWSTR lpszFile)
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

LONG WIN32API RegQueryMultipleValuesA(HKEY     hKey,
                                      PVALENTA val_list,
                                      DWORD    num_vals,
                                      LPTSTR   lpValueBuf,
                                      LPDWORD  ldwTotsize)
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

LONG WIN32API RegQueryMultipleValuesW(HKEY     hKey,
                                      PVALENTW val_list,
                                      DWORD    num_vals,
                                      LPWSTR   lpValueBuf,
                                      LPDWORD  ldwTotsize)
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
LONG WIN32API RegRemapPreDefKey(HKEY hKey)
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

LONG WIN32API RegReplaceKeyA(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpNewFile,
                             LPCSTR lpOldFile)
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

LONG WIN32API RegReplaceKeyW(HKEY   hKey,
                             LPCWSTR lpSubKey,
                             LPCWSTR lpNewFile,
                             LPCWSTR lpOldFile)
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

LONG WIN32API RegRestoreKeyA(HKEY   hKey,
                             LPCSTR lpszFile,
                             DWORD   fdw)
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

LONG WIN32API RegRestoreKeyW(HKEY    hKey,
                             LPCWSTR  lpszFile,
                             DWORD    fdw)
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

LONG WIN32API RegSaveKeyA(HKEY                  hKey,
                          LPCSTR                lpszFile,
                          LPSECURITY_ATTRIBUTES lpsa)
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

LONG WIN32API RegSaveKeyW(HKEY                  hKey,
                          LPCWSTR               lpszFile,
                          LPSECURITY_ATTRIBUTES lpsa)
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

LONG WIN32API RegSetKeySecurity(HKEY                 hKey,
                                SECURITY_INFORMATION si,
                                PSECURITY_DESCRIPTOR psd)
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

LONG WIN32API RegUnLoadKeyA(HKEY    hKey,
                            LPCSTR  lpszSubKey)
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

LONG WIN32API RegUnLoadKeyW(HKEY    hKey,
                            LPCWSTR  lpszSubKey)
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

LONG WIN32API RegNotifyChangeKeyValue(HKEY   hKey,
                                      BOOL   bWatchSubtree,
                                      DWORD  dwNotifyFilter,
                                      HANDLE hEvent,
                                      BOOL   fAsynchronus)
{
   dprintf(("ADVAPI32: RegNotifyChangeKeyValue() not implemented.\n"));

   return ERROR_ACCESS_DENIED;
}

