/* $Id: user32api.h,v 1.3 2003-11-12 14:10:20 sandervl Exp $ */
//Internal user32 functions

#ifndef __USER32API_H_
#define __USER32API_H_

//Notify that focus has changed (necessary for SetFocus(0) handling)
void SetFocusChanged();

extern BOOL fIgnoreKeystrokes;

#endif
