/* $Id: loadres.cpp,v 1.6 1999-08-20 15:03:41 sandervl Exp $ */

/*
 * Win32 resource API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <user32.h>
#include <winres.h>
#include <heapstring.h>

//******************************************************************************
//******************************************************************************
int WIN32API LoadStringA(HINSTANCE hinst, UINT wID, LPSTR lpBuffer, int cchBuffer)
{
 Win32Resource *winres;
 LPWSTR  resstring;
 int     resstrlen = 0;

    winres = (Win32Resource *)FindResourceA(hinst, (LPSTR)wID, RT_STRINGA);
    if(winres == NULL) {
	dprintf(("LoadStringA NOT FOUND from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
	return 0;
    }
    
    resstring = (LPWSTR)winres->lockResource();
    if(resstring) {
	resstrlen = min(lstrlenW(resstring)+1, cchBuffer);
	lstrcpynWtoA(lpBuffer, resstring, resstrlen);
	lpBuffer[resstrlen-1] = 0;
	resstrlen--;
	dprintf(("LoadStringA (%d) %s", resstrlen, lpBuffer));
    }
    delete winres;

    dprintf(("LoadStringA from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
    return(resstrlen);
}
//******************************************************************************
//******************************************************************************
int WIN32API LoadStringW(HINSTANCE hinst, UINT wID, LPWSTR lpBuffer, int cchBuffer)
{
 Win32Resource *winres;
 LPWSTR resstring;
 int    resstrlen = 0;

    winres = (Win32Resource *)FindResourceW(hinst, (LPWSTR)wID, RT_STRINGW);
    if(winres == NULL) {
	dprintf(("LoadStringW NOT FOUND from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
	return 0;
    }
    
    resstring = (LPWSTR)winres->lockResource();
    if(resstring) {
	resstrlen = min(lstrlenW(resstring)+1, cchBuffer);
	lstrcpynW(lpBuffer, resstring, resstrlen);
	lpBuffer[resstrlen-1] = 0;
	resstrlen--;
    }
    delete winres;

    dprintf(("LoadStringW from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
    return(resstrlen);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconA(HINSTANCE hinst, LPCSTR lpszIcon)
{
 HICON rc;

    rc = (HICON)FindResourceA(hinst, lpszIcon, RT_ICONA);
    dprintf(("LoadIconA (%X) returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconW(HINSTANCE hinst, LPCWSTR lpszIcon)
{
 HICON rc;

    rc = (HICON)FindResourceW(hinst, lpszIcon, RT_ICONW);
    dprintf(("LoadIconW (%X) returned %d\n", hinst, rc));
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
