/* $Id: advapi32.c,v 1.1 1999-05-24 20:19:32 ktk Exp $ */

/*
 * Win32 advanced API functions for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * @(#) ADVAPI32.C            1.0.1 1998/06/14 PH added stubs
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "misc.h"
#include "advapi32.h"
#include "unicode.h"
#include <winreg.h>

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1

//******************************************************************************
//******************************************************************************
HKEY ConvertKey(HKEY winkey)
{
  switch((int)winkey)
  {
   case WINHKEY_CLASSES_ROOT:   return HKEY_CLASSES_ROOT;
   case WINHKEY_CURRENT_USER:   return HKEY_CURRENT_USER;
   case WINHKEY_LOCAL_MACHINE:  return HKEY_LOCAL_MACHINE;
   case WINHKEY_USERS:          return HKEY_USERS;
  }
  return(winkey);
}

//******************************************************************************
//******************************************************************************
DWORD WIN32API RegCloseKey( HKEY arg1)
{
    dprintf(("RegCloseKey %X\n", arg1));
    return O32_RegCloseKey(ConvertKey(arg1));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegCreateKeyA( HKEY arg1, LPCSTR arg2, PHKEY  arg3)
{
#ifdef DEBUG
    WriteLog("RegCreateKey\n");
#endif
    return O32_RegCreateKey(ConvertKey(arg1), arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegCreateKeyW(HKEY arg1, LPCWSTR arg2, PHKEY  arg3)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegCreateKeyW %s\n", astring);
#endif
    rc = O32_RegCreateKey(ConvertKey(arg1), astring, arg3);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegCreateKeyExA( HKEY arg1, LPCSTR arg2, DWORD arg3, LPSTR arg4, DWORD arg5, REGSAM arg6, LPSECURITY_ATTRIBUTES arg7, PHKEY arg8, LPDWORD  arg9)
{
#ifdef DEBUG
    WriteLog("RegCreateKeyEx\n");
#endif
    return O32_RegCreateKeyEx(ConvertKey(arg1), arg2, arg3, arg4, arg5, arg6 | KEY_READ, arg7, arg8, arg9);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegCreateKeyExW( HKEY arg1, LPCWSTR arg2, DWORD arg3, LPWSTR arg4, DWORD arg5, REGSAM arg6, LPSECURITY_ATTRIBUTES arg7, PHKEY arg8, LPDWORD  arg9)
{
 char *astring1 = UnicodeToAsciiString((LPWSTR)arg2);
 char *astring2 = UnicodeToAsciiString(arg4);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegCreateKeyExW\n");
#endif
    rc = O32_RegCreateKeyEx(ConvertKey(arg1), astring1, arg3, astring2, arg5, arg6 | KEY_READ, arg7, arg8, arg9);
    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegDeleteKeyW(HKEY arg1, LPWSTR  arg2)
{
 char *astring = UnicodeToAsciiString(arg2);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegDeleteKeyW\n");
#endif
    rc = O32_RegDeleteKey(ConvertKey(arg1), astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegDeleteKeyA(HKEY arg1, LPCSTR  arg2)
{
#ifdef DEBUG
    WriteLog("RegDeleteKey\n");
#endif
    return O32_RegDeleteKey(ConvertKey(arg1), arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegDeleteValueA(HKEY arg1, LPSTR arg2)
{
#ifdef DEBUG
    WriteLog("RegDeleteValue\n");
#endif
    return O32_RegDeleteValue(ConvertKey(arg1), arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegDeleteValueW(HKEY arg1, LPWSTR arg2)
{
 char *astring = UnicodeToAsciiString(arg2);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegDeleteValueW\n");
#endif
    rc = O32_RegDeleteValue(ConvertKey(arg1), astring);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegEnumKeyA(HKEY arg1, DWORD arg2, LPSTR arg3, DWORD  arg4)
{
#ifdef DEBUG
    WriteLog("RegEnumKey\n");
#endif
    return O32_RegEnumKey(ConvertKey(arg1), arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegEnumKeyW(HKEY arg1, DWORD arg2, LPWSTR arg3, DWORD  arg4)
{
 char *astring;
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegEnumKeyW\n");
#endif
    rc = O32_RegEnumKey(ConvertKey(arg1), arg2, (char *)arg3, arg4);
    if(rc == ERROR_SUCCESS) {
   	astring = (char *)malloc(arg4);
   	strcpy(astring, (char *)arg3);
   	AsciiToUnicode(astring, arg3);
   	free(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegEnumKeyExA(HKEY arg1, DWORD arg2, LPSTR arg3, LPDWORD arg4, LPDWORD arg5, LPSTR arg6, LPDWORD arg7, LPFILETIME  arg8)
{
#ifdef DEBUG
    WriteLog("RegEnumKeyEx\n");
#endif
    return O32_RegEnumKeyEx(ConvertKey(arg1), arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegEnumKeyExW(HKEY arg1, DWORD arg2, LPWSTR arg3, LPDWORD arg4, LPDWORD arg5, LPWSTR arg6, LPDWORD arg7, LPFILETIME  arg8)
{
 char *astring;
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegEnumKeyExW\n");
#endif
    rc = O32_RegEnumKeyEx(ConvertKey(arg1), arg2, (char *)arg3, arg4, arg5, (char *)arg6, arg7, arg8);
    if(rc == ERROR_SUCCESS) {
   	astring = (char *)malloc(max(*arg4, *arg7));   //class & keyname
   	strcpy(astring, (char *)arg3);
   	AsciiToUnicode(astring, arg3);
   	if(arg6 != NULL) {
      		strcpy(astring, (char *)arg6);
      		AsciiToUnicode(astring, arg6);
   	}
   	free(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegEnumValueA(HKEY arg1, DWORD arg2, LPSTR arg3, LPDWORD arg4, LPDWORD arg5, LPDWORD arg6, LPBYTE arg7, LPDWORD  arg8)
{
#ifdef DEBUG
    WriteLog("RegEnumValue\n");
#endif
    return O32_RegEnumValue(ConvertKey(arg1), arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegEnumValueW(HKEY arg1, DWORD arg2, LPWSTR arg3, LPDWORD arg4, LPDWORD arg5, LPDWORD arg6, LPBYTE arg7, LPDWORD  arg8)
{
 char *astring;
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegEnumValueW\n");
#endif
    rc = O32_RegEnumValue(ConvertKey(arg1), arg2, (char *)arg3, arg4, arg5, arg6, arg7, arg8);
    if(rc == ERROR_SUCCESS) {
   astring = (char *)malloc(*arg4);
   strcpy(astring, (char *)arg3);
   AsciiToUnicode(astring, arg3);
   free(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegOpenKeyA(HKEY    arg1,
                             LPCSTR arg2, 
                             PHKEY   arg3)
{
 LONG rc;

    rc = O32_RegOpenKey(ConvertKey(arg1), arg2, arg3);
#ifdef DEBUG
    WriteLog("RegOpenKey returned %s %d (%d)\n", arg2, *arg3, rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegOpenKeyW(HKEY arg1, LPCWSTR arg2, PHKEY  arg3)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegOpenKeyW\n");
#endif
    rc = O32_RegOpenKey(ConvertKey(arg1), astring, arg3);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegOpenKeyExA(HKEY arg1, LPCSTR arg2, DWORD arg3, REGSAM arg4, PHKEY arg5)
{
 LONG rc;

    rc = O32_RegOpenKeyEx(ConvertKey(arg1), arg2, arg3, arg4, arg5);
#ifdef DEBUG
    WriteLog("RegOpenKeyExA %X %s returned %X (%d)\n", arg1, arg2, *arg5, rc);
#endif
    //SvL: This fixes crashes in pmwinx.dll. (if an app doesn't check the
    //     return value and uses the whatever *arg5 contains)
    if(rc) {
   *arg5 = 0;
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegOpenKeyExW(HKEY arg1, LPCWSTR arg2, DWORD arg3, REGSAM arg4, PHKEY arg5)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegOpenKeyExW %X %s\n", arg1, astring);
#endif
    rc = O32_RegOpenKeyEx(ConvertKey(arg1), astring, arg3, arg4, arg5);
    //SvL: This fixes crashes in pmwinx.dll. (if an app doesn't check the
    //     return value and uses the whatever *arg5 contains)
    if(rc) {
   *arg5 = 0;
    }
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegQueryInfoKeyA(HKEY arg1, LPSTR arg2, LPDWORD arg3, LPDWORD arg4, LPDWORD arg5, LPDWORD arg6, LPDWORD arg7, LPDWORD arg8, LPDWORD arg9, LPDWORD arg10, LPDWORD arg11, LPFILETIME  arg12)
{
#ifdef DEBUG
    WriteLog("RegQueryInfoKey\n");
#endif
    return O32_RegQueryInfoKey(ConvertKey(arg1), arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegQueryInfoKeyW(HKEY arg1, LPWSTR arg2, LPDWORD arg3, LPDWORD arg4, LPDWORD arg5, LPDWORD arg6, LPDWORD arg7, LPDWORD arg8, LPDWORD arg9, LPDWORD arg10, LPDWORD arg11, LPFILETIME  arg12)
{
 char *astring;
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegQueryInfoKeyW\n");
#endif
    rc = O32_RegQueryInfoKey(ConvertKey(arg1), (char *)arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
    if(rc == ERROR_SUCCESS) {
   	astring = (char *)malloc(*arg3);
	strcpy(astring, (char *)arg2);
	AsciiToUnicode(astring, arg2);
	free(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegQueryValueA(HKEY arg1, LPCSTR arg2, LPSTR arg3, PLONG  arg4)
{
#ifdef DEBUG
    WriteLog("RegQueryValue\n");
#endif
    return O32_RegQueryValue(ConvertKey(arg1), arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegQueryValueW(HKEY hkey, LPCWSTR lpszSubKey, LPWSTR lpszValue, PLONG pcbValue)
{
 char *astring1 = UnicodeToAsciiString((LPWSTR)lpszSubKey);
 char *astring2;
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegQueryValueW\n");
#endif
    rc = O32_RegQueryValue(ConvertKey(hkey), astring1, (char *)lpszValue, pcbValue);
    if(rc == ERROR_SUCCESS) {
   astring2 = (char *)malloc(*pcbValue);
   strcpy(astring2, (char *)lpszValue);
   AsciiToUnicode(astring2, lpszValue);
   free(astring2);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegQueryValueExA(HKEY arg1, LPSTR arg2, LPDWORD arg3, LPDWORD arg4, LPBYTE arg5, LPDWORD  arg6)
{
#ifdef DEBUG
    WriteLog("RegQueryValueEx %s\n", arg2);
#endif
    return O32_RegQueryValueEx(ConvertKey(arg1), arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//TODO: DOESN'T WORK FOR STRING DATA!!
//******************************************************************************
DWORD WIN32API RegQueryValueExW(HKEY arg1, LPWSTR arg2, LPDWORD arg3, LPDWORD arg4, LPBYTE arg5, LPDWORD  arg6)
{
 char *astring = UnicodeToAsciiString(arg2);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegQueryValueExW %s\n", astring);
#endif
    rc = O32_RegQueryValueEx(ConvertKey(arg1), astring, arg3, arg4, arg5, arg6);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegSetValueA(HKEY hkey, LPCSTR lpSubKey, DWORD dwType, LPCSTR lpData, DWORD cbData)
{
#ifdef DEBUG
   WriteLog("RegSetValueA %d, %s, %d, %s, %d\n", hkey, lpSubKey, dwType, lpData, cbData);
#endif
   //SvL: 8-11-'97: Bugfix: crash in pmwinx if size == 0 and string is large
   if(cbData == 0)
   cbData = strlen(lpData);

   return(O32_RegSetValue(ConvertKey(hkey), lpSubKey, dwType, lpData, cbData));
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegSetValueW(HKEY hkey, LPCWSTR lpSubKey, DWORD dwType, LPCWSTR lpData, DWORD cbData)
{
 char *astring1 = UnicodeToAsciiString((LPWSTR)lpSubKey);
 char *astring2 = UnicodeToAsciiString((LPWSTR)lpData);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegSetValue\n");
#endif
   //SvL: 8-11-'97: Bugfix: crash in pmwinx if size == 0 and string is large
   if(cbData == 0)
   cbData = strlen(astring2);

   rc = O32_RegSetValue(ConvertKey(hkey), astring1, dwType, astring2, cbData);
   FreeAsciiString(astring1);
   FreeAsciiString(astring2);
   return(rc);
}
//******************************************************************************
//TODO:Check for string length here too?
//******************************************************************************
DWORD WIN32API RegSetValueExA(HKEY arg1, LPSTR arg2, DWORD arg3, DWORD arg4, BYTE * arg5, DWORD  arg6)
{
#ifdef DEBUG
    WriteLog("RegSetValueEx\n");
#endif
    return O32_RegSetValueEx(ConvertKey(arg1), arg2, arg3, arg4, arg5, arg6);
}
//******************************************************************************
//TODO:Check for string length here too?
//******************************************************************************
DWORD WIN32API RegSetValueExW(HKEY arg1, LPWSTR arg2, DWORD arg3, DWORD arg4, BYTE * arg5, DWORD  arg6)
{
 char *astring = UnicodeToAsciiString(arg2);
 LONG  rc;

#ifdef DEBUG
    WriteLog("RegSetValueExW, NOT COMPLETE!!\n");
#endif
    rc = O32_RegSetValueEx(ConvertKey(arg1), astring, arg3, arg4, arg5, arg6);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API RegFlushKey(HKEY hkey)
{
#ifdef DEBUG
    WriteLog("OS2RegFlushKey, not implemented\n");
#endif
    return(ERROR_SUCCESS);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetFileSecurityA(LPCSTR               lpFileName,
                               SECURITY_INFORMATION RequestedInformation,
                               PSECURITY_DESCRIPTOR pSecurityDescriptor,
                               DWORD                nLength,
                               LPDWORD              lpnLengthNeeded)
{
#ifdef DEBUG
  WriteLog("GetFileSecurityA %s, not implemented\n", lpFileName);
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetFileSecurityW(LPCWSTR               lpFileName,
                               SECURITY_INFORMATION  RequestedInformation,
                               PSECURITY_DESCRIPTOR  pSecurityDescriptor,
                               DWORD                 nLength,
                               LPDWORD               lpnLengthNeeded)
{
#ifdef DEBUG
  WriteLog("GetFileSecurityW %s, not implemented\n",
           lpFileName);
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetFileSecurityA(LPCSTR lpFileName,
		               SECURITY_INFORMATION RequestedInformation,
              		       PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
#ifdef DEBUG
  WriteLog("SetFileSecurityA %s, not implemented\n", lpFileName);
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetFileSecurityW(LPCWSTR lpFileName,
              			SECURITY_INFORMATION RequestedInformation,
              			PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
#ifdef DEBUG
  WriteLog("SetFileSecurityW %s, not implemented\n", lpFileName);
#endif
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetUserNameA(  /*PLF Wed  98-02-11 13:33:39*/
    LPTSTR lpBuffer,        /* address of name buffer       */
    LPDWORD lpcchBuffer)    /* address of size of name buffer       */


    /* The GetUserName function retrieves the user name of the current
     *  thread. This is the name of the user currently logged onto the
     *  system.
     */

