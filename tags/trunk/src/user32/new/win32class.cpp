/* $Id: win32class.cpp,v 1.1 1999-05-24 20:20:04 ktk Exp $ */

/*
 * Win32 Window Class Managment Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <assert.h>
#include "misc.h"
#include "user32.h"
#include "win32class.h"
#include "win32wnd.h"

//******************************************************************************
//Win32WndClass methods:
//******************************************************************************
Win32WndClass::Win32WndClass(WNDCLASSEXA wndclass, BOOL isUnicode)
{
  //Insert it in front of the rest
  next        = wndclasses;
  wndclasses  = this;
  if((ULONG)wndclass->lpszClassName >> 16 != 0) {
	className  = (char *) malloc(strlen(wndclass->lpszClassName)+1);
	classNameW = (WCHAR *)malloc((strlen(wndclass->lpszClassName)+1)*sizeof(WCHAR));
	if(className == NULL || classNameW == NULL) {
		dprintf(("Win32Class ctr; className/classNameW == NULL"));
		exit(1);
	}
	strcpy(className, wndclass->lpszClassName);
	AsciiToUnicode(className, classNameW);
	classAtom    	= GlobalAddAtom(className);
  }
  else {
	className 	= NULL;
	classNameW      = NULL;
	classAtom	= (DWORD)wndclass->lpszClassName;
  }
  this->isUnicode       = isUnicode;

  dprintf(("USER32:  Win32Class ctor\n"));
  dprintf(("USER32:  wndclass->style %X\n", wndclass->style));
  dprintf(("USER32:  wndclass->lpfnWndProc %X\n", wndclass->lpfnWndProc));
  dprintf(("USER32:  wndclass->cbClsExtra %X\n", wndclass->cbClsExtra));
  dprintf(("USER32:  wndclass->cbWndExtra %X\n", wndclass->cbWndExtra));
  dprintf(("USER32:  wndclass->hInstance %X\n", wndclass->hInstance));
  dprintf(("USER32:  wndclass->hIcon %X\n", wndclass->hIcon));
  dprintf(("USER32:  wndclass->hCursor %X\n", wndclass->hCursor));
  dprintf(("USER32:  wndclass->hbrBackground %X\n", wndclass->hbrBackground));
  if((ULONG)wndclass->lpszClassName >> 16 == 0)
       dprintf(("USER32:  wndclass->lpszClassName %X\n", wndclass->lpszClassName));
  else dprintf(("USER32:  wndclass->lpszClassName %s\n", wndclass->lpszClassName));

  if((ULONG)wc.lpszMenuName >> 16 != 0) {//convert string name identifier to numeric id
       dprintf(("USER32:  lpszMenuName %s\n", wndclass->lpszMenuName));
  }
  else dprintf(("USER32:  wndclass->lpszMenuName %X\n", wndclass->lpszMenuName));

  nrExtraClassWords	= wndclass->cbClsExtra;
  nrExtraWindowWords	= wndclass->cbWndExtra;
  backgroundBrush	= wndclass->hbrBackGround;	//TODO: fErase of PAINSTRUCT in WM_PAINT if == NULL
  hCursor		= wndclass->hCursor;
  hIcon			= wndclass->hIcon;
  hInstance		= wndclass->hInstance;
  if(wndclass->lpszMenuName && (ULONG)wndclass->lpszMenuName >> 16) {
	menuName  = (ULONG)malloc(strlen(wndclass->lpszMenuName)+1);
	menuNameW = (ULONG)malloc((strlen(wndclass->lpszMenuName)+1)*sizeof(WCHAR));
	if(menuName == NULL || menuNameW == NULL) {
		dprintf(("Win32Class ctr; menuName/menuNameW == NULL"));
		exit(1);
	}
	strcpy((char *)menuName, wndclass->lpszMenuName);
	AsciiToUnicode(menuName, menuNameW);
  }
  else {
	menuName  = (ULONG)wndclass->lpszMenuName;
	menuNameW = (ULONG)wndclass->lpszMenuName;
  }

  windowStyle		= wndclass->style;
  windowProc		= wndclass->lpfnWndProc;

  //User data class words/longs
  if(nrExtraClassWords) {
	userClassLong = (ULONG *)malloc(nrExtraClassWords);
	if(userClassLong == NULL) {
		dprintf(("Win32Class ctr: userClassLong == NULL!"));
		exit(1);
	}
	memset(userClassLong, 0, nrExtraClassWords);
  }
  else	userClassLong = NULL;

  hIconSm = wndclass->hIconSm;

  dprintf(("Win32WndClass::Win32WndClass %d\n", id));
}
//******************************************************************************
//******************************************************************************
Win32WndClass::~Win32WndClass()
{
  Win32WndClass *wndclass = Win32WndClass::wndclasses;

  if(wndclass == this) {
	wndclasses = next;
  }
  else {
	while(wndclass->next != NULL) {
		if(wndclass->next == this) {
			wndclass->next = next;
			break;
		}
		wndclass = wndclass->next;
	}
  }
  if(userClassLong)	free(userClassLong);
  if(className)		free(className);
  if(classNameW)	free(classNameW);
  if(menuName && (ULONG)menuName >> 16) {
	free(menuName)
	assert(menuNameW);
	free(menuNameW);
  }
}
//******************************************************************************
//******************************************************************************
Win32WndClass *Win32WndClass::FindClass(HINSTANCE hInstance, LPSTR id)
{
  Win32WndClass *wndclass = wndclasses;

  if(wndclass == NULL)	return(NULL);

  if((ULONG)id >> 16 != 0) {
	if(stricmp(wndclass->className, id) == 0 && wndclass->hInstance == hInstance) {
		return(wndclass);
	}
	else {
		wndclass = wndclass->next;
		while(wndclass != NULL) {
			if(stricmp(wndclass->className, id) == 0 && wndclass->hInstance == hInstance) {
				return(wndclass);
			}
			wndclass = wndclass->next;
	        }
	}
  }
  else {
	if(wndclass->classAtom == (DWORD)id && wndclass->hInstance == hInstance) {
		return(wndclass);
	}
	else {
		wndclass = wndclass->next;
		while(wndclass != NULL) {
			if(wndclass->classAtom == (DWORD)id && wndclass->hInstance == hInstance) {
				return(wndclass);
			}
			wndclass = wndclass->next;
	        }
	}
  }
  dprintf(("Class %X (inst %X) not found!", id, hInstance);
  return(NULL);
}
//******************************************************************************
//******************************************************************************
BOOL Win32WndClass::getClassInfo(WNDCLASSEXA *wndclass)
{
  wndclass->cbClsExtra    = nrExtraClassWords;
  wndclass->cbWndExtra    = nrExtraWindowWords;
  wndclass->hbrBackGround = backgroundBrush;
  wndclass->hCursor	  = hCursor;
  wndclass->hIcon         = hIcon;
  wndclass->hInstance     = hInstance;
  wndclass->lpszMenuName  = (LPCTSTR)menuName;
  wndclass->lpszClassName = (className) ? (LPCTSTR)className : (LPCTSTR)classAtom;
  wndclass->style	  = windowStyle;
  wndclass->lpfnWndProc	  = windowProc;
  wndclass->hIconSm	  = hIconSm;
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL Win32WndClass::getClassInfo(WNDCLASSEXW *wndclass)
{
  wndclass->cbClsExtra    = nrExtraClassWords;
  wndclass->cbWndExtra    = nrExtraWindowWords;
  wndclass->hbrBackGround = backgroundBrush;
  wndclass->hCursor	  = hCursor;
  wndclass->hIcon         = hIcon;
  wndclass->hInstance     = hInstance;
  wndclass->lpszMenuName  = (LPCWSTR)menuNameW;
  wndclass->lpszClassName = (classNameW) ? (LPCWSTR)classNameW : (LPCWSTR)classAtom;
  wndclass->style	  = windowStyle;
  wndclass->lpfnWndProc	  = windowProc;
  wndclass->hIconSm	  = hIconSm;
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ULONG Win32WndClass::getClassName(LPSTR lpszClassName, ULONG cchClassName)
{
  if((ULONG)className >> 16 != 0) {
	strncpy(lpszClassName, className, cchClassName-1);
	return strlen(lpszClassName);
  }
  *(ULONG *)lpszClassName = classAtom;
  return(sizeof(ULONG));
}
//******************************************************************************
//******************************************************************************
ULONG Win32WndClass::getClassName(LPWSTR lpszClassName, ULONG cchClassName)
{
 ULONG len;

  if((ULONG)classNameW >> 16 != 0) {
	len = min(UniStrLen((UniChar*)classNameW)+1, cchClassName);
	memcpy(lpszClassName, classNameW, len*sizeof(WCHAR));
	return len;
  }
  *(ULONG *)lpszClassName = classAtom;
  return(sizeof(ULONG));
}
//******************************************************************************
//******************************************************************************
ULONG Win32WndClass::getClassLongA(int index, BOOL isUnicode)
{
  switch(index) {
	case GCL_CBCLSEXTRA:
		return nrExtraClassWords;
	case GCL_CBWNDEXTRA:
		return nrExtraWindowWords;
	case GCL_HBRBACKGROUND:
		return backgroundBrush;
	case GCL_HCURSOR:
		return hCursor;
	case GCL_HICON:
		return hIcon;
	case GCL_HMODULE:
		return hInstance;
	case GCL_MENUNAME:
		return (isUnicode) ? menuNameW : menuName;
	case GCL_STYLE:
		return windowStyle;
	case GCL_WNDPROC:
		return windowProc;
	case GCW_ATOM: //TODO: does this really happen in windows?
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	default:
		if(index > 0 && index < nrExtraClassWords - sizeof(ULONG)) {
			return userClassLong[index];
		}
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
  }
}
//******************************************************************************
//******************************************************************************
WORD Win32WndClass::getClassWord(int index);
{
  switch(index) {
	case GCW_ATOM: 
		return (WORD)classAtom;
	default:
		if(index > 0 && index < nrExtraClassWords - sizeof(WORD)) {
			return ((WORD *)userClassLong)[index];
		}
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
  }
}
//******************************************************************************
//TODO: What effects what immediately?
//******************************************************************************
ULONG Win32WndClass::setClassLong(int index, LONG lNewVal, BOOL isUnicode);
{
 ULONG rc;

  switch(index) {
	case GCL_CBCLSEXTRA: //TODO (doesn't affect allocated classes, so what does it do?)
		rc = nrExtraClassWords;
//		nrExtraClassWords = lNewVal;
		break;
	case GCL_CBWNDEXTRA:
		rc = nrExtraWindowWords;
		nrExtraWindowWords = lNewVal;
		break;
	case GCL_HBRBACKGROUND:
		rc = backgroundBrush;
		backgroundBrush = lNewVal;
		break;
	case GCL_HCURSOR:
		rc = hCursor;
		hCursor = lNewVal;
		break;
	case GCL_HICON:
		rc = hIcon;
		hIcon = lNewVal;
		break;
	case GCL_HMODULE:
		rc = hInstance;
		hInstance = lNewVal;
		break;
	case GCL_MENUNAME:
		if(isUnicode) {
			rc = menuNameW;
			menuNameW = lNewVal;	//FIXME
		}
		else {	
			rc = menuName;
			menuName = lNewVal;	//FIXME
		}
		break;
	case GCL_STYLE:
		rc = windowStyle;
		windowStyle = lNewVal;
		break;
	case GCL_WNDPROC:
		rc = windowProc;
		windowProc = lNewVal;
		break;
	case GCW_ATOM: //TODO: does this really happen in windows?
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	default:
		if(index > 0 && index < nrExtraClassWords - sizeof(ULONG)) {
			rc = userClassLong[index];
			userClassLong[index] = lNewVal;
			return(rc);
		}
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
  }
  return(rc);
}
//******************************************************************************
//******************************************************************************
WORD Win32WndClass::setClassWord(int index, WORD wNewVal);
{
 WORD rc;

  switch(index) {
	case GCW_ATOM: 
		rc = (WORD)classAtom;
		classAtom = wNewVal;
		return(rc);
	default:
		if(index > 0 && index < nrExtraClassWords - sizeof(WORD)) {
			rc = ((WORD *)userClassLong)[index];
			((WORD *)userClassLong)[index] = wNewVal;
			return(rc);
		}
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
  }
}
//******************************************************************************
//FIXME: Windows that still exists with this class
//******************************************************************************
void Win32WndClass::UnregisterClassA(HINSTANCE hinst, LPSTR id)
{
  Win32WndClass *wndclass;

  dprintf(("::UnregisterClass, destroy class %X!!\n", id));
  wndclass = FindClass(hinst, id);
  if(wndclass) {
	delete wndclass;
	return;
  }
  dprintf(("::UnregisterClass, couldn't find class %X!!\n", id));
}
//******************************************************************************
//******************************************************************************
Win32WndClass *Win32WndClass::wndclasses = NULL;

//******************************************************************************
//SvL: 18-7-'98: Moved here from user32.cpp
//******************************************************************************
ATOM WIN32API OS2RegisterClassA(WNDCLASS *lpWndClass)
{
 WNDCLASSEXA wc;
 Win32Class *wclass;

   memcpy(&wc, lpWndClass, sizeof(WNDCLASS));
   wc.hIconSm = 0;
    
   wclass = new Win32Class(&wc);
   if(wclass == NULL) {
	dprintf(("RegisterClassA wclass == NULL!"));
	return(0);
   }
   return(wclass->getAtom());
}
//******************************************************************************
//SvL: 18-7-'98: Moved here from user32.cpp
//******************************************************************************
ATOM WIN32API OS2RegisterClassExA(WNDCLASSEXA *lpWndClass)
{
 Win32Class *wclass;

   wclass = new Win32Class(lpWndClass);
   if(wclass == NULL) {
	dprintf(("RegisterClassExA wclass == NULL!"));
	return(0);
   }
   return(wclass->getAtom());
}
//******************************************************************************
//******************************************************************************
WORD WIN32API OS2RegisterClassW(WNDCLASSW *lpwc)
{
 ATOM rc;
 WNDCLASSEX wclass;

    dprintf(("RegisterClassW\n"));
    memcpy(&wclass, lpwc, sizeof(WNDCLASS));
    if(wclass.lpszMenuName && ((ULONG)wclass.lpszMenuName >> 16 != 0)) {
	wclass.lpszMenuName = UnicodeToAsciiString((LPWSTR)lpwc->lpszMenuName);
    }
    if(wclass.lpszClassName && ((ULONG)wclass.lpszClassName >> 16 != 0)) {
	wclass.lpszClassName = UnicodeToAsciiString((LPWSTR)lpwc->lpszClassName);
    }
    rc = OS2RegisterClassA(&wclass);

    if(lpwc->lpszMenuName && ((ULONG)lpwc->lpszMenuName >> 16 != 0)) {
	FreeAsciiString((char *)wclass.lpszMenuName);
    }
    if(lpwc->lpszClassName && ((ULONG)lpwc->lpszClassName >> 16 != 0)) {
	FreeAsciiString((char *)wclass.lpszClassName);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API OS2RegisterClassExW(CONST WNDCLASSEXW *lpwc)
{
 ATOM rc;
 WNDCLASSEXA wclass;

    dprintf(("RegisterClassExW\n"));
    memcpy(&wclass, lpwc, sizeof(WNDCLASSEXA));
    if(wclass.lpszMenuName && ((ULONG)wclass.lpszMenuName >> 16 != 0)) {
	wclass.lpszMenuName = UnicodeToAsciiString((LPWSTR)lpwc->lpszMenuName);
    }
    if(wclass.lpszClassName && ((ULONG)wclass.lpszClassName >> 16 != 0)) {
	wclass.lpszClassName = UnicodeToAsciiString((LPWSTR)lpwc->lpszClassName);
    }
    rc = OS2RegisterClassExA(&wclass);

    if(lpwc->lpszMenuName && ((ULONG)lpwc->lpszMenuName >> 16 != 0)) {
	FreeAsciiString((char *)wclass.lpszMenuName);
    }
    if(lpwc->lpszClassName && ((ULONG)lpwc->lpszClassName >> 16 != 0)) {
	FreeAsciiString((char *)wclass.lpszClassName);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OS2UnregisterClassA(LPCSTR lpszClassName, HINSTANCE hinst)
{
   dprintf(("USER32:  OS2UnregisterClassA\n"));
   Win32WndClass::UnregisterClass(hinst, (LPSTR)lpszClassName);

   //Spintest returns FALSE in dll termination, so pretend it succeeded
   return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OS2UnregisterClassW(LPWSTR lpszClassName, HINSTANCE hinst)
{
 char *astring = NULL;

  dprintf(("USER32:  OS2UnregisterClassW\n"));
  if((ULONG)lpszClassName >> 16 != 0) {
	astring = UnicodeToAsciiString(lpszClassName);
  }
  else 	astring = (char *)lpszClassName;

  Win32WndClass::UnregisterClass(hinst, (LPSTR)astring);
  if(astring >> 16 != 0)	
	FreeAsciiString((char *)astring);
  //Spintest returns FALSE in dll termination, so pretend it succeeded
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OS2GetClassInfoA(HINSTANCE hInstance, LPCSTR lpszClass, PWNDCLASSA lpwc)
{
 WNDCLASSEXA    wc;
 BOOL           rc;
 Win32WndClass *wndclass;

  dprintf(("USER32:  OS2GetClassInfoA\n"));

  wndclass = FindClass(hInstance, lpszClass);
  if(wndclass) {
	wndclass->getClassInfo(&wc);
	memcpy(lpwc, &wc, sizeof(WNDCLASSA));
	return(TRUE);
  }
  return(FALSE);  
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OS2GetClassInfoW(HINSTANCE  hinst, LPWSTR lpszClass, PWNDCLASSW lpwc)
{
 WNDCLASSEXW    wc;
 BOOL           rc;
 Win32WndClass *wndclass;
 char          *astring = NULL;

  dprintf(("USER32:  OS2GetClassInfoW\n"));

  if((ULONG)lpszClass >> 16 != 0) {
	astring = UnicodeToAsciiString(lpszClass);
  }
  else  astring = (char *)lpszClass;

  wndclass = FindClass(hInstance, astring);
  if((ULONG)astring >> 16 != 0)	
	FreeAsciiString((char *)astring);
  if(wndclass) {
	wndclass->getClassInfo(&wc);
	memcpy(lpwc, &wc, sizeof(WNDCLASSW));
	return(TRUE);
  }
  return(FALSE);  
}
/****************************************************************************
 * Name      : BOOL WIN32API OS2GetClassInfoExA
 * Purpose   : The GetClassInfoEx function retrieves information about a window
 *             class, including the handle of the small icon associated with the
 *             window class. GetClassInfo does not retrieve the handle of the small icon.
 * Parameters: HINSTANCE    hinst     handle of application instance
 *             LPCTSTR      lpszClass address of class name string
 *             LPWNDCLASSEX lpwcx     address of structure for class data
 * Variables :
 * Result    : If the function finds a matching class and successfully copies
 *               the data, the return value is TRUE;
 *             otherwise, it is FALSE.
 *             To get extended error information, call GetLastError.
 * Remark    : PH: does not obtain handle of the small icon
 *****************************************************************************/
