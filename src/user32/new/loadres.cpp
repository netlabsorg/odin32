/* $Id: loadres.cpp,v 1.7 1999-08-20 20:09:50 sandervl Exp $ */

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
#include <oslibres.h>

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
 Win32Resource *winres;
 HICON          hIcon;

    hIcon = OSLibWinQuerySysIcon((ULONG)lpszIcon);
    if(hIcon == 0) {//not a system icon
    	winres = (Win32Resource *)FindResourceA(hinst, lpszIcon, RT_ICONA);
    	if(winres == 0) {
    		winres = (Win32Resource *)FindResourceA(hinst, lpszIcon, RT_GROUP_ICONA);
    	}
    	if(winres) {
		hIcon = OSLibWinCreateIcon(winres->lockOS2Resource());
		delete winres;
    	}
    }
    dprintf(("LoadIconA (%X) returned %x\n", hinst, hIcon));

    return(hIcon);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API LoadIconW(HINSTANCE hinst, LPCWSTR lpszIcon)
{
 Win32Resource *winres;
 HICON          hIcon;

    hIcon = OSLibWinQuerySysIcon((ULONG)lpszIcon);
    if(hIcon == 0) {//not a system icon
    	winres = (Win32Resource *)FindResourceW(hinst, lpszIcon, RT_ICONW);
    	if(winres == 0) {
    		winres = (Win32Resource *)FindResourceW(hinst, lpszIcon, RT_GROUP_ICONW);
    	}
    	if(winres) {
		hIcon = OSLibWinCreateIcon(winres->lockOS2Resource());
		delete winres;
    	}
    }
    dprintf(("LoadIconW (%X) returned %x\n", hinst, hIcon));

    return(hIcon);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorA(HINSTANCE hinst, LPCSTR lpszCursor)
{
 Win32Resource *winres;
 HCURSOR        hCursor;

    hCursor = OSLibWinQuerySysPointer((ULONG)lpszCursor);
    if(hCursor == 0) {//not a system pointer
    	winres = (Win32Resource *)FindResourceA(hinst, lpszCursor, RT_CURSORA);
    	if(winres == 0) {
    		winres = (Win32Resource *)FindResourceA(hinst, lpszCursor, RT_GROUP_CURSORA);
    	}
    	if(winres) {
		hCursor = OSLibWinCreatePointer(winres->lockOS2Resource());
		delete winres;
    	}
    }
    dprintf(("LoadCursorA (%X) returned %x\n", hinst, hCursor));

    return(hCursor);
}
//******************************************************************************
//******************************************************************************
HCURSOR WIN32API LoadCursorW(HINSTANCE hinst, LPCWSTR lpszCursor)
{
 Win32Resource *winres;
 HCURSOR        hCursor;

    hCursor = OSLibWinQuerySysPointer((ULONG)lpszCursor);
    if(hCursor == 0) {//not a system pointer
    	winres = (Win32Resource *)FindResourceW(hinst, lpszCursor, RT_CURSORW);
    	if(winres == 0) {
    		winres = (Win32Resource *)FindResourceW(hinst, lpszCursor, RT_GROUP_CURSORW);
    	}
    	if(winres) {
		hCursor = OSLibWinCreatePointer(winres->lockOS2Resource());
		delete winres;
    	}
    }
    dprintf(("LoadCursorW (%X) returned %x\n", hinst, hCursor));

    return(hCursor);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API LoadBitmapA(HINSTANCE hinst, LPCSTR lpszBitmap)
{
 HBITMAP rc;

  if(HIWORD(lpszBitmap) != 0) 
  {  	//convert string name identifier to numeric id
    	dprintf(("lpszBitmap [%s]\n", lpszBitmap));
    
    	lpszBitmap = (LPCSTR)ConvertNameId(hinst, (char *)lpszBitmap);
  }
  else  dprintf(("lpszBitmap %08xh\n", (int)lpszBitmap));

  rc = O32_LoadBitmap(hinst, lpszBitmap);

  dprintf(("LoadBitmapA returned %08xh\n", rc));
  
  return(rc);
}
//******************************************************************************
//******************************************************************************
HBITMAP WIN32API LoadBitmapW(HINSTANCE hinst, LPCWSTR lpszBitmap)
{
 char   *astring = NULL;
 HBITMAP rc;

    if(HIWORD(lpszBitmap) != 0) {//convert string name identifier to numeric id
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
//TODO: Far from complete, but works for loading resources from exe
//fuLoad flag ignored
//******************************************************************************
HANDLE WIN32API LoadImageA(HINSTANCE hinst, LPCSTR lpszName, UINT uType,
		           int cxDesired, int cyDesired, UINT fuLoad)
{
 HANDLE hRet = 0;

  dprintf(("LoadImageA NOT COMPLETE (%d,%d)\n", cxDesired, cyDesired));

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
	default:
		dprintf(("LoadImageA: unsupported type %d!!", uType));
		return 0;
  }
  dprintf(("LoadImageA returned %d\n", (int)hRet));

  return(hRet);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API LoadImageW(HINSTANCE hinst, LPCWSTR lpszName, UINT uType,
			      int cxDesired, int cyDesired, UINT fuLoad)
{
 HANDLE hRet = 0;

  dprintf(("LoadImageW NOT COMPLETE (%d,%d)\n", cxDesired, cyDesired));

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
	default:
		dprintf(("LoadImageW: unsupported type %d!!", uType));
		return 0;
  }
  dprintf(("LoadImageW returned %d\n", (int)hRet));

  return(hRet);
}
//******************************************************************************
//******************************************************************************
