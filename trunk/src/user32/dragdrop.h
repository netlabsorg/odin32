/* $Id: dragdrop.h,v 1.2 2002-06-02 19:34:27 sandervl Exp $ */

/*
 * Win32 Drag 'n Drop functions for OS/2
 *
 * Copyright 2002 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __DRAGDROP_H__
#define __DRAGDROP_H__

BOOL  DragDropAccept(HWND hwnd);
BOOL  DragDropDragOver(HWND hwnd, DWORD dwEffect);
BOOL  DragDropDragLeave(HWND hwnd);
BOOL  DragDropFiles(HWND hwnd, POINT point, UINT cFiles, LPSTR pszFiles, UINT cbszFiles, BOOL fNonClient = FALSE);
BOOL  DragDropDragEnter(HWND hwnd, POINT point, UINT cFiles, LPSTR pszFiles, UINT cbszFiles, DWORD dwEffect, BOOL fNonClient = FALSE);

#endif //__DRAGDROP_H__

     
