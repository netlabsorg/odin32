/* $Id: profile.cpp,v 1.4 1999-06-10 20:48:01 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 profile API functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"

//******************************************************************************
//******************************************************************************
UINT WIN32API GetPrivateProfileIntA(LPCSTR arg1, LPCSTR arg2, INT arg3, LPCSTR  arg4)
{
    dprintf(("KERNEL32:  OS2GetPrivateProfileIntA %s, %s, file %s\n", arg1, arg2, arg4));
    return O32_GetPrivateProfileInt(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetPrivateProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName,
                                    INT nDefault, LPCWSTR lpFileName)
{
  char *asciiapp, *asciikey, *asciifile;
  UINT  rc;

    dprintf(("KERNEL32:  OS2GetPrivateProfileIntW\n"));
    asciiapp  = UnicodeToAsciiString((LPWSTR)lpAppName);
    asciikey  = UnicodeToAsciiString((LPWSTR)lpKeyName);
    asciifile = UnicodeToAsciiString((LPWSTR)lpFileName);
    rc = O32_GetPrivateProfileInt(asciiapp, asciikey, nDefault, asciifile);
    FreeAsciiString(asciifile);
    FreeAsciiString(asciikey);
    FreeAsciiString(asciiapp);
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetProfileIntA( LPCSTR arg1, LPCSTR arg2, INT  arg3)
{
 UINT rc;

    rc = O32_GetProfileInt(arg1, arg2, arg3);
    dprintf(("KERNEL32:  OS2GetProfileIntA %s %s returned %d\n", arg1, arg2, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, int nDefault)
{
  char *asciiapp, *asciikey;
  UINT  rc;

    dprintf(("KERNEL32:  OS2GetProfileIntW\n"));
    asciiapp  = UnicodeToAsciiString((LPWSTR)lpAppName);
    asciikey  = UnicodeToAsciiString((LPWSTR)lpKeyName);
    rc = O32_GetProfileInt(asciiapp, asciikey, nDefault);
    FreeAsciiString(asciikey);
    FreeAsciiString(asciiapp);
    return(rc);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetPrivateProfileStringA(LPCSTR arg1,
                                      LPCSTR arg2,
                                      LPCSTR arg3,
                                      LPSTR  arg4,
                                      UINT  arg5,
                                      LPCSTR arg6)
{
  dprintf(("KERNEL32: GetPrivateProfileStringA(%s,%s,%s,%08xh,%u,%s)\n",
           arg1,
           arg2,
           arg3,
           arg4,
           arg5,
           arg6));

  return O32_GetPrivateProfileString(arg1,
                                 arg2,
                                 arg3,
                                 arg4,
                                 arg5,
                                 arg6);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetPrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName,
                                      LPCWSTR lpDefault, LPWSTR lpReturnedString,
                                      UINT nSize, LPCWSTR lpFileName)
{
  char *asciiapp, *asciikey, *asciifile, *asciidefault;
  char *asciistring = (char *)malloc(nSize+1);
  UINT  rc;

    dprintf(("KERNEL32:  OS2GetPrivateProfileStringW\n"));
    asciiapp     = UnicodeToAsciiString((LPWSTR)lpAppName);
    asciikey     = UnicodeToAsciiString((LPWSTR)lpKeyName);
    asciifile    = UnicodeToAsciiString((LPWSTR)lpFileName);
    asciidefault = UnicodeToAsciiString((LPWSTR)lpDefault);
    rc = O32_GetPrivateProfileString(asciiapp, asciikey, asciidefault, asciistring,
                                 nSize, asciifile);
    if(rc)      AsciiToUnicode(asciistring, lpReturnedString);
    FreeAsciiString(asciidefault);
    FreeAsciiString(asciifile);
    FreeAsciiString(asciikey);
    FreeAsciiString(asciiapp);
    free(asciistring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetProfileStringA( LPCSTR arg1, LPCSTR arg2, LPCSTR arg3, LPSTR arg4, UINT arg5)
{
    dprintf(("KERNEL32:  GetProfileString %s\n", arg1));
    return O32_GetProfileString(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName,
                               LPCWSTR lpDefault, LPWSTR lpReturnedString,
                               UINT nSize)
{
  char *asciiapp, *asciikey, *asciifile, *asciidefault;
  char *asciistring = (char *)malloc(nSize+1);
  UINT  rc;

    dprintf(("KERNEL32:  OS2GetProfileStringW\n"));
    asciiapp     = UnicodeToAsciiString((LPWSTR)lpAppName);
    asciikey     = UnicodeToAsciiString((LPWSTR)lpKeyName);
    asciidefault = UnicodeToAsciiString((LPWSTR)lpDefault);
    rc = O32_GetProfileString(asciiapp, asciikey, asciidefault, asciistring, nSize);
    if(rc)      AsciiToUnicode(asciistring, lpReturnedString);
    FreeAsciiString(asciidefault);
    FreeAsciiString(asciikey);
    FreeAsciiString(asciiapp);
    free(asciistring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WriteProfileStringA(LPCSTR arg1, LPCSTR arg2, LPCSTR  arg3)
{
    dprintf(("KERNEL32:  WriteProfileString %s key %s value %s\n", arg1, arg2, arg3));
    return O32_WriteProfileString(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WriteProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName,
                                  LPCWSTR lpString)
{
  char *asciiapp, *asciikey, *asciistring;
  UINT  rc;

    dprintf(("KERNEL32:  OS2WriteProfileStringW\n"));
    asciiapp     = UnicodeToAsciiString((LPWSTR)lpAppName);
    asciikey     = UnicodeToAsciiString((LPWSTR)lpKeyName);
    asciistring  = UnicodeToAsciiString((LPWSTR)lpString);
    rc = O32_WriteProfileString(asciiapp, asciikey, asciistring);
    FreeAsciiString(asciistring);
    FreeAsciiString(asciikey);
    FreeAsciiString(asciiapp);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WritePrivateProfileStringA( LPCSTR arg1, LPCSTR arg2, LPCSTR arg3, LPCSTR  arg4)
{
    dprintf(("KERNEL32:  WritePrivateProfileString\n"));
    return O32_WritePrivateProfileString(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WritePrivateProfileStringW(LPCWSTR lpAppName, LPCWSTR lpKeyName,
                                         LPCWSTR lpString, LPCWSTR lpFileName)
{
  char *asciiapp, *asciikey, *asciifile, *asciistring;
  UINT  rc;

    dprintf(("KERNEL32:  OS2WritePrivateProfileStringW\n"));
    asciiapp     = UnicodeToAsciiString((LPWSTR)lpAppName);
    asciikey     = UnicodeToAsciiString((LPWSTR)lpKeyName);
    asciifile    = UnicodeToAsciiString((LPWSTR)lpFileName);
    asciistring  = UnicodeToAsciiString((LPWSTR)lpString);
    rc = O32_WritePrivateProfileString(asciiapp, asciikey, asciistring, asciifile);
    FreeAsciiString(asciistring);
    FreeAsciiString(asciifile);
    FreeAsciiString(asciikey);
    FreeAsciiString(asciiapp);
    return(rc);
}
//******************************************************************************
//******************************************************************************
INT WIN32API GetProfileSectionA(LPCSTR lpszSection, LPSTR lpszReturnBuffer,
                                DWORD cchBuffer)
{
  dprintf(("KERNEL32:  OS2GetProfileSectionA %s, not implemented\n", lpszSection));
  return(0);
}
//******************************************************************************
INT WIN32API GetProfileSectionW(LPCWSTR lpszSection,
                                LPWSTR lpszReturnBuffer,
                                DWORD cchBuffer)
{
  dprintf(("KERNEL32:  OS2GetProfileSectionW %s, not implemented\n", lpszSection));
  return(0);
}

INT WIN32API GetPrivateProfileSectionA(
    LPCSTR lpszSection,    /* address of section name      */
    LPSTR lpszReturnBuffer,/* address of return buffer     */
    DWORD cchBuffer,        /* size of return buffer        */
    LPCSTR lpszFile)       /* address of initialization filename   */


   /* The GetPrivateProfileSection function retrieves all of the keys
    * and values for the specified section from the given initialization
    * file. This function is provided for compatibility with 16-bit
    * applications written for Windows. New applications should store
    * initialization information in the registry.
    */
{
    dprintf(("GetPrivateProfileSection(%s, %s, %d) - not implemented\n",
            lpszFile, lpszSection, cchBuffer));
    return FALSE;   // return error
}


