/* $Id: oslibdnd.h,v 1.1 2003-07-28 11:25:50 sandervl Exp $ */
/*
 * Window Drag and Drop functions for OS/2
 *
 *
 * Copyright 2003 Sander van Leeuwen (sandervl@innotek.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __OSLIBDND_H__
#define __OSLIBDND_H__

#ifdef __cplusplus
extern "C" {
#endif

DWORD  WINAPI OSLibDragOver(LPVOID lpDragStruct, DWORD x, DWORD y);
DWORD  WINAPI OSLibDragLeave(LPVOID lpDragStruct);
DWORD  WINAPI OSLibDragDrop(LPVOID lpDragStruct, DWORD x, DWORD y, LPSTR lpszDnDString);
LPVOID WINAPI OSLibCreateDragStruct(HWND hwndWin32, DWORD x, DWORD y, LPSTR lpszDnDString);
void   WINAPI OSLibFreeDragStruct(LPVOID lpDragStruct);

#ifdef OS2_INCLUDED
BOOL   OSLibRenderFormat(PDRAGTRANSFER pDragTransfer);
ULONG  OSLibEndConversation();
#endif

#ifdef __cplusplus
}
#endif

#endif //__OSLIBDND_H__
