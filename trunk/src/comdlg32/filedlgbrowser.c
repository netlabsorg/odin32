/* $Id: filedlgbrowser.c,v 1.5 2000-03-28 15:26:48 cbratschi Exp $ */
/*
 *  Implementation of IShellBrowser for the File Open common dialog
 *
 * Copyright 2000 Christoph Bratschi (cbratschi@datacomm.ch)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *  Corel WINE 20000324 level
 */

#ifdef __WIN32OS2__
#include <windef.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <heapstring.h>
#include <misc.h>

#endif

#include <stdio.h>
#include "unknwn.h"
#include "filedlgbrowser.h"
#include "winuser.h"
#include "heap.h"
#include "wine/obj_dataobject.h"
#include "debugtools.h"
#include "cdlg.h"
#include "wine/undocshell.h"

DEFAULT_DEBUG_CHANNEL(commdlg)

#define SETDefFormatEtc(fe,cf,med) \
{ \
    (fe).cfFormat = cf;\
    (fe).dwAspect = DVASPECT_CONTENT; \
    (fe).ptd =NULL;\
    (fe).tymed = med;\
    (fe).lindex = -1;\
};


/**************************************************************************
*   Structure
*/
static ICOM_VTABLE(IShellBrowser) IShellBrowserImpl_Vtbl =
{
        ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
        /* IUnknown */
        IShellBrowserImpl_QueryInterface,
        IShellBrowserImpl_AddRef,
        IShellBrowserImpl_Release,
        /* IOleWindow */
        IShellBrowserImpl_GetWindow,
        IShellBrowserImpl_ContextSensitiveHelp,
        /*  IShellBrowser */
        IShellBrowserImpl_InsertMenusSB,
        IShellBrowserImpl_SetMenuSB,
        IShellBrowserImpl_RemoveMenusSB,
        IShellBrowserImpl_SetStatusTextSB,
        IShellBrowserImpl_EnableModelessSB,
        IShellBrowserImpl_TranslateAcceleratorSB,
        IShellBrowserImpl_BrowseObject,
        IShellBrowserImpl_GetViewStateStream,
        IShellBrowserImpl_GetControlWindow,
        IShellBrowserImpl_SendControlMsg,
        IShellBrowserImpl_QueryActiveShellView,
        IShellBrowserImpl_OnViewWindowActive,
        IShellBrowserImpl_SetToolbarItems
};

static ICOM_VTABLE(ICommDlgBrowser) IShellBrowserImpl_ICommDlgBrowser_Vtbl =
{
        ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
        /* IUnknown */
        IShellBrowserImpl_ICommDlgBrowser_QueryInterface,
        IShellBrowserImpl_ICommDlgBrowser_AddRef,
        IShellBrowserImpl_ICommDlgBrowser_Release,
        /* ICommDlgBrowser */
        IShellBrowserImpl_ICommDlgBrowser_OnDefaultCommand,
        IShellBrowserImpl_ICommDlgBrowser_OnStateChange,
        IShellBrowserImpl_ICommDlgBrowser_IncludeObject
};


/**************************************************************************
*   Local Prototypes
*/

HRESULT IShellBrowserImpl_ICommDlgBrowser_OnSelChange(ICommDlgBrowser *iface, IShellView *ppshv);
#if 0
LPITEMIDLIST GetSelectedPidl(IShellView *ppshv);
#endif

/**************************************************************************
*   External Prototypes
*/
extern const char *FileOpenDlgInfosStr;

extern HRESULT          GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST pidl,DWORD dwFlags,LPSTR lpstrFileName);
extern HRESULT          GetFileName(HWND hwnd, LPITEMIDLIST pidl, LPSTR lpstrFileName);
extern IShellFolder*    GetShellFolderFromPidl(LPITEMIDLIST pidlAbs);
extern LPITEMIDLIST     GetParentPidl(LPITEMIDLIST pidl);
extern LPITEMIDLIST     GetPidlFromName(IShellFolder *psf,LPCSTR lpcstrFileName);

extern BOOL    FILEDLG95_SHELL_FillIncludedItemList(HWND hwnd,
                                                        LPITEMIDLIST pidlCurrentFolder,
                                                        LPSTR lpstrMask);

extern int     FILEDLG95_LOOKIN_SelectItem(HWND hwnd,LPITEMIDLIST pidl);
extern BOOL    FILEDLG95_OnOpen(HWND hwnd);
extern HRESULT SendCustomDlgNotificationMessage(HWND hwndParentDlg, UINT uCode);


