/* $Id: win32class.h,v 1.13 2001-02-02 19:04:02 sandervl Exp $ */
/*
 * Win32 Window Class Managment Code for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 */
#ifndef __WIN32CLASS_H__
#define __WIN32CLASS_H__

#include <gen_object.h>

class Win32WndClass : public GenericObject
{
public:
        Win32WndClass(WNDCLASSEXA *wndclass, BOOL fUnicode = FALSE);
       ~Win32WndClass();

         ULONG  getClassLongA(int index, BOOL fUnicode = FALSE);
         ULONG  getClassLongW(int index)
         {
                return getClassLongA(index, TRUE);
         };
         WORD   getClassWord(int index);

         ULONG  setClassLongA(int index, LONG lNewVal, BOOL fUnicode = FALSE);
         ULONG  setClassLongW(int index, LONG lNewVal)
         {
                return setClassLongA(index, lNewVal, TRUE);
         }
         WORD   setClassWord(int index, WORD wNewVal);

         ATOM   getAtom()       { return (ATOM) classAtom; };
         BOOL   getClassInfo(WNDCLASSEXA *wndclass);
         BOOL   getClassInfo(WNDCLASSEXW *wndclass);

         ULONG  getClassName(LPSTR  lpszClassName, ULONG cchClassName);
         ULONG  getClassName(LPWSTR lpszClassName, ULONG cchClassName);

       WNDPROC  getWindowProc()         { return windowProc; };
         LPSTR  getMenuNameA()          { return menuNameA; };
         DWORD  getExtraWndWords()      { return nrExtraWindowWords; };

         HICON  getIcon()               { return hIcon; };
         HICON  getIconSm()             { return hIconSm; };
        HCURSOR getCursor()             { return hCursor; };

      HINSTANCE getInstance()           { return hInstance; };

        HBRUSH  getBackgroundBrush()    { return backgroundBrush; };
         ULONG  getStyle()              { return windowStyle; };

           HDC  getClassDC()            { return hdcClass; };

          void  setMenuName(LPSTR newMenuName);

          void  IncreaseWindowCount()   { cWindows++; };
          void  DecreaseWindowCount()   { cWindows--; };
          DWORD GetWindowCount()        { return cWindows; };

          BOOL  hasClassName(LPSTR classname, BOOL fUnicode = FALSE);

          BOOL  isAppClass(ULONG curProcessId);

 static   BOOL  UnregisterClassA(HINSTANCE hinst, LPSTR id);

 static Win32WndClass *FindClass(HINSTANCE hinst, LPSTR id);
 static Win32WndClass *FindClass(HINSTANCE hinst, LPWSTR id);

 static  void   DestroyAll();

private:
 BOOL           isUnicode;

 //Standard class words/longs
 ULONG          windowStyle;            //GCL_STYLE     * must be offset 14h *
 ULONG          nrExtraClassWords;      //GCL_CBCLSEXTRA
 ULONG          nrExtraWindowWords;     //GCL_CBWNDEXTRA
 HBRUSH         backgroundBrush;        //GCL_HBRBACKGROUND
 HCURSOR        hCursor;                //GCL_HCURSOR
 HICON          hIcon;                  //GCL_HICON
 HINSTANCE      hInstance;              //GCL_HMODULE
 PCHAR          menuNameA;              //GCL_MENUNAME
 WCHAR         *menuNameW;              //GCL_MENUNAME
 WNDPROC        windowProc;             //GCL_WNDPROC
 ULONG          classAtom;              //GCW_ATOM

 PCHAR          classNameA;
 WCHAR         *classNameW;
 HICON          hIconSm;                //GCW_HICONSM
 HDC            hdcClass;

 //User data class words/longs
 ULONG         *userClassLong;
 ULONG          processId;

 //nr of windows created with this class
 ULONG          cWindows;

 static GenericObject *wndclasses;
};

#endif //__WIN32CLASS_H__
