/* $Id: RESSTRING.C,v 1.1 1999-05-24 20:19:59 ktk Exp $ */

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
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "resstring.h"
#include "win32util.h"
#include "unicode.h"

static char *UnicodeToAsciiN(WCHAR *wstring, char *astring, int maxlen);

/******************************************************************************/
//Strings stored as original: WORD    len 
//                            UNICODE string (not null terminated)
/******************************************************************************/
int OS2LoadStringAscii(HINSTANCE hinst, UINT wID, LPSTR lpBuffer, int cchBuffer)
{
 APIRET  rc;
 WCHAR  *szbuf;
 char    modname[128];
 HMODULE hmodule;

  if(hinst != 0 && hinst != -1) {
	if(Win32QueryModuleName(hinst, modname, sizeof(modname)) == 0) {
#ifdef DEBUG
		WriteLog("Can't determine handle of dll %X\n", hinst);
#endif
		return(0);
	}
	rc = DosQueryModuleHandle(modname, &hmodule);
	if(rc) {
#ifdef DEBUG
		WriteLog("Can't determine handle of dll %s\n", modname);
#endif
		return(0);
	}
  }
  else 	hmodule = 0;

  rc = DosGetResource(hmodule, RT_RCDATA, wID, (PPVOID)&szbuf);
  if(rc) {
#ifdef DEBUG
	WriteLog("OS2LoadString failed with rc %d\n", rc);
#endif
	if(lpBuffer)	*lpBuffer = 0;
	return(0);	//TODO: SetLastError!
  }
  UnicodeToAsciiN(szbuf+1, lpBuffer, min((*szbuf), cchBuffer-1));
#ifdef DEBUG
  WriteLog("OS2LoadString retrieved string '%s' (size %d)\n", lpBuffer, (int)*szbuf);
#endif
  DosFreeResource(szbuf);
  return(strlen(lpBuffer));	//not including null terminator!
}
//******************************************************************************
//******************************************************************************
int OS2LoadStringUnicode(HINSTANCE hinst, UINT wID, WCHAR *lpBuffer, int cchBuffer)
{
 APIRET  rc;
 WCHAR  *szbuf;
 char    modname[128];
 HMODULE hmodule;
#ifdef DEBUG
 char   *astring;
#endif

  if(hinst != 0 && hinst != -1) {
	if(Win32QueryModuleName(hinst, modname, sizeof(modname)) == 0) {
#ifdef DEBUG
		WriteLog("Can't determine handle of dll %X\n", hinst);
#endif
		return(0);
	}
	rc = DosQueryModuleHandle(modname, &hmodule);
	if(rc) {
#ifdef DEBUG
		WriteLog("Can't determine handle of dll %s\n", modname);
#endif
		return(0);
	}
  }
  else 	hmodule = 0;

  rc = DosGetResource(hmodule, RT_RCDATA, wID, (PPVOID)&szbuf);
  if(rc) {
#ifdef DEBUG
	WriteLog("OS2LoadString failed with rc %d\n", rc);
#endif
	if(lpBuffer)	*lpBuffer = 0;
	return(0);	//TODO: SetLastError!
  }
  memcpy(lpBuffer, szbuf+1, min((*szbuf), cchBuffer-1)*sizeof(WCHAR));
  lpBuffer[min((*szbuf), cchBuffer-1)] = 0;

#ifdef DEBUG
  astring = UnicodeToAsciiString(lpBuffer);
  WriteLog("OS2LoadStringW retrieved string '%s' of size %d\n", astring, (int)*szbuf);
  FreeAsciiString(astring);
#endif
  DosFreeResource(szbuf);
  return(UniStrlen(lpBuffer));	//not including null terminator!
}
//******************************************************************************
//******************************************************************************
static char *UnicodeToAsciiN(WCHAR *wstring, char *astring, int maxlen)
{
 int i;

  for(i=0;i<maxlen;i++) {
	astring[i] = (UCHAR)wstring[i];
  }
  astring[maxlen] = 0;
  return(astring);
}
/******************************************************************************/
/******************************************************************************/