/**************************************************************************
*  IShellBrowserImpl_Construct
*/
IShellBrowser * IShellBrowserImpl_Construct(HWND hwndOwner)
{
    IShellBrowserImpl *sb;
    FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwndOwner,FileOpenDlgInfosStr);

    sb=(IShellBrowserImpl*)SHAlloc(sizeof(IShellBrowserImpl));

    /* Initialisation of the member variables */
    sb->ref=1;
    sb->hwndOwner = hwndOwner;

    /* Initialisation of the vTables */
    sb->lpVtbl = &IShellBrowserImpl_Vtbl;
    sb->lpVtbl2 = &IShellBrowserImpl_ICommDlgBrowser_Vtbl;

    COMDLG32_SHGetSpecialFolderLocation(hwndOwner,
                               CSIDL_DESKTOP,
                               &fodInfos->ShellInfos.pidlAbsCurrent);

    TRACE("%p\n", sb);

    return (IShellBrowser *) sb;
}

/**************************************************************************
*
*
*  The INTERFACE of the IShellBrowser object
*
*/

/*
 * IUnknown
 */

/***************************************************************************
*  IShellBrowserImpl_QueryInterface
*/
HRESULT WINAPI IShellBrowserImpl_QueryInterface(IShellBrowser *iface,
                                            REFIID riid,
                                            LPVOID *ppvObj)
{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    *ppvObj = NULL;

    if(IsEqualIID(riid, &IID_IUnknown))          /*IUnknown*/
    { *ppvObj = This;
    }
    else if(IsEqualIID(riid, &IID_IOleWindow))  /*IOleWindow*/
    { *ppvObj = (IOleWindow*)This;
    }

    else if(IsEqualIID(riid, &IID_IShellBrowser))  /*IShellBrowser*/
    { *ppvObj = (IShellBrowser*)This;
    }

    else if(IsEqualIID(riid, &IID_ICommDlgBrowser))  /*ICommDlgBrowser*/
    { *ppvObj = (ICommDlgBrowser*) &(This->lpVtbl2);
    }

    if(*ppvObj)
    { IUnknown_AddRef( (IShellBrowser*) *ppvObj);
      return S_OK;
    }
    return E_NOINTERFACE;
}

/**************************************************************************
*  IShellBrowser::AddRef
*/
ULONG WINAPI IShellBrowserImpl_AddRef(IShellBrowser * iface)
{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    return ++(This->ref);
}

/**************************************************************************
*  IShellBrowserImpl_Release
*/
ULONG WINAPI IShellBrowserImpl_Release(IShellBrowser * iface)
{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    if (!--(This->ref))
    {
      COMDLG32_SHFree(This);
      return 0;
    }
    return This->ref;
}

/*
 * IOleWindow
 */

/**************************************************************************
*  IShellBrowserImpl_GetWindow  (IOleWindow)
*
*  Inherited from IOleWindow::GetWindow
*
*  See Windows documentation for more details
*
*  Note : We will never be window less in the File Open dialog
*
*/
HRESULT WINAPI IShellBrowserImpl_GetWindow(IShellBrowser * iface,
                                           HWND * phwnd)
{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    if(!This->hwndOwner)
        return E_FAIL;

    *phwnd = This->hwndOwner;

    return (*phwnd) ? S_OK : E_UNEXPECTED;

}

/**************************************************************************
*  IShellBrowserImpl_ContextSensitiveHelp
*/
HRESULT WINAPI IShellBrowserImpl_ContextSensitiveHelp(IShellBrowser * iface,
                                                      BOOL fEnterMode)
{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}

/*
 * IShellBrowser
 */

