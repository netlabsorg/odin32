/* $Id: win32class.h,v 1.1 1999-05-24 20:20:04 ktk Exp $ */

#ifndef __WIN32CLASS_H__
#define __WIN32CLASS_H__

//SvL: 18-7-'98, Registers system window classes (button, listbox etc etc)
extern "C" {
void RegisterSystemClasses(ULONG hModule);
}

class Win32WndClass
{
public:
	Win32WndClass(WNDCLASSEXA wndclass, BOOL isUnicode);
       ~Win32WndClass();

 	 ULONG  getClassLongA(int index, BOOL isUnicode = FALSE);
 	 ULONG  getClassLongW(int index) 
	 { 
		return getClassLongA(index, TRUE); 
	 };
 	 WORD   getClassWord(int index);

 	 ULONG  setClassLongA(int index, LONG lNewVal, BOOL isUnicode = FALSE);
 	 ULONG  setClassLongW(int index, LONG lNewVal) 
	 {
	 	return setClassLongA(index, lNewVal, TRUE);
	 }
 	 WORD   setClassWord(int index, WORD wNewVal);

         ATOM   getAtom()	{ return (ATOM) classAtom; };
	 BOOL   getClassInfo(WNDCLASSEXA *wndclass);
	 BOOL   getClassInfo(WNDCLASSEXW *wndclass);

	 ULONG  getClassName(LPSTR  lpszClassName, ULONG cchClassName);
	 ULONG  getClassName(LPWSTR lpszClassName, ULONG cchClassName);

 static  void	UnregisterClassA(HINSTANCE hinst, LPSTR id);

 static Win32WndClass *FindClass(HINSTANCE hinst, LPSTR id);

private:
 BOOL           isUnicode;

 //Standard class words/longs
 ULONG		nrExtraClassWords;	//GCL_CBCLSEXTRA
 ULONG		nrExtraWindowWords;	//GCL_CBWNDEXTRA
 HBRUSH		backgroundBrush;	//GCL_HBRBACKGROUND
 HCURSOR	hCursor;		//GCL_HCURSOR
 HICON		hIcon;			//GCL_HICON
 HINSTANCE	hInstance;		//GCL_HMODULE
 ULONG		menuName;		//GCL_MENUNAME
 ULONG          menuNameW;		//GCL_MENUNAME
 ULONG		windowStyle;		//GCL_STYLE
 WINWNDPROC     windowProc;		//GCL_WNDPROC
 ULONG          classAtom;		//GCW_ATOM

 ULONG          className;
 ULONG          classNameW;
 HICON		hIconSm;

 //User data class words/longs
 ULONG	       *userClassLong;
 
 static	        Win32WndClass  *wndclasses;
  	        Win32WndClass  *next;
};

#endif //__WIN32CLASS_H__
