/* $Id: win32class.cpp,v 1.2 1999-07-14 08:35:36 sandervl Exp $ */
/*
 * Win32 Window Class Managment Code for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include <win32class.h>
#include <win32wnd.h>

//******************************************************************************
//Win32WndClass methods:
//******************************************************************************
Win32WndClass::Win32WndClass(WNDCLASSEXA *wndclass, BOOL isUnicode) : GenericObject(&wndclasses, OBJTYPE_CLASS)
{
  if(HIWORD(wndclass->lpszClassName)) {
	if(isUnicode) {
	  	classNameA = (PCHAR)malloc(lstrlenW((LPWSTR)wndclass->lpszClassName)+1);
	  	classNameW = (WCHAR *)malloc((lstrlenW((LPWSTR)wndclass->lpszClassName)+1)*sizeof(WCHAR));
	}
	else {
	  	classNameA = (PCHAR)malloc(strlen(wndclass->lpszClassName)+1);
	  	classNameW = (WCHAR *)malloc((strlen(wndclass->lpszClassName)+1)*sizeof(WCHAR));
	}
	if(classNameA == NULL || classNameW == NULL) {
		dprintf(("Win32Class ctr; classNameA/classNameW == NULL"));
		exit(1);
	}
	if(isUnicode) {
		lstrcpyW(classNameW, (LPWSTR)wndclass->lpszClassName);
		UnicodeToAscii(classNameW, classNameA);
	}
	else {
		strcpy((char *)classNameA, wndclass->lpszClassName);
		AsciiToUnicode(classNameA, classNameW);
	}
	classAtom    	= GlobalAddAtomA(classNameA);
  }
  else {
	classNameA 	= NULL;
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
  if(HIWORD(wndclass->lpszClassName))
       dprintf(("USER32:  wndclass->lpszClassName %X\n", wndclass->lpszClassName));
  else dprintf(("USER32:  wndclass->lpszClassName %s\n", wndclass->lpszClassName));

  if(HIWORD(wndclass->lpszMenuName)) {//convert string name identifier to numeric id
       dprintf(("USER32:  lpszMenuName %s\n", wndclass->lpszMenuName));
  }
  else dprintf(("USER32:  wndclass->lpszMenuName %X\n", wndclass->lpszMenuName));

  nrExtraClassWords	= wndclass->cbClsExtra;
  nrExtraWindowWords	= wndclass->cbWndExtra;
  backgroundBrush	= wndclass->hbrBackground;	//TODO: fErase of PAINSTRUCT in WM_PAINT if == NULL
  hCursor		= wndclass->hCursor;
  hIcon			= wndclass->hIcon;
  hInstance		= wndclass->hInstance;

  menuNameA = 0;
  menuNameW = 0;
  setMenuName((LPSTR)wndclass->lpszMenuName);

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

  cWindows = 0;
  hIconSm  = wndclass->hIconSm;
}
//******************************************************************************
//******************************************************************************
Win32WndClass::~Win32WndClass()
{
  if(userClassLong)	free(userClassLong);
  if(classNameA)	free(classNameA);
  if(classNameW)	free(classNameW);
  if(menuNameA && HIWORD(menuNameA)) {
	free(menuNameA);
	assert(menuNameW);
	free(menuNameW);
  }
}
//******************************************************************************
//******************************************************************************
Win32WndClass *Win32WndClass::FindClass(HINSTANCE hInstance, LPSTR id)
{
  Win32WndClass *wndclass = (Win32WndClass *)wndclasses;

  if(wndclass == NULL)	return(NULL);

  if(HIWORD(id) != 0) {
	if(stricmp(wndclass->classNameA, id) == 0 && wndclass->hInstance == hInstance) {
		return(wndclass);
	}
	else {
		wndclass = (Win32WndClass *)wndclass->GetNext();
		while(wndclass != NULL) {
			if(stricmp(wndclass->classNameA, id) == 0 && wndclass->hInstance == hInstance) {
				return(wndclass);
			}
			wndclass = (Win32WndClass *)wndclass->GetNext();
	        }
	}
  }
  else {
	if(wndclass->classAtom == (DWORD)id && wndclass->hInstance == hInstance) {
		return(wndclass);
	}
	else {
		wndclass = (Win32WndClass *)wndclass->GetNext();
		while(wndclass != NULL) {
			if(wndclass->classAtom == (DWORD)id && wndclass->hInstance == hInstance) {
				return(wndclass);
			}
			wndclass = (Win32WndClass *)wndclass->GetNext();
	        }
	}
  }
  dprintf(("Class %X (inst %X) not found!", id, hInstance));
  return(NULL);
}
//******************************************************************************
//******************************************************************************
BOOL Win32WndClass::getClassInfo(WNDCLASSEXA *wndclass)
{
  wndclass->cbClsExtra    = nrExtraClassWords;
  wndclass->cbWndExtra    = nrExtraWindowWords;
  wndclass->hbrBackground = backgroundBrush;
  wndclass->hCursor	  = hCursor;
  wndclass->hIcon         = hIcon;
  wndclass->hInstance     = hInstance;
  wndclass->lpszMenuName  = (LPCTSTR)menuNameA;
  wndclass->lpszClassName = (classNameA) ? (LPCTSTR)classNameA : (LPCTSTR)classAtom;
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
  wndclass->hbrBackground = backgroundBrush;
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
  if(HIWORD(classNameA)) {
	strncpy(lpszClassName, classNameA, cchClassName-1);
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

  if(HIWORD(classNameW)) {
	lstrcpyW(lpszClassName, classNameW);
	return lstrlenW(lpszClassName)*sizeof(WCHAR);
  }
  *(ULONG *)lpszClassName = classAtom;
  return(sizeof(ULONG));
}
//******************************************************************************
//******************************************************************************
void Win32WndClass::setMenuName(LPSTR newMenuName)
{
  if(HIWORD(menuNameA)) {
	free(menuNameA);
	free(menuNameW);
	menuNameA = 0;
        menuNameW = 0;
  }
  if(HIWORD(newMenuName)) {
	if(isUnicode) {
	  	menuNameA = (PCHAR)malloc(lstrlenW((LPWSTR)newMenuName)+1);
	  	menuNameW = (WCHAR *)malloc((lstrlenW((LPWSTR)newMenuName)+1)*sizeof(WCHAR));
	}
	else {
	  	menuNameA = (PCHAR)malloc(strlen(newMenuName)+1);
	  	menuNameW = (WCHAR *)malloc((strlen(newMenuName)+1)*sizeof(WCHAR));
	}
	if(menuNameA == NULL || menuNameW == NULL) {
		dprintf(("Win32Class ctr; menuName/menuNameW == NULL"));
		exit(1);
	}
	if(isUnicode) {
		lstrcpyW(menuNameW, (LPWSTR)newMenuName);
		UnicodeToAscii(menuNameW, menuNameA);
	}
	else {
		strcpy((char *)menuNameA, newMenuName);
		AsciiToUnicode(menuNameA, menuNameW);
	}
  }
  else {//id
	menuNameA = (PCHAR)newMenuName;
	menuNameW = (WCHAR *)newMenuName;
  }
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
		return (isUnicode) ? (ULONG)menuNameW : (ULONG)menuNameA;
	case GCL_STYLE:
		return windowStyle;
	case GCL_WNDPROC:
		return (ULONG)windowProc;
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
WORD Win32WndClass::getClassWord(int index)
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
ULONG Win32WndClass::setClassLongA(int index, LONG lNewVal, BOOL isUnicode)
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
		rc = 0;	//old value is meaningless (according to Wine)
		setMenuName((LPSTR)lNewVal);
		break;
	case GCL_STYLE:
		rc = windowStyle;
		windowStyle = lNewVal;
		break;
	case GCL_WNDPROC:
		rc = (ULONG)windowProc;
		windowProc = (WNDPROC)lNewVal;
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
WORD Win32WndClass::setClassWord(int index, WORD wNewVal)
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
GenericObject *Win32WndClass::wndclasses = NULL;