/**************************************************************************
*  IShellBrowserImpl_BrowseObject
*
*  See Windows documentation on IShellBrowser::BrowseObject for more details
*
*  This function will override user specified flags and will always
*  use SBSP_DEFBROWSER and SBSP_DEFMODE.
*/
HRESULT WINAPI IShellBrowserImpl_BrowseObject(IShellBrowser *iface,
                                              LPCITEMIDLIST pidl,
                                              UINT wFlags)
{
    HRESULT hRes;
    IShellFolder *psfTmp;
    IShellView *psvTmp;
    FileOpenDlgInfos *fodInfos;
    LPITEMIDLIST pidlTmp;

    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    fodInfos = (FileOpenDlgInfos *) GetPropA(This->hwndOwner,FileOpenDlgInfosStr);

    /* Format the pidl according to its parameter's category */
    if(wFlags & SBSP_RELATIVE)
    {

        /* SBSP_RELATIVE  A relative pidl (relative from the current folder) */
        hRes = IShellFolder_BindToObject(fodInfos->Shell.FOIShellFolder,
                                     pidl,
                                     NULL,
                                     &IID_IShellFolder,
                                     (LPVOID *)&psfTmp);
        if(FAILED(hRes))
        {
            return hRes;
        }
        /* create an absolute pidl */
        pidlTmp = COMDLG32_PIDL_ILCombine(fodInfos->ShellInfos.pidlAbsCurrent,
                                                        (LPITEMIDLIST)pidl);

    }
    else if(wFlags & SBSP_PARENT)
    {
        /* Browse the parent folder (ignores the pidl) */

        pidlTmp = GetParentPidl(fodInfos->ShellInfos.pidlAbsCurrent);
        psfTmp = GetShellFolderFromPidl(pidlTmp);

    }
    else
    {
        /* An absolute pidl (relative from the desktop) */
        pidlTmp =  COMDLG32_PIDL_ILClone((LPITEMIDLIST)pidl);
        psfTmp = GetShellFolderFromPidl(pidlTmp);
    }


    /* Retrieve the IShellFolder interface of the pidl specified folder */
    if(!psfTmp)
        return E_FAIL;

    /* If the pidl to browse to is equal to the actual pidl ...
       do nothing and pretend you did it*/
    if(COMDLG32_PIDL_ILIsEqual(pidlTmp,fodInfos->ShellInfos.pidlAbsCurrent))
    {
        IShellFolder_Release(psfTmp);
        COMDLG32_SHFree(pidlTmp);
        return NOERROR;
    }
#ifdef SHELL_NO_DESKTOP

    if(pidlTmp->mkid.cb == 0x00)
    {
        IShellFolder_Release(psfTmp);
                COMDLG32_SHFree(pidlTmp);
        return NOERROR;
    }
#endif

    /* Release the current fodInfos->Shell.FOIShellFolder and update its value */
    IShellFolder_Release(fodInfos->Shell.FOIShellFolder);
    fodInfos->Shell.FOIShellFolder = psfTmp;

    /* Create the associated view */
    if(SUCCEEDED(hRes = IShellFolder_CreateViewObject(psfTmp,
                                                      fodInfos->ShellInfos.hwndOwner,
                                                      &IID_IShellView,
                                                      (LPVOID *)&psvTmp)))
    {
        HWND hwndView;
        HWND hDlgWnd;
        BOOL bViewHasFocus;

        /* Check if listview has focus */
        bViewHasFocus = IsChild(fodInfos->ShellInfos.hwndView,GetFocus());

        /* Get the foldersettings from the old view */
        if(fodInfos->Shell.FOIShellView)
        {
          IShellView_GetCurrentInfo(fodInfos->Shell.FOIShellView,
                                  &fodInfos->ShellInfos.folderSettings);
        }

        /* Create the window */
        if(SUCCEEDED(hRes = IShellView_CreateViewWindow(psvTmp,
                                          NULL,
                                          &fodInfos->ShellInfos.folderSettings,
                                          fodInfos->Shell.FOIShellBrowser,
                                          &fodInfos->ShellInfos.rectView,
                                          &hwndView)))
        {
            /* Fit the created view in the appropriate RECT */
            MoveWindow(hwndView,
                       fodInfos->ShellInfos.rectView.left,
                       fodInfos->ShellInfos.rectView.top,
                       fodInfos->ShellInfos.rectView.right-fodInfos->ShellInfos.rectView.left,
                       fodInfos->ShellInfos.rectView.bottom-fodInfos->ShellInfos.rectView.top,
                       FALSE);

            /* Select the new folder in the Look In combo box of the Open file dialog */

            FILEDLG95_LOOKIN_SelectItem(fodInfos->DlgInfos.hwndLookInCB,pidlTmp);

            /* Release old pidlAbsCurrent memory and update its value */
            COMDLG32_SHFree((LPVOID)fodInfos->ShellInfos.pidlAbsCurrent);
            fodInfos->ShellInfos.pidlAbsCurrent = pidlTmp;

            /* Release the current fodInfos->Shell.FOIShellView and update its value */
            if(fodInfos->Shell.FOIShellView)
            {
                IShellView_DestroyViewWindow(fodInfos->Shell.FOIShellView);
                IShellView_Release(fodInfos->Shell.FOIShellView);
            }
#if 0
            ShowWindow(fodInfos->ShellInfos.hwndView,SW_HIDE);
#endif
            fodInfos->Shell.FOIShellView = psvTmp;

            fodInfos->ShellInfos.hwndView = hwndView;

            /* changes the tab order of the ListView to reflect the window's File Dialog */
            hDlgWnd = GetDlgItem(GetParent(hwndView), IDC_LOOKIN);
            SetWindowPos(hwndView, hDlgWnd, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

            /* Since we destroyed the old view if it had focus set focus
               to the newly created view */
            if (bViewHasFocus)
                SetFocus(fodInfos->ShellInfos.hwndView);

            return NOERROR;
        }
    }

    FILEDLG95_LOOKIN_SelectItem(fodInfos->DlgInfos.hwndLookInCB,fodInfos->ShellInfos.pidlAbsCurrent);
    return hRes;
}

/**************************************************************************
*  IShellBrowserImpl_EnableModelessSB
*/
HRESULT WINAPI IShellBrowserImpl_EnableModelessSB(IShellBrowser *iface,
                                              BOOL fEnable)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}

