/* $Id: oslibdnd.cpp,v 1.1 2003-07-28 11:28:10 sandervl Exp $ */
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
#define  INCL_WIN
#define  INCL_WINSTDDRAG
#define  INCL_PM
#include <os2wrap.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <dbglog.h>
#include <winconst.h>
#include <win32api.h>
#include <winuser32.h>

#include "oslibdnd.h"

static char *pszFile = NULL;
static HWND  hwndTarget = 0;
static PDRAGINFO curDragInfo = NULL;
static char *pszCurDragData = NULL;

#if 0
    FILE *tmpfile;
    tmpfile = fopen(pszFile, "wb+");    
    fwrite(lpszDnDString, 1, strlen(lpszDnDString)+1, tmpfile);
    fclose(tmpfile);
#endif
//******************************************************************************
//******************************************************************************
LPVOID OSLibCreateDragStruct(HWND hwndWin32, DWORD x, DWORD y, LPSTR lpszDnDString)
{
    PDRAGINFO  pdinfo;       /* Pointer to DRAGINFO structure        */
    HWND       hwnd;         /* Handle of calling (source) window    */
    BOOL       flResult;     /* Result indicator                     */
    DRAGITEM   ditem;        /* DRAGITEM structure                   */
    RECTL      rectl;
    ULONG      ScreenHeight;

    WinQueryWindowRect(HWND_DESKTOP, &rectl);
    ScreenHeight = rectl.yTop;

    pdinfo = DrgAllocDraginfo(1); /* Create the DRAGINFO structure   */
                                  /* Set the drag item               */
    if(pdinfo == NULL) {
        dprintf(("DrgAllocDraginfo FAILED!!"));
        return NULL;
    }
    pdinfo->usOperation  = DO_DEFAULT;
    pdinfo->hwndSource   = Win32ToOS2Handle(hwndWin32);
    pdinfo->xDrop        = x;
    pdinfo->yDrop        = ScreenHeight - y;

    char szDir[CCHMAXPATH*4];
    
    pszFile = (char *)malloc(CCHMAXPATH+1);
    if(pszFile == NULL) {
        DebugInt3();
        DrgFreeDraginfo(pdinfo);
        return NULL;
    }

    tmpnam(pszFile);
    strcpy(szDir, pszFile);

    char *tmp = strrchr(szDir, '\\');
    *tmp = 0;
    tmp++;

    dprintf(("temporary file %s", pszFile));

    strcat(szDir, "\\");
    
    ditem.hwndItem       = pdinfo->hwndSource;
    ditem.ulItemID       = 0;
    ditem.hstrType       = DrgAddStrHandle(DRT_TEXT);
    ditem.hstrRMF        = DrgAddStrHandle("<DRM_OS2FILE, DRF_TEXT>");
    ditem.hstrContainerName  = DrgAddStrHandle(szDir);
    ditem.hstrSourceName = 0; //done during rendering
    ditem.hstrTargetName = 0; 
    ditem.cxOffset       = 0;
    ditem.cyOffset       = 0;
    ditem.fsControl      = 0;
    ditem.fsSupportedOps = DO_COPYABLE | DO_MOVEABLE;

    flResult= DrgSetDragitem(pdinfo, &ditem, (ULONG)sizeof(ditem), 0);
    if(!flResult) dprintf(("DrgSetDragitem failed for %x", &ditem));
    
    return pdinfo;
}
//******************************************************************************
//******************************************************************************
void OSLibFreeDragStruct(LPVOID lpDragStruct)
{
    PDRAGINFO pdinfo = (PDRAGINFO)lpDragStruct;
    BOOL       flResult;     /* Result indicator                     */

    flResult = DrgDeleteDraginfoStrHandles(pdinfo);    
    if(!flResult) dprintf(("DrgDeleteDraginfoStrHandles failed for %x", lpDragStruct));

    flResult = DrgFreeDraginfo(pdinfo);
    if(!flResult) dprintf(("DrgFreeDragInfo failed for %x", lpDragStruct));

    return;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDragOver(LPVOID lpDragStruct, DWORD x, DWORD y)
{
    PDRAGINFO pdinfo = (PDRAGINFO)lpDragStruct;
    RECTL      rectl;
    ULONG      ScreenHeight, ret;
    HWND       hwnd;
    POINTL     pointl;

    WinQueryWindowRect(HWND_DESKTOP, &rectl);
    ScreenHeight = rectl.yTop;

    pdinfo->xDrop = x;
    pdinfo->yDrop = ScreenHeight - y;

    pointl.x = pdinfo->xDrop;
    pointl.y = pdinfo->yDrop;
    hwnd = WinWindowFromPoint(HWND_DESKTOP, &pointl, TRUE);
    if(hwnd == 0) return DROPEFFECT_NONE_W;

    if(hwnd != hwndTarget) {
        OSLibDragLeave(lpDragStruct);
        dprintf(("Post DM_DRAGOVER to %x (%x)", OS2ToWin32Handle(hwnd), hwnd));
    }
    hwndTarget = hwnd;

    ret = (ULONG)WinSendMsg(hwnd, DM_DRAGOVER, (MPARAM)pdinfo, MPFROM2SHORT(pdinfo->xDrop, pdinfo->yDrop));

    if(LOWORD(ret) != DOR_DROP) {
        return DROPEFFECT_NONE_W;
    }
    if(HIWORD(ret) == DO_LINK) {
        return DROPEFFECT_LINK_W;
    }
    if(HIWORD(ret) == DO_MOVE) {
        return DROPEFFECT_MOVE_W;
    }
    return DROPEFFECT_COPY_W;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDragLeave(LPVOID lpDragStruct)
{
    PDRAGINFO  pdinfo = (PDRAGINFO)lpDragStruct;
    ULONG      ret;

    if(hwndTarget) {
        dprintf(("Post DM_DRAGLEAVE to %x (%x)", OS2ToWin32Handle(hwndTarget), hwndTarget));
        ret = (ULONG)WinSendMsg(hwndTarget, DM_DRAGLEAVE, (MPARAM)pdinfo, 0);
    }
    return DROPEFFECT_NONE_W;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDragDrop(LPVOID lpDragStruct, DWORD x, DWORD y, LPSTR lpszDnDString)
{
    PDRAGINFO pdinfo = (PDRAGINFO)lpDragStruct;
    RECTL      rectl;
    ULONG      ScreenHeight, ret;
    HWND       hwnd;
    POINTL     pointl;

    WinQueryWindowRect(HWND_DESKTOP, &rectl);
    ScreenHeight = rectl.yTop;

    pdinfo->xDrop = x;
    pdinfo->yDrop = ScreenHeight - y;

    pointl.x = pdinfo->xDrop;
    pointl.y = pdinfo->yDrop;
    hwnd = WinWindowFromPoint(HWND_DESKTOP, &pointl, TRUE);
    if(hwnd == 0) return DROPEFFECT_NONE_W;

    hwndTarget = hwnd;
    curDragInfo = pdinfo;
    pszCurDragData = lpszDnDString;

    dprintf(("dnd data %s", pszCurDragData));
   
    ret = (ULONG)WinSendMsg(hwnd, DM_DROP, (MPARAM)pdinfo, 0);

    pszCurDragData = NULL;
    curDragInfo = NULL;
    hwndTarget = 0;
    return DROPEFFECT_COPY_W;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibRenderFormat(PDRAGTRANSFER pDragTransfer)
{
    PDRAGITEM pDragItem = pDragTransfer->pditem;
    int size = 0, ulBytes;
    char *tmp;

#ifdef DEBUG
    size = DrgQueryStrNameLen(pDragTransfer->hstrSelectedRMF);
    tmp = (char *)malloc(size+2);
    if(tmp == NULL) {
        DebugInt3();
        return FALSE;
    }
    size = DrgQueryStrName(pDragTransfer->hstrSelectedRMF, size+1, tmp);
    dprintf(("Rendering method %s", tmp));
    free(tmp);
#endif

    size = DrgQueryStrNameLen(pDragTransfer->hstrRenderToName);
    tmp = (char *)malloc(size+2);
    if(tmp == NULL) {
        DebugInt3();
        return FALSE;
    }
    size = DrgQueryStrName(pDragTransfer->hstrRenderToName, size+1, tmp);

    dprintf(("Render to file %s", tmp));

    dprintf(("dnd data %s", pszCurDragData));

    FILE *tmpfile;
    tmpfile = fopen(tmp, "wb+");    
    fwrite(pszCurDragData, 1, strlen(pszCurDragData), tmpfile);
    fclose(tmpfile);

    DrgPostTransferMsg(pDragTransfer->hwndClient, DM_RENDERCOMPLETE, pDragTransfer,
                        DMFL_RENDEROK, 0, TRUE);

    DrgFreeDragtransfer(pDragTransfer);
    free(tmp);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibEndConversation()
{
    return 0;
}
//******************************************************************************
//******************************************************************************
