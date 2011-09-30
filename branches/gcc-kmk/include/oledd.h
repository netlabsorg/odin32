/* $Id: oledd.h,v 1.1 2002-06-02 19:32:08 sandervl Exp $ */

/*
 *  OLE Drag and Drop private functions
 */

#ifndef __WINE_OLEDD_H
#define __WINE_OLEDD_H

typedef BOOL (* WIN32API PFN_ACCEPTSDRAGDROP)(HWND hwnd);
typedef BOOL (* WIN32API PFN_DRAGOVER)(HWND hwnd, DWORD dwEffect);
typedef BOOL (* WIN32API PFN_DRAGENTER)(HWND hwnd, HDROP hDrop, DWORD dwEffect);
typedef BOOL (* WIN32API PFN_DRAGLEAVE)(HWND hwnd);
typedef BOOL (* WIN32API PFN_DROPFILES)(HWND hwnd);

BOOL WIN32API OLEDD_AcceptsDragDrop(HWND hwnd);
BOOL WIN32API OLEDD_DragOver(HWND hwnd, DWORD dwEffect);
BOOL WIN32API OLEDD_DragEnter(HWND hwnd, HDROP hDrop, DWORD dwEffect);
BOOL WIN32API OLEDD_DragLeave(HWND hwnd);
BOOL WIN32API OLEDD_DropFiles(HWND hwnd);

#endif  /* __WINE_OLEDD_H */

