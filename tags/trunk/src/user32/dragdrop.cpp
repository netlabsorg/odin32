/* $Id: dragdrop.cpp,v 1.1 2002-06-02 10:08:09 sandervl Exp $ */

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

#define DBG_LOCALLOG    DBG_dragdrop
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
ULONG DragDropFiles(HWND hwnd, UINT cFiles, POINT point, LPSTR pszFiles, UINT cbszFiles, BOOL fNonClient)
{
    DROPFILES *pDropFile;
    HGLOBAL    hDropFile;
    DWORD      dwExStyle;

    dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    
    //Is it correct if the window or parent accepts files or must we check the topparent parent?
    hwnd = (dwExStyle & WS_EX_ACCEPTFILES) ? hwnd : GetParent(hwnd);
    cbszFiles++;    //extra terminating 0

    if(IsWindowUnicode(hwnd)) {
        dprintf(("unicode dropfiles"));
        cbszFiles *= 2;
    }

    hDropFile = GlobalAlloc(0, sizeof(DROPFILES)+cbszFiles);
    pDropFile = (DROPFILES *)GlobalLock(hDropFile);
    if(pDropFile == NULL) {
        DebugInt3();
        return 0;
    }
    pDropFile->pFiles = sizeof(DROPFILES);
    pDropFile->fNC    = fNonClient;
    pDropFile->fWide  = ::IsWindowUnicode(hwnd);
    pDropFile->pt     = point;
    if(IsWindowUnicode(hwnd)) {
        LPWSTR lpszFilesW = (LPWSTR)(pDropFile+1);
        while(*pszFiles) {
            int len = strlen(pszFiles);
            MultiByteToWideChar(CP_ACP, 0, pszFiles, -1, lpszFilesW, len);
            pszFiles   += len + 1;
            lpszFilesW += len + 1;
        }
        *lpszFilesW = 0;
    }
    else {
        //copy strings (excluding terminating 0)
        memcpy((pDropFile+1), pszFiles, cbszFiles-1);
    }
    GlobalUnlock(hDropFile);
    SendMessageA(hwnd, WM_DROPFILES, hDropFile, 0);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL DragDropAccept(HWND hwnd)
{
    DWORD      dwExStyle;

    dwExStyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    //Is it correct if the window or parent accepts files or must we check the topparent parent?
    if(dwExStyle & WS_EX_ACCEPTFILES) {
        return TRUE;
    }
    DWORD dwStyle = GetWindowLongA(GetParent(hwnd), GWL_EXSTYLE);
    if(!(dwStyle & WS_EX_ACCEPTFILES)) {
        return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
     
