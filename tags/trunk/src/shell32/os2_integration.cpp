/* $Id: os2_integration.cpp,v 1.1 2002-03-08 11:00:59 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 2002 Patrick Haller (patrick.haller@innotek.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_BASE
#define INCL_WIN
#define INCL_WINWORKPLACE
#include <os2.h>

#include <os2wrap.h>
#include <win32type.h>
#include <misc.h>
#include <string.h>

#include "os2_integration.h"


// These constants shall resemble ODIN SHELL error codes
#ifndef S_OK
#define S_OK 0
#endif

#ifndef SE_ERR_OOM
#define SE_ERR_OOM              8       /* out of memory */
#endif

#ifndef SE_ERR_ASSOCINCOMPLETE
#define SE_ERR_ASSOCINCOMPLETE          27
#endif


#ifdef SE_ERR_NOASSOC
#define SE_ERR_NOASSOC                  31
#endif




DWORD ShellExecuteOS2(HWND   hwndWin32,
                      LPCSTR pszOperation,
                      LPCSTR pszFile,
                      LPCSTR pszParameters,
                      LPCSTR pszDirectory,
                      INT    iShowCmd)
{
  APIRET rc;                                              /* API return code */
  char   szFileFull[260]; 
  
  // verify parameter block
  
  // Supported operations
  if ( (NULL == pszOperation) &&
       (stricmp( pszOperation, "open") != 0) &&
       (stricmp( pszOperation, "play") != 0) )
  {
    // abort execution with error
    return SE_ERR_ASSOCINCOMPLETE;
  }
      
  // @@@PH
  // work directory currently ignored
  // parameters currently ignored
  
  // FS: is saved by the wrapper macros
  
  // get fully-qualified name
  rc = DosQueryPathInfo(pszFile,               /* query the file information */
                        FIL_QUERYFULLNAME,
                        &szFileFull,
                        sizeof(szFileFull));
  if (rc != NO_ERROR)                                    /* check for errors */
    return SE_ERR_ASSOCINCOMPLETE;
                   
  
  // Finally try to open the WPS object
  HOBJECT hObject = WinQueryObject( szFileFull );
  if (NULLHANDLE == hObject)
    return SE_ERR_OOM;
    
  WinSetObjectData(hObject, "OPEN=DEFAULT");
  
  return S_OK;
}