{
    #define USERNAME "USER"
    if(*lpcchBuffer < sizeof(USERNAME))
        return FALSE;
    strcpy(lpBuffer, USERNAME);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetUserNameW( /*KSO Thu 21.05.1998 */
   LPWSTR lpBuffer,
   LPDWORD lpccBuffer
   )
{

   if ( *lpccBuffer >= sizeof(USERNAME)*2 )
   {
      AsciiToUnicode(USERNAME, lpBuffer);
      return TRUE;
   }
      return FALSE;
}
//******************************************************************************
//******************************************************************************



BOOL WIN32API ReportEventA(     /*PLF Sat  98-03-07 00:36:43*/
     HANDLE     hEventLog,
     WORD       wType,
     WORD       wCategory,
     DWORD      dwEventID,
     PSID       lpUserSid,
     WORD       wNumStrings,
     DWORD      dwDataSize,
     LPCSTR   *lpStrings,
     LPVOID     lpRawData
    )
{
    dprintf(("ReportEventA(): NIY\n"));
    return TRUE;
}


BOOL WIN32API ReportEventW(     /*PLF Sat  98-03-07 00:36:43*/
     HANDLE     hEventLog,
     WORD       wType,
     WORD       wCategory,
     DWORD      dwEventID,
     PSID       lpUserSid,
     WORD       wNumStrings,
     DWORD      dwDataSize,
     LPCWSTR   *lpStrings,
     LPVOID     lpRawData
    )
{
    dprintf(("ReportEventW(): NIY\n"));
    return TRUE;
}


BOOL WIN32API SetSecurityDescriptorDacl( /*PLF Sat  98-03-07 02:48:45*/
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    BOOL bDaclPresent,
    PACL pDacl,
    BOOL bDaclDefaulted
    )

{
    dprintf(("SetSecurityDescriptorDacl(): NIY - returning error\n"));
    return FALSE;
}



/*PLF Sat  98-03-07 02:59:20*/
BOOL WIN32API InitializeSecurityDescriptor(PSECURITY_DESCRIPTOR pSecurityDescriptor,
					   DWORD dwRevision)
{
    dprintf(("InitializeSecurityDescriptor() NIY\n"));
    return FALSE;
}

/*PLF Sat  98-03-07 02:59:20*/
HANDLE WIN32API RegisterEventSourceA(LPCSTR lpUNCServerName, LPCSTR lpSourceName)
{
    dprintf(("RegisterEventSourceA() NIY\n"));
    return FALSE;
}

/*PLF Sat  98-03-07 02:59:20*/
HANDLE WIN32API RegisterEventSourceW(LPCWSTR lpUNCServerName, LPCWSTR lpSourceName)
{
    dprintf(("RegisterEventSourceW() NIY\n"));
    return FALSE;
}


/*PLF Sat  98-03-07 02:59:20*/
BOOL WIN32API DeregisterEventSource(HANDLE hEventLog)
{
    dprintf(("DeregisterEventSource() NIY\n"));
    return FALSE;
}


/*PLF Sat  98-03-07 02:59:20*/
BOOL WIN32API AdjustTokenPrivileges(
            HANDLE TokenHandle,
            BOOL DisableAllPrivileges,
            PTOKEN_PRIVILEGES NewState,
            DWORD BufferLength,
            PTOKEN_PRIVILEGES PreviousState,
            LPDWORD ReturnLength
)
{
    dprintf(("AdjustTokenPrivileges() NIY\n"));
    return FALSE;
}

/*PLF Sat  98-03-07 02:59:20*/
BOOL WIN32API LookupPrivilegeValueA(LPCSTR lpSystemName,
                                       LPCSTR lpName,
                                       LPVOID lpLuid)
{
    dprintf(("LookupPrivilegeValueA() NIY\n"));
    return FALSE;
}

BOOL WIN32API LookupPrivilegeValueW(LPCWSTR lpSystemName,
                                       LPCWSTR lpName,
                                       LPVOID lpLuid)
{
    dprintf(("LookupPrivilegeValueW() NIY\n"));
    return FALSE;
}


/*PLF Sat  98-03-07 02:59:20*/
BOOL WIN32API OpenProcessToken(HANDLE ProcessHandle,
			       DWORD DesiredAccess,
			       PHANDLE TokenHandle
)
{
    dprintf(("OpenProcessToken() NIY\n"));
    return FALSE;
}


//******************************************************************************
/* Stubs added to get up office97 */
//******************************************************************************
/*KSO Thu 21.05.1998*/
LONG WIN32API RegNotifyChangeKeyValue (
   HKEY hKey,
   BOOL bWatchSubtree,
   DWORD dwNotifyFilter,
   HANDLE hEvent,
   BOOL fAsynchronus
                        )
{
   dprintf(("RegNotifyChangeKeyValue() NIY\n"));
   return FALSE;
}
//******************************************************************************
//******************************************************************************
/*KSO Thu 21.05.1998*/
BOOL WIN32API SetThreadToken (
   PHANDLE Thread,
   HANDLE Token
   )
{
   dprintf(("SetThreadToken() NIY\n"));
   return FALSE;
}

//******************************************************************************
//******************************************************************************
/*KSO Thu 21.05.1998*/
BOOL WIN32API OpenThreadToken (
   HANDLE ThreadHandle,
   DWORD DesiredAccess,
   BOOL OpenAsSelf,
   PHANDLE TokenHandle
   )
{
   dprintf(("OpenThreadToken() NIY\n"));
   return FALSE;
}





/*****************************************************************************
 * Name      : AbortSystemShutdownA
 * Purpose   : The AbortSystemShutdown function stops a system shutdown started
 *             by using the InitiateSystemShutdown function.
 * Parameters: LPTSTR  lpMachineName  address of name of computer to stop shutting down
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AbortSystemShutdownA(LPTSTR lpMachineName)
{
  dprintf(("ADVAPI32: AbortSystemShutdownA(%s) not implemented.\n",
           lpMachineName));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AbortSystemShutdownW
 * Purpose   : The AbortSystemShutdown function stops a system shutdown started
 *             by using the InitiateSystemShutdown function.
 * Parameters: LPWSTR  lpMachineName  address of name of computer to stop shutting down
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AbortSystemShutdownW(LPWSTR lpMachineName)
{
  dprintf(("ADVAPI32: AbortSystemShutdownW(%s) not implemented.\n",
           lpMachineName));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AccessCheck
 * Purpose   : The AccessCheck function is used by a server application to
 *             check a client's access to an object against the access control
 *             associated with the object.
 * Parameters: PSECURITY_DESCRIPTOR pSecurityDescriptor address of security descriptor
 *             HANDLE               ClientToken         handle of client access token
 *             DWORD                DesiredAccess       access mask to request
 *             PGENERIC_MAPPING     GenericMapping      address of generic-mapping structure
 *             PPRIVILEGE_SET       PrivilegeSet        address of privilege-set structure
 *             LPDWORD              PrivilegeSetLength  size of privilege-set structure
 *             LPDWORD              GrantedAccess       address of granted access mask
 *             LPBOOL               AccessStatus        address of flag indicating whether access granted
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define PGENERIC_MAPPING LPVOID
#define PPRIVILEGE_SET   LPVOID
BOOL WIN32API AccessCheck(PSECURITY_DESCRIPTOR pSecurityDescriptor,
                             HANDLE               ClientToken,
                             DWORD                DesiredAccess,
                             PGENERIC_MAPPING     GenericMapping,
                             PPRIVILEGE_SET       PrivilegeSet,
                             LPDWORD              PrivilegeSetLength,
                             LPDWORD              GrantedAccess,
                             LPBOOL               AccessStatus)
{
  dprintf(("ADVAPI32: AccessCheck(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pSecurityDescriptor,
           ClientToken,
           DesiredAccess,
           GenericMapping,
           PrivilegeSet,
           PrivilegeSetLength,
           GrantedAccess,
           AccessStatus));

  return (TRUE); /* always grant access */
}


/*****************************************************************************
 * Name      : AccessCheckAndAuditAlarmA
 * Purpose   : The AccessCheckAndAuditAlarm function performs an access
 *             validation and generates corresponding audit messages. An
 *             application can also use this function to determine whether
 *             necessary privileges are held by a client process. This function
 *             is generally used by a server application impersonating a client
 *             process. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCSTR              SubsystemName      address of string for subsystem name
 *             LPVOID               HandleId           address of handle identifier
 *             LPTSTR               ObjectTypeName     address of string for object type
 *             LPTSTR               ObjectName         address of string for object name
 *             PSECURITY_DESCRIPTOR SecurityDescriptor address of security descriptor
 *             DWORD                DesiredAccess      mask for requested access rights
 *             PGENERIC_MAPPING     GenericMapping     address of GENERIC_MAPPING
 *             BOOL                 ObjectCreation     object-creation flag
 *             LPDWORD              GrantedAccess      address of mask for granted rights
 *             LPBOOL               AccessStatus       address of flag for results
 *             LPBOOL               pfGenerateOnClose  address of flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AccessCheckAndAuditAlarmA(LPCSTR              SubsystemName,
                                           LPVOID               HandleId,
                                           LPTSTR               ObjectTypeName,
                                           LPTSTR               ObjectName,
                                           PSECURITY_DESCRIPTOR SecurityDescriptor,
                                           DWORD                DesiredAccess,
                                           PGENERIC_MAPPING     GenericMapping,
                                           BOOL                 ObjectCreation,
                                           LPDWORD              GrantedAccess,
                                           LPBOOL               AccessStatus,
                                           LPBOOL               pfGenerateOnClose)
{
  dprintf(("ADVAPI32: AccessCheckAndAuditAlarmA(%s,%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           ObjectTypeName,
           ObjectName,
           SecurityDescriptor,
           DesiredAccess,
           GenericMapping,
           ObjectCreation,
           GrantedAccess,
           AccessStatus,
           pfGenerateOnClose));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AccessCheckAndAuditAlarmW
 * Purpose   : The AccessCheckAndAuditAlarm function performs an access
 *             validation and generates corresponding audit messages. An
 *             application can also use this function to determine whether
 *             necessary privileges are held by a client process. This function
 *             is generally used by a server application impersonating a client
 *             process. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCSTR              SubsystemName      address of string for subsystem name
 *             LPVOID               HandleId           address of handle identifier
 *             LPTSTR               ObjectTypeName     address of string for object type
 *             LPTSTR               ObjectName         address of string for object name
 *             PSECURITY_DESCRIPTOR SecurityDescriptor address of security descriptor
 *             DWORD                DesiredAccess      mask for requested access rights
 *             PGENERIC_MAPPING     GenericMapping     address of GENERIC_MAPPING
 *             BOOL                 ObjectCreation     object-creation flag
 *             LPDWORD              GrantedAccess      address of mask for granted rights
 *             LPBOOL               AccessStatus       address of flag for results
 *             LPBOOL               pfGenerateOnClose  address of flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AccessCheckAndAuditAlarmW(LPCWSTR              SubsystemName,
                                           LPVOID               HandleId,
                                           LPWSTR               ObjectTypeName,
                                           LPWSTR               ObjectName,
                                           PSECURITY_DESCRIPTOR SecurityDescriptor,
                                           DWORD                DesiredAccess,
                                           PGENERIC_MAPPING     GenericMapping,
                                           BOOL                 ObjectCreation,
                                           LPDWORD              GrantedAccess,
                                           LPBOOL               AccessStatus,
                                           LPBOOL               pfGenerateOnClose)
{
  dprintf(("ADVAPI32: AccessCheckAndAuditAlarmW(%s,%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           ObjectTypeName,
           ObjectName,
           SecurityDescriptor,
           DesiredAccess,
           GenericMapping,
           ObjectCreation,
           GrantedAccess,
           AccessStatus,
           pfGenerateOnClose));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AddAccessAllowedAce
 * Purpose   : The AddAccessAllowedAce function adds an access-allowed ACE to
 *             an ACL. The access is granted to a specified SID. An ACE is an
 *             access-control entry. An ACL is an access-control list. A SID is
 *             a security identifier.
 * Parameters: PACL  pAcl           address of access-control list
 *             DWORD dwAceRevision  ACL revision level
 *             DWORD AccessMask     access mask
 *             PSID  pSid           address of security identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AddAccessAllowedAce(PACL  pAcl,
                                     DWORD dwAceRevision,
                                     DWORD AccessMask,
                                     PSID  pSid)
{
  dprintf(("ADVAPI32: AddAccessAllowedAce(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           dwAceRevision,
           AccessMask,
           pSid));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AddAccessDeniedAce
 * Purpose   : The AddAccessDeniedAce function adds an access-denied ACE to an
 *             ACL. The access is denied to a specified SID. An ACE is an
 *             access-control entry. An ACL is an access-control list. A SID
 *             is a security identifier.
 * Parameters: PACL  pAcl           address of access-control list
 *             DWORD dwAceRevision  ACL revision level
 *             DWORD AccessMask     access mask
 *             PSID  pSid           address of security identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AddAccessDeniedAce(PACL  pAcl,
                                    DWORD dwAceRevision,
                                    DWORD AccessMask,
                                    PSID  pSid)
{
  dprintf(("ADVAPI32: AddAccessDeniedAce(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           dwAceRevision,
           AccessMask,
           pSid));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AddAce
 * Purpose   : The AddAce function adds one or more ACEs to a specified ACL.
 *             An ACE is an access-control entry. An ACL is an access-control list.
 * Parameters: PACL   pAcl               address of access-control list
 *             DWORD  dwAceRevision      ACL revision level
 *             DWORD  dwStartingAceIndex index of ACE position in ACL
 *             LPVOID pAceList           address of one or more ACEs
 *             DWORD  nAceListLength     size of buffer for ACEs
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AddAce(PACL   pAcl,
                        DWORD  dwAceRevision,
                        DWORD  dwStartingAceIndex,
                        LPVOID pAceList,
                        DWORD  nAceListLength)
{
  dprintf(("ADVAPI32: AddAce(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           dwAceRevision,
           dwStartingAceIndex,
           pAceList,
           nAceListLength));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AddAuditAccessAce
 * Purpose   : The AddAuditAccessAce function adds a system-audit ACE to a
 *             system ACL. The access of a specified SID is audited. An ACE is
 *             an access-control entry. An ACL is an access-control list. A SID
 *             is a security identifier.
 * Parameters: PACL  pAcl           address of access-control list
 *             DWORD dwAceRevision  ACL revision level
 *             DWORD dwAccessMask   access mask
 *             PSID  pSid           address of security identifier
 *             BOOL  bAuditSuccess  flag for auditing successful access
 *             BOOL  bAuditFailure  flag for auditing unsuccessful access attempts
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AddAuditAccessAce(PACL  pAcl,
                                   DWORD dwAceRevision,
                                   DWORD dwAccessMask,
                                   PSID  pSid,
                                   BOOL  bAuditSuccess,
                                   BOOL  bAuditFailure)
{
  dprintf(("ADVAPI32: AddAuditAccessAce(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           dwAceRevision,
           dwAccessMask,
           pSid,
           bAuditSuccess,
           bAuditFailure));

  return (FALSE);
}


/*****************************************************************************
 * Name      : AdjustTokenGroups
 * Purpose   : The AdjustTokenGroups function adjusts groups in the specified
 *             access token. TOKEN_ADJUST_GROUPS access is required to enable
 *             or disable groups in an access token.
 * Parameters: HANDLE         TokenHandle     handle of token that contains groups
 *             BOOL           ResetToDefault  flag for default settings
 *             PTOKEN_GROUPS  NewState        address of new group information
 *             DWORD          BufferLength    size of buffer for previous information
 *             PTOKEN_GROUPS  PreviousState   address of previous group information
 *             LPDWORD         ReturnLength    address of required buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define PTOKEN_GROUPS LPVOID
BOOL WIN32API AdjustTokenGroups(HANDLE         TokenHandle,
                                   BOOL           ResetToDefault,
                                   PTOKEN_GROUPS  NewState,
                                   DWORD          BufferLength,
                                   PTOKEN_GROUPS  PreviousState,
                                   LPDWORD         ReturnLength)
{
  dprintf(("ADVAPI32: AdjustTokenGroups(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           TokenHandle,
           ResetToDefault,
           NewState,
           BufferLength,
           PreviousState,
           ReturnLength));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : AllocateAndInitializeSid
 * Purpose   : The AllocateAndInitializeSid function allocates and initializes
 *             a security identifier (SID) with up to eight subauthorities.
 * Parameters: PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority address of identifier authority
 *             BYTE   nSubAuthorityCount count of subauthorities
 *             DWORD  dwSubAuthority0    subauthority 0
 *             DWORD  dwSubAuthority1    subauthority 1
 *             DWORD  dwSubAuthority2    subauthority 2
 *             DWORD  dwSubAuthority3    subauthority 3
 *             DWORD  dwSubAuthority4    subauthority 4
 *             DWORD  dwSubAuthority5    subauthority 5
 *             DWORD  dwSubAuthority6    subauthority 6
 *             DWORD  dwSubAuthority7    subauthority 7
 *             PSID   *pSid              address of pointer to SID
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define PSID_IDENTIFIER_AUTHORITY LPVOID
BOOL WIN32API AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY  pIdentifierAuthority,
                                          BYTE  nSubAuthorityCount,
                                          DWORD  dwSubAuthority0,
                                          DWORD  dwSubAuthority1,
                                          DWORD  dwSubAuthority2,
                                          DWORD  dwSubAuthority3,
                                          DWORD  dwSubAuthority4,
                                          DWORD  dwSubAuthority5,
                                          DWORD  dwSubAuthority6,
                                          DWORD  dwSubAuthority7,
                                          PSID  *pSid)
{
  dprintf(("ADVAPI32: AllocateAndInitializeSid(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pIdentifierAuthority,
           nSubAuthorityCount,
           dwSubAuthority0,
           dwSubAuthority1,
           dwSubAuthority2,
           dwSubAuthority3,
           dwSubAuthority4,
           dwSubAuthority5,
           dwSubAuthority6,
           dwSubAuthority7,
           pSid));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : AllocateLocallyUniqueId
 * Purpose   : The AllocateLocallyUniqueId function allocates a locally unique
 *             identifier (LUID).
 * Parameters: PLUID  Luid  address of locally unique identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AllocateLocallyUniqueId(PLUID  Luid)
{
  dprintf(("ADVAPI32: AllocateLocallyUniqueId(%08xh) not implemented.\n",
           Luid));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : AreAllAccessesGranted
 * Purpose   : The AreAllAccessesGranted function checks whether a set of
 *             requested access rights has been granted. The access rights are
 *             represented as bit flags in a 32-bit access mask.
 * Parameters: DWORD GrantedAccess  access mask for granted access rights
 *             DWORD DesiredAccess  access mask for requested access rights
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AreAllAccessesGranted(DWORD  GrantedAccess,
                                       DWORD  DesiredAccess)
{
  dprintf(("ADVAPI32: AreAllAccessesGranted(%08xh,%08xh) not implemented.\n",
           GrantedAccess,
           DesiredAccess));

  return (TRUE); /* grant all access */
}


/*****************************************************************************
 * Name      : AreAnyAccessesGranted
 * Purpose   : The AreAnyAccessesGranted function tests whether any of a set of
 *             requested access rights has been granted. The access rights are
 *             represented as bit flags in a 32-bit access mask.
 * Parameters: DWORD GrantedAccess  access mask for granted access rights
 *             DWORD DesiredAccess  access mask for requested access rights
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AreAnyAccessesGranted(DWORD  GrantedAccess,
                                       DWORD  DesiredAccess)
{
  dprintf(("ADVAPI32: AreAnyAccessesGranted(%08xh,%08xh) not implemented.\n",
           GrantedAccess,
           DesiredAccess));

  return (TRUE); /* grant all access */
}