/**************************************************************************
*  IShellBrowserImpl_GetControlWindow
*/
HRESULT WINAPI IShellBrowserImpl_GetControlWindow(IShellBrowser *iface,
                                              UINT id,
                                              HWND *lphwnd)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}
/**************************************************************************
*  IShellBrowserImpl_GetViewStateStream
*/
HRESULT WINAPI IShellBrowserImpl_GetViewStateStream(IShellBrowser *iface,
                                                DWORD grfMode,
                                                LPSTREAM *ppStrm)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}
/**************************************************************************
*  IShellBrowserImpl_InsertMenusSB
*/
HRESULT WINAPI IShellBrowserImpl_InsertMenusSB(IShellBrowser *iface,
                                           HMENU hmenuShared,
                                           LPOLEMENUGROUPWIDTHS lpMenuWidths)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}
/**************************************************************************
*  IShellBrowserImpl_OnViewWindowActive
*/
HRESULT WINAPI IShellBrowserImpl_OnViewWindowActive(IShellBrowser *iface,
                                                IShellView *ppshv)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}
/**************************************************************************
*  IShellBrowserImpl_QueryActiveShellView
*/
HRESULT WINAPI IShellBrowserImpl_QueryActiveShellView(IShellBrowser *iface,
                                                  IShellView **ppshv)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    FileOpenDlgInfos *fodInfos;

    TRACE("(%p)\n", This);

    fodInfos = (FileOpenDlgInfos *) GetPropA(This->hwndOwner,FileOpenDlgInfosStr);

    if(!(*ppshv = fodInfos->Shell.FOIShellView))
    {
        return E_FAIL;
    }
    IShellView_AddRef(fodInfos->Shell.FOIShellView);
    return NOERROR;
}
/**************************************************************************
*  IShellBrowserImpl_RemoveMenusSB
*/
HRESULT WINAPI IShellBrowserImpl_RemoveMenusSB(IShellBrowser *iface,
                                           HMENU hmenuShared)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}
/**************************************************************************
*  IShellBrowserImpl_SendControlMsg
*/
HRESULT WINAPI IShellBrowserImpl_SendControlMsg(IShellBrowser *iface,
                                            UINT id,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            LRESULT *pret)

{
    ICOM_THIS(IShellBrowserImpl, iface);
    LRESULT lres;

    TRACE("(%p)->(0x%08x 0x%08x 0x%08x 0x%08lx %p)\n", This, id, uMsg, wParam, lParam, pret);

    switch (id)
    {
      case FCW_TOOLBAR:
        lres = SendDlgItemMessageA( This->hwndOwner, IDC_TOOLBAR, uMsg, wParam, lParam);
        break;
      default:
        FIXME("ctrl id: %x\n", id);
        return E_NOTIMPL;
    }
    if (pret) *pret = lres;
    return S_OK;
}
/**************************************************************************
*  IShellBrowserImpl_SetMenuSB
*/
HRESULT WINAPI IShellBrowserImpl_SetMenuSB(IShellBrowser *iface,
                                       HMENU hmenuShared,
                                       HOLEMENU holemenuReserved,
                                       HWND hwndActiveObject)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}
