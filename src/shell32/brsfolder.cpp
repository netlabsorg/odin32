/* $Id: brsfolder.cpp,v 1.1 1999-10-09 11:13:18 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#define ICOM_CINTERFACE 1

#include <winbase.h>
#include "winerror.h"
#include "heap.h"
#include "resource.h"
#include "dlgs.h"
#include "win.h"
#include "sysmetrics.h"
#include "debugtools.h"
#include "winreg.h"
#include "authors.h"
#include "winnls.h"
#include "commctrl.h"
#include "spy.h"

#include "wine/obj_base.h"
#include "wine/obj_enumidlist.h"
#include "wine/obj_shellfolder.h"
#include "wine/undocshell.h"

#include "pidl.h"
#include "shell32_main.h"
#include "shellapi.h"

#include <misc.h>


/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(shell32-brsfolder)

#define         IDD_TREEVIEW 99

static HWND             hwndTreeView;
static LPBROWSEINFOA  lpBrowseInfo;
static LPITEMIDLIST     pidlRet;

static void FillTreeView(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpifq, HTREEITEM hParent);


/*****************************************************************************
 * Implementation                                                            *
 *****************************************************************************/

static void InitializeTreeView(HWND hwndParent)
{
        HIMAGELIST      hImageList;
        IShellFolder *  lpsf;
        HRESULT hr;

  dprintf(("SHELL32:brsfolder:InitializeTreeView(%08xh)\n",
           hwndParent));

        hwndTreeView = GetDlgItem (hwndParent, IDD_TREEVIEW);
        Shell_GetImageList(NULL, &hImageList);

        if (hImageList && hwndTreeView)
        { TreeView_SetImageList(hwndTreeView, hImageList, 0);
        }

        hr = SHGetDesktopFolder(&lpsf);

        if (SUCCEEDED(hr) && hwndTreeView)
        { TreeView_DeleteAllItems(hwndTreeView);
          FillTreeView(lpsf, NULL, TVI_ROOT);
        }

        if (SUCCEEDED(hr))
        { IShellFolder_Release(lpsf);
        }
}

static int GetIcon(LPITEMIDLIST lpi, UINT uFlags)
{
        SHFILEINFOA    sfi;

  dprintf(("SHELL32:brsfolder:GetIcon(%08xh,%08xh)\n",
           lpi,
           uFlags));

        SHGetFileInfoA((LPCSTR)lpi,0,&sfi, sizeof(SHFILEINFOA), uFlags);
        return sfi.iIcon;
}

static void GetNormalAndSelectedIcons(LPITEMIDLIST lpifq,LPTVITEMA lpTV_ITEM)
{
  dprintf(("SHELL32:brsfolder:GetNormalAndSelectedIcons(%08xh,%08xh)\n",
           lpifq,
           lpTV_ITEM));

        lpTV_ITEM->iImage = GetIcon(lpifq, SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
        lpTV_ITEM->iSelectedImage = GetIcon(lpifq, SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);

        return;
}

typedef struct tagID
{
   LPSHELLFOLDER lpsfParent;
   LPITEMIDLIST  lpi;
   LPITEMIDLIST  lpifq;
} TV_ITEMDATA, *LPTV_ITEMDATA;

static BOOL GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, LPSTR lpFriendlyName)
{
        BOOL   bSuccess=TRUE;
        STRRET str;

  dprintf(("SHELL32:brsfolder:GetName(%08xh,%08xh,%08xh,%08xh)\n",
           lpsf,
           lpi,
           dwFlags,
           lpFriendlyName));

        if (SUCCEEDED(IShellFolder_GetDisplayNameOf(lpsf, lpi, dwFlags, &str)))
        {
          if(FAILED(StrRetToStrNA (lpFriendlyName, MAX_PATH, &str, lpi)))
          {
              bSuccess = FALSE;
          }
        }
        else
          bSuccess = FALSE;

        TRACE("-- %s\n",lpFriendlyName);
        return bSuccess;
}

static void FillTreeView(IShellFolder * lpsf, LPITEMIDLIST  pidl, HTREEITEM hParent)
{
        TVITEMA         tvi;
        TVINSERTSTRUCTA tvins;
        HTREEITEM       hPrev = 0;
        LPENUMIDLIST    lpe=0;
        LPITEMIDLIST    pidlTemp=0;
        LPTV_ITEMDATA   lptvid=0;
        ULONG           ulFetched;
        HRESULT         hr;
        char            szBuff[256];
        HWND            hwnd=GetParent(hwndTreeView);

  dprintf(("SHELL32:brsfolder:FillTreeView(%08xh,%08xh,%08xh)\n",
           lpsf,
           pidl,
           hParent));

        SetCapture(GetParent(hwndTreeView));
        SetCursor(LoadCursorA(0, IDC_WAITA));

        hr=IShellFolder_EnumObjects(lpsf,hwnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS,&lpe);

        if (SUCCEEDED(hr))
        { while (NOERROR == IEnumIDList_Next(lpe,1,&pidlTemp,&ulFetched))
          { ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER;
            IShellFolder_GetAttributesOf(lpsf, 1, &pidlTemp, &ulAttrs);
            if (ulAttrs & (SFGAO_HASSUBFOLDER | SFGAO_FOLDER))
            { if (ulAttrs & SFGAO_FOLDER)
              { tvi.mask  = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

                if (ulAttrs & SFGAO_HASSUBFOLDER)
                {  tvi.cChildren=1;
                   tvi.mask |= TVIF_CHILDREN;
                }

                if (! ( lptvid = (LPTV_ITEMDATA)SHAlloc(sizeof(TV_ITEMDATA)) ) )
                  goto Done;

                if (!GetName(lpsf, pidlTemp, SHGDN_NORMAL, szBuff))
                  goto Done;

                tvi.pszText    = szBuff;
                tvi.cchTextMax = MAX_PATH;
                tvi.lParam = (LPARAM)lptvid;

                IShellFolder_AddRef(lpsf);
                lptvid->lpsfParent = lpsf;
                lptvid->lpi     = ILClone(pidlTemp);
                lptvid->lpifq   = ILCombine(pidl, pidlTemp);
                GetNormalAndSelectedIcons(lptvid->lpifq, &tvi);

                tvins.DUMMYUNIONNAME.item         = tvi;
                tvins.hInsertAfter = hPrev;
                tvins.hParent      = hParent;

                hPrev = (HTREEITEM)TreeView_InsertItemA (hwndTreeView, &tvins);

              }
            }
            SHFree(pidlTemp);  /* Finally, free the pidl that the shell gave us... */
            pidlTemp=0;
          }
        }

Done:
        ReleaseCapture();
        SetCursor(LoadCursorA(0, IDC_ARROWA));

        if (lpe)
          lpe->lpvtbl->fnRelease(lpe);
        if (pidlTemp )
          SHFree(pidlTemp);
}