/*****************************************************************************
 * Name      : BackupEventLogA
 * Purpose   : The BackupEventLog function saves the specified event log to a
 *             backup file. The function does not clear the event log.
 * Parameters: HANDLE  hEventLog        handle to event log
 *             LPCSTR lpBackupFileName  name of backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API BackupEventLogA(HANDLE  hEventLog,
                                 LPCSTR lpBackupFileName)
{
  dprintf(("ADVAPI32: BackupEventLogA(%08xh,%s) not implemented.\n",
           hEventLog,
           lpBackupFileName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : BackupEventLogW
 * Purpose   : The BackupEventLog function saves the specified event log to a
 *             backup file. The function does not clear the event log.
 * Parameters: HANDLE  hEventLog        handle to event log
 *             LPCWSTR lpBackupFileName  name of backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API BackupEventLogW(HANDLE  hEventLog,
                                 LPCWSTR lpBackupFileName)
{
  dprintf(("ADVAPI32: BackupEventLogW() not implemented.\n",
           hEventLog,
           lpBackupFileName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ChangeServiceConfigA
 * Purpose   : The ChangeServiceConfig function changes the configuration
 *             parameters of a service.
 * Parameters: SC_HANDLE hService           handle of service
 *             DWORD     dwServiceType      type of service
 *             DWORD     dwStartType        when to start service
 *             DWORD     dwErrorControl     severity if service fails to start
 *             LPCSTR   lpBinaryPathName   address of service binary file name
 *             LPCSTR   lpLoadOrderGroup   address of load ordering group name
 *             LPDWORD   lpdwTagId          address of variable to get tag identifier
 *             LPCSTR   lpDependencies     address of array of dependency names
 *             LPCSTR   lpServiceStartName address of account name of service
 *             LPCSTR   lpPassword         address of password for service account
 *             LPCSTR   lpDisplayName      address of display name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define SC_HANDLE HANDLE
BOOL WIN32API ChangeServiceConfigA(SC_HANDLE hService,
                                      DWORD     dwServiceType,
                                      DWORD     dwStartType,
                                      DWORD     dwErrorControl,
                                      LPCSTR   lpBinaryPathName,
                                      LPCSTR   lpLoadOrderGroup,
                                      LPDWORD   lpdwTagId,
                                      LPCSTR   lpDependencies,
                                      LPCSTR   lpServiceStartName,
                                      LPCSTR   lpPassword,
                                      LPCSTR   lpDisplayName)
{
  dprintf(("ADVAPI32: ChangeServiceConfigA(%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s,%s) not implemented.\n",
           hService,
           dwServiceType,
           dwStartType,
           dwErrorControl,
           lpBinaryPathName,
           lpLoadOrderGroup,
           lpdwTagId,
           lpDependencies,
           lpServiceStartName,
           lpPassword,
           lpDisplayName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ChangeServiceConfigW
 * Purpose   : The ChangeServiceConfig function changes the configuration
 *             parameters of a service.
 * Parameters: SC_HANDLE hService           handle of service
 *             DWORD     dwServiceType      type of service
 *             DWORD     dwStartType        when to start service
 *             DWORD     dwErrorControl     severity if service fails to start
 *             LPCWSTR   lpBinaryPathName   address of service binary file name
 *             LPCWSTR   lpLoadOrderGroup   address of load ordering group name
 *             LPDWORD   lpdwTagId          address of variable to get tag identifier
 *             LPCWSTR   lpDependencies     address of array of dependency names
 *             LPCWSTR   lpServiceStartName address of account name of service
 *             LPCWSTR   lpPassword         address of password for service account
 *             LPCWSTR   lpDisplayName      address of display name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ChangeServiceConfigW(SC_HANDLE hService,
                                      DWORD     dwServiceType,
                                      DWORD     dwStartType,
                                      DWORD     dwErrorControl,
                                      LPCWSTR   lpBinaryPathName,
                                      LPCWSTR   lpLoadOrderGroup,
                                      LPDWORD   lpdwTagId,
                                      LPCWSTR   lpDependencies,
                                      LPCWSTR   lpServiceStartName,
                                      LPCWSTR   lpPassword,
                                      LPCWSTR   lpDisplayName)
{
  dprintf(("ADVAPI32: ChangeServiceConfigW(%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s,%s) not implemented.\n",
           hService,
           dwServiceType,
           dwStartType,
           dwErrorControl,
           lpBinaryPathName,
           lpLoadOrderGroup,
           lpdwTagId,
           lpDependencies,
           lpServiceStartName,
           lpPassword,
           lpDisplayName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ClearEventLogA
 * Purpose   : The ClearEventLog function clears the specified event log, and
 *             optionally saves the current copy of the logfile to a backup file.
 * Parameters: HANDLE  hEventLog         handle to event log
 *             LPCSTR lpBackupFileName  name of backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ClearEventLogA(HANDLE  hEventLog,
                                LPCSTR lpBackupFileName)
{
  dprintf(("ADVAPI32: ClearEventLogA(%08xh,%s) not implemented.\n",
           hEventLog,
           lpBackupFileName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ClearEventLogW
 * Purpose   : The ClearEventLog function clears the specified event log, and
 *             optionally saves the current copy of the logfile to a backup file.
 * Parameters: HANDLE  hEventLog         handle to event log
 *             LPCSTR lpBackupFileName  name of backup file
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ClearEventLogW(HANDLE  hEventLog,
                                LPCWSTR lpBackupFileName)
{
  dprintf(("ADVAPI32: ClearEventLogW(%08xh,%s) not implemented.\n",
           hEventLog,
           lpBackupFileName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CloseEventLog
 * Purpose   : The CloseEventLog function closes the specified event log.
 * Parameters: HANDLE  hEventLog  handle to event log
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CloseEventLog(HANDLE hEventLog)
{
  dprintf(("ADVAPI32: CloseEventLog(%08xh) not implemented.\n",
           hEventLog));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CloseServiceHandle
 * Purpose   : The CloseServiceHandle function closes a handle to a service
 *             control manager database as returned by the OpenSCManager function,
 *             or it closes a handle to a service object as returned by either
 *             the OpenService or CreateService function.
 * Parameters: SC_HANDLE  hSCObject  handle of service or service control manager database
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CloseServiceHandle(SC_HANDLE hSCObject)
{
  dprintf(("ADVAPI32: CloseServiceHandle(%08xh) not implemented.\n",
           hSCObject));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ControlService
 * Purpose   : The ControlService function sends a control code to a Win32 service.
 * Parameters: SC_HANDLE        hService        handle of service
 *             DWORD            dwControl       control code
 *             LPSERVICE_STATUS lpServiceStatus address of service status structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ControlService(SC_HANDLE        hService,
                                DWORD            dwControl,
                                LPSERVICE_STATUS lpServiceStatus)
{
  dprintf(("ADVAPI32: ControlService(%08xh,%08xh,%08xh) not implemented.\n",
           hService,
           dwControl,
           lpServiceStatus));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CopySid
 * Purpose   : The CopySid function copies a security identifier (SID) to a buffer.
 * Parameters: DWORD nDestinationSidLength  size of buffer for copied SID
 *             PSID  pDestinationSid        address of buffer for copied SID
 *             PSID  pSourceSid             address of source SID
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CopySid(DWORD nDestinationSidLength,
                         PSID  pDestinationSid,
                         PSID  pSourceSid)
{
  dprintf(("ADVAPI32: CopySid(%08xh,%08xh,%08xh)\n",
           nDestinationSidLength,
           pDestinationSid,
           pSourceSid));

  memcpy((LPVOID)pDestinationSid, /* that's all :) */
         (LPVOID)pSourceSid,
         nDestinationSidLength);

  return (TRUE);
}


/*****************************************************************************
 * Name      : CreatePrivateObjectSecurity
 * Purpose   : The CreatePrivateObjectSecurity function allocates and initializes
 *             a self-relative security descriptor for a new protected server's
 *             object. This function is called when a new protected server object is being created.
 * Parameters: PSECURITY_DESCRIPTOR  ParentDescriptor  address of parent directory SD
 *             PSECURITY_DESCRIPTOR  CreatorDescriptor address of creator SD
 *             PSECURITY_DESCRIPTOR *NewDescriptor     address of pointer to new SD
 *             BOOL                  IsDirectoryObject container flag for new SD
 *             HANDLE                Token             handle of client's access token
 *             PGENERIC_MAPPING      GenericMapping    address of access-rights structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CreatePrivateObjectSecurity(PSECURITY_DESCRIPTOR  ParentDescriptor,
                                             PSECURITY_DESCRIPTOR  CreatorDescriptor,
                                             PSECURITY_DESCRIPTOR *NewDescriptor,
                                             BOOL                  IsDirectoryObject,
                                             HANDLE                Token,
                                             PGENERIC_MAPPING      GenericMapping)
{
  dprintf(("ADVAPI32: CreatePrivateObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ParentDescriptor,
           CreatorDescriptor,
           NewDescriptor,
           IsDirectoryObject,
           Token,
           GenericMapping));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CreateProcessAsUserA
 * Purpose   : The CreateProcessAsUser function creates a new process and its
 *             primary thread. The new process then executes a specified executable
 *             file. The CreateProcessAsUser function behaves just like the
 *             CreateProcess function, with one important difference: the
 *             created process runs in a context in which the system sees the
 *             user represented by the hToken parameter as if that user had
 *             logged on to the system and then called the CreateProcess function.
 * Parameters: HANDLE                 hToken               handle to a token that represents a logged-on user
 *             LPCSTR                lpApplicationName    pointer to name of executable module
 *             LPTSTR                 lpCommandLine        pointer to command line string
 *             LPSECURITY_ATTRIBUTES  lpProcessAttributes  pointer to process security attributes
 *             LPSECURITY_ATTRIBUTES  lpThreadAttributes   pointer to thread security attributes
 *             BOOL                   bInheritHandles      new process inherits handles
 *             DWORD                  dwCreationFlags      creation flags
 *             LPVOID                 lpEnvironment        pointer to new environment block
 *             LPCSTR                lpCurrentDirectory   pointer to current directory name
 *             LPSTARTUPINFO          lpStartupInfo        pointer to STARTUPINFO
 *             LPPROCESS_INFORMATION  lpProcessInformation pointer to PROCESS_INFORMATION
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CreateProcessAsUserA(HANDLE                 hToken,
                                      LPCSTR                lpApplicationName,
                                      LPTSTR                 lpCommandLine,
                                      LPSECURITY_ATTRIBUTES  lpProcessAttributes,
                                      LPSECURITY_ATTRIBUTES  lpThreadAttributes,
                                      BOOL                   bInheritHandles,
                                      DWORD                  dwCreationFlags,
                                      LPVOID                 lpEnvironment,
                                      LPCSTR                 lpCurrentDirectory,
                                      LPSTARTUPINFOA         lpStartupInfo,
                                      LPPROCESS_INFORMATION  lpProcessInformation)
{
  dprintf(("ADVAPI32: CreateProcessAsUserA(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%s,%08xh,%08xh) not implemented.\n",
           hToken,
           lpApplicationName,
           lpCommandLine,
           lpProcessAttributes,
           lpThreadAttributes,
           bInheritHandles,
           dwCreationFlags,
           lpEnvironment,
           lpCurrentDirectory,
           lpStartupInfo,
           lpProcessInformation));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CreateProcessAsUserW
 * Purpose   : The CreateProcessAsUser function creates a new process and its
 *             primary thread. The new process then executes a specified executable
 *             file. The CreateProcessAsUser function behaves just like the
 *             CreateProcess function, with one important difference: the
 *             created process runs in a context in which the system sees the
 *             user represented by the hToken parameter as if that user had
 *             logged on to the system and then called the CreateProcess function.
 * Parameters: HANDLE                 hToken               handle to a token that represents a logged-on user
 *             LPCWSTR                lpApplicationName    pointer to name of executable module
 *             LPWSTR                 lpCommandLine        pointer to command line string
 *             LPSECURITY_ATTRIBUTES  lpProcessAttributes  pointer to process security attributes
 *             LPSECURITY_ATTRIBUTES  lpThreadAttributes   pointer to thread security attributes
 *             BOOL                   bInheritHandles      new process inherits handles
 *             DWORD                  dwCreationFlags      creation flags
 *             LPVOID                 lpEnvironment        pointer to new environment block
 *             LPCWSTR                lpCurrentDirectory   pointer to current directory name
 *             LPSTARTUPINFO          lpStartupInfo        pointer to STARTUPINFO
 *             LPPROCESS_INFORMATION  lpProcessInformation pointer to PROCESS_INFORMATION
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API CreateProcessAsUserW(HANDLE                 hToken,
                                      LPCWSTR                lpApplicationName,
                                      LPWSTR                 lpCommandLine,
                                      LPSECURITY_ATTRIBUTES  lpProcessAttributes,
                                      LPSECURITY_ATTRIBUTES  lpThreadAttributes,
                                      BOOL                   bInheritHandles,
                                      DWORD                  dwCreationFlags,
                                      LPVOID                 lpEnvironment,
                                      LPCWSTR                lpCurrentDirectory,
                                      LPSTARTUPINFOA         lpStartupInfo,
                                      LPPROCESS_INFORMATION  lpProcessInformation)
{
  dprintf(("ADVAPI32: CreateProcessAsUserW(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%s,%08xh,%08xh) not implemented.\n",
           hToken,
           lpApplicationName,
           lpCommandLine,
           lpProcessAttributes,
           lpThreadAttributes,
           bInheritHandles,
           dwCreationFlags,
           lpEnvironment,
           lpCurrentDirectory,
           lpStartupInfo,
           lpProcessInformation));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : CreateServiceA
 * Purpose   : The CreateService function creates a service object and adds it
 *             to the specified service control manager database.
 * Parameters: SC_HANDLE hSCManager         handle of service control manager database
 *             LPCSTR   lpServiceName      address of name of service to start
 *             LPCSTR   lpDisplayName      address of display name
 *             DWORD     dwDesiredAccess    type of access to service
 *             DWORD     dwServiceType      type of service
 *             DWORD     dwStartType        when to start service
 *             DWORD     dwErrorControl     severity if service fails to start
 *             LPCSTR   lpBinaryPathName   address of name of binary file
 *             LPCSTR   lpLoadOrderGroup   address of name of load ordering group
 *             LPDWORD   lpdwTagId          address of variable to get tag identifier
 *             LPCSTR   lpDependencies     address of array of dependency names
 *             LPCSTR   lpServiceStartName address of account name of service
 *             LPCSTR   lpPassword         address of password for service account
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API CreateServiceA(SC_HANDLE hSCManager,
                                     LPCSTR   lpServiceName,
                                     LPCSTR   lpDisplayName,
                                     DWORD     dwDesiredAccess,
                                     DWORD     dwServiceType,
                                     DWORD     dwStartType,
                                     DWORD     dwErrorControl,
                                     LPCSTR   lpBinaryPathName,
                                     LPCSTR   lpLoadOrderGroup,
                                     LPDWORD   lpdwTagId,
                                     LPCSTR   lpDependencies,
                                     LPCSTR   lpServiceStartName,
                                     LPCSTR   lpPassword)
{
  dprintf(("ADVAPI32: CreateServiceA(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s) not implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           dwDesiredAccess,
           dwServiceType,
           dwStartType,
           dwErrorControl,
           lpBinaryPathName,
           lpLoadOrderGroup,
           lpdwTagId,
           lpDependencies,
           lpServiceStartName,
           lpPassword));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : CreateServiceW
 * Purpose   : The CreateService function creates a service object and adds it
 *             to the specified service control manager database.
 * Parameters: SC_HANDLE hSCManager         handle of service control manager database
 *             LPCWSTR   lpServiceName      address of name of service to start
 *             LPCWSTR   lpDisplayName      address of display name
 *             DWORD     dwDesiredAccess    type of access to service
 *             DWORD     dwServiceType      type of service
 *             DWORD     dwStartType        when to start service
 *             DWORD     dwErrorControl     severity if service fails to start
 *             LPCWSTR   lpBinaryPathName   address of name of binary file
 *             LPCWSTR   lpLoadOrderGroup   address of name of load ordering group
 *             LPDWORD   lpdwTagId          address of variable to get tag identifier
 *             LPCWSTR   lpDependencies     address of array of dependency names
 *             LPCWSTR   lpServiceStartName address of account name of service
 *             LPCWSTR   lpPassword         address of password for service account
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API CreateServiceW(SC_HANDLE hSCManager,
                                     LPCWSTR   lpServiceName,
                                     LPCWSTR   lpDisplayName,
                                     DWORD     dwDesiredAccess,
                                     DWORD     dwServiceType,
                                     DWORD     dwStartType,
                                     DWORD     dwErrorControl,
                                     LPCWSTR   lpBinaryPathName,
                                     LPCWSTR   lpLoadOrderGroup,
                                     LPDWORD   lpdwTagId,
                                     LPCWSTR   lpDependencies,
                                     LPCWSTR   lpServiceStartName,
                                     LPCWSTR   lpPassword)
{
  dprintf(("ADVAPI32: CreateServiceW(%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%s,%s,%08xh,%s,%s,%s) not implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           dwDesiredAccess,
           dwServiceType,
           dwStartType,
           dwErrorControl,
           lpBinaryPathName,
           lpLoadOrderGroup,
           lpdwTagId,
           lpDependencies,
           lpServiceStartName,
           lpPassword));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : DeleteAce
 * Purpose   : The DeleteAce function deletes an ACE from an ACL.
 *             An ACE is an access-control entry. An ACL is an access-control list.
 * Parameters: PACL  pAcl        address of access-control list
 *             DWORD dwAceIndex  index of ACE position in ACL
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API DeleteAce(PACL  pAcl,
                           DWORD dwAceIndex)
{
  dprintf(("ADVAPI32: DeleteAce(%08xh, %08xh) not implemented.\n",
           pAcl,
           dwAceIndex));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : DeleteService
 * Purpose   : The DeleteService function marks the specified service for
 *             deletion from the service control manager database.
 * Parameters: SC_HANDLE  hService  handle of service
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API DeleteService(SC_HANDLE hService)
{
  dprintf(("ADVAPI32: DeleteService(%08xh) not implemented.\n",
           hService));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : DestroyPrivateObjectSecurity
 * Purpose   : The DestroyPrivateObjectSecurity function deletes a protected
 *             server object's security descriptor. This security descriptor
 *             must have been created by a call to the CreatePrivateObjectSecurity function.
 * Parameters: PSECURITY_DESCRIPTOR  * ObjectDescriptor  address of pointer to SECURITY_DESCRIPTOR
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API DestroyPrivateObjectSecurity(PSECURITY_DESCRIPTOR *ObjectDescriptor)
{
  dprintf(("ADVAPI32: DestroyPrivateObjectSecurity(%08xh) not implemented.\n",
           ObjectDescriptor));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : DuplicateToken
 * Purpose   : The DuplicateToken function creates a new access token that
 *             duplicates one already in existence.
 * Parameters: HANDLE                       ExistingTokenHandle  handle of token to duplicate
 *             SECURITY_IMPERSONATION_LEVEL ImpersonationLevel   impersonation level
 *             PHANDLE                      DuplicateTokenHandle handle of duplicated token
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define SECURITY_IMPERSONATION_LEVEL DWORD
BOOL WIN32API DuplicateToken(HANDLE                       ExistingTokenHandle,
                                SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
                                PHANDLE                      DuplicateTokenHandle)
{
  dprintf(("ADVAPI32: DuplicateToken(%08x,%08xh,%08xh) not implemented.\n",
           ExistingTokenHandle,
           ImpersonationLevel,
           DuplicateTokenHandle));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : EnumDependentServicesA
 * Purpose   : The EnumDependentServices function enumerates services that
 *             depend on another specified service; that is, the specified
 *             service must be running before the enumerated services can run.
 *             The name and status of each dependent service are provided.
 * Parameters: SC_HANDLE             hService           handle of service
 *             DWORD                 dwServiceState     state of services to enumerate
 *             LPENUM_SERVICE_STATUS lpServices         address of service status buffer
 *             DWORD                 cbBufSize          size of service status buffer
 *             LPDWORD               pcbBytesNeeded     address of variable for bytes needed
 *             LPDWORD               lpServicesReturned address of variable for number returned
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define LPENUM_SERVICE_STATUS LPVOID
BOOL WIN32API EnumDependentServicesA(SC_HANDLE             hService,
                                        DWORD                 dwServiceState,
                                        LPENUM_SERVICE_STATUS lpServices,
                                        DWORD                 cbBufSize,
                                        LPDWORD               pcbBytesNeeded,
                                        LPDWORD               lpServicesReturned)
{
  dprintf(("ADVAPI32: EnumDependentServicesA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hService,
           dwServiceState,
           lpServices,
           cbBufSize,
           pcbBytesNeeded,
           lpServicesReturned));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : EnumDependentServicesW
 * Purpose   : The EnumDependentServices function enumerates services that
 *             depend on another specified service; that is, the specified
 *             service must be running before the enumerated services can run.
 *             The name and status of each dependent service are provided.
 * Parameters: SC_HANDLE             hService           handle of service
 *             DWORD                 dwServiceState     state of services to enumerate
 *             LPENUM_SERVICE_STATUS lpServices         address of service status buffer
 *             DWORD                 cbBufSize          size of service status buffer
 *             LPDWORD               pcbBytesNeeded     address of variable for bytes needed
 *             LPDWORD               lpServicesReturned address of variable for number returned
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EnumDependentServicesW(SC_HANDLE             hService,
                                        DWORD                 dwServiceState,
                                        LPENUM_SERVICE_STATUS lpServices,
                                        DWORD                 cbBufSize,
                                        LPDWORD               pcbBytesNeeded,
                                        LPDWORD               lpServicesReturned)
{
  dprintf(("ADVAPI32: EnumDependentServicesW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hService,
           dwServiceState,
           lpServices,
           cbBufSize,
           pcbBytesNeeded,
           lpServicesReturned));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : EnumServicesStatusA
 * Purpose   : The EnumServicesStatus function enumerates services in the specified
 *             service control manager database. The name and status of each service are provided.
 * Parameters: SC_HANDLE             hSCManager          handle of service control manager database
 *             DWORD                 dwServiceType       type of services to enumerate
 *             DWORD                 dwServiceState      state of services to enumerate
 *             LPENUM_SERVICE_STATUS lpServices          address of service status buffer
 *             DWORD                 cbBufSize           size of service status buffer
 *             LPDWORD               pcbBytesNeeded      address of variable for bytes needed
 *             LPDWORD               lpServicesReturned  address of variable for number returned
 *             LPDWORD               lpResumeHandle      address of variable for next entry
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EnumServicesStatusA(SC_HANDLE             hSCManager,
                                     DWORD                 dwServiceType,
                                     DWORD                 dwServiceState,
                                     LPENUM_SERVICE_STATUS lpServices,
                                     DWORD                 cbBufSize,
                                     LPDWORD               pcbBytesNeeded,
                                     LPDWORD               lpServicesReturned,
                                     LPDWORD               lpResumeHandle)
{
  dprintf(("ADVAPI32: EnumServicesStatusA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           dwServiceType,
           dwServiceState,
           lpServices,
           cbBufSize,
           pcbBytesNeeded,
           lpServicesReturned,
           lpResumeHandle));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : EnumServicesStatusW
 * Purpose   : The EnumServicesStatus function enumerates services in the specified
 *             service control manager database. The name and status of each service are provided.
 * Parameters: SC_HANDLE             hSCManager          handle of service control manager database
 *             DWORD                 dwServiceType       type of services to enumerate
 *             DWORD                 dwServiceState      state of services to enumerate
 *             LPENUM_SERVICE_STATUS lpServices          address of service status buffer
 *             DWORD                 cbBufSize           size of service status buffer
 *             LPDWORD               pcbBytesNeeded      address of variable for bytes needed
 *             LPDWORD               lpServicesReturned  address of variable for number returned
 *             LPDWORD               lpResumeHandle      address of variable for next entry
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EnumServicesStatusW(SC_HANDLE             hSCManager,
                                     DWORD                 dwServiceType,
                                     DWORD                 dwServiceState,
                                     LPENUM_SERVICE_STATUS lpServices,
                                     DWORD                 cbBufSize,
                                     LPDWORD               pcbBytesNeeded,
                                     LPDWORD               lpServicesReturned,
                                     LPDWORD               lpResumeHandle)
{
  dprintf(("ADVAPI32: EnumServicesStatusW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           dwServiceType,
           dwServiceState,
           lpServices,
           cbBufSize,
           pcbBytesNeeded,
           lpServicesReturned,
           lpResumeHandle));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : EqualPrefixSid
 * Purpose   : The EqualPrefixSid function tests two security-identifier (SID)
 *             prefix values for equality. An SID prefix is the entire SID except
 *             for the last subauthority value.
 * Parameters: PSID  pSid1  address of first SID to compare
 *             PSID  pSid2  address of second SID to compare
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EqualPrefixSid(PSID pSid1,
                                PSID pSid2)
{
  dprintf(("ADVAPI32: EqualPrefixSid(%08xh,%08xh) not correctly implemented.\n",
           pSid1,
           pSid2));

  return ( lstrcmpA( (LPCSTR)pSid1,
                        (LPCSTR)pSid2) == 0 );       /* @@@PH roughly ... :) */
}


