/* $Id: syscolor.h,v 1.16 2003-01-03 21:43:17 sandervl Exp $ */

/*
 * WINE:
 * System color objects
 *
 * Copyright  Alexandre Julliard, 1994
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_SYSCOLOR_H
#define __WINE_SYSCOLOR_H

#ifdef __cplusplus
extern "C" {
#endif

HBRUSH WIN32API GetOS2ColorBrush(int nIndex);
COLORREF WIN32API GetOS2Color(int nIndex);

extern void SYSCOLOR_Init(int fOverride = -1);
extern void SYSCOLOR_Save(void);

extern INT SYSCOLOR_GetLastColor(VOID);
extern INT SYSCOLOR_GetNumColors(VOID);
extern BOOL SYSCOLOR_GetUseWinColors(VOID);

HBRUSH WIN32API GetControlBrush(HWND hwnd, HDC hdc, DWORD ctlType);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /* __WINE_SYSCOLOR_H */
