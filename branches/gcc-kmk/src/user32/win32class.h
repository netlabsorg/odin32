/* $Id: win32class.h,v 1.18 2004-04-20 10:11:43 sandervl Exp $ */
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

#include "gen_object.h"

#define RELEASE_CLASSOBJ(a)       { a->release(); a = NULL; }

typedef enum {
  WNDCLASS_ASCII,
  WNDCLASS_UNICODE
} WNDCLASS_TYPE;

typedef enum {
  WNDPROC_ASCII,
  WNDPROC_UNICODE
} WNDPROC_TYPE;

class Win32WndClass : public GenericObject
{
public:
        Win32WndClass(WNDCLASSEXA *wndclass, WNDCLASS_TYPE fClassType = WNDCLASS_ASCII);
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

       WNDPROC  getWindowProc(WNDPROC_TYPE type);
         //NOTE: Only to be used when a class has both ascii & unicode window procedures!
         void   setWindowProc(WNDPROC pfnWindowProc, WNDPROC_TYPE type);

         LPSTR  getMenuNameA()          { return menuNameA; };
         DWORD  getExtraWndBytes()      { return nrExtraWindowBytes; };

         HICON  getIcon()               { return hIcon; };
         HICON  getIconSm()             { return hIconSm; };
        HCURSOR getCursor()             { return hCursor; };

      HINSTANCE getInstance()           { return hInstance; };

        HBRUSH  getBackgroundBrush()    { return backgroundBrush; };
         ULONG  getStyle()              { return windowStyle; };

           HDC  getClassDC()            { return hdcClass; };

          void  setMenuName(LPSTR newMenuName);

          BOOL  hasClassName(LPSTR classname, BOOL fUnicode);

          BOOL  isAppClass(ULONG curProcessId);

 static   BOOL  UnregisterClassA(HINSTANCE hinst, LPSTR id);

 //Locates class in linked list and increases reference count (if found)
 //Class object must be unreferenced after usage
 static Win32WndClass *FindClass(HINSTANCE hinst, LPSTR id);
 static Win32WndClass *FindClass(HINSTANCE hinst, LPWSTR id);

 static  void   DestroyAll();

private:
 WNDCLASS_TYPE  fClassType;

 //Standard class words/longs
 ULONG          windowStyle;            //GCL_STYLE     * must be offset 14h *
 ULONG          nrExtraClassBytes;      //GCL_CBCLSEXTRA
 ULONG          nrExtraWindowBytes;     //GCL_CBWNDEXTRA
 HBRUSH         backgroundBrush;        //GCL_HBRBACKGROUND
 HCURSOR        hCursor;                //GCL_HCURSOR
 HICON          hIcon;                  //GCL_HICON
 HINSTANCE      hInstance;              //GCL_HMODULE
 PCHAR          menuNameA;              //GCL_MENUNAME
 WCHAR         *menuNameW;              //GCL_MENUNAME
 WNDPROC        pfnWindowProcA;         //GCL_WNDPROC
 WNDPROC        pfnWindowProcW;         //GCL_WNDPROC
 ULONG          classAtom;              //GCW_ATOM

 PCHAR          classNameA;
 WCHAR         *classNameW;
 HICON          hIconSm;                //GCW_HICONSM
 HDC            hdcClass;

 //User data class bytse
 char          *userClassBytes;
 ULONG          processId;

 static GenericObject *wndclasses;
 static VMutex         critsect;
};

ATOM WIN32API InternalRegisterClass(LPSTR lpszClassName, DWORD dwStyle,
                                    WNDPROC pfnClassA, WNDPROC pfnClassW,
                                    UINT cbExtraWindowWords, LPCSTR lpszCursor,
                                    HBRUSH hBrush);

#endif //__WIN32CLASS_H__