INT WIN32API GetPrivateProfileSectionW(
    LPCWSTR lpszSection,    /* address of section name      */
    LPWSTR lpszReturnBuffer,/* address of return buffer     */
    DWORD cchBuffer,        /* size of return buffer        */
    LPCWSTR lpszFile)       /* address of initialization filename   */


   /* The GetPrivateProfileSection function retrieves all of the keys
    * and values for the specified section from the given initialization
    * file. This function is provided for compatibility with 16-bit
    * applications written for Windows. New applications should store
    * initialization information in the registry.
    */
{
    dprintf(("GetPrivateProfileSectionW(%s, %s, %d) - not implemented\n",
            lpszFile, lpszSection, cchBuffer));
    return FALSE;   // return error
}
/*****************************************************************************
 * Name      : BOOL WriteProfileSectionA
 * Purpose   : The WriteProfileSection function replaces the contents of the
 *             specified section in the WIN.INI file with the specified keys and values.
 *             This function is provided for compatibility with 16-bit Windows
 *             applications. Win32-based applications should store initialization
 *             information in the registry.
 * Parameters: LPCTSTR lpszSection       address of string with section name
 *             LPCTSTR lpszKeysAndValues address of string with data
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WriteProfileSectionA(LPCTSTR lpszSection,
                                   LPCTSTR lpszKeysAndValues)
{
  dprintf(("KERNEL32: WriteProfileSectionA(%s,%s,%s) not implemented.\n",
           lpszSection,
           lpszKeysAndValues));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WriteProfileSectionW
 * Purpose   : The WriteProfileSection function replaces the contents of the
 *             specified section in the WIN.INI file with the specified keys and values.
 *             This function is provided for compatibility with 16-bit Windows
 *             applications. Win32-based applications should store initialization
 *             information in the registry.
 * Parameters: LPCWSTR lpszSection       address of string with section name
 *             LPCWSTR lpszKeysAndValues address of string with data
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WriteProfileSectionW(LPCWSTR lpszSection,
                                   LPCWSTR lpszKeysAndValues)
{
  dprintf(("KERNEL32: WriteProfileSectionW(%s,%s,%s) not implemented.\n",
           lpszSection,
           lpszKeysAndValues));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL GetPrivateProfileStructA
 * Purpose   : The GetPrivateProfileStructA function retrieves the data associated
 *             with the specified key in the given section of an initialization
 *             file. As it retrieves the data, the function calculates a checksum
 *             and compares it with the checksum calculated by the WritePrivateProfileStruct
 *             function when the data was added to the file. This function is provided
 *             for compatibility with 16-bit Windows-based applications. Win32-based
 *             applications should store initialization information in the registry.
 * Parameters: LPCTSTR lpszSection  address of section name
 *             LPCTSTR lpszKey      address of key name
 *             LPVOID  lpStruct     address of return buffer
 *             UINT    uSizeStruct  size of return buffer
 *             LPCTSTR szFile       address of initialization filename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetPrivateProfileStructA(LPCTSTR lpszSection,
                                          LPCTSTR lpszKey,
                                          LPVOID  lpStruct,
                                          UINT    uSizeStruct,
                                          LPCTSTR szFile)
{
  dprintf(("KERNEL32: GetPrivateProfileStructA(%s,%s,%08xh,%08xh,%s) not implemented.\n",
           lpszSection,
           lpszKey,
           lpStruct,
           uSizeStruct,
           szFile));

  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL GetPrivateProfileStructW
 * Purpose   : The GetPrivateProfileStructW function retrieves the data associated
 *             with the specified key in the given section of an initialization
 *             file. As it retrieves the data, the function calculates a checksum
 *             and compares it with the checksum calculated by the WritePrivateProfileStruct
 *             function when the data was added to the file. This function is provided
 *             for compatibility with 16-bit Windows-based applications. Win32-based
 *             applications should store initialization information in the registry.
 * Parameters: LPCWSTR lpszSection  address of section name
 *             LPCWSTR lpszKey      address of key name
 *             LPVOID  lpStruct     address of return buffer
 *             UINT    uSizeStruct  size of return buffer
 *             LPCWSTR szFile       address of initialization filename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetPrivateProfileStructW(LPCWSTR lpszSection,
                                          LPCWSTR lpszKey,
                                          LPVOID  lpStruct,
                                          UINT    uSizeStruct,
                                          LPCWSTR szFile)
{
  dprintf(("KERNEL32: GetPrivateProfileStructW(%s,%s,%08xh,%08xh,%s) not implemented.\n",
           lpszSection,
           lpszKey,
           lpStruct,
           uSizeStruct,
           szFile));

  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL GetPrivateProfileSectionNamesA
 * Purpose   : The GetPrivateProfileSectionNames function retrieves the names
 *             of all sections in an initialization file. This function is
 *             provided for compatibility with 16-bit Windows-based applications.
 *             Win32-based applications should store initialization information
 *             in the registry.
 * Parameters: LPTSTR  lpszReturnBuffer address of return buffer
 *             DWORD   nSize            size of return buffer
 *             LPCTSTR lpFileName       address of initialization filename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetPrivateProfileSectionNamesA(LPTSTR  lpszReturnBuffer,
                                                 DWORD   nSize,
                                                 LPCTSTR lpFileName)
{
  dprintf(("KERNEL32: GetPrivateProfileSectionNamesA(%08xh,%08xh,%s) not implemented.\n",
           lpszReturnBuffer,
           nSize,
           lpFileName));

  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL GetPrivateProfileSectionNamesW
 * Purpose   : The GetPrivateProfileSectionNames function retrieves the names
 *             of all sections in an initialization file. This function is
 *             provided for compatibility with 16-bit Windows-based applications.
 *             Win32-based applications should store initialization information
 *             in the registry.
 * Parameters: LPWSTR  lpszReturnBuffer address of return buffer
 *             DWORD   nSize            size of return buffer
 *             LPCTSTR lpFileName       address of initialization filename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetPrivateProfileSectionNamesW(LPWSTR  lpszReturnBuffer,
                                              DWORD   nSize,
                                              LPCWSTR lpFileName)
{
  dprintf(("KERNEL32: GetPrivateProfileSectionNamesW(%08xh,%08xh,%s) not implemented.\n",
           lpszReturnBuffer,
           nSize,
           lpFileName));

  return FALSE;
}
/*****************************************************************************
 * Name      : BOOL WritePrivateProfileSectionA
 * Purpose   : The WritePrivateProfileSection function replaces the keys and
 *             values under the specified section in an initialization file.
 *             This function is provided for compatibility with 16-bit
 *             Windows-based applications. Win32-based applications should
 *             store initialization information in the registry.
 * Parameters: LPCTSTR lpszSection       address of string with section name
 *             LPCTSTR lpszKeysAndValues address of string with data
 *             LPCTSTR lpszFile          address of string with filename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WritePrivateProfileSectionA(LPCTSTR lpszSection,
                                             LPCTSTR lpszKeysAndValues,
                                             LPCTSTR lpszFile)
{
  dprintf(("KERNEL32: WritePrivateProfileSectionA(%s,%s,%s) not implemented.\n",
           lpszSection,
           lpszKeysAndValues,
           lpszFile));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WritePrivateProfileSectionW
 * Purpose   : The WritePrivateProfileSection function replaces the keys and
 *             values under the specified section in an initialization file.
 *             This function is provided for compatibility with 16-bit
 *             Windows-based applications. Win32-based applications should
 *             store initialization information in the registry.
 * Parameters: LPCWSTR lpszSection       address of string with section name
 *             LPCWSTR lpszKeysAndValues address of string with data
 *             LPCWSTR lpszFile          address of string with filename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WritePrivateProfileSectionW(LPCWSTR lpszSection,
                                             LPCWSTR lpszKeysAndValues,
                                             LPCWSTR lpszFile)
{
  dprintf(("KERNEL32: WritePrivateProfileSectionW(%s,%s,%s) not implemented.\n",
           lpszSection,
           lpszKeysAndValues,
           lpszFile));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WritePrivateProfileStructA
 * Purpose   : The WritePrivateProfileStruct function copies data into the
 *             specified key in the given section of an initialization file.
 *             As it copies the data, the function calculates a checksum and
 *             appends it to the end of the data. The GetPrivateProfileStruct
 *             function uses the checksum to ensure the integrity of the data.
 *             This function is provided for compatibility with 16-bit
 *             Windows-based applications. Win32-based applications should store
 *             initialization information in the registry.
 * Parameters: LPCTSTR lpszSection address of section name
 *             LPCTSTR lpszKey     address of key name
 *             LPVOID  lpvStruct   address of buffer that contains data to add
 *             UINT    uSizeStruct size, in bytes, of the buffer
 *             LPCTSTR lpszFile    address of initialization filename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WritePrivateProfileStructA(LPCTSTR lpszSection,
                                            LPCTSTR lpszKey,
                                            LPVOID  lpvStruct,
                                            UINT    uSizeStruct,
                                            LPCTSTR lpszFile)
{
  dprintf(("KERNEL32: WritePrivateProfileStructA(%s,%s,%08xh,%u,%s) not implemented.\n",
           lpszSection,
           lpszKey,
           lpvStruct,
           uSizeStruct,
           lpszFile));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WritePrivateProfileStructW
 * Purpose   : The WritePrivateProfileStruct function copies data into the
 *             specified key in the given section of an initialization file.
 *             As it copies the data, the function calculates a checksum and
 *             appends it to the end of the data. The GetPrivateProfileStruct
 *             function uses the checksum to ensure the integrity of the data.
 *             This function is provided for compatibility with 16-bit
 *             Windows-based applications. Win32-based applications should store
 *             initialization information in the registry.
 * Parameters: LPCWSTR lpszSection address of section name
 *             LPCWSTR lpszKey     address of key name
 *             LPVOID  lpvStruct   address of buffer that contains data to add
 *             UINT    uSizeStruct size, in bytes, of the buffer
 *             LPCWSTR lpszFile    address of initialization filename
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WritePrivateProfileStructW(LPCWSTR lpszSection,
                                            LPCWSTR lpszKey,
                                            LPVOID  lpvStruct,
                                            UINT    uSizeStruct,
                                            LPCWSTR lpszFile)
{
  dprintf(("KERNEL32: WritePrivateProfileStructW(%s,%s,%08xh,%u,%s) not implemented.\n",
           lpszSection,
           lpszKey,
           lpvStruct,
           uSizeStruct,
           lpszFile));

  return (FALSE);
}
