#ifndef __OSLIBINPUT_H__
#define __OSLIBINPUT_H__

BOOL OSLibGetDIState(DWORD len, LPVOID ptr);

BOOL OSLibQueryMousePos(DWORD *rx, DWORD *ry, DWORD *state);
BOOL OSLibMoveCursor(DWORD x, DWORD y);
void OSLibInit();

#endif