/*****************************************************************************
 * Name      : EqualSid
 * Purpose   : The EqualSid function tests two security identifier (SID) values
 *             for equality. Two SIDs must match exactly to be considered equal.
 * Parameters: PSID  pSid1  address of first SID to compare
 *             PSID  pSid2  address of second SID to compare
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API EqualSid(PSID pSid1,
                          PSID pSid2)
{
  dprintf(("ADVAPI32: EqualSid(%08xh, %08xh) not correctly implemented.\n",
           pSid1,
           pSid2));

  return ( lstrcmpA( (LPCSTR)pSid1,
                        (LPCSTR)pSid2) == 0 );       /* @@@PH roughly ... :) */
}


/*****************************************************************************
 * Name      : FindFirstFreeAce
 * Purpose   : The FindFirstFreeAce function retrieves a pointer to the first
 *             free byte in an access-control list (ACL).
 * Parameters: PACL   pAcl   address of access-control list
 *             LPVOID *pAce  address of pointer to first free byte
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API FindFirstFreeAce(PACL pAcl,
                                  LPVOID *pAce)
{
  dprintf(("ADVAPI32: FindFirstFreeAce(%08xh, %08xh) not implemented.\n",
           pAcl,
           pAce));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : FreeSid
 * Purpose   : The FreeSid function frees a security identifier (SID) previously
 *             allocated by using the AllocateAndInitializeSid function.
 * Parameters: PSID  pSid   address of SID to free
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

VOID WIN32API FreeSid(PSID pSid)
{
  dprintf(("ADVAPI32: FreeSid(%08xh) not implemented.\n",
           pSid));
}


/*****************************************************************************
 * Name      : GetAce
 * Purpose   : The GetAce function obtains a pointer to an ACE in an ACL.
 *             An ACE is an access control entry. An ACL is an access control list.
 * Parameters: PACL   pAcl        address of access-control list
 *             DWORD  dwAceIndex  index of ACE to retrieve
 *             LPVOID *pAce       address of pointer to ACE
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetAce(PACL   pAcl,
                        DWORD  dwAceIndex,
                        LPVOID *pAce)
{
  dprintf(("ADVAPI32: GetAce(%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           dwAceIndex,
           pAce));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetAclInformation
 * Purpose   : The GetAclInformation function retrieves information about an
 *             access-control list (ACL).
 * Parameters: PACL                  pAcl                  address of access-control list
 *             LPVOID                pAclInformation       address of ACL information
 *             DWORD                 nAclInformationLength size of ACL information
 *             ACL_INFORMATION_CLASS dwAclInformationClass class of requested information
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define ACL_INFORMATION_CLASS DWORD
BOOL WIN32API GetAclInformation(PACL                  pAcl,
                                   LPVOID                pAclInformation,
                                   DWORD                 nAclInformationLength,
                                   ACL_INFORMATION_CLASS dwAclInformationClass)
{
  dprintf(("ADVAPI32: GetAclInformation(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           pAclInformation,
           nAclInformationLength,
           dwAclInformationClass));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetKernelObjectSecurity
 * Purpose   : The GetKernelObjectSecurity function retrieves a copy of the
 *             security descriptor protecting a kernel object.
 * Parameters: HANDLE                Handle                handle of object to query
 *             SECURITY_INFORMATION  RequestedInformation  requested information
 *             PSECURITY_DESCRIPTOR  pSecurityDescriptor   address of security descriptor
 *             DWORD                 nLength               size of buffer for security descriptor
 *             LPDWORD               lpnLengthNeeded      address of required size of buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetKernelObjectSecurity(HANDLE               Handle,
                                         SECURITY_INFORMATION RequestedInformation,
                                         PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                         DWORD                nLength,
                                         LPDWORD              lpnLengthNeeded)
{
  dprintf(("ADVAPI32: GetKernelObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           Handle,
           RequestedInformation,
           pSecurityDescriptor,
           nLength,
           lpnLengthNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetLengthSid
 * Purpose   : The GetLengthSid function returns the length, in bytes, of a
 *             valid SID structure. A SID is a security identifier.
 * Parameters: PSID  pSid  address of SID to query
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

DWORD WIN32API GetLengthSid(PSID pSid)
{
  dprintf(("ADVAPI32: GetLengthSid(%08xh) not correctly implemented.\n",
           pSid));

  return (lstrlenA( (LPCSTR)pSid)); /* @@@PH might not work */
}


