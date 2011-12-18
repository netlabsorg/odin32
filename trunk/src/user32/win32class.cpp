/* $Id: win32class.cpp,v 1.31 2004-04-20 10:11:42 sandervl Exp $ */
/*
 * Win32 Window Class Managment Code for OS/2
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Parts copied from ReWind (Get/SetClassLong for GCL_WNDPROC)
 *     Copyright 1993, 1996 Alexandre Julliard
 *               1998 Juergen Schmied (jsch)
 *
 *
 * TODO: Right now all class atoms are global. This must be changed.
 * TODO: Global atoms of classes with CS_GLOBALCLASS flag are not deleted
 *       Must all be changed if we want to support global app classes
 *       that can be used by other apps. (low priority)
 *
 * NOTE: To access a class object, you must call FindClass. This method
 *       increases the reference count of the object. When you're done
 *       with the object, you MUST call the release method!
 *       This mechanism prevents premature destruction of objects when there
 *       are still clients using it.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <misc.h>
#include "win32class.h"
#include "win32wnd.h"
#include <win/winproc.h>
#include <unicode.h>

#define DBG_LOCALLOG    DBG_win32class
#include "dbglocal.h"

static BOOL fDestroyAll = FALSE;

//******************************************************************************
//Win32WndClass methods:
//******************************************************************************
Win32WndClass::Win32WndClass(WNDCLASSEXA *wndclass, WNDCLASS_TYPE fClassType)
                  : GenericObject(&wndclasses, &critsect)
{
  this->fClassType = fClassType;
  processId = 0;

  if(HIWORD(wndclass->lpszClassName))
  {
        if(fClassType == WNDCLASS_UNICODE) {
                INT len = lstrlenW((LPWSTR)wndclass->lpszClassName)+1;

                classNameA = (PCHAR)_smalloc(len);
                classNameW = (WCHAR *)_smalloc(len*sizeof(WCHAR));
        }
        else {
                INT len = strlen(wndclass->lpszClassName)+1;

                classNameA = (PCHAR)_smalloc(len);
                classNameW = (WCHAR *)_smalloc(len*sizeof(WCHAR));
        }
        if(classNameA == NULL || classNameW == NULL) {
                dprintf(("Win32Class ctr; classNameA/classNameW == NULL"));
                exit(1);
        }
        if(fClassType == WNDCLASS_UNICODE) {
                lstrcpyW(classNameW, (LPWSTR)wndclass->lpszClassName);
                UnicodeToAscii(classNameW, classNameA);
        }
        else {
                strcpy((char *)classNameA, wndclass->lpszClassName);
                AsciiToUnicode(classNameA, classNameW);
        }
        classAtom = 0;
        //SvL: If a system control has already be registered, use that atom instead
        //     of creating a new one
        if(wndclass->style & CS_GLOBALCLASS) {
                classAtom = GlobalFindAtomA(classNameA);
        }
        if(!classAtom) classAtom = GlobalAddAtomA(classNameA);
  }
  else {
        classNameA      = NULL;
        classNameW      = NULL;
        classAtom       = (DWORD)wndclass->lpszClassName;
  }
  if(!(wndclass->style & CS_GLOBALCLASS)) {
        processId = GetCurrentProcessId();
  }
  menuNameA = 0;
  menuNameW = 0;
  setMenuName((LPSTR)wndclass->lpszMenuName);

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
       dprintf(("USER32:  wndclass->lpszClassName %s\n", classNameA));
  else dprintf(("USER32:  wndclass->lpszClassName %X\n", wndclass->lpszClassName));

  dprintf(("USER32:  wndclass->classAtom %x", classAtom));

  if(HIWORD(wndclass->lpszMenuName)) {//convert string name identifier to numeric id
       dprintf(("USER32:  lpszMenuName %s\n", menuNameA));
  }
  else dprintf(("USER32:  wndclass->lpszMenuName %X\n", menuNameA));
  dprintf(("USER32:  wndclass->hIconSm %X\n", wndclass->hIconSm));

  nrExtraClassBytes     = wndclass->cbClsExtra;
  nrExtraWindowBytes    = wndclass->cbWndExtra;
  backgroundBrush       = wndclass->hbrBackground;
  hCursor               = wndclass->hCursor;
  hIcon                 = wndclass->hIcon;
  hInstance             = wndclass->hInstance;

  if(wndclass->style & CS_CLASSDC) {
        hdcClass = 0; //TODO:
  }
  else  hdcClass = 0;

  windowStyle           = wndclass->style;

  pfnWindowProcA = 0;
  pfnWindowProcW = 0;
  if(fClassType == WNDCLASS_UNICODE) {
      WINPROC_SetProc((HWINDOWPROC *)&pfnWindowProcW, wndclass->lpfnWndProc,  WIN_PROC_32W, WIN_PROC_CLASS);
  }
  else {
      WINPROC_SetProc((HWINDOWPROC *)&pfnWindowProcA, wndclass->lpfnWndProc, WIN_PROC_32A, WIN_PROC_CLASS);
  }
  dprintf2(("Window class ptr %x/%x", pfnWindowProcA, pfnWindowProcW));

  //User data class words/longs
  if(nrExtraClassBytes) {
        userClassBytes = (char *)_smalloc(nrExtraClassBytes);
        if(userClassBytes == NULL) {
                dprintf(("Win32Class ctr: userClassBytes == NULL!"));
                exit(1);
        }
        memset(userClassBytes, 0, nrExtraClassBytes);
  }
  else  userClassBytes = NULL;

  hIconSm  = wndclass->hIconSm;
}
//******************************************************************************
//******************************************************************************
Win32WndClass::~Win32WndClass()
{
  if(classNameA) {
      dprintf(("Win32WndClass dtor, destroy class %s\n", classNameA));
  }

  //SvL: Don't delete global classes
  if(classNameA && !(windowStyle & CS_GLOBALCLASS)) {
      GlobalDeleteAtom(classAtom);
  }

  if(pfnWindowProcA)
      WINPROC_FreeProc((HWINDOWPROC)pfnWindowProcA, WIN_PROC_CLASS);
  if(pfnWindowProcW)
      WINPROC_FreeProc((HWINDOWPROC)pfnWindowProcW, WIN_PROC_CLASS);

  if(userClassBytes)    free(userClassBytes);
  if(classNameA)        free(classNameA);
  if(classNameW)        free(classNameW);
  if(menuNameA && HIWORD(menuNameA)) {
      free(menuNameA);
      assert(menuNameW);
      free(menuNameW);
  }
}
//******************************************************************************
//******************************************************************************
void Win32WndClass::DestroyAll()
{
    fDestroyAll = TRUE;
    GenericObject::DestroyAll(wndclasses);
}
//******************************************************************************
//******************************************************************************
BOOL Win32WndClass::hasClassName(LPSTR classname, BOOL fUnicode)
{
  if(HIWORD(classname) == 0) {
      return classAtom == (DWORD)classname;
  }
  if(fUnicode) {
        if(classNameW)
            return (lstrcmpW(classNameW, (LPWSTR)classname) == 0);
        return FALSE;
  }
  else {
        if(classNameA)
            return (strcmp(classNameA, classname) == 0);
        return FALSE;
  }
}
//******************************************************************************
//Locates class in linked list and increases reference count (if found)
//Class object must be unreferenced after usage
//******************************************************************************
Win32WndClass *Win32WndClass::FindClass(HINSTANCE hInstance, LPSTR id)
{
  lock(&critsect);

  Win32WndClass *wndclass = (Win32WndClass *)wndclasses;

  if(wndclass == NULL) {
        unlock(&critsect);
        return(NULL);
  }

  if(HIWORD(id) != 0) {
//CB: read comment below!
        if(lstrcmpiA(wndclass->classNameA, id) == 0 && wndclass->hInstance == hInstance) {
                wndclass->addRef();
                unlock(&critsect);
                return(wndclass);
        }
        else {
                wndclass = (Win32WndClass *)wndclass->GetNext();
                while(wndclass != NULL) {
                        if(lstrcmpiA(wndclass->classNameA, id) == 0)
                        {
                                //SvL: According to Wine, if the instance handle is the one of the main exe, everything is ok
                                if(hInstance == NULL || GetModuleHandleA(NULL) == hInstance ||
                                   wndclass->hInstance == hInstance)
                                {
                                    wndclass->addRef();
                                    unlock(&critsect);
                                    return(wndclass);
                                }
                        }
                        wndclass = (Win32WndClass *)wndclass->GetNext();
                }
        }
  }
  else {
//CB: without HInstance check, test program finds class
//CB: need more code to compare instance; convert 0 to exe module handle
        if(wndclass->classAtom == (DWORD)id /*&& wndclass->hInstance == hInstance*/) {
                wndclass->addRef();
                unlock(&critsect);
                return(wndclass);
        }
        else {
                wndclass = (Win32WndClass *)wndclass->GetNext();
                while(wndclass != NULL) {
                        if(wndclass->classAtom == (DWORD)id /* && wndclass->hInstance == hInstance*/) {
                                wndclass->addRef();
                                unlock(&critsect);
                                return(wndclass);
                        }
                        wndclass = (Win32WndClass *)wndclass->GetNext();
                }
        }
  }
  unlock(&critsect);
  dprintf(("Class %X (inst %X) not found!", id, hInstance));
  return(NULL);
}
//******************************************************************************
//Locates class in linked list and increases reference count (if found)
//Class object must be unreferenced after usage
//******************************************************************************
Win32WndClass *Win32WndClass::FindClass(HINSTANCE hInstance, LPWSTR id)
{
 LPSTR          lpszClassName;
 Win32WndClass *winclass;

  if(HIWORD(id)) {
       lpszClassName = UnicodeToAsciiString((LPWSTR)id);
  }
  else lpszClassName = (LPSTR)id;

  winclass = FindClass(hInstance, lpszClassName);

  if(HIWORD(id)) {
        FreeAsciiString((char *)lpszClassName);
  }
  return winclass;
}
//******************************************************************************
//An app can only access another process' class if it's global
//(all system classes are global)
//NOTE: NOT USED NOW
//******************************************************************************
BOOL Win32WndClass::isAppClass(ULONG curProcessId)
{
  if(windowStyle & CS_GLOBALCLASS)
    return TRUE;

  return curProcessId = processId;
}
//******************************************************************************
//******************************************************************************
BOOL Win32WndClass::getClassInfo(WNDCLASSEXA *wndclass)
{
  wndclass->cbClsExtra    = nrExtraClassBytes;
  wndclass->cbWndExtra    = nrExtraWindowBytes;
  wndclass->hbrBackground = backgroundBrush;
  wndclass->hCursor       = hCursor;
  wndclass->hIcon         = hIcon;
  wndclass->hInstance     = hInstance;
  wndclass->lpszMenuName  = (LPCTSTR)menuNameA;
  wndclass->lpszClassName = (classNameA) ? (LPCTSTR)classNameA : (LPCTSTR)classAtom;
  wndclass->style         = windowStyle;
  wndclass->lpfnWndProc   = (WNDPROC)getClassLongA(GCL_WNDPROC, FALSE);
  wndclass->hIconSm       = hIconSm;
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL Win32WndClass::getClassInfo(WNDCLASSEXW *wndclass)
{
  wndclass->cbClsExtra    = nrExtraClassBytes;
  wndclass->cbWndExtra    = nrExtraWindowBytes;
  wndclass->hbrBackground = backgroundBrush;
  wndclass->hCursor       = hCursor;
  wndclass->hIcon         = hIcon;
  wndclass->hInstance     = hInstance;
  wndclass->lpszMenuName  = (LPCWSTR)menuNameW;
  wndclass->lpszClassName = (classNameW) ? (LPCWSTR)classNameW : (LPCWSTR)classAtom;
  wndclass->style         = windowStyle;
  wndclass->lpfnWndProc   = (WNDPROC)getClassLongA(GCL_WNDPROC, TRUE);
  wndclass->hIconSm       = hIconSm;
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
ULONG Win32WndClass::getClassName(LPSTR lpszClassName, ULONG cchClassName)
{
  if(HIWORD(classNameA)) {
        lstrcpynA(lpszClassName, classNameA, cchClassName-1);
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
        lstrcpynW(lpszClassName, classNameW, cchClassName-1);
        return lstrlenW(lpszClassName);
  }
  *(ULONG *)lpszClassName = classAtom;
  return(sizeof(ULONG));
}
//******************************************************************************
//******************************************************************************
WNDPROC Win32WndClass::getWindowProc(WNDPROC_TYPE type)
{
    WNDPROC proc;

    if(type == WNDPROC_UNICODE) {
         proc = (pfnWindowProcW) ? pfnWindowProcW : pfnWindowProcA;
    }
    else proc = (pfnWindowProcA) ? pfnWindowProcA : pfnWindowProcW;

    return proc;
};
//******************************************************************************
//NOTE: Only to be used when a class has both ascii & unicode window procedures!
//      Otherwise use SetClassLong GCL_WNDPROC
//******************************************************************************
void Win32WndClass::setWindowProc(WNDPROC pfnWindowProc, WNDPROC_TYPE type)
{
    if(type == WNDPROC_UNICODE) {
         WINPROC_SetProc((HWINDOWPROC *)&pfnWindowProcW, pfnWindowProc,  WIN_PROC_32W, WIN_PROC_CLASS);
    }
    else WINPROC_SetProc((HWINDOWPROC *)&pfnWindowProcA, pfnWindowProc,  WIN_PROC_32A, WIN_PROC_CLASS);
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
        if(fClassType == WNDCLASS_UNICODE) {
                menuNameA = (PCHAR)_smalloc(lstrlenW((LPWSTR)newMenuName)+1);
                menuNameW = (WCHAR *)_smalloc((lstrlenW((LPWSTR)newMenuName)+1)*sizeof(WCHAR));
        }
        else {
                menuNameA = (PCHAR)_smalloc(strlen(newMenuName)+1);
                menuNameW = (WCHAR *)_smalloc((strlen(newMenuName)+1)*sizeof(WCHAR));
        }
        if(menuNameA == NULL || menuNameW == NULL) {
                dprintf(("Win32Class ctr; menuName/menuNameW == NULL"));
                DebugInt3();
                return;
        }
        if(fClassType == WNDCLASS_UNICODE) {
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
ULONG Win32WndClass::getClassLongA(int index, BOOL fUnicode)
{
  switch(index) {
        case GCL_CBCLSEXTRA:
                return nrExtraClassBytes;
        case GCL_CBWNDEXTRA:
                return nrExtraWindowBytes;
        case GCL_HBRBACKGROUND:
                return backgroundBrush;
        case GCL_HCURSOR:
                return hCursor;
        case GCL_HICON:
                return hIcon;
        case GCL_HICONSM:
                return hIconSm;
        case GCL_HMODULE:
                return hInstance;
        case GCL_MENUNAME:
                return (fUnicode) ? (ULONG)menuNameW : (ULONG)menuNameA;
        case GCL_STYLE:
                return windowStyle;
        case GCL_WNDPROC:
        {
                WNDPROC pfnWindowProc = pfnWindowProcA;

                if(pfnWindowProcW)
                {
                    if(!pfnWindowProc || fUnicode)
                        pfnWindowProc = pfnWindowProcW;
                }
                return (ULONG) WINPROC_GetProc((HWINDOWPROC)pfnWindowProc, (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A);
        }
        case GCW_ATOM: //TODO: does this really happen in windows?
                SetLastError(ERROR_INVALID_PARAMETER);
                return 0;
        default:
                if(index >= 0 && index + sizeof(ULONG) <= nrExtraClassBytes) {
                        //Note: NT4, SP6 does not set the last error to 0
                        SetLastError(ERROR_SUCCESS);
                        return *(ULONG *)(userClassBytes + index);
                }
                if(classNameA) {
                     dprintf2(("WARNING: getClassLong %s: %d -> wrong INDEX", classNameA, index));
                }
                else dprintf2(("WARNING: getClassLong %d: %d -> wrong INDEX", classAtom, index));
                SetLastError(ERROR_INVALID_INDEX);  //verified in NT4, SP6
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
                if(index >= 0 && index + sizeof(WORD) <= nrExtraClassBytes) {
                        //Note: NT4, SP6 does not set the last error to 0
                        SetLastError(ERROR_SUCCESS);
                        return *(WORD *)(userClassBytes + index);
                }
                if(classNameA) {
                     dprintf2(("WARNING: getClassWord %s: %d -> wrong INDEX", classNameA, index));
                }
                else dprintf2(("WARNING: getClassWord %d: %d -> wrong INDEX", classAtom, index));
                SetLastError(ERROR_INVALID_INDEX);  //verified in NT4, SP6
                return 0;
  }
}
//******************************************************************************
//TODO: What effects what immediately?
//******************************************************************************
ULONG Win32WndClass::setClassLongA(int index, LONG lNewVal, BOOL fUnicode)
{
 ULONG rc;

  switch(index) {
        case GCL_CBCLSEXTRA: //TODO (doesn't affect allocated classes, so what does it do?)
                rc = nrExtraClassBytes;
//              nrExtraClassBytes = lNewVal;
                break;
        case GCL_CBWNDEXTRA:
                rc = nrExtraWindowBytes;
                nrExtraWindowBytes = lNewVal;
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
        case GCL_HICONSM:
                rc = hIconSm;
                hIconSm = lNewVal;
                break;
        case GCL_HMODULE:
                rc = hInstance;
                hInstance = lNewVal;
                break;
        case GCL_MENUNAME:
                rc = 0; //old value is meaningless (according to Wine)
                setMenuName((LPSTR)lNewVal);
                break;
        case GCL_STYLE:
                rc = windowStyle;
                windowStyle = lNewVal;
                break;
        case GCL_WNDPROC:
        {
                //Note: Type of SetWindowLong determines new window proc type
                //      UNLESS the new window proc has already been registered
                //      (use the old type in that case)
                //      (VERIFIED in NT 4, SP6)
                //TODO: Is that also true for GCL_WNDPROC???????????????
                WNDPROC *proc = &pfnWindowProcA;
                WINDOWPROCTYPE type = (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A;

                if(pfnWindowProcW)
                {
                    if(!*proc || fUnicode)
                        proc = &pfnWindowProcW;
                }
                rc = (LONG)WINPROC_GetProc((HWINDOWPROC)*proc, type );
                WINPROC_SetProc((HWINDOWPROC *)proc, (WNDPROC)lNewVal, type, WIN_PROC_CLASS);

                /* now free the one that we didn't set */
                if(pfnWindowProcA && pfnWindowProcW)
                {
                    if (proc == &pfnWindowProcA)
                    {
                        WINPROC_FreeProc( (HWINDOWPROC)pfnWindowProcW, WIN_PROC_CLASS );
                        pfnWindowProcW = 0;
                    }
                    else
                    {
                        WINPROC_FreeProc( (HWINDOWPROC)pfnWindowProcA, WIN_PROC_CLASS );
                        pfnWindowProcA = 0;
                    }
                }
                break;
        }
        case GCW_ATOM: //TODO: does this really happen in windows?
                SetLastError(ERROR_INVALID_PARAMETER);
                return 0;
        default:
                if(index >= 0 && index + sizeof(ULONG) <= nrExtraClassBytes) {
                        rc = *(ULONG *)(userClassBytes + index);
                        *(ULONG *)(userClassBytes + index) = lNewVal;
                        break;
                }
                SetLastError(ERROR_INVALID_INDEX);  //verified in NT4, SP6
                if(classNameA) {
                     dprintf2(("WARNING: Win32WndClass::setClassLongA %s: %d %x -> wrong INDEX", classNameA, index, lNewVal));
                }
                else dprintf2(("WARNING: Win32WndClass::setClassLongA %d: %d %x -> wrong INDEX", classAtom, index, lNewVal));
                return 0;
  }
  SetLastError(ERROR_SUCCESS);
  if(classNameA) {
        dprintf2(("Win32WndClass::setClassLongA %s: %d %x returned %x", classNameA, index, lNewVal, rc));
  }
  else  dprintf2(("Win32WndClass::setClassLongA %d: %d %x returned %x", classAtom, index, lNewVal, rc));
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
                if(index >= 0 && index + sizeof(WORD) <= nrExtraClassBytes) {
                        rc = *(WORD *)(userClassBytes + index);
                        *(WORD *)(userClassBytes + index) = wNewVal;
                        //Note: NT4, SP6 does not set the last error to 0
                        SetLastError(ERROR_SUCCESS);
                        return(rc);
                }
                SetLastError(ERROR_INVALID_INDEX);  //verified in NT4, SP6
                if(classNameA) {
                     dprintf2(("WARNING: setClassWord %s: %d %x -> wrong INDEX", classNameA, index, wNewVal));
                }
                else dprintf2(("WARNING: setClassWord %d: %d %x -> wrong INDEX", classAtom, index, wNewVal));
                return 0;
  }
}
//******************************************************************************
//FIXME: Windows that still exists with this class
//******************************************************************************
BOOL Win32WndClass::UnregisterClassA(HINSTANCE hinst, LPSTR id)
{
  Win32WndClass *wndclass;

  if(HIWORD(id)) {
       dprintf(("Win32WndClass::UnregisterClassA class %s, instance %x!!", id, hinst));
  }
  else dprintf(("Win32WndClass::UnregisterClassA class %x, instance %x!!", id, hinst));

  wndclass = FindClass(hinst, id);
  if(wndclass) {
        if(wndclass->getRefCount() != 1) {
            wndclass->markDeleted();
            RELEASE_CLASSOBJ(wndclass);
            dprintf2(("Win32WndClass::UnregisterClassA class %x still has windows!!", id));
            SetLastError(ERROR_CLASS_HAS_WINDOWS);
            return FALSE;
        }
        wndclass->markDeleted();
        RELEASE_CLASSOBJ(wndclass);

        SetLastError(ERROR_SUCCESS);
        return TRUE;
  }
  dprintf(("::UnregisterClass, couldn't find class %X!!\n", id));
  SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
  return FALSE;
}
//******************************************************************************
//******************************************************************************
GenericObject   *Win32WndClass::wndclasses = NULL;
VMutex           Win32WndClass::critsect;
