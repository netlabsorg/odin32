/* $Id: loadres.cpp,v 1.3 1999-07-20 07:42:35 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 resource API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#include <os2win.h>
#include "user32.h"
#include "resstring.h"

//NOTE: HINSTANCE 0xFFFFFFFF == hinstance executable
//******************************************************************************
//******************************************************************************
int WIN32API LoadStringA(HINSTANCE hinst, UINT wID, LPSTR lpBuffer, int cchBuffer)
{
    dprintf(("LoadString from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
    return OS2LoadStringAscii(hinst, wID, lpBuffer, cchBuffer);
}
//******************************************************************************
//******************************************************************************
int WIN32API LoadStringW(HINSTANCE hinst, UINT wID, LPWSTR lpBuffer, int cchBuffer)
{
    dprintf(("LoadStringW %d\n", wID));
    return OS2LoadStringUnicode(hinst, wID, (WCHAR *)lpBuffer, cchBuffer);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconA(HINSTANCE hinst, LPCSTR lpszIcon)
{
 HICON rc;

    if((int)lpszIcon >> 16 != 0) {//convert string name identifier to numeric id
         dprintf(("LoadIcon %s\n", lpszIcon));
	 lpszIcon = (LPCSTR)ConvertNameId(hinst, (char *)lpszIcon);
    }
    else dprintf(("LoadIcon %d\n", (int)lpszIcon));

    rc = O32_LoadIcon(hinst, lpszIcon);

    dprintf(("LoadIcon returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorA(HINSTANCE hinst, LPCSTR lpszCursor)
{
 HCURSOR rc;

    if((int)lpszCursor >> 16 != 0) {//convert string name identifier to numeric id
         dprintf(("LoadCursor %s\n", lpszCursor));
	 lpszCursor = (LPCSTR)ConvertNameId(hinst, (char *)lpszCursor);
    }
    else dprintf(("LoadCursor %d\n", (int)lpszCursor));

    rc = O32_LoadCursor(hinst, lpszCursor);

    dprintf(("LoadCursor from %X returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API LoadBitmapA(HINSTANCE hinst, LPCSTR lpszBitmap)
{
 HBITMAP rc;

  if((int)lpszBitmap >> 16 != 0) 
  {  //convert string name identifier to numeric id
    dprintf(("lpszBitmap [%s]\n",
             lpszBitmap));
    
    lpszBitmap = (LPCSTR)ConvertNameId(hinst, 
                                       (char *)lpszBitmap);
  }
  else 
    dprintf(("lpszBitmap %08xh\n",
             (int)lpszBitmap));

  rc = O32_LoadBitmap(hinst, lpszBitmap);

  dprintf(("LoadBitmapA returned %08xh\n",
           rc));
  
  return(rc);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API LoadBitmapW(HINSTANCE hinst, LPCWSTR lpszBitmap)
{
 char   *astring = NULL;
 HBITMAP rc;

    if((int)lpszBitmap >> 16 != 0) {//convert string name identifier to numeric id
	 astring = UnicodeToAsciiString((LPWSTR)lpszBitmap);
         dprintf(("lpszBitmap %s\n", astring));

	 lpszBitmap = (LPWSTR)ConvertNameId(hinst, (char *)astring);
    }
    else dprintf(("lpszBitmap %d\n", (int)lpszBitmap));

    rc = O32_LoadBitmap(hinst, (char *)lpszBitmap);
    if(astring)
	FreeAsciiString(astring);

    dprintf(("LoadBitmapW returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorW(HINSTANCE hinst, LPCWSTR lpszCursor)
{
 char   *astring = NULL;
 HCURSOR rc;

    if((int)lpszCursor >> 16 != 0) {//convert string name identifier to numeric id
	 astring = UnicodeToAsciiString((LPWSTR)lpszCursor);
         dprintf(("lpszCursor %s\n", astring));
	 lpszCursor = (LPWSTR)ConvertNameId(hinst, (char *)astring);
    }
    else dprintf(("lpszCursor %d\n", (int)lpszCursor));

    rc = O32_LoadCursor(hinst, (char *)lpszCursor);
    if(astring)
	FreeAsciiString(astring);

    dprintf(("LoadCursorW returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconW(HINSTANCE hinst, LPCWSTR lpszIcon)
{
 char  *astring = NULL;
 HICON rc;

    if((int)lpszIcon >> 16 != 0) {//convert string name identifier to numeric id
	 astring = UnicodeToAsciiString((LPWSTR)lpszIcon);

         dprintf(("lpszIcon %s\n", astring));
	 lpszIcon = (LPWSTR)ConvertNameId(hinst, (char *)astring);
    }
    else dprintf(("lpszIcon %d\n", (int)lpszIcon));

    rc = O32_LoadIcon(hinst, (char *)lpszIcon);
    if(astring)
	FreeAsciiString(astring);

    dprintf(("LoadIconW returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
//TODO: Far from complete, but works for loading resources from exe
//fuLoad flag ignored
//******************************************************************************
HANDLE WIN32API LoadImageA(HINSTANCE hinst, LPCSTR lpszName, UINT uType,
		           int cxDesired, int cyDesired, UINT fuLoad)
{
 HANDLE hRet = 0;

  dprintf(("OS2LoadImageA NOT COMPLETE (%d,%d)\n", cxDesired, cyDesired));

  switch(uType) {
	case IMAGE_BITMAP:
		hRet = (HANDLE)LoadBitmapA(hinst, lpszName);
		break;
	case IMAGE_CURSOR:
		hRet = (HANDLE)LoadCursorA(hinst, lpszName);
		break;
	case IMAGE_ICON:
		hRet = (HANDLE)LoadIconA(hinst, lpszName);
		break;
  }
  dprintf(("OS2LoadImageA returned %d\n", (int)hRet));

  return(hRet);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API LoadImageW(HINSTANCE hinst, LPCWSTR lpszName, UINT uType,
			      int cxDesired, int cyDesired, UINT fuLoad)
{
 HANDLE hRet = 0;

  dprintf(("OS2LoadImageW NOT COMPLETE (%d,%d)\n", cxDesired, cyDesired));

  switch(uType) {
	case IMAGE_BITMAP:
		hRet = (HANDLE)LoadBitmapW(hinst, lpszName);
		break;
	case IMAGE_CURSOR:
		hRet = (HANDLE)LoadCursorW(hinst, lpszName);
		break;
	case IMAGE_ICON:
		hRet = (HANDLE)LoadIconW(hinst, lpszName);
		break;
  }
  dprintf(("OS2LoadImageW returned %d\n", (int)hRet));

  return(hRet);
}
//******************************************************************************
//******************************************************************************