/*****************************************************************************
 * Name      : GetNumberOfEventLogRecords
 * Purpose   : The GetNumberOfEventLogRecords function retrieves the number of
 *             records in the specified event log.
 * Parameters: HANDLE  hEventLog        handle to event log
 *             LPDWORD  NumberOfRecords  buffer for number of records
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetNumberOfEventLogRecords(HANDLE hEventLog,
                                            LPDWORD NumberOfRecords)
{
  dprintf(("ADVAPI32: GetNumberOfEventLogRecords(%08xh,%08xh) not implemented.\n",
           hEventLog,
           NumberOfRecords));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetOldestEventLogRecord
 * Purpose   : The GetOldestEventLogRecord function retrieves the absolute
 *             record number of the oldest record in the specified event log.
 * Parameters: HANDLE  hEventLog     handle to event log
 *             LPDWORD  OldestRecord  buffer for number of oldest record
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetOldestEventLogRecord(HANDLE hEventLog,
                                         LPDWORD OldestRecord)
{
  dprintf(("ADVAPI32: GetOldestEventLogRecord(%08xh,%08xh) not implemented.\n",
           hEventLog,
           OldestRecord));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetPrivateObjectSecurity
 * Purpose   : The GetPrivateObjectSecurity retrieves information from a
 *             protected server object's security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR ObjectDescriptor    address of SD to query
 *             SECURITY_INFORMATION SecurityInformation requested information
 *             PSECURITY_DESCRIPTOR ResultantDescriptor address of retrieved SD
 *             DWORD                DescriptorLength    size of buffer for retrieved SD
 *             LPDWORD               ReturnLength        address of buffer size required for SD
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetPrivateObjectSecurity(PSECURITY_DESCRIPTOR ObjectDescriptor,
                                          SECURITY_INFORMATION SecurityInformation,
                                          PSECURITY_DESCRIPTOR ResultantDescriptor,
                                          DWORD                DescriptorLength,
                                          LPDWORD               ReturnLength)
{
  dprintf(("ADVAPI32: GetPrivateObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           ObjectDescriptor,
           SecurityInformation,
           ResultantDescriptor,
           DescriptorLength,
           ReturnLength));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetSecurityDescriptorControl
 * Purpose   : The GetSecurityDescriptorControl function retrieves a security
 *             descriptor's control and revision information.
 * Parameters: PSECURITY_DESCRIPTOR         pSecurityDescriptor address of security descriptor
 *             PSECURITY_DESCRIPTOR_CONTROL pControl            address of  control structure
 *             LPDWORD                      lpdwRevision        address of revision value
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define PSECURITY_DESCRIPTOR_CONTROL LPVOID
BOOL WIN32API GetSecurityDescriptorControl(PSECURITY_DESCRIPTOR         pSecurityDescriptor,
                                              PSECURITY_DESCRIPTOR_CONTROL pControl,
                                              LPDWORD                      lpdwRevision)
{
  dprintf(("ADVAPI32: GetSecurityDescriptorControl(%08xh,%08xh,%08xh) not implemented.\n",
           pSecurityDescriptor,
           pControl,
           lpdwRevision));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetSecurityDescriptorDacl
 * Purpose   : The GetSecurityDescriptorDacl function retrieves a pointer to the
 *             discretionary access-control list (ACL) in a specified security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR pSecurityDescriptor address of security descriptor
 *             LPBOOL               lpbDaclPresent      address of flag for presence of disc. ACL
 *             PACL                 *pDacl              address of pointer to ACL
 *             LPBOOL               lpbDaclDefaulted    address of flag for default disc. ACL
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetSecurityDescriptorDacl(PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                           LPBOOL               lpbDaclPresent,
                                           PACL                 *pDacl,
                                           LPBOOL               lpbDaclDefaulted)
{
  dprintf(("ADVAPI32: GetSecurityDescriptorDacl(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pSecurityDescriptor,
           lpbDaclPresent,
           pDacl,
           lpbDaclDefaulted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetSecurityDescriptorGroup
 * Purpose   : The GetSecurityDescriptorGroup function retrieves the primary
 *             group information from a security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR pSecurityDescriptor  address of security descriptor
 *             PSID                 *pGroup              address of pointer to group security identifier (SID)
 *             LPBOOL               lpbGroupDefaulted    address of flag for default
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetSecurityDescriptorGroup(PSECURITY_DESCRIPTOR  pSecurityDescriptor,
                                            PSID                  *pGroup,
                                            LPBOOL                lpbGroupDefaulted)
{
  dprintf(("ADVAPI32: GetSecurityDescriptorGroup(%08xh,%08xh,%08xh) not implemented.\n",
           pSecurityDescriptor,
           pGroup,
           lpbGroupDefaulted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetSecurityDescriptorLength
 * Purpose   : The GetSecurityDescriptorLength function returns the length, in
 *             bytes, of a structurally valid SECURITY_DESCRIPTOR structure. The
 *             length includes the length of all associated structures, such as
 *             SID and ACL structures.
 * Parameters: PSECURITY_DESCRIPTOR  pSecurityDescriptor address of security descriptor
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define SECURITY_DESCRIPTOR DWORD
DWORD WIN32API GetSecurityDescriptorLength(PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
  dprintf(("ADVAPI32: GetSecurityDescriptorLength(%08xh) not correctly implemented.\n",
           pSecurityDescriptor));

  return ( sizeof(SECURITY_DESCRIPTOR) );
}


/*****************************************************************************
 * Name      : GetSecurityDescriptorOwner
 * Purpose   : The GetSecurityDescriptorOwner function retrieves the owner
 *             information from a security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR pSecurityDescriptor address of security descriptor
 *             PSID                 *pOwner             address of pointer to owner security identifier (SID)
 *             LPBOOL               lpbOwnerDefaulted   address of flag for default
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetSecurityDescriptorOwner(PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                            PSID                 *pOwner,
                                            LPBOOL               lpbOwnerDefaulted)
{
  dprintf(("ADVAPI32: GetSecurityDescriptorOwner(%08xh,%08xh,%08xh) not implemented.\n",
           pSecurityDescriptor,
           pOwner,
           lpbOwnerDefaulted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetSecurityDescriptorSacl
 * Purpose   : The GetSecurityDescriptorSacl function retrieves a pointer to
 *             the system access-control list (ACL) in a specified security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR pSecurityDescriptor address of security descriptor
 *             LPBOOL               lpbSaclPresent      address of flag for presence of system ACL
 *             PACL                 *pSacl              address of pointer to ACL
 *             LPBOOL               lpbSaclDefaulted    address of flag for default system ACL
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetSecurityDescriptorSacl(PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                           LPBOOL               lpbSaclPresent,
                                           PACL                 *pSacl,
                                           LPBOOL               lpbSaclDefaulted)
{
  dprintf(("ADVAPI32: GetSecurityDescriptorSacl(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pSecurityDescriptor,
           lpbSaclPresent,
           pSacl,
           lpbSaclDefaulted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetServiceDisplayNameA
 * Purpose   : The GetServiceDisplayName function obtains the display name that
 *             is associated with a particular service name. The service name
 *             is the same as the service's registry key name.
 * Parameters: SC_HANDLE hSCManager     handle to a service control manager database
 *             LPCSTR   lpServiceName  the service name
 *             LPTSTR    lpDisplayName  buffer to receive the service's display name
 *             LPDWORD   lpcchBuffer    size of display name buffer and display name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetServiceDisplayNameA(SC_HANDLE hSCManager,
                                        LPCSTR   lpServiceName,
                                        LPTSTR    lpDisplayName,
                                        LPDWORD   lpcchBuffer)
{
  dprintf(("ADVAPI32: GetServiceDisplayNameA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           lpcchBuffer));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetServiceDisplayNameW
 * Purpose   : The GetServiceDisplayName function obtains the display name that
 *             is associated with a particular service name. The service name
 *             is the same as the service's registry key name.
 * Parameters: SC_HANDLE hSCManager     handle to a service control manager database
 *             LPCWSTR   lpServiceName  the service name
 *             LPWSTR    lpDisplayName  buffer to receive the service's display name
 *             LPDWORD   lpcchBuffer    size of display name buffer and display name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetServiceDisplayNameW(SC_HANDLE hSCManager,
                                        LPCWSTR   lpServiceName,
                                        LPWSTR    lpDisplayName,
                                        LPDWORD   lpcchBuffer)
{
  dprintf(("ADVAPI32: GetServiceDisplayNameW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpServiceName,
           lpDisplayName,
           lpcchBuffer));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetServiceKeyNameA
 * Purpose   : The GetServiceKeyName function obtains the service name that is
 *             associated with a particular service's display name. The service
 *             name is the same as the service's registry key name.
 * Parameters: SC_HANDLE hSCManager     handle to a service control manager database
 *             LPCSTR   lpDisplayName  the service's display name
 *             LPTSTR    lpServiceName  buffer to receive the service name
 *             LPDWORD   lpcchBuffer    size of service name buffer and service name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetServiceKeyNameA(SC_HANDLE hSCManager,
                                    LPCSTR   lpDisplayName,
                                    LPTSTR    lpServiceName,
                                    LPDWORD   lpcchBuffer)
{
  dprintf(("ADVAPI32: GetServiceKeyNameA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpDisplayName,
           lpServiceName,
           lpcchBuffer));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetServiceKeyNameW
 * Purpose   : The GetServiceKeyName function obtains the service name that is
 *             associated with a particular service's display name. The service
 *             name is the same as the service's registry key name.
 * Parameters: SC_HANDLE hSCManager     handle to a service control manager database
 *             LPCWSTR   lpDisplayName  the service's display name
 *             LPWSTR    lpServiceName  buffer to receive the service name
 *             LPDWORD   lpcchBuffer    size of service name buffer and service name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API GetServiceKeyNameW(SC_HANDLE hSCManager,
                                    LPCWSTR   lpDisplayName,
                                    LPWSTR    lpServiceName,
                                    LPDWORD   lpcchBuffer)
{
  dprintf(("ADVAPI32: GetServiceKeyNameW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hSCManager,
           lpDisplayName,
           lpServiceName,
           lpcchBuffer));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : GetSidIdentifierAuthority
 * Purpose   : The GetSidIdentifierAuthority function returns the address of
 *             the SID_IDENTIFIER_AUTHORITY structure in a specified security identifier (SID).
 * Parameters: PSID  pSid   address of SID to query
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

PSID_IDENTIFIER_AUTHORITY WIN32API GetSidIdentifierAuthority(PSID pSid)
{
  dprintf(("ADVAPI32: GetSidIdentifierAuthority(%08xh) not implemented.\n",
           pSid));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : GetSidLengthRequired
 * Purpose   : The GetSidLengthRequired function returns the length, in bytes,
 *             of the buffer required to store a SID structure with a specified
 *             number of subauthorities.
 * Parameters: UCHAR  nSubAuthorityCount  count of subauthorities
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define SID DWORD
DWORD WIN32API GetSidLengthRequired(UCHAR nSubAuthorityCount)
{
  dprintf(("ADVAPI32: GetSidLengthRequired(%u) not correctly implemented.\n",
           nSubAuthorityCount));

  return ( sizeof(SID) );
}


/*****************************************************************************
 * Name      : GetSidSubAuthority
 * Purpose   : The GetSidSubAuthority function returns the address of a
 *             specified subauthority in a SID structure. The subauthority
 *             value is a relative identifier (RID). A SID is a security identifier.
 * Parameters: PSID  pSid          address of security identifier to query
 *             DWORD nSubAuthority index of subauthority to retrieve
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

LPDWORD WIN32API GetSidSubAuthority(PSID  pSid,
                                      DWORD nSubAuthority)
{
  dprintf(("ADVAPI32: GetSidSubAuthority(%08xh,%08xh) not implemented.\n",
           pSid,
           nSubAuthority));

  return ( (LPDWORD)pSid ); /* signal failure */
}


/*****************************************************************************
 * Name      : GetSidSubAuthorityCount
 * Purpose   : The GetSidSubAuthorityCount function returns the address of the
 *             field in a SID structure containing the subauthority count. A
 *             SID is a security identifier.
 * Parameters: PSID  pSid  address of security identifier to query
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

PUCHAR  WIN32API GetSidSubAuthorityCount(PSID pSid)
{
  static UCHAR ucSubAuthorityCount = 1;

  dprintf(("ADVAPI32: GetSidSubAuthorityCount(%08xh) not implemented.\n",
           pSid));

  return (&ucSubAuthorityCount);
}


/*****************************************************************************
 * Name      : GetTokenInformation
 * Purpose   : The GetTokenInformation function retrieves a specified type of
 *             information about an access token. The calling process must have
 *             appropriate access rights to obtain the information.
 * Parameters: HANDLE                   TokenHandle             handle of access token
 *             TOKEN_INFORMATION_CLASS  TokenInformationClass   type of information to retrieve
 *             LPVOID                   TokenInformation        address of retrieved information
 *             DWORD                    TokenInformationLength  size of information buffer
 *             LPDWORD                   ReturnLength            address of required buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define TOKEN_INFORMATION_CLASS DWORD
BOOL WIN32API GetTokenInformation(HANDLE                  TokenHandle,
                                     TOKEN_INFORMATION_CLASS TokenInformationClass,
                                     LPVOID                  TokenInformation,
                                     DWORD                   TokenInformationLength,
                                     LPDWORD                  ReturnLength)
{
  dprintf(("ADVAPI32: GetTokenInformation(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           TokenHandle,
           TokenInformationClass,
           TokenInformation,
           TokenInformationLength,
           ReturnLength));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ImpersonateLoggedOnUser
 * Purpose   : The ImpersonateLoggedOnUser function lets the calling thread
 *             impersonate a user. The user is represented by a token handle
 *             obtained by calling the LogonUser function
 * Parameters: HANDLE hToken
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ImpersonateLoggedOnUser(HANDLE hToken)
{
  dprintf(("ADVAPI32: ImpersonateLoggedOnUser(%08xh) not implemented.\n",
           hToken));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : ImpersonateNamedPipeClient
 * Purpose   : The ImpersonateNamedPipeClient function impersonates a named-pipe
 *             client application.
 * Parameters: HANDLE  hNamedPipe  handle of a named pipe
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ImpersonateNamedPipeClient(HANDLE hNamedPipe)
{
  dprintf(("ADVAPI32: ImpersonateNamedPipeClient(%08xh) not implemented.\n",
           hNamedPipe));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : ImpersonateSelf
 * Purpose   : The ImpersonateSelf function obtains an access token that
 *             impersonates the security context of the calling process. The
 *             token is assigned to the calling thread.
 * Parameters: SECURITY_IMPERSONATION_LEVEL  ImpersonationLevel  impersonation level
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ImpersonateSelf(SECURITY_IMPERSONATION_LEVEL ImpersonationLevel)
{
  dprintf(("ADVAPI32: ImpersonateSelf(%08xh) not implemented.\n",
           ImpersonationLevel));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : InitializeAcl
 * Purpose   : The InitializeAcl function creates a new ACL structure.
 *             An ACL is an access-control list.
 * Parameters: PACL  pAcl          address of access-control list
 *             DWORD nAclLength    size of access-control list
 *             DWORD dwAclRevision revision level of access-control list
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API InitializeAcl(PACL  pAcl,
                               DWORD nAclLength,
                               DWORD dwAclRevision)
{
  dprintf(("ADVAPI32: InitializeAcl(%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           nAclLength,
           dwAclRevision));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : InitializeSid
 * Purpose   : The InitializeSid function initializes a SID structure. An SID
 *             is a security identifier.
 * Parameters: PSID                      pSid                 address of SID to initialize
 *             PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority address of identifier authority
 *             BYTE                      nSubAuthorityCount   count of subauthorities
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API InitializeSid(PSID                      pSid,
                               PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
                               BYTE                      nSubAuthorityCount)
{
  dprintf(("ADVAPI32: InitializeSid(%08xh,%08xh,%08xh) not implemented.\n",
           pSid,
           pIdentifierAuthority,
           nSubAuthorityCount));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : InitiateSystemShutdownA
 * Purpose   : The InitiateSystemShutdown function initiates a shutdown and
 *             optional restart of the specified computer.
 * Parameters: LPTSTR lpMachineName        address of name of computer to shut down
 *             LPTSTR lpMessage            address of message to display in dialog box
 *             DWORD  dwTimeout            time to display dialog box
 *             BOOL   bForceAppsClosed     force applications with unsaved changes flag
 *             BOOL   bRebootAfterShutdown reboot flag
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API InitiateSystemShutdownA(LPTSTR lpMachineName,
                                         LPTSTR lpMessage,
                                         DWORD  dwTimeout,
                                         BOOL   bForceAppsClosed,
                                         BOOL   bRebootAfterShutdown)
{
  dprintf(("ADVAPI32: InitiateSystemShutdownA(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpMachineName,
           lpMessage,
           dwTimeout,
           bForceAppsClosed,
           bRebootAfterShutdown));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : InitiateSystemShutdownW
 * Purpose   : The InitiateSystemShutdown function initiates a shutdown and
 *             optional restart of the specified computer.
 * Parameters: LPWSTR lpMachineName        address of name of computer to shut down
 *             LPWSTR lpMessage            address of message to display in dialog box
 *             DWORD  dwTimeout            time to display dialog box
 *             BOOL   bForceAppsClosed     force applications with unsaved changes flag
 *             BOOL   bRebootAfterShutdown reboot flag
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API InitiateSystemShutdownW(LPWSTR lpMachineName,
                                         LPWSTR lpMessage,
                                         DWORD  dwTimeout,
                                         BOOL   bForceAppsClosed,
                                         BOOL   bRebootAfterShutdown)
{
  dprintf(("ADVAPI32: InitiateSystemShutdownW(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpMachineName,
           lpMessage,
           dwTimeout,
           bForceAppsClosed,
           bRebootAfterShutdown));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : IsTextUnicode
 * Purpose   : The IsTextUnicode function determines whether a buffer probably
 *             contains a form of Unicode text. The function uses various
 *             statistical and deterministic methods to make its determination,
 *             under the control of flags passed via lpi. When the function
 *             returns, the results of such tests are reported via lpi. If all
 *             specified tests are passed, the function returns TRUE; otherwise,
 *             it returns FALSE.
 * Parameters: CONST LPVOID lpBuffer pointer to an input buffer to be examined
 *             int          cb       the size in bytes of the input buffer
 *             LPINT        lpi      pointer to flags that condition text examination and receive results
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

DWORD WIN32API IsTextUnicode(CONST LPVOID  lpBuffer,
                                int           cb,
                                LPINT         lpi)
{
  DWORD dwResult = 0;

  dprintf(("ADVAPI32: IsTextUnicode(%08xh,%08xh,%08xh) not implemented.\n",
           lpBuffer,
           cb,
           lpi));

  if (cb & 0x0001) dwResult |= IS_TEXT_UNICODE_ODD_LENGTH;

  return (dwResult); /* signal failure */
}