/**************************************************************************
*  IShellBrowserImpl_SetStatusTextSB
*/
HRESULT WINAPI IShellBrowserImpl_SetStatusTextSB(IShellBrowser *iface,
                                             LPCOLESTR lpszStatusText)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}
/**************************************************************************
*  IShellBrowserImpl_SetToolbarItems
*/
HRESULT WINAPI IShellBrowserImpl_SetToolbarItems(IShellBrowser *iface,
                                             LPTBBUTTON lpButtons,
                                             UINT nButtons,
                                             UINT uFlags)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}
/**************************************************************************
*  IShellBrowserImpl_TranslateAcceleratorSB
*/
HRESULT WINAPI IShellBrowserImpl_TranslateAcceleratorSB(IShellBrowser *iface,
                                                    LPMSG lpmsg,
                                                    WORD wID)

{
    ICOM_THIS(IShellBrowserImpl, iface);

    TRACE("(%p)\n", This);

    /* Feature not implemented */
    return E_NOTIMPL;
}

/*
 * ICommDlgBrowser
 */

/***************************************************************************
*  IShellBrowserImpl_ICommDlgBrowser_QueryInterface
*/
HRESULT WINAPI IShellBrowserImpl_ICommDlgBrowser_QueryInterface(ICommDlgBrowser *iface,
                                            REFIID riid,
                                            LPVOID *ppvObj)
{
    _ICOM_THIS_FromICommDlgBrowser(IShellBrowser,iface);

    TRACE("(%p)\n", This);

    return IShellBrowserImpl_QueryInterface(This,riid,ppvObj);
}

/**************************************************************************
*  IShellBrowserImpl_ICommDlgBrowser_AddRef
*/
ULONG WINAPI IShellBrowserImpl_ICommDlgBrowser_AddRef(ICommDlgBrowser * iface)
{
    _ICOM_THIS_FromICommDlgBrowser(IShellBrowser,iface);

    TRACE("(%p)\n", This);

    return IShellBrowserImpl_AddRef(This);
}

/**************************************************************************
*  IShellBrowserImpl_ICommDlgBrowser_Release
*/
ULONG WINAPI IShellBrowserImpl_ICommDlgBrowser_Release(ICommDlgBrowser * iface)
{
    _ICOM_THIS_FromICommDlgBrowser(IShellBrowser,iface);

    TRACE("(%p)\n", This);

    return IShellBrowserImpl_Release(This);
}
/**************************************************************************
*  IShellBrowserImpl_ICommDlgBrowser_OnDefaultCommand
*
*   Called when a user double-clicks in the view or presses the ENTER key
*/
HRESULT WINAPI IShellBrowserImpl_ICommDlgBrowser_OnDefaultCommand(ICommDlgBrowser *iface,
                                                                  IShellView *ppshv)
{
    LPITEMIDLIST pidl;
    FileOpenDlgInfos *fodInfos;

    _ICOM_THIS_FromICommDlgBrowser(IShellBrowserImpl,iface);

    TRACE("(%p)\n", This);

    fodInfos = (FileOpenDlgInfos *) GetPropA(This->hwndOwner,FileOpenDlgInfosStr);

    /* If the selected object is not a folder, send a IDOK command to parent window */
    pidl = GetSelectedPidl(ppshv);
    if (pidl)
    {
        HRESULT hRes;

        ULONG  ulAttr = SFGAO_FOLDER | SFGAO_HASSUBFOLDER;
        IShellFolder_GetAttributesOf(fodInfos->Shell.FOIShellFolder, 1, &pidl, &ulAttr);
        if (ulAttr)
            hRes = IShellBrowser_BrowseObject((IShellBrowser *)This,pidl,SBSP_RELATIVE);
        /* Tell the dialog that the user selected a file */
        else
        {
            hRes = PostMessageA(This->hwndOwner, WM_COMMAND, IDOK, 0L);
        }

        /* Free memory used by pidl */
        COMDLG32_SHFree((LPVOID)pidl);

        return hRes;
    }

    return E_FAIL;
}