BOOL WIN32API OS2GetClassInfoExA(HINSTANCE     hInstance,
                                 LPCTSTR       lpszClass,
                                 LPWNDCLASSEXA lpwcx)
{
 BOOL           rc;
 Win32WndClass *wndclass;
  
  dprintf(("USER32:GetClassInfoExA (%08xh,%x,%08x).\n",
         hInstance,
         lpszClass,
         lpwcx));
  
  wndclass = FindClass(hInstance, lpszClass);
  if(wndclass) {
	wndclass->getClassInfo(lpwcx);
	return(TRUE);
  }
  return(FALSE);  
}
/*****************************************************************************
 * Name      : BOOL WIN32API OS2GetClassInfoExW
 * Purpose   : The GetClassInfoEx function retrieves information about a window
 *             class, including the handle of the small icon associated with the
 *             window class. GetClassInfo does not retrieve the handle of the small icon.
 * Parameters: HINSTANCE    hinst     handle of application instance
 *             LPCWSTR      lpszClass address of class name string
 *             LPWNDCLASSEX lpwcx     address of structure for class data
 * Variables :
 * Result    : If the function finds a matching class and successfully copies
 *               the data, the return value is TRUE;
 *             otherwise, it is FALSE.
 *             To get extended error information, call GetLastError.
 * Remark    : does not obtain handle of the small icon
 *
 *****************************************************************************/