/*****************************************************************************
 * Name      : IsValidAcl
 * Purpose   : The IsValidAcl function validates an access-control list (ACL).
 * Parameters: PACL  pAcl  address of access-control list
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API IsValidAcl(PACL pAcl)
{
  dprintf(("ADVAPI32: IsValidAcl(%08xh) not implemented.\n",
           pAcl));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : IsValidSecurityDescriptor
 * Purpose   : The IsValidSecurityDescriptor function validates a
 *             SECURITY_DESCRIPTOR structure. Validation is performed by
 *             checking the revision level of each component in the security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR pSecurityDescriptor
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API IsValidSecurityDescriptor(PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
  dprintf(("ADVAPI32: IsValidSecurityDescriptor(%08xh) not implemented.\n",
           pSecurityDescriptor));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : IsValidSid
 * Purpose   : The IsValidSid function validates a SID structure by verifying
 *             that the revision number is within a known range and that the
 *             number of subauthorities is less than the maximum. A SID is a
 *             security identifier.
 * Parameters: PSID pSid  address of SID to query
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API IsValidSid(PSID pSid)
{
  dprintf(("ADVAPI32: IsValidSid(%08xh) not implemented.\n",
           pSid));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : LockServiceDatabase
 * Purpose   : The LockServiceDatabase function locks a specified database.
 * Parameters: SC_HANDLE  hSCManager  handle of service control manager database
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define SC_LOCK DWORD
SC_LOCK WIN32API LockServiceDatabase(SC_HANDLE hSCManager)
{
  dprintf(("ADVAPI32: LockServiceDatabase(%08xh) not implemented.\n",
           hSCManager));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : LogonUserA
 * Purpose   : The LogonUser function attempts to perform a user logon
 *             operation. You specify the user with a user name and domain,
 *             and authenticate the user with a clear-text password. If the
 *             function succeeds, you receive a handle to a token that
 *             represents the logged-on user. You can then use this token
 *             handle to impersonate the specified user, or to create a process
 *             running in the context of the specified user.
 * Parameters: LPTSTR  lpszUsername    string that specifies the user name
 *             LPTSTR  lpszDomain      string that specifies the domain or servero
 *             LPTSTR  lpszPassword    string that specifies the password
 *             DWORD   dwLogonType     specifies the type of logon operation
 *             DWORD   dwLogonProvider specifies the logon provider
 *             PHANDLE phToken         pointer to variable to receive token handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LogonUserA(LPTSTR  lpszUsername,
                            LPTSTR  lpszDomain,
                            LPTSTR  lpszPassword,
                            DWORD   dwLogonType,
                            DWORD   dwLogonProvider,
                            PHANDLE phToken)
{
  dprintf(("ADVAPI32: LogonUserA(%s,%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           lpszUsername,
           lpszDomain,
           lpszPassword,
           dwLogonType,
           dwLogonProvider,
           phToken));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : LogonUserW
 * Purpose   : The LogonUser function attempts to perform a user logon
 *             operation. You specify the user with a user name and domain,
 *             and authenticate the user with a clear-text password. If the
 *             function succeeds, you receive a handle to a token that
 *             represents the logged-on user. You can then use this token
 *             handle to impersonate the specified user, or to create a process
 *             running in the context of the specified user.
 * Parameters: LPWSTR  lpszUsername    string that specifies the user name
 *             LPWSTR  lpszDomain      string that specifies the domain or servero
 *             LPWSTR  lpszPassword    string that specifies the password
 *             DWORD   dwLogonType     specifies the type of logon operation
 *             DWORD   dwLogonProvider specifies the logon provider
 *             PHANDLE phToken         pointer to variable to receive token handle
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LogonUserW(LPWSTR  lpszUsername,
                            LPWSTR  lpszDomain,
                            LPWSTR  lpszPassword,
                            DWORD   dwLogonType,
                            DWORD   dwLogonProvider,
                            PHANDLE phToken)
{
  dprintf(("ADVAPI32: LogonUserW(%s,%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           lpszUsername,
           lpszDomain,
           lpszPassword,
           dwLogonType,
           dwLogonProvider,
           phToken));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : LookupAccountNameA
 * Purpose   : The LookupAccountName function accepts the name of a system and
 *             an account as input. It retrieves a security identifier (SID)
 *             for the account and the name of the domain on which the account was found.
 * Parameters: LPCSTR       lpSystemName           address of string for system name
 *             LPCSTR       lpAccountName          address of string for account name
 *             PSID          Sid                    address of security identifier
 *             LPDWORD       cbSid                  address of size of security identifier
 *             LPTSTR        ReferencedDomainName   address of string for referenced domain
 *             LPDWORD       cbReferencedDomainName address of size of domain string
 *             PSID_NAME_USE peUse                  address of SID-type indicator
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define PSID_NAME_USE LPVOID
BOOL WIN32API LookupAccountNameA(LPCSTR       lpSystemName,
                                    LPCSTR       lpAccountName,
                                    PSID          Sid,
                                    LPDWORD       cbSid,
                                    LPTSTR        ReferencedDomainName,
                                    LPDWORD       cbReferencedDomainName,
                                    PSID_NAME_USE peUse)
{
  dprintf(("ADVAPI32: LookupAccountNameA(%s,%s,%08xh,%08xh,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           lpAccountName,
           Sid,
           cbSid,
           ReferencedDomainName,
           cbReferencedDomainName,
           peUse));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupAccountNameW
 * Purpose   : The LookupAccountName function accepts the name of a system and
 *             an account as input. It retrieves a security identifier (SID)
 *             for the account and the name of the domain on which the account was found.
 * Parameters: LPCWSTR       lpSystemName           address of string for system name
 *             LPCWSTR       lpAccountName          address of string for account name
 *             PSID          Sid                    address of security identifier
 *             LPDWORD       cbSid                  address of size of security identifier
 *             LPWSTR        ReferencedDomainName   address of string for referenced domain
 *             LPDWORD       cbReferencedDomainName address of size of domain string
 *             PSID_NAME_USE peUse                  address of SID-type indicator
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupAccountNameW(LPCWSTR       lpSystemName,
                                    LPCWSTR       lpAccountName,
                                    PSID          Sid,
                                    LPDWORD       cbSid,
                                    LPWSTR        ReferencedDomainName,
                                    LPDWORD       cbReferencedDomainName,
                                    PSID_NAME_USE peUse)
{
  dprintf(("ADVAPI32: LookupAccountNameW(%s,%s,%08xh,%08xh,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           lpAccountName,
           Sid,
           cbSid,
           ReferencedDomainName,
           cbReferencedDomainName,
           peUse));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupAccountSidA
 * Purpose   : The LookupAccountSid function accepts a security identifier (SID)
 *             as input. It retrieves the name of the account for this SID and
 *             the name of the first domain on which this SID is found.
 * Parameters: LPCSTR       lpSystemName           address of string for system name
 *             PSID          Sid                    address of security identifier
 *             LPTSTR        Name                   address of string for account name
 *             LPDWORD       cbName                 address of size account string
 *             LPTSTR        ReferencedDomainName   address of string for referenced domain
 *             LPDWORD       cbReferencedDomainName address of size domain string
 *             PSID_NAME_USE peUse                  address of structure for SID type
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupAccountSidA(LPCSTR        lpSystemName,
                                   PSID           Sid,
                                   LPTSTR         Name,
                                   LPDWORD        cbName,
                                   LPTSTR         ReferencedDomainName,
                                   LPDWORD        cbReferencedDomainName,
                                   PSID_NAME_USE  peUse)
{
  dprintf(("ADVAPI32: LookupAccountSidA(%s,%08xh,%s,%08xh,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           Sid,
           Name,
           cbName,
           ReferencedDomainName,
           cbReferencedDomainName,
           peUse));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupAccountSidW
 * Purpose   : The LookupAccountSid function accepts a security identifier (SID)
 *             as input. It retrieves the name of the account for this SID and
 *             the name of the first domain on which this SID is found.
 * Parameters: LPCWSTR       lpSystemName           address of string for system name
 *             PSID          Sid                    address of security identifier
 *             LPWSTR        Name                   address of string for account name
 *             LPDWORD       cbName                 address of size account string
 *             LPWSTR        ReferencedDomainName   address of string for referenced domain
 *             LPDWORD       cbReferencedDomainName address of size domain string
 *             PSID_NAME_USE peUse                  address of structure for SID type
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupAccountSidW(LPCWSTR        lpSystemName,
                                   PSID           Sid,
                                   LPWSTR         Name,
                                   LPDWORD        cbName,
                                   LPWSTR         ReferencedDomainName,
                                   LPDWORD        cbReferencedDomainName,
                                   PSID_NAME_USE  peUse)
{
  dprintf(("ADVAPI32: LookupAccountSidW(%s,%08xh,%s,%08xh,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           Sid,
           Name,
           cbName,
           ReferencedDomainName,
           cbReferencedDomainName,
           peUse));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupPrivilegeDisplayNameA
 * Purpose   : The LookupPrivilegeDisplayName function retrieves a displayable
 *             name representing a specified privilege.
 * Parameters: LPCSTR lpSystemName   address of string specifying the system
 *             LPCSTR lpName         address of string specifying the privilege
 *             LPTSTR  lpDisplayName  address of string receiving the displayable name
 *             LPDWORD cbDisplayName  address of size of string for displayable name
 *             LPDWORD lpLanguageId   address of language identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupPrivilegeDisplayNameA(LPCSTR lpSystemName,
                                             LPCSTR lpName,
                                             LPTSTR  lpDisplayName,
                                             LPDWORD cbDisplayName,
                                             LPDWORD lpLanguageId)
{
  dprintf(("ADVAPI32: LookupPrivilegeDisplayNameA(%s,%s,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           lpName,
           lpDisplayName,
           cbDisplayName,
           lpLanguageId));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupPrivilegeDisplayNameW
 * Purpose   : The LookupPrivilegeDisplayName function retrieves a displayable
 *             name representing a specified privilege.
 * Parameters: LPCWSTR lpSystemName   address of string specifying the system
 *             LPCWSTR lpName         address of string specifying the privilege
 *             LPWSTR  lpDisplayName  address of string receiving the displayable name
 *             LPDWORD cbDisplayName  address of size of string for displayable name
 *             LPDWORD lpLanguageId   address of language identifier
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupPrivilegeDisplayNameW(LPCWSTR lpSystemName,
                                             LPCWSTR lpName,
                                             LPWSTR  lpDisplayName,
                                             LPDWORD cbDisplayName,
                                             LPDWORD lpLanguageId)
{
  dprintf(("ADVAPI32: LookupPrivilegeDisplayNameW(%s,%s,%s,%08xh,%08xh) not implemented.\n",
           lpSystemName,
           lpName,
           lpDisplayName,
           cbDisplayName,
           lpLanguageId));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupPrivilegeNameA
 * Purpose   : The LookupPrivilegeName function retrieves the name corresponding
 *             to the privilege represented on a specific system by a specified
 *             locally unique identifier (LUID).
 * Parameters: LPCSTR lpSystemName address of string specifying the system
 *             PLUID   lpLuid       address of locally unique identifier
 *             LPTSTR  lpName       address of string specifying the privilege
 *             LPDWORD cbName       address of size of string for displayable name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupPrivilegeNameA(LPCSTR lpSystemName,
                                      PLUID   lpLuid,
                                      LPTSTR  lpName,
                                      LPDWORD cbName)
{
  dprintf(("ADVAPI32: LookupPrivilegeNameA(%s,%08xh,%s,%08xh) not implemented.\n",
           lpSystemName,
           lpLuid,
           lpName,
           cbName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : LookupPrivilegeNameW
 * Purpose   : The LookupPrivilegeName function retrieves the name corresponding
 *             to the privilege represented on a specific system by a specified
 *             locally unique identifier (LUID).
 * Parameters: LPCWSTR lpSystemName address of string specifying the system
 *             PLUID   lpLuid       address of locally unique identifier
 *             LPWSTR  lpName       address of string specifying the privilege
 *             LPDWORD cbName       address of size of string for displayable name
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API LookupPrivilegeNameW(LPCWSTR lpSystemName,
                                      PLUID   lpLuid,
                                      LPWSTR  lpName,
                                      LPDWORD cbName)
{
  dprintf(("ADVAPI32: LookupPrivilegeNameW(%s,%08xh,%s,%08xh) not implemented.\n",
           lpSystemName,
           lpLuid,
           lpName,
           cbName));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : MakeAbsoluteSD
 * Purpose   : The MakeAbsoluteSD function creates a security descriptor in
 *             absolute format by using a security descriptor in self-relative
 *             format as a template.
 * Parameters: PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor    address self-relative SD
 *             PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor        address of absolute SD
 *             LPDWORD              lpdwAbsoluteSecurityDescriptorSize address of size of absolute SD
 *             PACL                 pDacl                              address of discretionary ACL
 *             LPDWORD              lpdwDaclSize                       address of size of discretionary ACL
 *             PACL                 pSacl                              address of system ACL
 *             LPDWORD              lpdwSaclSize                       address of size of system ACL
 *             PSID                 pOwner                             address of owner SID
 *             LPDWORD              lpdwOwnerSize                      address of size of owner SID
 *             PSID                 pPrimaryGroup                      address of primary-group SID
 *             LPDWORD              lpdwPrimaryGroupSize               address of size of group SID
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API MakeAbsoluteSD(PSECURITY_DESCRIPTOR  pSelfRelativeSecurityDescriptor,
                                PSECURITY_DESCRIPTOR  pAbsoluteSecurityDescriptor,
                                LPDWORD               lpdwAbsoluteSecurityDescriptorSize,
                                PACL                  pDacl,
                                LPDWORD               lpdwDaclSize,
                                PACL                  pSacl,
                                LPDWORD               lpdwSaclSize,
                                PSID                  pOwner,
                                LPDWORD               lpdwOwnerSize,
                                PSID                  pPrimaryGroup,
                                LPDWORD               lpdwPrimaryGroupSize)
{
  dprintf(("ADVAPI32: MakeAbsoluteSD(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pSelfRelativeSecurityDescriptor,
           pAbsoluteSecurityDescriptor,
           lpdwAbsoluteSecurityDescriptorSize,
           pDacl,
           lpdwDaclSize,
           pSacl,
           lpdwSaclSize,
           pOwner,
           lpdwOwnerSize,
           pPrimaryGroup,
           lpdwPrimaryGroupSize));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : MakeSelfRelativeSD
 * Purpose   : The MakeSelfRelativeSD function creates a security descriptor
 *             in self-relative format by using a security descriptor in absolute
 *             format as a template.
 * Parameters: PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor     address of absolute SD
 *             PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor address self-relative SD
 *             LPDWORD              lpdwBufferLength                address of SD size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API MakeSelfRelativeSD(PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor,
                                    PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
                                    LPDWORD              lpdwBufferLength)
{
  dprintf(("ADVAPI32: MakeSelfRelativeSD(%08xh,%08xh,%08xh) not implemented.\n",
           pAbsoluteSecurityDescriptor,
           pSelfRelativeSecurityDescriptor,
           lpdwBufferLength));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : MapGenericMask
 * Purpose   : The MapGenericMask function maps the generic access rights in
 *             an access mask to specific and standard access rights. The function
 *             applies a mapping supplied in a GENERIC_MAPPING structure.
 * Parameters: LPDWORD            AccessMask     address of access mask
 *             PGENERIC_MAPPING  GenericMapping address of GENERIC_MAPPING structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

VOID WIN32API MapGenericMask(LPDWORD           AccessMask,
                                PGENERIC_MAPPING GenericMapping)
{
  dprintf(("ADVAPI32: MapGenericMask(%08xh,%08xh) not implemented.\n",
           AccessMask,
           GenericMapping));
}


/*****************************************************************************
 * Name      : NotifyBootConfigStatus
 * Purpose   : The NotifyBootConfigStatus function notifies the service control
 *             manager as to the acceptability of the configuration that booted
 *             the system. An acceptable configuration triggers the storage of
 *             that configuration as the last-known good configuration; an
 *             unacceptable configuration triggers a system reboot.
 * Parameters: BOOL  BootAcceptable  indicates acceptability of boot configuration
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API NotifyBootConfigStatus(BOOL BootAcceptable)
{
  dprintf(("ADVAPI32: NotifyBootConfigStatus(%08xh) not implemented.\n",
           BootAcceptable));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : NotifyChangeEventLog
 * Purpose   : The NotifyChangeEventLog function lets an application receive
 *             notification when an event is written to the event log file
 *             specified by hEventLog. When the event is written to the event
 *             log file, the function causes the event object specified by
 *             hEvent to become signaled.
 * Parameters: HANDLE  hEventLog  special default locale value to be converted
 *             HANDLE  hEvent     special default locale value to be converted
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API NotifyChangeEventLog(HANDLE  hEventLog,
                                      HANDLE  hEvent)
{
  dprintf(("ADVAPI32: NotifyChangeEventLog(%08xh,%08xh) not implemented.\n",
           hEventLog,
           hEvent));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectCloseAuditAlarmA
 * Purpose   : The ObjectCloseAuditAlarm function generates audit messages when
 *             a handle of an object is deleted. Alarms are not supported in the
 *             current version of Windows NT.
 * Parameters: LPCSTR SubsystemName   address of string for subsystem name
 *             LPVOID  HandleId        address of handle identifier
 *             BOOL    GenerateOnClose flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectCloseAuditAlarmA(LPCSTR SubsystemName,
                                        LPVOID  HandleId,
                                        BOOL    GenerateOnClose)
{
  dprintf(("ADVAPI32: ObjectCloseAuditAlarmA(%s,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           GenerateOnClose));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectCloseAuditAlarmW
 * Purpose   : The ObjectCloseAuditAlarm function generates audit messages when
 *             a handle of an object is deleted. Alarms are not supported in the
 *             current version of Windows NT.
 * Parameters: LPCWSTR SubsystemName   address of string for subsystem name
 *             LPVOID  HandleId        address of handle identifier
 *             BOOL    GenerateOnClose flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectCloseAuditAlarmW(LPCWSTR SubsystemName,
                                        LPVOID  HandleId,
                                        BOOL    GenerateOnClose)
{
  dprintf(("ADVAPI32: ObjectCloseAuditAlarmW(%s,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           GenerateOnClose));

  return (FALSE); /* signal failure */
}