/**************************************************************************
*  IShellBrowserImpl_ICommDlgBrowser_OnStateChange
*/
HRESULT WINAPI IShellBrowserImpl_ICommDlgBrowser_OnStateChange(ICommDlgBrowser *iface,
                                                               IShellView *ppshv,
                                                               ULONG uChange)
{

    _ICOM_THIS_FromICommDlgBrowser(IShellBrowserImpl,iface);

    TRACE("(%p)\n", This);

    switch (uChange)
    {
        case CDBOSC_SETFOCUS:
             /* FIXME: Reset the default button.
                This should be taken care of by defdlg. If control
                other than button receives focus the default button
                should be restored. */
             SendMessageA(This->hwndOwner, DM_SETDEFID, IDOK, 0);
            break;
        case CDBOSC_KILLFOCUS:
            {
                FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(This->hwndOwner,FileOpenDlgInfosStr);
                if(fodInfos->DlgInfos.dwDlgProp & FODPROP_SAVEDLG)
                    SetDlgItemTextA(fodInfos->ShellInfos.hwndOwner,IDOK,"&Save");
            }
            break;
        case CDBOSC_SELCHANGE:
            return IShellBrowserImpl_ICommDlgBrowser_OnSelChange(iface,ppshv);
        case CDBOSC_RENAME:
            break;
    }

    return NOERROR;
}
/**************************************************************************
*  IShellBrowserImpl_ICommDlgBrowser_IncludeObject
*/
HRESULT WINAPI IShellBrowserImpl_ICommDlgBrowser_IncludeObject(ICommDlgBrowser *iface,
                                                               IShellView * ppshv,
                                                               LPCITEMIDLIST pidl)
{
    FileOpenDlgInfos *fodInfos;
    ULONG ulAttr;
    STRRET str;
    WCHAR szPathW[MAX_PATH];

    _ICOM_THIS_FromICommDlgBrowser(IShellBrowserImpl,iface);

    TRACE("(%p)\n", This);
//CB: slow!!!
    fodInfos = (FileOpenDlgInfos *) GetPropA(This->hwndOwner,FileOpenDlgInfosStr);

    ulAttr = SFGAO_HIDDEN | SFGAO_FOLDER | SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR | SFGAO_LINK;
    IShellFolder_GetAttributesOf(fodInfos->Shell.FOIShellFolder, 1, &pidl, &ulAttr);

    if( (ulAttr & SFGAO_HIDDEN)                                         /* hidden */
      | !(ulAttr & (SFGAO_FILESYSTEM | SFGAO_FILESYSANCESTOR))) /* special folder */
        return S_FALSE;
    /* always include directorys and links */
    if(ulAttr & (SFGAO_FOLDER | SFGAO_LINK))
        return S_OK;
    /* Check if there is a mask to apply if not */
    if(!fodInfos->ShellInfos.lpstrCurrentFilter ||
       !lstrlenW(fodInfos->ShellInfos.lpstrCurrentFilter))
        return S_OK;

    if (SUCCEEDED(IShellFolder_GetDisplayNameOf(fodInfos->Shell.FOIShellFolder, pidl, SHGDN_FORPARSING, &str)))
    {
      if (SUCCEEDED(StrRetToBufW(&str, pidl,szPathW, MAX_PATH)))
      {
          if (COMDLG32_PathMatchSpecW(szPathW, fodInfos->ShellInfos.lpstrCurrentFilter))
            return S_OK;
      }
    }
    return S_FALSE;
}

