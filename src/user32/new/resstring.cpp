/* $Id: resstring.cpp,v 1.1 1999-07-14 08:35:35 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 resource string functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#define INCL_BASE
#define INCL_WIN
#define INCL_WINERRORS
#define INCL_DOSFILEMGR
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "resstring.h"
#include "win32util.h"
#include "unicode.h"

/******************************************************************************/
//Strings stored as original: WORD    len
//                            UNICODE string (not null terminated)
/******************************************************************************/
int OS2LoadStringAscii(HINSTANCE hinst,
                       UINT      wID,
                       LPSTR     lpBuffer,
                       int       cchBuffer)
{
  APIRET  rc;
  WCHAR  *szbuf;
  char    modname[128];
  HMODULE hmodule;

  if(hinst != 0 && hinst != -1)
  {
    if(Win32QueryModuleName(hinst,
                            modname,
                            sizeof(modname)) == 0)
    {
      dprintf(("USER32: OS2LoadStringAscii: Can't determine handle of dll %08xh\n",
               hinst));
      return(0);
   }

   rc = DosQueryModuleHandle(modname,
                             &hmodule);
   if(rc)
   {
     dprintf(("USER32: OS2LoadStringAscii: Can't determine handle of dll %s\n",
              modname));

     return(0);
   }
  }
  else
    hmodule = 0;

  rc = DosGetResource(hmodule,
                      RT_RCDATA,
                      wID,
                      (PPVOID)&szbuf);
  if(rc)
  {
    dprintf(("USER32: OS2LoadStringAscii failed with rc %08xh\n",
             rc));

    if(lpBuffer)
      *lpBuffer = 0;

    return(0);  //TODO: SetLastError!
  }

  UnicodeToAsciiN(szbuf+1,
                  lpBuffer,
                  min((*szbuf+1), cchBuffer));

  dprintf(("USER32: OS2LoadStringA retrieved string '%s' (size %d)\n",
           lpBuffer,
           (int)*szbuf));

  DosFreeResource(szbuf);
  return(strlen(lpBuffer));              //not including null terminator!
}
//******************************************************************************
//******************************************************************************
int OS2LoadStringUnicode(HINSTANCE hinst,
                         UINT      wID,
                         WCHAR     *lpBuffer,
                         int       cchBuffer)
{
  APIRET  rc;
  WCHAR  *szbuf;
  char    modname[128];
  HMODULE hmodule;

#ifdef DEBUG
 char   *astring;
#endif

  if(hinst != 0 && hinst != -1)
  {
    if(Win32QueryModuleName(hinst,
                            modname,
                            sizeof(modname)) == 0)
    {
      dprintf(("USER32: OS2LoadStringUnicode: Can't determine handle of dll %08xh\n",
               hinst));
      return(0);
   }

   rc = DosQueryModuleHandle(modname,
                             &hmodule);
   if(rc)
   {
     dprintf(("USER32: OS2LoadStringUnicode: Can't determine handle of dll %s\n",
              modname));

     return(0);
   }
  }
  else
    hmodule = 0;

  rc = DosGetResource(hmodule,
                      RT_RCDATA,
                      wID,
                      (PPVOID)&szbuf);
  if(rc)
  {
    dprintf(("USER32: OS2LoadStringUnicode failed with rc %08xh\n",
             rc));

    if(lpBuffer)
      *lpBuffer = 0;

    return(0);  //TODO: SetLastError!
  }


  memcpy(lpBuffer,
         szbuf+1,
         min((*szbuf), cchBuffer-1) * sizeof(WCHAR));

  lpBuffer[min((*szbuf), cchBuffer-1)] = 0;

#ifdef DEBUG
  astring = UnicodeToAsciiString(lpBuffer);
  WriteLog("USER32: OS2LoadStringUnicode retrieved string '%s' of size %d\n",
           astring,
           (int)*szbuf);
  FreeAsciiString(astring);
#endif

  DosFreeResource(szbuf);
  return(UniStrlen(lpBuffer));           //not including null terminator!
}

