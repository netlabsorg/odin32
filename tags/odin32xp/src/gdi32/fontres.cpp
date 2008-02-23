/* $Id: fontres.cpp,v 1.1 2004-01-11 11:42:13 sandervl Exp $ */


/*
 * GDI32 font apis
 *
 * Copyright 2003 Sander van Leeuwen (sandervl@innotek.de)
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
#include <ctype.h>
#include <string.h>
#include <dbglog.h>
#include <unicode.h>
#include <stats.h>
#include <objhandle.h>
#include "oslibgpi.h"

#define DBG_LOCALLOG    DBG_fontres
#include "dbglocal.h"

#define TESTFONT_RESNAME    	"TESTFONT"

//just picking a reasonable maximum; don't want to mess with linked lists
#define MAX_PUBLICFONTS		32

typedef struct {
  char  szFaceName[32];
  char  szFontResName[260];
  char  szFileName[260];
  char *pFontData;
  DWORD dwFontSize;
} SCALABLEFONTS;


static SCALABLEFONTS    *publicfonts = NULL;
static CRITICAL_SECTION  fntcritsect = {0};

#define FntLock()	EnterCriticalSection(&fntcritsect)
#define FntUnlock()     LeaveCriticalSection(&fntcritsect)


//******************************************************************************
// Remember the font filename for GetFontData if this is a font previously
// registered with AddFontResource
//******************************************************************************
BOOL RegisterFont(HFONT hFont, LPSTR lfFaceName)
{
    if(!publicfonts) {
        return FALSE;
    }

    FntLock();
    for(int i=0;i<MAX_PUBLICFONTS;i++) {
        if(strcmp(publicfonts[i].szFaceName, lfFaceName) == 0) {
#ifdef DEBUG
            dprintf(("Remember font file %s for font %x", publicfonts[i].szFileName, hFont));
            if(GetFileAttributesA(publicfonts[i].szFileName) == -1) {
                dprintf(("ERROR: font file does not exist anymore!!!!!!"));
            }
#endif
            ObjSetHandleGDI32Data(hFont, HNDL_FONT, (DWORD)&publicfonts[i]);
            break;
        }
    }
    FntUnlock();
    if(i == MAX_PUBLICFONTS) {
        return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFontData(HDC hdc, DWORD dwTable,
                           DWORD dwOffset,
                           LPVOID lpvBuffer,
                           DWORD dbData)
{
  dprintf(("GDI32: GetFontData, not implemented (GDI_ERROR)\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

  return(GDI_ERROR);
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceA(LPCSTR szFont)
{
    HINSTANCE hInstance;

    dprintf(("GDI32: AddFontResourceA %s", szFont));
    hInstance = LoadLibraryA(szFont);
    if(hInstance) {
        dprintf(("AddFontResourceA: executable file; NOT IMPLEMENTED"));
        FreeLibrary(hInstance);
        return 1;
    }
    return 1;
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceW(LPCWSTR szFont)
{
 char *astring = UnicodeToAsciiString((LPWSTR)szFont);
 BOOL  rc;

    dprintf(("GDI32: AddFontResourceW"));
    // NOTE: This will not work as is (needs UNICODE support)
    rc = AddFontResourceA(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceA(LPCSTR lpszFont)
{
    dprintf(("GDI32: RemoveFontResourceA %s", lpszFont));
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceW(LPCWSTR szFont)
{
 char *astring = UnicodeToAsciiString((LPWSTR)szFont);
 BOOL  rc;

    dprintf(("GDI32: RemoveFontResourceW"));
    rc = RemoveFontResourceA(astring);
    FreeAsciiString(astring);
    return(rc);
}
/*****************************************************************************
 * Name      : BOOL CreateScalableFontResourceA
 * Purpose   : The CreateScalableFontResourceA function creates a font resource
 *             file for a scalable font.
 * Parameters: DWORD   fdwHidden       flag for read-only embedded font
 *             LPCSTR lpszFontRes     address of filename for font resource
 *             LPCSTR lpszFontFile    address of filename for scalable font
 *             LPCSTR lpszCurrentPath address of path to font file
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CreateScalableFontResourceA(DWORD fdwHidden,
                                          LPCSTR lpszFontRes,
                                          LPCSTR lpszFontFile,
                                          LPCSTR lpszCurrentPath)
{
  dprintf(("GDI32: CreateScalableFontResourceA %x %s %s %s not implemented", fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath));

//  return OSLibGpiLoadFonts((LPSTR)lpszFontFile);
  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL CreateScalableFontResourceW
 * Purpose   : The CreateScalableFontResourceW function creates a font resource
 *             file for a scalable font.
 * Parameters: DWORD   fdwHidden       flag for read-only embedded font
 *             LPCSTR lpszFontRes     address of filename for font resource
 *             LPCSTR lpszFontFile    address of filename for scalable font
 *             LPCSTR lpszCurrentPath address of path to font file
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CreateScalableFontResourceW(DWORD fdwHidden,
                                          LPCWSTR lpszFontRes,
                                          LPCWSTR lpszFontFile,
                                          LPCWSTR lpszCurrentPath)
{
  LPSTR lpszFontFileA = NULL, lpszFontResA = NULL, lpszCurrentPathA = NULL;

  dprintf(("GDI32: CreateScalableFontResourceW %x %ls %ls %ls not implemented", fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath));

  STACK_strdupWtoA(lpszFontFile, lpszFontFileA);
  STACK_strdupWtoA(lpszFontRes, lpszFontResA);
  STACK_strdupWtoA(lpszCurrentPath, lpszCurrentPathA);
  return CreateScalableFontResourceA(fdwHidden, lpszFontResA, lpszFontFileA, lpszCurrentPathA);
}