/**************************************************************************
*  IShellBrowserImpl_ICommDlgBrowser_OnSelChange
*/
HRESULT IShellBrowserImpl_ICommDlgBrowser_OnSelChange(ICommDlgBrowser *iface, IShellView *ppshv)
{
    ULONG             uAttr;
    FileOpenDlgInfos *fodInfos;
    UINT              nFiles = 0;                  /* Intial to zero */
    UINT              nCurrLength;
    UINT              nFileToOpen;
    UINT              nAllLength = 2;              /* Include intial '"' and final NULL */
    UINT              nSize = MAX_PATH;
    UINT              nFileSelected = 0;
    LPITEMIDLIST      pidlSelection;
    LPSTR             lpstrTemp = NULL;
    LPSTR             lpstrAllFile = NULL;
    char              lpstrCurrFile[MAX_PATH];

    _ICOM_THIS_FromICommDlgBrowser(IShellBrowserImpl,iface);

    fodInfos = (FileOpenDlgInfos *) GetPropA(This->hwndOwner,FileOpenDlgInfosStr);
    TRACE("(%p)\n", This);

    /* Locate memory and Get selected item counter */
    if((lpstrAllFile = (LPSTR)SHAlloc(nSize * sizeof(char))) != NULL)
    {
        ZeroMemory(lpstrAllFile, nSize * sizeof(char));
        *lpstrAllFile =  '\"';
        lpstrTemp = lpstrAllFile + 1;
        nFileSelected = GetNumSelected(fodInfos->Shell.FOIShellView);     /* Get all selected counter */
    }

    /* Count all selected files we have */
    for(nFileToOpen = 0; nFileToOpen < nFileSelected; nFileToOpen++)
    {   /* get the file selected */
        pidlSelection = NULL;
        uAttr = SFGAO_FOLDER | SFGAO_HASSUBFOLDER;
        ZeroMemory(lpstrCurrFile, MAX_PATH * sizeof(char));
        EnumSelectedPidls(fodInfos->Shell.FOIShellView, nFileToOpen, &pidlSelection);

        /* get the file name and attrib of the selected files*/
        GetName(fodInfos->Shell.FOIShellFolder, pidlSelection, SHGDN_NORMAL, lpstrCurrFile);
        IShellFolder_GetAttributesOf(fodInfos->Shell.FOIShellFolder, 1, &pidlSelection, &uAttr);
        COMDLG32_SHFree((LPVOID) pidlSelection);

        if(!uAttr)
        {   /* Get the new file name */
            nCurrLength = lstrlenA(lpstrCurrFile);
            if(nAllLength + nCurrLength + 3 > nSize)
            {   /* increase the memory and transfer string to new location */
                nSize += MAX_PATH;
                if((lpstrTemp = (LPSTR)SHAlloc(nSize * sizeof(char))) != NULL)
                {   /* Transfer old file names */
                    ZeroMemory(lpstrTemp, nSize * sizeof(char));
                    lstrcpyA(lpstrTemp, lpstrAllFile);
                    SHFree(lpstrAllFile);
                    lpstrAllFile = lpstrTemp;
                    lpstrTemp = lpstrAllFile + nAllLength - 1;
                }
                else
                {   /* if failure, stop the loop to get filename */
                    nFileSelected = 0;
                }
            }

            if(lpstrTemp != NULL)
            {   /* Add the new file name */
                nFiles++;
                lstrcpyA(lpstrTemp, lpstrCurrFile);
                *(lpstrTemp + nCurrLength) = '\"';
                *(lpstrTemp + nCurrLength + 1) = ' ';
                *(lpstrTemp + nCurrLength + 2) = '\"';
                nAllLength += nCurrLength + 3;
                lpstrTemp = lpstrAllFile + nAllLength - 1;
            }
        }
    }

    if(lpstrAllFile)
    {
        if(nFiles > 1)
        {
            *(lpstrTemp - 2) = '\0';
            SetWindowTextA(fodInfos->DlgInfos.hwndFileName, lpstrAllFile);
        }
        else
        {
            *(lpstrTemp - 3) = '\0';
            SetWindowTextA(fodInfos->DlgInfos.hwndFileName, lpstrAllFile + 1);
        }

        fodInfos->DlgInfos.dwDlgProp |= FODPROP_USEVIEW;
        SendCustomDlgNotificationMessage(This->hwndOwner, CDN_SELCHANGE);
        SHFree( lpstrAllFile );
    }
    else
    {
        SetWindowTextA(fodInfos->DlgInfos.hwndFileName, "");
    }

    if(fodInfos->DlgInfos.dwDlgProp & FODPROP_SAVEDLG)
        SetDlgItemTextA(fodInfos->ShellInfos.hwndOwner, IDOK, "&Save");
    else
        SetDlgItemTextA(fodInfos->ShellInfos.hwndOwner, IDOK, "&Open");

    fodInfos->DlgInfos.dwDlgProp &= ~FODPROP_USEVIEW;

    return nFileSelected ? S_OK : E_FAIL;
}

/***********************************************************************
 *          GetSelectedPidl
 *
 * Return the pidl of the first selected item in the view
 */