static LRESULT MsgNotify(HWND hWnd,  UINT CtlID, LPNMHDR lpnmh)
{
        NMTREEVIEWA     *pnmtv   = (NMTREEVIEWA *)lpnmh;
        LPTV_ITEMDATA   lptvid;  /* Long pointer to TreeView item data */
        IShellFolder *  lpsf2=0;


  dprintf(("SHELL32:brsfolder:MsgNotify(%08xh,%08xh,%08xh)\n",
           hWnd,
           CtlID,
           lpnmh));

        switch (pnmtv->hdr.idFrom)
        { case IDD_TREEVIEW:
            switch (pnmtv->hdr.code)
            { case TVN_DELETEITEM:
                { FIXME("TVN_DELETEITEM\n");
                  lptvid=(LPTV_ITEMDATA)pnmtv->itemOld.lParam;
                  IShellFolder_Release(lptvid->lpsfParent);
                  SHFree(lptvid->lpi);
                  SHFree(lptvid->lpifq);
                  SHFree(lptvid);
                }
                break;

              case TVN_ITEMEXPANDING:
                { FIXME("TVN_ITEMEXPANDING\n");
                  if ((pnmtv->itemNew.state & TVIS_EXPANDEDONCE))
                    break;

                  lptvid=(LPTV_ITEMDATA)pnmtv->itemNew.lParam;
                  if (SUCCEEDED(IShellFolder_BindToObject(lptvid->lpsfParent, lptvid->lpi,0,(REFIID)&IID_IShellFolder,(LPVOID *)&lpsf2)))
                  { FillTreeView( lpsf2, lptvid->lpifq, pnmtv->itemNew.hItem );
                  }
                  TreeView_SortChildren(hwndTreeView, pnmtv->itemNew.hItem, FALSE);
                }
                break;
              case TVN_SELCHANGED:
                lptvid=(LPTV_ITEMDATA)pnmtv->itemNew.lParam;
                pidlRet = lptvid->lpifq;
                break;

              default:
                FIXME("unhandled\n");
                break;
            }
            break;

          default:
            break;
        }

        return 0;
}


/*************************************************************************
 *             BrsFolderDlgProc32  (not an exported API function)
 */
ODINFUNCTION4(BOOL, BrsFolderDlgProc, HWND,   hWnd,
                                      UINT,   msg,
                                      WPARAM, wParam,
                                      LPARAM, lParam)
{
        switch(msg)
        { case WM_INITDIALOG:
            pidlRet = NULL;
            lpBrowseInfo = (LPBROWSEINFOA) lParam;
            if (lpBrowseInfo->lpfn)
              FIXME("Callbacks not implemented\n");
            if (lpBrowseInfo->ulFlags)
              FIXME("flag %x not implemented\n", lpBrowseInfo->ulFlags);
            if (lpBrowseInfo->lpszTitle)
              FIXME("title %s not displayed\n", lpBrowseInfo->lpszTitle);
            if ( lpBrowseInfo->pidlRoot )
              FIXME("root is desktop\n");

            InitializeTreeView ( hWnd);
            return 1;

          case WM_NOTIFY:
            MsgNotify( hWnd, (UINT)wParam, (LPNMHDR)lParam);
            break;

          case WM_COMMAND:
            switch (wParam)
            { case IDOK:
                pdump ( pidlRet );
                SHGetPathFromIDListA(pidlRet, lpBrowseInfo->pszDisplayName);
                EndDialog(hWnd, (DWORD) ILClone(pidlRet));
                return TRUE;

              case IDCANCEL:
                EndDialog(hWnd, 0);
                return TRUE;
            }
            break;
        }
        return 0;
}

/*************************************************************************
 * SHBrowseForFolderA [SHELL32.209]
 *
 */
ODINFUNCTION1(LPITEMIDLIST, SHBrowseForFolderA, LPBROWSEINFOA, lpbi)
{
        TRACE("(%lx,%s) empty stub!\n", (DWORD)lpbi, lpbi->lpszTitle);

        return (LPITEMIDLIST) DialogBoxParamA( shell32_hInstance,
                        "SHBRSFORFOLDER_MSGBOX", 0,
                        BrsFolderDlgProc, (INT)lpbi );
}