/*****************************************************************************
 * Name      : ObjectOpenAuditAlarmA
 * Purpose   : The ObjectOpenAuditAlarm function generates audit messages when
 *             a client application attempts to gain access to an object or to
 *             create a new one. Alarms are not supported in the current version
 *             of Windows NT.
 * Parameters: LPCSTR              SubsystemName       address of string for subsystem name
 *             LPVOID               HandleId            address of handle identifier
 *             LPTSTR               ObjectTypeName      address of string for object type
 *             LPTSTR               ObjectName          address of string for object name
 *             PSECURITY_DESCRIPTOR pSecurityDescriptor address of security descriptor
 *             HANDLE               ClientToken         handle of client's access token
 *             DWORD                DesiredAccess       mask for desired access rights
 *             DWORD                GrantedAccess       mask for granted access rights
 *             PPRIVILEGE_SET       Privileges          address of privileges
 *             BOOL                 ObjectCreation      flag for object creation
 *             BOOL                 AccessGranted       flag for results
 *             LPBOOL               GenerateOnClose     address of flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectOpenAuditAlarmA(LPCSTR              SubsystemName,
                                       LPVOID               HandleId,
                                       LPTSTR               ObjectTypeName,
                                       LPTSTR               ObjectName,
                                       PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                       HANDLE               ClientToken,
                                       DWORD                DesiredAccess,
                                       DWORD                GrantedAccess,
                                       PPRIVILEGE_SET       Privileges,
                                       BOOL                 ObjectCreation,
                                       BOOL                 AccessGranted,
                                       LPBOOL               GenerateOnClose)
{
  dprintf(("ADVAPI32: ObjectOpenAuditAlarmA(%s,%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           ObjectTypeName,
           ObjectName,
           pSecurityDescriptor,
           ClientToken,
           DesiredAccess,
           GrantedAccess,
           Privileges,
           ObjectCreation,
           AccessGranted,
           GenerateOnClose));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectOpenAuditAlarmW
 * Purpose   : The ObjectOpenAuditAlarm function generates audit messages when
 *             a client application attempts to gain access to an object or to
 *             create a new one. Alarms are not supported in the current version
 *             of Windows NT.
 * Parameters: LPCWSTR              SubsystemName       address of string for subsystem name
 *             LPVOID               HandleId            address of handle identifier
 *             LPWSTR               ObjectTypeName      address of string for object type
 *             LPWSTR               ObjectName          address of string for object name
 *             PSECURITY_DESCRIPTOR pSecurityDescriptor address of security descriptor
 *             HANDLE               ClientToken         handle of client's access token
 *             DWORD                DesiredAccess       mask for desired access rights
 *             DWORD                GrantedAccess       mask for granted access rights
 *             PPRIVILEGE_SET       Privileges          address of privileges
 *             BOOL                 ObjectCreation      flag for object creation
 *             BOOL                 AccessGranted       flag for results
 *             LPBOOL               GenerateOnClose     address of flag for audit generation
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectOpenAuditAlarmW(LPCWSTR              SubsystemName,
                                       LPVOID               HandleId,
                                       LPWSTR               ObjectTypeName,
                                       LPWSTR               ObjectName,
                                       PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                       HANDLE               ClientToken,
                                       DWORD                DesiredAccess,
                                       DWORD                GrantedAccess,
                                       PPRIVILEGE_SET       Privileges,
                                       BOOL                 ObjectCreation,
                                       BOOL                 AccessGranted,
                                       LPBOOL               GenerateOnClose)
{
  dprintf(("ADVAPI32: ObjectOpenAuditAlarmW(%s,%08xh,%s,%s,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           SubsystemName,
           HandleId,
           ObjectTypeName,
           ObjectName,
           pSecurityDescriptor,
           ClientToken,
           DesiredAccess,
           GrantedAccess,
           Privileges,
           ObjectCreation,
           AccessGranted,
           GenerateOnClose));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectPrivilegeAuditAlarmA
 * Purpose   : The ObjectPrivilegeAuditAlarm function generates audit messages
 *             as a result of a client's attempt to perform a privileged operation
 *             on a server application object using an already opened handle of
 *             that object. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCSTR        lpszSubsystem   address of string for subsystem name
 *             LPVOID         lpvHandleId     address of handle identifier
 *             HANDLE         hClientToken    handle of client's access token
 *             DWORD          dwDesiredAccess mask for desired access rights
 *             PPRIVILEGE_SET pps             address of privileges
 *             BOOL           fAccessGranted  flag for results
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectPrivilegeAuditAlarmA(LPCSTR        lpszSubsystem,
                                            LPVOID         lpvHandleId,
                                            HANDLE         hClientToken,
                                            DWORD          dwDesiredAccess,
                                            PPRIVILEGE_SET pps,
                                            BOOL           fAccessGranted)
{
  dprintf(("ADVAPI32: ObjectPrivilegeAuditAlarmA(%s,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpszSubsystem,
           lpvHandleId,
           hClientToken,
           dwDesiredAccess,
           pps,
           fAccessGranted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ObjectPrivilegeAuditAlarmW
 * Purpose   : The ObjectPrivilegeAuditAlarm function generates audit messages
 *             as a result of a client's attempt to perform a privileged operation
 *             on a server application object using an already opened handle of
 *             that object. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCWSTR        lpszSubsystem   address of string for subsystem name
 *             LPVOID         lpvHandleId     address of handle identifier
 *             HANDLE         hClientToken    handle of client's access token
 *             DWORD          dwDesiredAccess mask for desired access rights
 *             PPRIVILEGE_SET pps             address of privileges
 *             BOOL           fAccessGranted  flag for results
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ObjectPrivilegeAuditAlarmW(LPCWSTR        lpszSubsystem,
                                            LPVOID         lpvHandleId,
                                            HANDLE         hClientToken,
                                            DWORD          dwDesiredAccess,
                                            PPRIVILEGE_SET pps,
                                            BOOL           fAccessGranted)
{
  dprintf(("ADVAPI32: ObjectPrivilegeAuditAlarmW(%s,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           lpszSubsystem,
           lpvHandleId,
           hClientToken,
           dwDesiredAccess,
           pps,
           fAccessGranted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenBackupEventLogA
 * Purpose   : The OpenBackupEventLog function opens a handle of a backup event
 *             log. This handle can be used with the BackupEventLog function.
 * Parameters: LPCSTR  lpszUNCServerName  backup file server name
 *             LPCSTR  lpszFileName       backup filename
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

HANDLE WIN32API OpenBackupEventLogA(LPCSTR  lpszUNCServerName,
                                       LPCSTR  lpszFileName)
{
  dprintf(("ADVAPI32: OpenBackupEventLogA(%s,%s) not implemented.\n",
           lpszUNCServerName,
           lpszFileName));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenBackupEventLogW
 * Purpose   : The OpenBackupEventLog function opens a handle of a backup event
 *             log. This handle can be used with the BackupEventLog function.
 * Parameters: LPCWSTR  lpszUNCServerName  backup file server name
 *             LPCWSTR  lpszFileName       backup filename
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

HANDLE WIN32API OpenBackupEventLogW(LPCWSTR  lpszUNCServerName,
                                       LPCWSTR  lpszFileName)
{
  dprintf(("ADVAPI32: OpenBackupEventLogW(%s,%s) not implemented.\n",
           lpszUNCServerName,
           lpszFileName));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenEventLogA
 * Purpose   : The OpenEventLog function opens a handle of an event log.
 * Parameters: LPCSTR  lpszUNCServerName
 *             LPCSTR  lpszSourceName
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

HANDLE WIN32API OpenEventLogA(LPCSTR  lpszUNCServerName,
                                 LPCSTR  lpszSourceName)
{
  dprintf(("ADVAPI32: OpenEventLogA(%s,%s) not implemented.\n",
           lpszUNCServerName,
           lpszSourceName));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenEventLogW
 * Purpose   : The OpenEventLog function opens a handle of an event log.
 * Parameters: LPCWSTR  lpszUNCServerName
 *             LPCWSTR  lpszSourceName
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

HANDLE WIN32API OpenEventLogW(LPCWSTR  lpszUNCServerName,
                                 LPCWSTR  lpszSourceName)
{
  dprintf(("ADVAPI32: OpenEventLogW(%s,%s) not implemented.\n",
           lpszUNCServerName,
           lpszSourceName));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenSCManagerA
 * Purpose   : The OpenSCManager function establishes a connection to the
 *             service control manager on the specified computer and opens the
 *             specified database.
 * Parameters: LPCSTR lpszMachineName   address of machine name string
 *             LPCSTR lpszDatabaseName  address of database name string
 *             DWORD   fdwDesiredAccess  type of access
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API OpenSCManagerA(LPCSTR lpszMachineName,
                                     LPCSTR lpszDatabaseName,
                                     DWORD   fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenSCManagerA(%s,%s,%s) not implemented.\n",
           lpszMachineName,
           lpszDatabaseName,
           fdwDesiredAccess));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenSCManagerW
 * Purpose   : The OpenSCManager function establishes a connection to the
 *             service control manager on the specified computer and opens the
 *             specified database.
 * Parameters: LPCWSTR lpszMachineName   address of machine name string
 *             LPCWSTR lpszDatabaseName  address of database name string
 *             DWORD   fdwDesiredAccess  type of access
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API OpenSCManagerW(LPCWSTR lpszMachineName,
                                     LPCWSTR lpszDatabaseName,
                                     DWORD   fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenSCManagerW(%s,%s,%s) not implemented.\n",
           lpszMachineName,
           lpszDatabaseName,
           fdwDesiredAccess));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenServiceA
 * Purpose   : The OpenService function opens a handle to an existing service.
 * Parameters: SC_HANDLE schSCManager      handle of service control manager database
 *             LPCSTR   lpszServiceName   address of name of service to start
 *             DWORD     fdwDesiredAccess  type of access to service
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API OpenServiceA(SC_HANDLE schSCManager,
                                   LPCSTR   lpszServiceName,
                                   DWORD     fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenServiceA(%08xh,%s,%08xh) not implemented.\n",
           schSCManager,
           lpszServiceName,
           fdwDesiredAccess));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : OpenServiceW
 * Purpose   : The OpenService function opens a handle to an existing service.
 * Parameters: SC_HANDLE schSCManager      handle of service control manager database
 *             LPCWSTR   lpszServiceName   address of name of service to start
 *             DWORD     fdwDesiredAccess  type of access to service
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SC_HANDLE WIN32API OpenServiceW(SC_HANDLE schSCManager,
                                   LPCWSTR   lpszServiceName,
                                   DWORD     fdwDesiredAccess)
{
  dprintf(("ADVAPI32: OpenServiceW(%08xh,%s,%08xh) not implemented.\n",
           schSCManager,
           lpszServiceName,
           fdwDesiredAccess));

  return (NULL); /* signal failure */
}


/*****************************************************************************
 * Name      : PrivilegeCheck
 * Purpose   : The PrivilegeCheck function tests the security context represented
 *             by a specific access token to discover whether it contains the
 *             specified privileges. This function is typically called by a server
 *             application to check the privileges of a client's access token.
 * Parameters: HANDLE          hClientToken handle of client's access token
 *             PPRIVILEGE_SET  pps          address of privileges
 *             LPBOOL          lpfResult    address of flag for result
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API PrivilegeCheck(HANDLE         hClientToken,
                                PPRIVILEGE_SET pps,
                                LPBOOL         lpfResult)
{
  dprintf(("ADVAPI32: PrivilegeCheck(%08xh,%08xh,%08xh) not implemented.\n",
           hClientToken,
           pps,
           lpfResult));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : PrivilegedServiceAuditAlarmA
 * Purpose   : The PrivilegedServiceAuditAlarm function generates audit messages
 *             when an attempt is made to perform privileged system service
 *             operations. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCSTR        lpszSubsystem  address of string for subsystem name
 *             LPCSTR        lpszService    address of string for service name
 *             HANDLE         hClientToken   handle of access token
 *             PPRIVILEGE_SET pps            address of privileges
 *             BOOL           fAccessGranted flag for granted access rights
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API PrivilegedServiceAuditAlarmA(LPCSTR        lpszSubsystem,
                                              LPCSTR        lpszService,
                                              HANDLE         hClientToken,
                                              PPRIVILEGE_SET pps,
                                              BOOL           fAccessGranted)
{
  dprintf(("ADVAPI32: PrivilegedServiceAuditAlarmA(%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           lpszSubsystem,
           lpszService,
           hClientToken,
           pps,
           fAccessGranted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : PrivilegedServiceAuditAlarmW
 * Purpose   : The PrivilegedServiceAuditAlarm function generates audit messages
 *             when an attempt is made to perform privileged system service
 *             operations. Alarms are not supported in the current version of Windows NT.
 * Parameters: LPCWSTR        lpszSubsystem  address of string for subsystem name
 *             LPCWSTR        lpszService    address of string for service name
 *             HANDLE         hClientToken   handle of access token
 *             PPRIVILEGE_SET pps            address of privileges
 *             BOOL           fAccessGranted flag for granted access rights
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API PrivilegedServiceAuditAlarmW(LPCWSTR        lpszSubsystem,
                                              LPCWSTR        lpszService,
                                              HANDLE         hClientToken,
                                              PPRIVILEGE_SET pps,
                                              BOOL           fAccessGranted)
{
  dprintf(("ADVAPI32: PrivilegedServiceAuditAlarmW(%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           lpszSubsystem,
           lpszService,
           hClientToken,
           pps,
           fAccessGranted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceConfigA
 * Purpose   : The QueryServiceConfig function retrieves the configuration
 *             parameters of the specified service.
 * Parameters: SC_HANDLE              schService       handle of service
 *             LPQUERY_SERVICE_CONFIG lpqscServConfig  address of service config. structure
 *             DWORD                  cbBufSize        size of service configuration buffer
 *             LPDWORD                lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define LPQUERY_SERVICE_CONFIG LPVOID
BOOL WIN32API QueryServiceConfigA(SC_HANDLE              schService,
                                     LPQUERY_SERVICE_CONFIG lpqscServConfig,
                                     DWORD                  cbBufSize,
                                     LPDWORD                lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceConfigA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schService,
           lpqscServConfig,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceConfigW
 * Purpose   : The QueryServiceConfig function retrieves the configuration
 *             parameters of the specified service.
 * Parameters: SC_HANDLE              schService       handle of service
 *             LPQUERY_SERVICE_CONFIG lpqscServConfig  address of service config. structure
 *             DWORD                  cbBufSize        size of service configuration buffer
 *             LPDWORD                lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceConfigW(SC_HANDLE              schService,
                                     LPQUERY_SERVICE_CONFIG lpqscServConfig,
                                     DWORD                  cbBufSize,
                                     LPDWORD                lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceConfigW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schService,
           lpqscServConfig,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceLockStatusA
 * Purpose   : The QueryServiceLockStatus function retrieves the lock status
 *             of the specified service control manager database.
 * Parameters: SC_HANDLE                   schSCManager     handle of svc. ctrl. mgr. database
 *             LPQUERY_SERVICE_LOCK_STATUS lpqslsLockStat   address of lock status structure
 *             DWORD                       cbBufSize        size of service configuration buffer
 *             LPDWORD                     lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define LPQUERY_SERVICE_LOCK_STATUS LPVOID
BOOL WIN32API QueryServiceLockStatusA(SC_HANDLE                   schSCManager,
                                         LPQUERY_SERVICE_LOCK_STATUS lpqslsLockStat,
                                         DWORD                       cbBufSize,
                                         LPDWORD                     lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceLockStatusA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schSCManager,
           lpqslsLockStat,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceLockStatusW
 * Purpose   : The QueryServiceLockStatus function retrieves the lock status
 *             of the specified service control manager database.
 * Parameters: SC_HANDLE                   schSCManager     handle of svc. ctrl. mgr. database
 *             LPQUERY_SERVICE_LOCK_STATUS lpqslsLockStat   address of lock status structure
 *             DWORD                       cbBufSize        size of service configuration buffer
 *             LPDWORD                     lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceLockStatusW(SC_HANDLE                   schSCManager,
                                         LPQUERY_SERVICE_LOCK_STATUS lpqslsLockStat,
                                         DWORD                       cbBufSize,
                                         LPDWORD                     lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceLockStatusW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schSCManager,
           lpqslsLockStat,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceObjectSecurity
 * Purpose   : The QueryServiceObjectSecurity function retrieves a copy of the
 *             security descriptor protecting a service object.
 * Parameters: SC_HANDLE             schService       handle of service
 *             SECURITY_INFORMATION  fdwSecurityInfo  type of security information requested
 *             PSECURITY_DESCRIPTOR  psdSecurityDesc  address of security descriptor
 *             DWORD                 cbBufSize        size of security descriptor buffer
 *             LPDWORD               lpcbBytesNeeded  address of variable for bytes needed
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceObjectSecurity(SC_HANDLE             schService,
                                            SECURITY_INFORMATION  fdwSecurityInfo,
                                            PSECURITY_DESCRIPTOR  psdSecurityDesc,
                                            DWORD                 cbBufSize,
                                            LPDWORD               lpcbBytesNeeded)
{
  dprintf(("ADVAPI32: QueryServiceObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           schService,
           fdwSecurityInfo,
           psdSecurityDesc,
           cbBufSize,
           lpcbBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : QueryServiceStatus
 * Purpose   : The QueryServiceStatus function retrieves the current status of
 *             the specified service.
 * Parameters: SC_HANDLE         schService        handle of service
 *             LPSERVICE_STATUS  lpssServiceStatus address of service status structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API QueryServiceStatus(SC_HANDLE         schService,
                                    LPSERVICE_STATUS  lpssServiceStatus)
{
  dprintf(("ADVAPI32: QueryServiceStatus(%08xh,%08xh) not implemented.\n",
           schService,
           lpssServiceStatus));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ReadEventLogW
 * Purpose   : The ReadEventLog function reads a whole number of entries from
 *             the specified event log. The function can be used to read log
 *             entries in forward or reverse chronological order.
 * Parameters: HANDLE hEventLog                 handle of event log
 *             DWORD  dwReadFlags               specifies how to read log
 *             DWORD  dwRecordOffset            number of first record
 *             LPVOID lpBuffer                  address of buffer for read data
 *             DWORD  nNumberOfBytesToRead      number of bytes to read
 *             DWORD  *pnBytesRea               number of bytes read
 *             DWORD  *pnMinNumberOfBytesNeeded number of bytes required for next record
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ReadEventLogW(HANDLE hEventLog,
                               DWORD  dwReadFlags,
                               DWORD  dwRecordOffset,
                               LPVOID lpBuffer,
                               DWORD  nNumberOfBytesToRead,
                               DWORD  *pnBytesRead,
                               DWORD  *pnMinNumberOfBytesNeeded)
{
  dprintf(("ADVAPI32: ReadEventLogW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hEventLog,
           dwReadFlags,
           dwRecordOffset,
           lpBuffer,
           nNumberOfBytesToRead,
           pnBytesRead,
           pnMinNumberOfBytesNeeded));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : ReadEventLogA
 * Purpose   : The ReadEventLog function reads a whole number of entries from
 *             the specified event log. The function can be used to read log
 *             entries in forward or reverse chronological order.
 * Parameters: HANDLE hEventLog                 handle of event log
 *             DWORD  dwReadFlags               specifies how to read log
 *             DWORD  dwRecordOffset            number of first record
 *             LPVOID lpBuffer                  address of buffer for read data
 *             DWORD  nNumberOfBytesToRead      number of bytes to read
 *             DWORD  *pnBytesRea               number of bytes read
 *             DWORD  *pnMinNumberOfBytesNeeded number of bytes required for next record
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API ReadEventLogA(HANDLE hEventLog,
                               DWORD  dwReadFlags,
                               DWORD  dwRecordOffset,
                               LPVOID lpBuffer,
                               DWORD  nNumberOfBytesToRead,
                               DWORD  *pnBytesRead,
                               DWORD  *pnMinNumberOfBytesNeeded)
{
  dprintf(("ADVAPI32: ReadEventLogA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hEventLog,
           dwReadFlags,
           dwRecordOffset,
           lpBuffer,
           nNumberOfBytesToRead,
           pnBytesRead,
           pnMinNumberOfBytesNeeded));

  return (FALSE); /* signal failure */
}



