/* $Id: dragdrop.h,v 1.1 2002-06-02 10:08:10 sandervl Exp $ */

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
ULONG DragDropFiles(HWND hwnd, UINT cFiles, POINT point, LPSTR pszFiles, UINT cbszFiles, BOOL fNonClient = FALSE);

#endif //__DRAGDROP_H__

     