BOOL WIN32API OS2GetClassInfoExW(HINSTANCE     hInstance,
                                 LPCWSTR       lpszClass,
                                 LPWNDCLASSEXW lpwcx)
{
 BOOL           rc;
 Win32WndClass *wndclass;
 char          *astring = NULL;

  dprintf(("USER32: OS2GetClassInfoExW\n"));

  if((ULONG)lpszClass >> 16 != 0) {
	astring = UnicodeToAsciiString(lpszClass);
  }
  else  astring = (char *)lpszClass;

  wndclass = FindClass(hInstance, astring);
  if((ULONG)astring >> 16 != 0)	
	FreeAsciiString((char *)astring);
  if(wndclass) {
	wndclass->getClassInfo(lpwcx);
	return(TRUE);
  }
  return(FALSE);  
}
//******************************************************************************
//******************************************************************************
int WIN32API OS2GetClassNameA(HWND hwnd, LPSTR lpszClassName, int cchClassName)
{
 Win32Window *wnd;

    dprintf(("USER32: OS2GetClassNameA\n"));
    wnd = WIN2OS2HWND(hwnd);
    if(wnd == NULL) {
	dprintf(("GetClassNameA wnd == NULL"));
	return(0);
    }
    return (wnd->getClass())->getClassName(lpszClassName, cchClassName);
}
//******************************************************************************
//******************************************************************************
int WIN32API OS2GetClassNameW(HWND hwnd, LPWSTR lpszClassName, int cchClassName)
{
 Win32Window *wnd;

    dprintf(("USER32: OS2GetClassNameW\n"));
    wnd = WIN2OS2HWND(hwnd);
    if(wnd == NULL) {
	dprintf(("GetClassNameA wnd == NULL"));
	return(0);
    }
    return (wnd->getClass())->getClassName(lpszClassName, cchClassName);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2SetClassLongA(HWND hwnd, int nIndex, LONG lNewVal)
{
 Win32Window *wnd;

    dprintf(("USER32: OS2SetClassLongA\n"));
    wnd = WIN2OS2HWND(hwnd);
    if(wnd == NULL) {
	dprintf(("SetClassLongA wnd == NULL"));
	return(0);
    }
    return (wnd->getClass())->setClassLongA(nIndex, lNewVal);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2SetClassLongW(HWND hwnd, int nIndex, LONG lNewVal)
{
 Win32Window *wnd;

    dprintf(("USER32: OS2SetClassLongW\n"));
    wnd = WIN2OS2HWND(hwnd);
    if(wnd == NULL) {
	dprintf(("SetClassLongW wnd == NULL"));
	return(0);
    }
    return (wnd->getClass())->setClassLongW(nIndex, lNewVal);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API OS2SetClassWord(HWND hwnd, int nIndex, WORD  wNewVal)
{
 Win32Window *wnd;

    dprintf(("USER32: OS2SetClassWordA\n"));
    wnd = WIN2OS2HWND(hwnd);
    if(wnd == NULL) {
	dprintf(("SetClassWordA wnd == NULL"));
	return(0);
    }
    return (wnd->getClass())->setClassWord(nIndex, wNewVal);
}
//******************************************************************************
//******************************************************************************
WORD WIN32API OS2GetClassWord(HWND hwnd, int nIndex)
{
 Win32Window *wnd;

    dprintf(("USER32: OS2GetClassWordA\n"));
    wnd = WIN2OS2HWND(hwnd);
    if(wnd == NULL) {
	dprintf(("GetClassWordA wnd == NULL"));
	return(0);
    }
    return (wnd->getClass())->getClassWord(nIndex);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2GetClassLongA(HWND hwnd, int nIndex)
{
 Win32Window *wnd;

    dprintf(("USER32: OS2GetClassLongA\n"));
    wnd = WIN2OS2HWND(hwnd);
    if(wnd == NULL) {
	dprintf(("OS2GetClassLongA wnd == NULL"));
	return(0);
    }
    return (wnd->getClass())->getClassLongA(nIndex);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API OS2GetClassLongW(HWND hwnd, int nIndex)
{
 Win32Window *wnd;

    dprintf(("USER32: OS2GetClassLongW\n"));
    wnd = WIN2OS2HWND(hwnd);
    if(wnd == NULL) {
	dprintf(("OS2GetClassLongW wnd == NULL"));
	return(0);
    }
    return (wnd->getClass())->getClassLongW(nIndex);
}
//******************************************************************************
//******************************************************************************
