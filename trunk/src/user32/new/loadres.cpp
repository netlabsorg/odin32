/* $Id: loadres.cpp,v 1.5 1999-08-20 11:52:18 sandervl Exp $ */

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
#include <string.h>
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
	lpBuffer[cchBuffer-1] = 0;
	dprintf(("LoadStringA %s", lpBuffer));
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
	lpBuffer[cchBuffer-1] = 0;
    }
    delete winres;

    dprintf(("LoadStringW from %X, id %d buffersize %d\n", hinst, wID, cchBuffer));
    return(resstrlen);
}
//******************************************************************************
//TODO: Standard windows icons!
//******************************************************************************
HICON WIN32API LoadIconA(HINSTANCE hinst, LPCSTR lpszIcon)
{
 HICON rc;

    rc = (HICON)FindResourceA(hinst, lpszIcon, RT_ICONA);
    if(rc == 0) {
    	rc = (HICON)FindResourceA(hinst, lpszIcon, RT_GROUP_ICONA);
    }
    dprintf(("LoadIconA (%X) returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//TODO: Standard windows icons!
//******************************************************************************
HICON WIN32API LoadIconW(HINSTANCE hinst, LPCWSTR lpszIcon)
{
 HICON rc;

    rc = (HICON)FindResourceW(hinst, lpszIcon, RT_ICONW);
    if(rc == 0) {
    	rc = (HICON)FindResourceW(hinst, lpszIcon, RT_GROUP_ICONW);
    }
    dprintf(("LoadIconW (%X) returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//TODO: Standard windows cursors!
//******************************************************************************
HCURSOR WIN32API LoadCursorA(HINSTANCE hinst, LPCSTR lpszCursor)
{
 HCURSOR rc;

    rc = (HCURSOR) FindResourceA(hinst, lpszCursor, RT_CURSORA);

    dprintf(("LoadCursor from %X returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//TODO: Standard windows cursors!
//******************************************************************************
HCURSOR WIN32API LoadCursorW(HINSTANCE hinst, LPCWSTR lpszCursor)
{
 HCURSOR rc;

    rc = (HCURSOR) FindResourceW(hinst, lpszCursor, RT_CURSORW);

    dprintf(("LoadCursorW from %X returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//TODO: Standard windows bitmaps!
//******************************************************************************
HBITMAP WIN32API LoadBitmapA(HINSTANCE hinst, LPCSTR lpszBitmap)
{
 HBITMAP rc;

    rc = (HBITMAP) FindResourceA(hinst, lpszBitmap, RT_BITMAPA);

    dprintf(("LoadBitmapA from %X returned %d\n", hinst, rc));
    return(rc);
}
//******************************************************************************
//TODO: Standard windows bitmaps!
//******************************************************************************
HBITMAP WIN32API LoadBitmapW(HINSTANCE hinst, LPCWSTR lpszBitmap)
{
 HBITMAP rc;

    rc = (HBITMAP) FindResourceW(hinst, lpszBitmap, RT_BITMAPW);

    dprintf(("LoadBitmapW from %X returned %d\n", hinst, rc));
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