LPITEMIDLIST GetSelectedPidl(IShellView *ppshv)
{

    IDataObject *doSelected;
    LPITEMIDLIST pidlSelected = NULL;

    TRACE("sv=%p\n", ppshv);

    /* Get an IDataObject from the view */
    if(SUCCEEDED(IShellView_GetItemObject(ppshv,
                                          SVGIO_SELECTION,
                                          &IID_IDataObject,
                                          (LPVOID *)&doSelected)))
    {
        STGMEDIUM medium;
        FORMATETC formatetc;

        /* Set the FORMATETC structure*/
        SETDefFormatEtc(formatetc,
                        RegisterClipboardFormatA(CFSTR_SHELLIDLIST),
                        TYMED_HGLOBAL);

        /* Get the pidl from IDataObject */
        if(SUCCEEDED(IDataObject_GetData(doSelected,&formatetc,&medium)))
        {
            LPIDA cida = GlobalLock(medium.u.hGlobal);
            TRACE("cida=%p\n", cida);
            pidlSelected =  COMDLG32_PIDL_ILClone((LPITEMIDLIST)(&((LPBYTE)cida)[cida->aoffset[1]]));

            if(medium.pUnkForRelease)
                IUnknown_Release(medium.pUnkForRelease);
            else
            {
              GlobalUnlock(medium.u.hGlobal);
              GlobalFree(medium.u.hGlobal);
            }
        }
        IDataObject_Release(doSelected);
        return pidlSelected;
    }

    return NULL;
}

/***********************************************************************
 *          EnumSelectedPidls
 *
 * Return the pidl(s) of the selected item(s) in the view.
 *
*/
BOOL EnumSelectedPidls( IShellView *ppshv,  /*[in]*/
                        UINT nPidlIndex,  /*[in]*/
                        LPITEMIDLIST *pidlSelected /*[out]*/ )
{

    IDataObject *doSelected;
    BOOL retVal = TRUE;

    /* Get an IDataObject from the view */
    if(SUCCEEDED(IShellView_GetItemObject(ppshv,
                                          SVGIO_SELECTION,
                                          &IID_IDataObject,
                                          (LPVOID *)&doSelected)))
    {
        STGMEDIUM medium;
        FORMATETC formatetc;

        /* Set the FORMATETC structure*/
        SETDefFormatEtc(formatetc,
                        RegisterClipboardFormatA(CFSTR_SHELLIDLIST),
                        TYMED_HGLOBAL);

        /* Get the pidls from IDataObject */
        if(SUCCEEDED(IDataObject_GetData(doSelected,&formatetc,&medium)))
        {
            LPIDA cida = GlobalLock(medium.u.hGlobal);
            if(nPidlIndex < cida->cidl)
            {
                *pidlSelected = COMDLG32_PIDL_ILClone((LPITEMIDLIST)(&((LPBYTE)cida)[cida->aoffset[nPidlIndex + 1]]));
            }
            else
            {
                retVal = FALSE;
            }

            if(medium.pUnkForRelease)
            {
                IUnknown_Release(medium.pUnkForRelease);
            }
            else
            {
                GlobalUnlock(medium.u.hGlobal);
                GlobalFree(medium.u.hGlobal);
            }
        }
        IDataObject_Release(doSelected);
        return retVal;
    }
    return FALSE;
}

/***********************************************************************
 *          GetNumSelected
 *
 * Return the number of selected items in the view.
 *
*/
UINT GetNumSelected( IShellView *ppshv )
{
    IDataObject *doSelected;
    UINT retVal = 0;

       /* Get an IDataObject from the view */
    if(SUCCEEDED(IShellView_GetItemObject(ppshv,
                                          SVGIO_SELECTION,
                                          &IID_IDataObject,
                                          (LPVOID *)&doSelected)))
    {
        STGMEDIUM medium;
        FORMATETC formatetc;

        /* Set the FORMATETC structure*/
        SETDefFormatEtc(formatetc,
                        RegisterClipboardFormatA(CFSTR_SHELLIDLIST),
                        TYMED_HGLOBAL);

        /* Get the pidls from IDataObject */
        if(SUCCEEDED(IDataObject_GetData(doSelected,&formatetc,&medium)))
        {
            LPIDA cida = GlobalLock(medium.u.hGlobal);
            retVal = cida->cidl;

            if(medium.pUnkForRelease)
                IUnknown_Release(medium.pUnkForRelease);
            else
            {
                GlobalUnlock(medium.u.hGlobal);
                GlobalFree(medium.u.hGlobal);
            }
        }
        IDataObject_Release(doSelected);
        return retVal;
    }

    return 0;
}



