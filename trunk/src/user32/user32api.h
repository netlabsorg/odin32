/* $Id: user32api.h,v 1.2 2003-02-13 10:12:25 sandervl Exp $ */
//Internal user32 functions

#ifndef __USER32API_H_
#define __USER32API_H_

HWND WINAPI GetAncestor( HWND hwnd, UINT type );

//Notify that focus has changed (necessary for SetFocus(0) handling)
void SetFocusChanged();

extern BOOL fIgnoreKeystrokes;

#endif
