/* $Id: oslibinput.h,v 1.2 1999-11-08 13:50:41 sandervl Exp $ */
#ifndef __OSLIBINPUT_H__
#define __OSLIBINPUT_H__

BOOL OSLibGetDIState(DWORD len, LPVOID ptr);

BOOL OSLibQueryMousePos(DWORD *rx, DWORD *ry, DWORD *state);
BOOL OSLibMoveCursor(DWORD x, DWORD y);
void OSLibInit();

#endif