/*****************************************************************************
 * Name      : O32_RegConnectRegistryA
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
  dprintf(("ADVAPI32: RegConnectRegistryA(%s,%08xh,%08xh) not implemented.\n",
           lpszComputerName,
           hKey,
           phkResult));
  
  if (lpszComputerName == NULL) /* local registry ? */
  {
    /* @@@PH experimental !!! */
    *phkResult = hKey;
    
    return (NO_ERROR);
  }
  
  return (ERROR_ACCESS_DENIED); /* signal failure */
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
  
  if (lpszComputerName != NULL)
    pszAscii = UnicodeToAsciiString((LPWSTR)lpszComputerName);
  else
    pszAscii = NULL;
  
  dprintf(("ADVAPI32: RegConnectRegistryW(%s,%08xh,%08xh) not implemented.\n",
           pszAscii,
           hKey,
           phkResult));
  
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
  dprintf(("ADVAPI32: RegGetKeySecurity(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hKey,
           SecInf,
           pSecDesc,
           lpcbSecDesc));

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

LONG WIN32API RegLoadKeyA(HKEY    hKey,
                             LPCSTR lpszSubKey,
                             LPCSTR lpszFile)
{
  dprintf(("ADVAPI32: RegLoadKeyA(%08xh,%s,%s) not implemented.\n",
           hKey,
           lpszSubKey,
           lpszFile));

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

LONG WIN32API RegLoadKeyW(HKEY    hKey,
                             LPCWSTR lpszSubKey,
                             LPCWSTR lpszFile)
{
  dprintf(("ADVAPI32: RegLoadKeyW(%08xh,%s,%s) not implemented.\n",
           hKey,
           lpszSubKey,
           lpszFile));

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

#define PVALENT LPVOID
LONG WIN32API RegQueryMultipleValuesA(HKEY    hKey,
                                         PVALENT val_list,
                                         DWORD   num_vals,
                                         LPTSTR  lpValueBuf,
                                         LPDWORD ldwTotsize)
{
  dprintf(("ADVAPI32: RegQueryMultipleValuesA(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hKey,
           val_list,
           num_vals,
           lpValueBuf,
           ldwTotsize));

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

LONG WIN32API RegQueryMultipleValuesW(HKEY    hKey,
                                         PVALENT val_list,
                                         DWORD   num_vals,
                                         LPWSTR  lpValueBuf,
                                         LPDWORD ldwTotsize)
{
  dprintf(("ADVAPI32: RegQueryMultipleValuesW(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hKey,
           val_list,
           num_vals,
           lpValueBuf,
           ldwTotsize));

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
  dprintf(("ADVAPI32: RegRemapPreDefKey(%08xh) not implemented.\n",
           hKey));

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

LONG WIN32API RegReplaceKeyA(HKEY    hKey,
                                LPCSTR lpSubKey,
                                LPCSTR lpNewFile,
                                LPCSTR lpOldFile)
{
  dprintf(("ADVAPI32: RegReplaceKeyA(%08xh,%s,%s,%s) not implemented.\n",
           hKey,
           lpSubKey,
           lpNewFile,
           lpOldFile));

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

LONG WIN32API RegReplaceKeyW(HKEY    hKey,
                                LPCWSTR lpSubKey,
                                LPCWSTR lpNewFile,
                                LPCWSTR lpOldFile)
{
  dprintf(("ADVAPI32: RegReplaceKeyW(%08xh,%s,%s,%s) not implemented.\n",
           hKey,
           lpSubKey,
           lpNewFile,
           lpOldFile));

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

LONG WIN32API RegRestoreKeyA(HKEY    hKey,
                                LPCSTR lpszFile,
                                DWORD   fdw)
{
  dprintf(("ADVAPI32: RegRestoreKeyA(%08xh,%s,%08xh) not implemented.\n",
           hKey,
           lpszFile,
           fdw));

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
                                LPCWSTR lpszFile,
                                DWORD   fdw)
{
  dprintf(("ADVAPI32: RegRestoreKeyW(%08xh,%s,%08xh) not implemented.\n",
           hKey,
           lpszFile,
           fdw));

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
                             LPCSTR               lpszFile,
                             LPSECURITY_ATTRIBUTES lpsa)
{
  dprintf(("ADVAPI32: RegSaveKeyA(%08xh,%s,%08xh) not implemented.\n",
           hKey,
           lpszFile,
           lpsa));

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
  dprintf(("ADVAPI32: RegSaveKeyW(%08xh,%s,%08xh) not implemented.\n",
           hKey,
           lpszFile,
           lpsa));

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
  dprintf(("ADVAPI32: RegSetKeySecurity(%08xh,%08xh,%08xh) not implemented.\n",
           hKey,
           si,
           psd));

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
                               LPCSTR lpszSubKey)
{
  dprintf(("ADVAPI32: RegUnLoadKeyA(%08xh,%s) not implemented.\n",
           hKey,
           lpszSubKey));

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
                               LPCWSTR lpszSubKey)
{
  dprintf(("ADVAPI32: RegUnLoadKeyW(%08xh,%s) not implemented.\n",
           hKey,
           lpszSubKey));

  return (ERROR_ACCESS_DENIED); /* signal failure */
}


/*****************************************************************************
 * Name      : RegisterServiceCtrlHandlerA
 * Purpose   : The RegisterServiceCtrlHandler function registers a function to
 *             handle service control requests for a service.
 * Parameters: LPCSTR            lpszServiceName address of name of service
 *             LPHANDLER_FUNCTION lpHandlerProc   address of handler function
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define SERVICE_STATUS_HANDLE DWORD
#define LPHANDLER_FUNCTION    LPVOID
SERVICE_STATUS_HANDLE WIN32API RegisterServiceCtrlHandlerA(LPCSTR            lpszServiceName,
                                                              LPHANDLER_FUNCTION lpHandlerProc)
{
  dprintf(("ADVAPI32: RegisterServiceCtrlHandlerA(%s,%08xh) not implemented.\n",
           lpszServiceName,
           lpHandlerProc));

  return (0); /* signal failure */
}


/*****************************************************************************
 * Name      : RegisterServiceCtrlHandlerW
 * Purpose   : The RegisterServiceCtrlHandler function registers a function to
 *             handle service control requests for a service.
 * Parameters: LPCWSTR            lpszServiceName address of name of service
 *             LPHANDLER_FUNCTION lpHandlerProc   address of handler function
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

SERVICE_STATUS_HANDLE WIN32API RegisterServiceCtrlHandlerW(LPCWSTR            lpszServiceName,
                                                              LPHANDLER_FUNCTION lpHandlerProc)
{
  dprintf(("ADVAPI32: RegisterServiceCtrlHandlerW(%s,%08xh) not implemented.\n",
           lpszServiceName,
           lpHandlerProc));

  return (0); /* signal failure */
}



/*****************************************************************************
 * Name      : RevertToSelf
 * Purpose   : The RevertToSelf function terminates the impersonation of a client application.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API RevertToSelf(VOID)
{
  dprintf(("ADVAPI32: RevertToSelf() not implemented.\n"));

  return (TRUE); /* signal OK */
}


/*****************************************************************************
 * Name      : SetAclInformation
 * Purpose   : The SetAclInformation function sets information about an access-control list (ACL).
 * Parameters: PACL                  pAcl       address of access-control list
 *             LPVOID                lpvAclInfo address of ACL information
 *             DWORD                 cbAclInfo  size of ACL information
 *             ACL_INFORMATION_CLASS aclic      specifies class of requested info
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define ACL_INFORMATION_CLASS DWORD
BOOL WIN32API SetAclInformation(PACL                  pAcl,
                                   LPVOID                lpvAclInfo,
                                   DWORD                 cbAclInfo,
                                   ACL_INFORMATION_CLASS aclic)
{
  dprintf(("ADVAPI32: SetAclInformation(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pAcl,
           lpvAclInfo,
           cbAclInfo,
           aclic));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetKernelObjectSecurity
 * Purpose   : The SetKernelObjectSecurity function sets the security of a kernel
 *             object. For example, this can be a process, thread, or event.
 * Parameters: HANDLE               hObject handle of object
 *             SECURITY_INFORMATION si      type of information to set
 *             PSECURITY_DESCRIPTOR psd     address of security descriptor
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetKernelObjectSecurity(HANDLE               hObject,
                                         SECURITY_INFORMATION si,
                                         PSECURITY_DESCRIPTOR psd)
{
  dprintf(("ADVAPI32: SetKernelObjectSecurity(%08xh,%08xh,%08xh) not implemented.\n",
           hObject,
           si,
           psd));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetPrivateObjectSecurity
 * Purpose   : The SetPrivateObjectSecurity function modifies a private
 *             object's security descriptor.
 * Parameters: SECURITY_INFORMATION  si           type of security information
 *             PSECURITY_DESCRIPTOR  psdSource    address of SD to apply to object
 *             PSECURITY_DESCRIPTOR  *lppsdTarget address of object's SD
 *             PGENERIC_MAPPING      pgm          address of access-mapping structure
 *             HANDLE                hClientToken handle of client access token
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetPrivateObjectSecurity(SECURITY_INFORMATION  si,
                                          PSECURITY_DESCRIPTOR  psdSource,
                                          PSECURITY_DESCRIPTOR  *lppsdTarget,
                                          PGENERIC_MAPPING      pgm,
                                          HANDLE                hClientToken)
{
  dprintf(("ADVAPI32: SetPrivateObjectSecurity(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           si,
           psdSource,
           lppsdTarget,
           pgm,
           hClientToken));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetSecurityDescriptorGroup
 * Purpose   : The SetSecurityDescriptorGroup function sets the primary group
 *             information of an absolute-format security descriptor, replacing
 *             any primary group information already present in the security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR psd             address of security descriptor
 *             PSID                 psidGroup       address of SID for group
 *             BOOL                 fGroupDefaulted flag for default
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetSecurityDescriptorGroup(PSECURITY_DESCRIPTOR psd,
                                            PSID                 psidGroup,
                                            BOOL                 fGroupDefaulted)
{
  dprintf(("ADVAPI32: SetSecurityDescriptorGroup(%08xh,%08xh,%08xh) not implemented.\n",
           psd,
           psidGroup,
           fGroupDefaulted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetSecurityDescriptorOwner
 * Purpose   : The SetSecurityDescriptorOwner function sets the owner information
 *             of an absolute-format security descriptor. It replaces any owner
 *             information already present in the security descriptor.
 * Parameters: PSECURITY_DESCRIPTOR psd             address of security descriptor
 *             PSID                 psidOwner       address of SID for owner
 *             BOOL                 fOwnerDefaulted flag for default
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetSecurityDescriptorOwner(PSECURITY_DESCRIPTOR psd,
                                            PSID                 psidOwner,
                                            BOOL                 fOwnerDefaulted)
{
  dprintf(("ADVAPI32: SetSecurityDescriptorOwner(%08xh,%08xh,%08xh) not implemented.\n",
           psd,
           psidOwner,
           fOwnerDefaulted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetSecurityDescriptorSacl
 * Purpose   : The SetSecurityDescriptorSacl function sets information in a
 *             system access-control list (ACL). If there is already a system
 *             ACL present in the security descriptor, it is replaced.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetSecurityDescriptorSacl(PSECURITY_DESCRIPTOR  psd,
                                           BOOL                  fSaclPresent,
                                           PACL                  pAcl,
                                           BOOL                  fSaclDefaulted)
{
  dprintf(("ADVAPI32: SetSecurityDescriptorSacl(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           psd,
           fSaclPresent,
           pAcl,
           fSaclDefaulted));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetServiceBits
 * Purpose   : The SetServiceBits function registers a service's service type
 *             with the Service Control Manager and the Server service. The
 *             Server service can then announce the registered service type
 *             as one it currently supports. The LAN Manager functions
 *             NetServerGetInfo and NetServerEnum obtain a specified machine's
 *             supported service types.
 *             A service type is represented as a set of bit flags; the
 *             SetServiceBits function sets or clears combinations of those bit flags.
 * Parameters: SERVICE_STATUS_HANDLE hServiceStatus     service status handle
 *             DWORD                 dwServiceBits      service type bits to set or clear
 *             BOOL                  bSetBitsOn         flag to set or clear the service type bits
 *             BOOL                  bUpdateImmediately flag to announce server type immediately
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetServiceBits(SERVICE_STATUS_HANDLE hServiceStatus,
                                DWORD                 dwServiceBits,
                                BOOL                  bSetBitsOn,
                                BOOL                  bUpdateImmediately)
{
  dprintf(("ADVAPI32: SetServiceBits(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hServiceStatus,
           dwServiceBits,
           bSetBitsOn,
           bUpdateImmediately));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetServiceObjectSecurity
 * Purpose   : The SetServiceObjectSecurity function sets the security
 *             descriptor of a service object.
 * Parameters: SC_HANDLE             schService      handle of service
 *             SECURITY_INFORMATION  fdwSecurityInfo type of security information requested
 *             PSECURITY_DESCRIPTOR  psdSecurityDesc address of security descriptor
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetServiceObjectSecurity(SC_HANDLE             schService,
                                          SECURITY_INFORMATION  fdwSecurityInfo,
                                          PSECURITY_DESCRIPTOR  psdSecurityDesc)
{
  dprintf(("ADVAPI32: SetServiceObjectSecurity(%08xh,%08xh,%08xh) not implemented.\n",
           schService,
           fdwSecurityInfo,
           psdSecurityDesc));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetServiceStatus
 * Purpose   : The SetServiceStatus function updates the service control
 *             manager's status information for the calling service.
 * Parameters: SERVICE_STATUS_HANDLE sshServiceStatus  service status handle
 *             LPSERVICE_STATUS      lpssServiceStatus address of status structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API SetServiceStatus(SERVICE_STATUS_HANDLE sshServiceStatus,
                                  LPSERVICE_STATUS      lpssServiceStatus)
{
  dprintf(("ADVAPI32: SetServiceStatus(%08xh,%08xh) not implemented.\n",
           sshServiceStatus,
           lpssServiceStatus));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : SetTokenInformation
 * Purpose   : The SetTokenInformation function sets various types of
 *             information for a specified access token. The information it
 *             sets replaces existing information. The calling process must
 *             have appropriate access rights to set the information.
 * Parameters: HANDLE                  hToken         handle of access token
 *             TOKEN_INFORMATION_CLASS tic            type of information to set
 *             LPVOID                  lpvInformation address of information to set
 *             DWORD                   cbInformation  size of information buffer
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define TOKEN_INFORMATION_CLASS DWORD
BOOL WIN32API SetTokenInformation(HANDLE                  hToken,
                                     TOKEN_INFORMATION_CLASS tic,
                                     LPVOID                  lpvInformation,
                                     DWORD                   cbInformation)
{
  dprintf(("ADVAPI32: SetTokenInformation(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hToken,
           tic,
           lpvInformation,
           cbInformation));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : StartServiceA
 * Purpose   : The StartService function starts the execution of a service.
 * Parameters: SC_HANDLE schService        handle of service
 *             DWORD     dwNumServiceArgs  number of arguments
 *             LPCSTR   *lpszServiceArgs  address of array of argument string pointers
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API StartServiceA(SC_HANDLE schService,
                               DWORD     dwNumServiceArgs,
                               LPCSTR   *lpszServiceArgs)
{
  dprintf(("ADVAPI32: StartServiceA(%08xh,%08xh,%s) not implemented.\n",
           schService,
           dwNumServiceArgs,
           lpszServiceArgs));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : StartServiceCtrlDispatcherW
 * Purpose   : The StartServiceCtrlDispatcher function connects the main thread
 *             of a service process to the service control manager, which causes
 *             the thread to be the service control dispatcher thread for the calling process.
 * Parameters: LPSERVICE_TABLE_ENTRY  lpsteServiceTable  address of service table
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

#define LPSERVICE_TABLE_ENTRY LPVOID
BOOL WIN32API StartServiceCtrlDispatcherW(LPSERVICE_TABLE_ENTRY lpsteServiceTable)
{
  dprintf(("ADVAPI32: StartServiceCtrlDispatcherW(%08xh) not implemented.\n",
           lpsteServiceTable));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : StartServiceCtrlDispatcherA
 * Purpose   : The StartServiceCtrlDispatcher function connects the main thread
 *             of a service process to the service control manager, which causes
 *             the thread to be the service control dispatcher thread for the calling process.
 * Parameters: LPSERVICE_TABLE_ENTRY  lpsteServiceTable  address of service table
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API StartServiceCtrlDispatcherA(LPSERVICE_TABLE_ENTRY lpsteServiceTable)
{
  dprintf(("ADVAPI32: StartServiceCtrlDispatcherA(%08xh) not implemented.\n",
           lpsteServiceTable));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : StartServiceW
 * Purpose   : The StartService function starts the execution of a service.
 * Parameters: SC_HANDLE schService        handle of service
 *             DWORD     dwNumServiceArgs  number of arguments
 *             LPCWSTR   *lpszServiceArgs  address of array of argument string pointers
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API StartServiceW(SC_HANDLE schService,
                               DWORD     dwNumServiceArgs,
                               LPCWSTR   *lpszServiceArgs)
{
  dprintf(("ADVAPI32: StartServiceW(%08xh,%08xh,%s) not implemented.\n",
           schService,
           dwNumServiceArgs,
           lpszServiceArgs));

  return (FALSE); /* signal failure */
}


/*****************************************************************************
 * Name      : UnlockServiceDatabase
 * Purpose   : The UnlockServiceDatabase function unlocks a service control
 *             manager database by releasing the specified lock.
 * Parameters: SC_LOCK  sclLock  service control manager database lock to be released
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]

 *****************************************************************************/

BOOL WIN32API UnlockServiceDatabase(SC_LOCK sclLock)
{
  dprintf(("ADVAPI32: UnlockServiceDatabase(%08xh) not implemented.\n",
           sclLock));

  return (FALSE); /* signal failure */
}

