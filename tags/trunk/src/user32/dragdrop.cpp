/* $Id: dragdrop.cpp,v 1.4 2003-07-28 11:27:45 sandervl Exp $ */

/*
 * Win32 Drag 'n Drop functions for OS/2
 *
 * Copyright 2002 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <windows.h>
#include <dbglog.h>
#include <oledd.h>

#define DBG_LOCALLOG    DBG_dragdrop
#include "dbglocal.h"

static PFN_DRAGENTER       pfnDragEnter       = NULL;
static PFN_DRAGLEAVE       pfnDragLeave       = NULL;
static PFN_DROPFILES       pfnDropFiles       = NULL;
static PFN_DRAGOVER        pfnDragOver        = NULL;
static PFN_ACCEPTSDRAGDROP pfnAcceptsDragDrop = NULL;
static HANDLE              hOLE32             = 0;

//******************************************************************************
//******************************************************************************
ULONG DragDropFiles(HWND hwnd, POINT point, UINT cFiles, LPSTR pszFiles, UINT cbszFiles, BOOL fNonClient)
{
    DROPFILES *pDropFile;
    HGLOBAL    hDropFile;
    DWORD      dwExStyle;
    HWND       orghwnd = hwnd;

    dprintf(("DragDropFiles %x (%d,%d) %d %s %d %d", hwnd, point.x, point.y, cFiles, pszFiles, cbszFiles, fNonClient));

    dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);

    //TODO: Is it correct if the window or parent accepts files or must we check the top parent?
    hwnd = (dwExStyle & WS_EX_ACCEPTFILES) ? hwnd : GetParent(hwnd);
    dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);

    if(!(dwExStyle & WS_EX_ACCEPTFILES)) {
        if(pfnDropFiles) {
            return pfnDropFiles(orghwnd);
        }
        return FALSE;
    }

    cbszFiles++;    //extra terminating 0
    hDropFile = GlobalAlloc(0, sizeof(DROPFILES)+cbszFiles);
    pDropFile = (DROPFILES *)GlobalLock(hDropFile);
    if(pDropFile == NULL) {
        DebugInt3();
        return 0;
    }
    pDropFile->pFiles = sizeof(DROPFILES);
    pDropFile->fNC    = fNonClient;
    pDropFile->fWide  = FALSE;
    pDropFile->pt     = point;
    //copy strings (excluding terminating 0)
    memcpy((pDropFile+1), pszFiles, cbszFiles-1);
    GlobalUnlock(hDropFile);
    SendMessageA(hwnd, WM_DROPFILES, hDropFile, 0);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL DragDropDragOver(HWND hwnd, DWORD dwEffect)
{
    dprintf(("DragDropDragOver %x %x", hwnd, dwEffect));
    if(pfnDragOver) {
        return pfnDragOver(hwnd, dwEffect);
    }
    return TRUE;    //ignore
}
//******************************************************************************
//******************************************************************************
BOOL DragDropDragEnter(HWND hwnd, POINT point, UINT cFiles, LPSTR pszFiles, UINT cbszFiles, 
                       DWORD dwEffect, BOOL fNonClient)
{
    DROPFILES *pDropFile;
    HGLOBAL    hDropFile;

    dprintf(("DragDropDragEnter %x (%d,%d) %d %s %d %d", hwnd, point.x, point.y, cFiles, pszFiles, cbszFiles, fNonClient));

    if(pfnDragEnter) {
        cbszFiles++;    //extra terminating 0
        hDropFile = GlobalAlloc(0, sizeof(DROPFILES)+cbszFiles);
        pDropFile = (DROPFILES *)GlobalLock(hDropFile);
        if(pDropFile == NULL) {
            DebugInt3();
            return FALSE;
        }
        pDropFile->pFiles = sizeof(DROPFILES);
        pDropFile->fNC    = fNonClient;
        pDropFile->fWide  = FALSE;
        pDropFile->pt     = point;
        //copy strings (excluding terminating 0)
        memcpy((pDropFile+1), pszFiles, cbszFiles-1);
        GlobalUnlock(hDropFile);

        return pfnDragEnter(hwnd, hDropFile, dwEffect);
    }
    return TRUE;    //ignore
}
//******************************************************************************
//******************************************************************************
BOOL DragDropDragLeave(HWND hwnd)
{
    dprintf(("DragDropDragLeave %x", hwnd));
    if(pfnDragLeave) {
        return pfnDragLeave(hwnd);
    }
    return TRUE;    //ignore
}
//******************************************************************************
//******************************************************************************
BOOL DragDropAccept(HWND hwnd)
{
    DWORD      dwExStyle;

    dprintf(("DragDropDragAccept %x", hwnd));

    dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    //Is it correct if the window or parent accepts files or must we check the topparent parent?
    if(dwExStyle & WS_EX_ACCEPTFILES) {
        return TRUE;
    }
    DWORD dwStyle = GetWindowLongA(GetParent(hwnd), GWL_EXSTYLE);
    if(!(dwStyle & WS_EX_ACCEPTFILES)) {
        if(pfnAcceptsDragDrop == NULL) {
            //check for OLE drag & drop

            hOLE32 = GetModuleHandleA("OLE32.DLL");
            if(hOLE32 == 0) {
                //if ole32.dll isn't loaded, then ole drag and drop can't be active
                return FALSE;
            }
            //make sure the dll doesn't get unloaded
            hOLE32 = LoadLibraryA("OLE32.DLL");

            pfnAcceptsDragDrop = (PFN_ACCEPTSDRAGDROP)GetProcAddress(hOLE32, "OLEDD_AcceptsDragDrop");
            pfnDragOver        = (PFN_DRAGOVER)GetProcAddress(hOLE32, "OLEDD_DragOver");
            pfnDragLeave       = (PFN_DRAGLEAVE)GetProcAddress(hOLE32, "OLEDD_DragLeave");
            pfnDragEnter       = (PFN_DRAGENTER)GetProcAddress(hOLE32, "OLEDD_DragEnter");
            pfnDropFiles       = (PFN_DROPFILES)GetProcAddress(hOLE32, "OLEDD_DropFiles");
            if(!pfnAcceptsDragDrop || !pfnDragOver || !pfnDragLeave || !pfnDragEnter || !pfnDropFiles) {
                dprintf(("OLE DD functions not found!!"));
                DebugInt3();
                return FALSE;
            }
        }
        if(pfnAcceptsDragDrop) {
            return pfnAcceptsDragDrop(hwnd);
        }
        return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
     
