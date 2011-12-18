/*
 * COMMDLG - File Open Dialogs Win95 look and feel
 *
 * FIXME: The whole concept of handling unicode is badly broken.
 *	many hook-messages expecting a pointer to a
 *	OPENFILENAMEA or W structure. With the current architecture
 *	we would have to convert the beast at every call to a hook.
 *	we have to find a better solution but if would likely cause
 *	a complete rewrite with after we shouldhandle the
 *	OPENFILENAME structure without any converting (jsch).
 *
 * FIXME: any hook gets a OPENFILENAMEA structure
 *
 * FIXME: CDN_FILEOK is wrong implemented, other CDN_ messages likely too
 *
 * FIXME: old style hook messages are not implemented (except FILEOKSTRING)
 *
 * FIXME: lpstrCustomFilter not handled
 *
 * FIXME: if the size of lpstrFile (nMaxFile) is too small the first
 * two bytes of lpstrFile should contain the needed size
 *
 * FIXME: algorithm for selecting the initial directory is too simple
 *
 * FIXME: add to recent docs
 *
 * FIXME: flags not implemented: OFN_CREATEPROMPT, OFN_DONTADDTORECENT,
 * OFN_ENABLEINCLUDENOTIFY, OFN_ENABLESIZING, OFN_EXTENSIONDIFFERENT,
 * OFN_NODEREFERENCELINKS, OFN_NOREADONLYRETURN,
 * OFN_NOTESTFILECREATE, OFN_OVERWRITEPROMPT, OFN_USEMONIKERS
 *
 * FIXME: lCustData for lpfnHook (WM_INITDIALOG)
 *
 *
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __WIN32OS2__
#include <windef.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <heapstring.h>
#include <misc.h>
#include <win/shlwapi.h>

#define MapHModuleSL(a) a
#define MapHModuleLS(a) a
#define snprintf  wsnprintfA
#endif

#include "winbase.h"
#include "ntddk.h"
#include "winnls.h"
#include "heap.h"
#include "commdlg.h"
#include "dlgs.h"
#include "cdlg.h"
#include "debugtools.h"
#include "cderr.h"
#include "shellapi.h"
#include "shlguid.h"
#include "filedlgbrowser.h"
#include "shlwapi.h"
#include "wine/obj_contextmenu.h"

DEFAULT_DEBUG_CHANNEL(commdlg);

#define UNIMPLEMENTED_FLAGS \
(OFN_CREATEPROMPT | OFN_DONTADDTORECENT |\
OFN_ENABLEINCLUDENOTIFY | OFN_ENABLESIZING | OFN_EXTENSIONDIFFERENT |\
OFN_NODEREFERENCELINKS | OFN_NOREADONLYRETURN |\
OFN_NOTESTFILECREATE | OFN_OVERWRITEPROMPT /*| OFN_USEMONIKERS*/)

#define IsHooked(fodInfos) \
	((fodInfos->ofnInfos->Flags & OFN_ENABLEHOOK) && fodInfos->ofnInfos->lpfnHook)
/***********************************************************************
 * Data structure and global variables
 */
typedef struct SFolder
{
  int m_iImageIndex;    /* Index of picture in image list */
  HIMAGELIST hImgList;
  int m_iIndent;      /* Indentation index */
  LPITEMIDLIST pidlItem;  /* absolute pidl of the item */

} SFOLDER,*LPSFOLDER;

typedef struct tagLookInInfo
{
  int iMaxIndentation;
  UINT uSelectedItem;
} LookInInfos;


/***********************************************************************
 * Defines and global variables
 */

/* Draw item constant */
#define ICONWIDTH 18
#define XTEXTOFFSET 3

/* AddItem flags*/
#define LISTEND -1

/* SearchItem methods */
#define SEARCH_PIDL 1
#define SEARCH_EXP  2
#define ITEM_NOTFOUND -1

/* Undefined windows message sent by CreateViewObject*/
#define WM_GETISHELLBROWSER  WM_USER+7

/* NOTE
 * Those macros exist in windowsx.h. However, you can't really use them since
 * they rely on the UNICODE defines and can't be used inside Wine itself.
 */

/* Combo box macros */
#define CBAddString(hwnd,str) \
  SendMessageA(hwnd,CB_ADDSTRING,0,(LPARAM)str);

#define CBInsertString(hwnd,str,pos) \
  SendMessageA(hwnd,CB_INSERTSTRING,(WPARAM)pos,(LPARAM)str);

#define CBDeleteString(hwnd,pos) \
  SendMessageA(hwnd,CB_DELETESTRING,(WPARAM)pos,0);

#define CBSetItemDataPtr(hwnd,iItemId,dataPtr) \
  SendMessageA(hwnd,CB_SETITEMDATA,(WPARAM)iItemId,(LPARAM)dataPtr);

#define CBGetItemDataPtr(hwnd,iItemId) \
  SendMessageA(hwnd,CB_GETITEMDATA,(WPARAM)iItemId,0)

#define CBGetLBText(hwnd,iItemId,str) \
  SendMessageA(hwnd,CB_GETLBTEXT,(WPARAM)iItemId,(LPARAM)str);

#define CBGetCurSel(hwnd) \
  SendMessageA(hwnd,CB_GETCURSEL,0,0);

#define CBSetCurSel(hwnd,pos) \
  SendMessageA(hwnd,CB_SETCURSEL,(WPARAM)pos,0);

#define CBGetCount(hwnd) \
    SendMessageA(hwnd,CB_GETCOUNT,0,0);
#define CBShowDropDown(hwnd,show) \
  SendMessageA(hwnd,CB_SHOWDROPDOWN,(WPARAM)show,0);
#define CBSetItemHeight(hwnd,index,height) \
  SendMessageA(hwnd,CB_SETITEMHEIGHT,(WPARAM)index,(LPARAM)height);


const char *FileOpenDlgInfosStr = "FileOpenDlgInfos"; /* windows property description string */
const char *LookInInfosStr = "LookInInfos"; /* LOOKIN combo box property */

static const char defaultFilter[] = "*.*";

/***********************************************************************
 * Prototypes
 */

/* Internal functions used by the dialog */
static LRESULT FILEDLG95_FillControls(HWND hwnd, WPARAM wParam, LPARAM lParam);
static LRESULT FILEDLG95_OnWMCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
static LRESULT FILEDLG95_OnWMGetIShellBrowser(HWND hwnd);
       BOOL    FILEDLG95_OnOpen(HWND hwnd);
static LRESULT FILEDLG95_InitControls(HWND hwnd);
static void    FILEDLG95_Clean(HWND hwnd);

/* Functions used by the shell navigation */
static LRESULT FILEDLG95_SHELL_Init(HWND hwnd);
static BOOL    FILEDLG95_SHELL_UpFolder(HWND hwnd);
static BOOL    FILEDLG95_SHELL_ExecuteCommand(HWND hwnd, LPCSTR lpVerb);
static void    FILEDLG95_SHELL_Clean(HWND hwnd);
static BOOL    FILEDLG95_SHELL_BrowseToDesktop(HWND hwnd);

/* Functions used by the filetype combo box */
static HRESULT FILEDLG95_FILETYPE_Init(HWND hwnd);
static BOOL    FILEDLG95_FILETYPE_OnCommand(HWND hwnd, WORD wNotifyCode);
static int     FILEDLG95_FILETYPE_SearchExt(HWND hwnd,LPCSTR lpstrExt);
static void    FILEDLG95_FILETYPE_Clean(HWND hwnd);

/* Functions used by the Look In combo box */
static HRESULT FILEDLG95_LOOKIN_Init(HWND hwndCombo);
static LRESULT FILEDLG95_LOOKIN_DrawItem(LPDRAWITEMSTRUCT pDIStruct);
static BOOL    FILEDLG95_LOOKIN_OnCommand(HWND hwnd, WORD wNotifyCode);
static int     FILEDLG95_LOOKIN_AddItem(HWND hwnd,LPITEMIDLIST pidl, int iInsertId);
static int     FILEDLG95_LOOKIN_SearchItem(HWND hwnd,WPARAM searchArg,int iSearchMethod);
static int     FILEDLG95_LOOKIN_InsertItemAfterParent(HWND hwnd,LPITEMIDLIST pidl);
static int     FILEDLG95_LOOKIN_RemoveMostExpandedItem(HWND hwnd);
       int     FILEDLG95_LOOKIN_SelectItem(HWND hwnd,LPITEMIDLIST pidl);
static void    FILEDLG95_LOOKIN_Clean(HWND hwnd);

/* Miscellaneous tool functions */
HRESULT       GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST pidl,DWORD dwFlags,LPSTR lpstrFileName);
HRESULT       GetFileName(HWND hwnd, LPITEMIDLIST pidl, LPSTR lpstrFileName);
IShellFolder* GetShellFolderFromPidl(LPITEMIDLIST pidlAbs);
LPITEMIDLIST  GetParentPidl(LPITEMIDLIST pidl);
LPITEMIDLIST  GetPidlFromName(IShellFolder *psf,LPCSTR lpcstrFileName);

/* Shell memory allocation */
static void *MemAlloc(UINT size);
static void MemFree(void *mem);

BOOL WINAPI GetFileName95(FileOpenDlgInfos *fodInfos);
HRESULT WINAPI FileOpenDlgProc95(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT SendCustomDlgNotificationMessage(HWND hwndParentDlg, UINT uCode);
HRESULT FILEDLG95_HandleCustomDialogMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FILEDLG95_OnOpenMultipleFiles(HWND hwnd, LPSTR lpstrFileList, UINT nFileCount, UINT sizeUsed);
static BOOL BrowseSelectedFolder(HWND hwnd);

/***********************************************************************
 *      GetFileName95
 *
 * Creates an Open common dialog box that lets the user select
 * the drive, directory, and the name of a file or set of files to open.
 *
 * IN  : The FileOpenDlgInfos structure associated with the dialog
 * OUT : TRUE on success
 *       FALSE on cancel, error, close or filename-does-not-fit-in-buffer.
 */
BOOL WINAPI GetFileName95(FileOpenDlgInfos *fodInfos)
{

    LRESULT lRes;
    LPCVOID template;
    HRSRC hRes;
    HANDLE hDlgTmpl = 0;

    /* test for missing functionality */
    if (fodInfos->ofnInfos->Flags & UNIMPLEMENTED_FLAGS)
    {
      FIXME("Flags 0x%08lx not yet implemented\n",
         fodInfos->ofnInfos->Flags & UNIMPLEMENTED_FLAGS);
    }

    /* Create the dialog from a template */

    if(!(hRes = FindResourceA(COMMDLG_hInstance32,MAKEINTRESOURCEA(NEWFILEOPENORD),RT_DIALOGA)))
    {
        COMDLG32_SetCommDlgExtendedError(CDERR_FINDRESFAILURE);
        return FALSE;
    }
    if (!(hDlgTmpl = LoadResource(COMMDLG_hInstance32, hRes )) ||
        !(template = LockResource( hDlgTmpl )))
    {
        COMDLG32_SetCommDlgExtendedError(CDERR_LOADRESFAILURE);
        return FALSE;
    }

    /* old style hook messages */
    if (IsHooked(fodInfos))
    {
      fodInfos->HookMsg.fileokstring = RegisterWindowMessageA(FILEOKSTRINGA);
      fodInfos->HookMsg.lbselchstring = RegisterWindowMessageA(LBSELCHSTRINGA);
      fodInfos->HookMsg.helpmsgstring = RegisterWindowMessageA(HELPMSGSTRINGA);
      fodInfos->HookMsg.sharevistring = RegisterWindowMessageA(SHAREVISTRINGA);
    }

    lRes = DialogBoxIndirectParamA(COMMDLG_hInstance32,
                                  (LPDLGTEMPLATEA) template,
                                  fodInfos->ofnInfos->hwndOwner,
                                  (DLGPROC) FileOpenDlgProc95,
                                  (LPARAM) fodInfos);

    /* Unable to create the dialog */
    if( lRes == -1)
        return FALSE;

    return lRes;
}

/***********************************************************************
 *      GetFileDialog95A
 *
 * Call GetFileName95 with this structure and clean the memory.
 *
 * IN  : The OPENFILENAMEA initialisation structure passed to
 *       GetOpenFileNameA win api function (see filedlg.c)
 */
BOOL  WINAPI GetFileDialog95A(LPOPENFILENAMEA ofn,UINT iDlgType)
{

  BOOL ret;
  FileOpenDlgInfos *fodInfos;
  HINSTANCE hInstance;
  LPCSTR lpstrInitialDir = (LPCSTR)-1;
  LPSTR lpstrSavDir = NULL;
  DWORD dwFlags = 0;

  /* Initialise FileOpenDlgInfos structure*/
  fodInfos = (FileOpenDlgInfos*)MemAlloc(sizeof(FileOpenDlgInfos));
  ZeroMemory(fodInfos, sizeof(FileOpenDlgInfos));

  /* Pass in the original ofn */
  fodInfos->ofnInfos = ofn;

  /* Save original hInstance value */
  hInstance = ofn->hInstance;
  fodInfos->ofnInfos->hInstance = MapHModuleLS(ofn->hInstance);

  /* save current directory */
  if (ofn->Flags & OFN_NOCHANGEDIR)
  {
     lpstrSavDir = MemAlloc(MAX_PATH);
     GetCurrentDirectoryA(MAX_PATH, lpstrSavDir);
  }

  dwFlags = ofn->Flags;
  ofn->Flags = ofn->Flags|OFN_WINE;

  /* Initialise the dialog property */
  fodInfos->DlgInfos.dwDlgProp = 0;
  fodInfos->DlgInfos.hwndCustomDlg = (HWND)NULL;

  switch(iDlgType)
  {
    case OPEN_DIALOG :
      ret = GetFileName95(fodInfos);
      break;
    case SAVE_DIALOG :
      fodInfos->DlgInfos.dwDlgProp |= FODPROP_SAVEDLG;
#ifdef __WIN32OS2__
      //NT4 seems to ignore this flag (fixes ElstarFormular save dialog)
      ofn->Flags &= ~OFN_FILEMUSTEXIST;
#endif
      ret = GetFileName95(fodInfos);
      break;
    default :
      ret = 0;
  }

  if (lpstrSavDir)
  {
      SetCurrentDirectoryA(lpstrSavDir);
      MemFree(lpstrSavDir);
  }

  if (lpstrInitialDir != (LPCSTR)-1)
  {
    MemFree((LPVOID)(ofn->lpstrInitialDir));
    ofn->lpstrInitialDir = lpstrInitialDir;
  }

  ofn->Flags = dwFlags;
  ofn->hInstance = hInstance;
  MemFree((LPVOID)(fodInfos));
  return ret;
}

/***********************************************************************
 *      GetFileDialog95W
 *
 * Copy the OPENFILENAMEW structure in a FileOpenDlgInfos structure.
 * Call GetFileName95 with this structure and clean the memory.
 *
 * FIXME: lpstrCustomFilter has to converted back
 *
 */

/* converting IN arguments */
#define AllocInArgWtoA(arg, save) \
  if(arg) \
  { \
    DWORD _len = WideCharToMultiByte( CP_ACP, 0, arg, -1, NULL, 0, NULL, NULL ); \
    save = arg; \
    arg =  MemAlloc(_len); \
    WideCharToMultiByte( CP_ACP, 0, save, -1, (LPSTR)arg, _len, NULL, NULL ); \
  }

#define FreeInArg(arg, save) \
  if(arg) \
  { \
    MemFree((LPSTR)arg); \
    arg = save; \
  }

/* converting OUT arguments */
#define AllocOutArgWtoA(arg, save, len) \
  if(arg) \
  { \
    save = arg; \
    arg = MemAlloc(len); \
  }

#define FreeOutArg(arg, save, len) \
  if(arg) \
  { \
    MultiByteToWideChar( CP_ACP, 0, (LPCSTR)(arg), -1, (save), (len) ); \
    MemFree(arg); \
    arg = save; \
  }

BOOL  WINAPI GetFileDialog95W(LPOPENFILENAMEW ofn,UINT iDlgType)
{
  BOOL ret;
  FileOpenDlgInfos *fodInfos;
  HINSTANCE hInstance;

  /* out arguments */
  LPWSTR lpstrFile = NULL;
  LPWSTR lpstrFileTitle = NULL;
#ifdef __WIN32OS2__
  LPSTR lpstrSavDir = NULL;
#endif

  /* in/out arguments */
  LPWSTR lpstrCustomFilter = NULL;

  /* input arguments */
  LPCWSTR lpstrFilter = NULL;
  LPCWSTR lpstrInitialDir = NULL;
  LPCWSTR lpstrTitle = NULL;
  LPCWSTR lpstrDefExt = NULL;
  LPCWSTR lpTemplateName = NULL;
  DWORD dwFlags;

  /* Initialise FileOpenDlgInfos structure*/
  fodInfos = (FileOpenDlgInfos*)MemAlloc(sizeof(FileOpenDlgInfos));
  ZeroMemory(fodInfos, sizeof(FileOpenDlgInfos));

  /*  Pass in the original ofn */
  fodInfos->ofnInfos = (LPOPENFILENAMEA) ofn;

  /* convert lpstrFilter */
  if (ofn->lpstrFilter)
  {
    LPCWSTR  s;
    LPSTR y;
    int n, len;

    lpstrFilter = ofn->lpstrFilter;

    /* filter is a list...  title\0ext\0......\0\0 */
    s = ofn->lpstrFilter;

    while (*s) s = s+strlenW(s)+1;
    s++;
    n = s - ofn->lpstrFilter; /* already divides by 2. ptr magic */
    len = WideCharToMultiByte( CP_ACP, 0, ofn->lpstrFilter, n, NULL, 0, NULL, NULL );
    y = (LPSTR)MemAlloc(len);
    WideCharToMultiByte( CP_ACP, 0, ofn->lpstrFilter, n, y, len, NULL, NULL );
#ifdef __WIN32OS2__
    ofn->lpstrFilter = (LPWSTR)y;
#else
    (LPSTR)ofn->lpstrFilter = y;
#endif
  }

  /* convert lpstrCustomFilter */
  if (ofn->lpstrCustomFilter)
  {
    LPWSTR  s;
    LPSTR y;
    int n, len;

    lpstrCustomFilter = ofn->lpstrCustomFilter;
    /* filter is a list...  title\0ext\0......\0\0 */
    s = ofn->lpstrCustomFilter;
    while (*s) s = s+strlenW(s)+1;
    s++;
    n = s - ofn->lpstrCustomFilter;
    len = WideCharToMultiByte( CP_ACP, 0, ofn->lpstrCustomFilter, n, NULL, 0, NULL, NULL );
    y = (LPSTR)MemAlloc(len);
    WideCharToMultiByte( CP_ACP, 0, ofn->lpstrCustomFilter, n, y, len, NULL, NULL );
#ifdef __WIN32OS2__
    ofn->lpstrCustomFilter = (LPWSTR)y;
#else
    (LPSTR)ofn->lpstrCustomFilter = y;
#endif
  }

  /* convert string arguments, save others */
  AllocOutArgWtoA(ofn->lpstrFile, lpstrFile, ofn->nMaxFile);
  AllocOutArgWtoA(ofn->lpstrFileTitle, lpstrFileTitle, ofn->nMaxFileTitle);
  AllocInArgWtoA(ofn->lpstrInitialDir, lpstrInitialDir);
  AllocInArgWtoA(ofn->lpstrTitle, lpstrTitle);
  AllocInArgWtoA(ofn->lpstrDefExt, lpstrDefExt);
#ifdef __WIN32OS2__
  if(HIWORD(ofn->lpTemplateName)) {
#endif
       AllocInArgWtoA(ofn->lpTemplateName, lpTemplateName);
#ifdef __WIN32OS2__
  }
  else lpTemplateName = ofn->lpTemplateName;
#endif

  dwFlags = ofn->Flags;
  hInstance = ofn->hInstance;

#ifdef __WIN32OS2__
  /* save current directory */
  if (ofn->Flags & OFN_NOCHANGEDIR)
  {
     lpstrSavDir = MemAlloc(MAX_PATH);
     GetCurrentDirectoryA(MAX_PATH, lpstrSavDir);
  }
#endif

  ofn->Flags = ofn->Flags|OFN_WINE|OFN_UNICODE;
  ofn->hInstance = MapHModuleLS(ofn->hInstance);

  switch(iDlgType)
  {
  case OPEN_DIALOG :
      ret = GetFileName95(fodInfos);
      break;
  case SAVE_DIALOG :
      fodInfos->DlgInfos.dwDlgProp |= FODPROP_SAVEDLG;
#ifdef __WIN32OS2__
      //NT4 seems to ignore this flag (fixes ElstarFormular save dialog)
      ofn->Flags &= ~OFN_FILEMUSTEXIST;
#endif
      ret = GetFileName95(fodInfos);
      break;
  default :
      ret = 0;
  }

#ifdef __WIN32OS2__
  if (lpstrSavDir)
  {
      SetCurrentDirectoryA(lpstrSavDir);
      MemFree(lpstrSavDir);
  }
#endif

  /* restore saved IN arguments and convert OUT arguments back */
  ofn->Flags = dwFlags;
  ofn->hInstance = hInstance;
  FreeInArg(ofn->lpstrFilter, lpstrFilter);
  FreeInArg(ofn->lpstrCustomFilter, lpstrCustomFilter);
  FreeOutArg(ofn->lpstrFile, lpstrFile, ofn->nMaxFile);
  FreeOutArg(ofn->lpstrFileTitle, lpstrFileTitle, ofn->nMaxFileTitle);
  FreeInArg(ofn->lpstrInitialDir, lpstrInitialDir);
  FreeInArg(ofn->lpstrTitle, lpstrTitle);
  FreeInArg(ofn->lpstrDefExt, lpstrDefExt);
#ifdef __WIN32OS2__
  if(HIWORD(lpTemplateName)) {
       FreeInArg(ofn->lpTemplateName, lpTemplateName);
  }
  else ofn->lpTemplateName = lpTemplateName;
#else
  FreeInArg(ofn->lpTemplateName, lpTemplateName);
#endif

  MemFree((LPVOID)(fodInfos));
  return ret;
}

void ArrangeCtrlPositions( HWND hwndChildDlg, HWND hwndParentDlg)
{
    HWND hwndChild,hwndStc32;
    RECT rectParent, rectChild, rectCtrl, rectStc32, rectTemp;
    POINT ptMoveCtl;
    POINT ptParentClient;

    TRACE("\n");

    ptMoveCtl.x = ptMoveCtl.y = 0;
    hwndStc32=GetDlgItem(hwndChildDlg,stc32);
    GetClientRect(hwndParentDlg,&rectParent);
    GetClientRect(hwndChildDlg,&rectChild);

    if(hwndStc32)
    {
      GetWindowRect(hwndStc32,&rectStc32);
      MapWindowPoints(0, hwndChildDlg,(LPPOINT)&rectStc32,2);
      CopyRect(&rectTemp,&rectStc32);

      SetRect(&rectStc32,rectStc32.left,rectStc32.top,rectStc32.left + (rectParent.right-rectParent.left),rectStc32.top+(rectParent.bottom-rectParent.top));
      SetWindowPos(hwndStc32,0,rectStc32.left,rectStc32.top,rectStc32.right-rectStc32.left,rectStc32.bottom-rectStc32.top,SWP_NOMOVE|SWP_NOZORDER | SWP_NOACTIVATE);

      if(rectStc32.right < rectTemp.right)
      {
        ptParentClient.x = max((rectParent.right-rectParent.left),(rectChild.right-rectChild.left));
        ptMoveCtl.x = 0;
      }
      else
      {
        ptMoveCtl.x = (rectStc32.right - rectTemp.right);
        ptParentClient.x = max((rectParent.right-rectParent.left),((rectChild.right-rectChild.left)+rectStc32.right-rectTemp.right));
      }

      if(rectStc32.bottom < rectTemp.bottom)
      {
        ptParentClient.y = max((rectParent.bottom-rectParent.top),(rectChild.bottom-rectChild.top));
        ptMoveCtl.y = 0;
      }
      else
      {
        ptMoveCtl.y = (rectStc32.bottom - rectTemp.bottom);
        ptParentClient.y = max((rectParent.bottom-rectParent.top),((rectChild.bottom-rectChild.top)+rectStc32.bottom-rectTemp.bottom));
      }
    }
    else
    {
      if( (GetWindow(hwndChildDlg,GW_CHILD)) == (HWND) NULL) return;

      SetRectEmpty(&rectTemp);
      ptParentClient.x = max((rectParent.right-rectParent.left),(rectChild.right-rectChild.left));
      ptParentClient.y = (rectParent.bottom-rectParent.top) + (rectChild.bottom-rectChild.top);
      ptMoveCtl.y = rectParent.bottom-rectParent.top;
      ptMoveCtl.x=0;
    }
    SetRect(&rectParent,rectParent.left,rectParent.top,rectParent.left+ptParentClient.x,rectParent.top+ptParentClient.y);
    AdjustWindowRectEx( &rectParent,GetWindowLongA(hwndParentDlg,GWL_STYLE),FALSE,GetWindowLongA(hwndParentDlg,GWL_EXSTYLE));

    SetWindowPos(hwndChildDlg, 0, 0,0, ptParentClient.x,ptParentClient.y, SWP_NOZORDER );
    SetWindowPos(hwndParentDlg, 0, rectParent.left,rectParent.top, (rectParent.right- rectParent.left),
        (rectParent.bottom-rectParent.top),SWP_NOMOVE | SWP_NOZORDER);

    hwndChild = GetWindow(hwndChildDlg,GW_CHILD);
    if(hwndStc32)
    {
      GetWindowRect(hwndStc32,&rectStc32);
      MapWindowPoints( 0, hwndChildDlg,(LPPOINT)&rectStc32,2);
    }
    else
      SetRect(&rectStc32,0,0,0,0);

    if (hwndChild )
    {
      do
      {
        if(hwndChild != hwndStc32)
        {
          if (GetWindowLongA( hwndChild, GWL_STYLE ) & WS_MAXIMIZE)
				continue;
          GetWindowRect(hwndChild,&rectCtrl);
          MapWindowPoints( 0, hwndParentDlg,(LPPOINT)&rectCtrl,2);

          /*
            Check the initial position of the controls relative to the initial
            position and size of stc32 (before it is expanded).
          */
          if (rectCtrl.left >= rectTemp.right && rectCtrl.top >= rectTemp.bottom)
          {
            rectCtrl.left += ptMoveCtl.x;
            rectCtrl.top  += ptMoveCtl.y;
          }
          else if (rectCtrl.left >= rectTemp.right)
	  {
            rectCtrl.left += ptMoveCtl.x;
	  }
          else if (rectCtrl.top >= rectTemp.bottom)
          {
	    rectCtrl.top  += ptMoveCtl.y;
	  }

          SetWindowPos( hwndChild, 0, rectCtrl.left, rectCtrl.top,
				rectCtrl.right-rectCtrl.left,rectCtrl.bottom-rectCtrl.top,
				SWP_NOSIZE | SWP_NOZORDER );
        }
      } while ((hwndChild=GetWindow( hwndChild, GW_HWNDNEXT )) != (HWND)NULL);
    }
    hwndChild = GetWindow(hwndParentDlg,GW_CHILD);

    if(hwndStc32)
    {
      GetWindowRect(hwndStc32,&rectStc32);
      MapWindowPoints( 0, hwndChildDlg,(LPPOINT)&rectStc32,2);
      ptMoveCtl.x = rectStc32.left - 0;
      ptMoveCtl.y = rectStc32.top - 0;
      if (hwndChild )
      {
        do
        {
          if(hwndChild != hwndChildDlg)
          {
            if (GetWindowLongA( hwndChild, GWL_STYLE ) & WS_MAXIMIZE)
              continue;
            GetWindowRect(hwndChild,&rectCtrl);
            MapWindowPoints( 0, hwndParentDlg,(LPPOINT)&rectCtrl,2);

            rectCtrl.left += ptMoveCtl.x;
            rectCtrl.top += ptMoveCtl.y;

            SetWindowPos( hwndChild, 0, rectCtrl.left, rectCtrl.top,
                rectCtrl.right-rectCtrl.left,rectCtrl.bottom-rectCtrl.top,
                SWP_NOSIZE |SWP_NOZORDER );
          }
        } while ((hwndChild=GetWindow( hwndChild, GW_HWNDNEXT )) != (HWND)NULL);
      }
    }
}


HRESULT WINAPI FileOpenDlgProcUserTemplate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(GetParent(hwnd),FileOpenDlgInfosStr);

#if 0
    TRACE("0x%04x\n", uMsg);
#endif

    switch(uMsg)
    {
      case WM_INITDIALOG:
      {
        fodInfos = (FileOpenDlgInfos *)lParam;
        lParam = (LPARAM) fodInfos->ofnInfos;
        ArrangeCtrlPositions(hwnd,GetParent(hwnd));

        if(fodInfos && IsHooked(fodInfos))
          return CallWindowProcA((WNDPROC)fodInfos->ofnInfos->lpfnHook,hwnd,uMsg,wParam,lParam);
        return 0;
      }
    }

    if(fodInfos && IsHooked(fodInfos))
      return CallWindowProcA((WNDPROC)fodInfos->ofnInfos->lpfnHook,hwnd,uMsg,wParam,lParam);

    return DefWindowProcA(hwnd,uMsg,wParam,lParam);
}

HWND CreateTemplateDialog(FileOpenDlgInfos *fodInfos, HWND hwnd)
{
    LPCVOID template;
    HRSRC hRes;
    HANDLE hDlgTmpl = 0;
    HWND hChildDlg = 0;

    TRACE("\n");

    if (fodInfos->ofnInfos->Flags & OFN_ENABLETEMPLATE ||
        fodInfos->ofnInfos->Flags & OFN_ENABLETEMPLATEHANDLE)
    {
      if (fodInfos->ofnInfos->Flags  & OFN_ENABLETEMPLATEHANDLE)
      {
        if( !(template = LockResource( fodInfos->ofnInfos->hInstance)))
        {
          COMDLG32_SetCommDlgExtendedError(CDERR_LOADRESFAILURE);
          return (HWND)NULL;
        }
      }
      else
      {
        if (!(hRes = FindResourceA(MapHModuleSL(fodInfos->ofnInfos->hInstance),
             (fodInfos->ofnInfos->lpTemplateName), RT_DIALOGA)))
        {
          COMDLG32_SetCommDlgExtendedError(CDERR_FINDRESFAILURE);
          return (HWND)NULL;
        }
        if (!(hDlgTmpl = LoadResource( MapHModuleSL(fodInfos->ofnInfos->hInstance),
             hRes )) || !(template = LockResource( hDlgTmpl )))
        {
          COMDLG32_SetCommDlgExtendedError(CDERR_LOADRESFAILURE);
          return (HWND)NULL;
    	}
      }

      hChildDlg= CreateDialogIndirectParamA(fodInfos->ofnInfos->hInstance,template,hwnd,(DLGPROC)FileOpenDlgProcUserTemplate,(LPARAM)fodInfos);
      if(hChildDlg)
      {
        ShowWindow(hChildDlg,SW_SHOW);
        return hChildDlg;
      }
    }
    else if( IsHooked(fodInfos))
    {
      RECT rectHwnd;
      DLGTEMPLATE tmplate;
      GetClientRect(hwnd,&rectHwnd);
      tmplate.style = WS_CHILD | WS_CLIPSIBLINGS;
      tmplate.dwExtendedStyle = 0;
      tmplate.cdit = 0;
      tmplate.x = 0;
      tmplate.y = 0;
      tmplate.cx = rectHwnd.right-rectHwnd.left;
      tmplate.cy = rectHwnd.bottom-rectHwnd.top;

      return CreateDialogIndirectParamA(fodInfos->ofnInfos->hInstance,&tmplate,hwnd,(DLGPROC)FileOpenDlgProcUserTemplate,(LPARAM)fodInfos);
    }
    return (HWND)NULL;
}

/***********************************************************************
*          SendCustomDlgNotificationMessage
*
* Send CustomDialogNotification (CDN_FIRST -- CDN_LAST) message to the custom template dialog
*/

HRESULT SendCustomDlgNotificationMessage(HWND hwndParentDlg, UINT uCode)
{
    FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwndParentDlg,FileOpenDlgInfosStr);

    TRACE("0x%04x 0x%04x\n",hwndParentDlg, uCode);

    if(!fodInfos) return 0;

    if(fodInfos->ofnInfos->Flags & OFN_UNICODE)
      FIXME("sending OPENFILENAMEA structure. Hook is expecting OPENFILENAMEW!\n");

    if(fodInfos->DlgInfos.hwndCustomDlg)
    {
        OFNOTIFYA ofnNotify;
	HRESULT ret;
        ofnNotify.hdr.hwndFrom=hwndParentDlg;
        ofnNotify.hdr.idFrom=0;
        ofnNotify.hdr.code = uCode;
        ofnNotify.lpOFN = fodInfos->ofnInfos;
	TRACE("CALL NOTIFY for %x\n", uCode);
	ret = SendMessageA(fodInfos->DlgInfos.hwndCustomDlg,WM_NOTIFY,0,(LPARAM)&ofnNotify);
	TRACE("RET NOTIFY\n");
	return ret;
    }
    return TRUE;
}

/***********************************************************************
*         FILEDLG95_HandleCustomDialogMessages
*
* Handle Custom Dialog Messages (CDM_FIRST -- CDM_LAST) messages
*/
HRESULT FILEDLG95_HandleCustomDialogMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LPSTR lpstrFileSpec;
    int reqSize;
    char lpstrPath[MAX_PATH];
    FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);
    if(!fodInfos) return -1;

    switch(uMsg)
    {
        case CDM_GETFILEPATH:
            GetDlgItemTextA(hwnd,IDC_FILENAME,lpstrPath, sizeof(lpstrPath));
            lpstrFileSpec = (LPSTR)PathFindFileNameA(lpstrPath);
            if (lpstrFileSpec==lpstrPath)
	    {
                char lpstrCurrentDir[MAX_PATH];
                /* Prepend the current path */
                SHGetPathFromIDListA(fodInfos->ShellInfos.pidlAbsCurrent,lpstrCurrentDir);
                if ((LPSTR)lParam!=NULL)
                    snprintf((LPSTR)lParam,(int)wParam,"%s\\%s",lpstrCurrentDir,lpstrPath);
                reqSize=strlen(lpstrCurrentDir)+1+strlen(lpstrPath)+1;
            }
	    else
	    {
                lstrcpynA((LPSTR)lParam,(LPSTR)lpstrPath,(int)wParam);
                reqSize=strlen(lpstrPath);
            }
            /* return the required buffer size */
            return reqSize;

        case CDM_GETFOLDERPATH:
	    SHGetPathFromIDListA(fodInfos->ShellInfos.pidlAbsCurrent,lpstrPath);
            if ((LPSTR)lParam!=NULL)
                lstrcpynA((LPSTR)lParam,lpstrPath,(int)wParam);
            return strlen(lpstrPath);

        case CDM_GETSPEC:
	    reqSize=GetDlgItemTextA(hwnd,IDC_FILENAME,lpstrPath, sizeof(lpstrPath));
            lpstrFileSpec = (LPSTR)PathFindFileNameA(lpstrPath);
            if ((LPSTR)lParam!=NULL)
                lstrcpynA((LPSTR)lParam, lpstrFileSpec, (int)wParam);
            return strlen(lpstrFileSpec);

        case CDM_SETCONTROLTEXT:
	    if ( 0 != lParam )
	        SetDlgItemTextA( hwnd, (UINT) wParam, (LPSTR) lParam );
	    return TRUE;

        case CDM_HIDECONTROL:
        case CDM_SETDEFEXT:
            FIXME("CDM_HIDECONTROL,CDM_SETCONTROLTEXT,CDM_SETDEFEXT not implemented\n");
            return -1;
    }
    return TRUE;
}

/***********************************************************************
 *          FileOpenDlgProc95
 *
 * File open dialog procedure
 */
HRESULT WINAPI FileOpenDlgProc95(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#if 0
  TRACE("0x%04x 0x%04x\n", hwnd, uMsg);
#endif

  switch(uMsg)
  {
    case WM_INITDIALOG:
      {
         FileOpenDlgInfos * fodInfos = (FileOpenDlgInfos *)lParam;

	 /* Adds the FileOpenDlgInfos in the property list of the dialog
            so it will be easily accessible through a GetPropA(...) */
      	 SetPropA(hwnd, FileOpenDlgInfosStr, (HANDLE) fodInfos);

      	 fodInfos->DlgInfos.hwndCustomDlg =
     	   CreateTemplateDialog((FileOpenDlgInfos *)lParam, hwnd);

         FILEDLG95_InitControls(hwnd);
         SendCustomDlgNotificationMessage(hwnd,CDN_INITDONE);
      	 FILEDLG95_FillControls(hwnd, wParam, lParam);
         SendCustomDlgNotificationMessage(hwnd,CDN_SELCHANGE);
         SetWindowPos(fodInfos->DlgInfos.hwndCustomDlg, HWND_BOTTOM,
                      0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
         return 0;
       }
    case WM_COMMAND:
      return FILEDLG95_OnWMCommand(hwnd, wParam, lParam);
    case WM_DRAWITEM:
      {
        switch(((LPDRAWITEMSTRUCT)lParam)->CtlID)
        {
        case IDC_LOOKIN:
          FILEDLG95_LOOKIN_DrawItem((LPDRAWITEMSTRUCT) lParam);
          return TRUE;
        }
      }
      return FALSE;

    case WM_GETISHELLBROWSER:
      return FILEDLG95_OnWMGetIShellBrowser(hwnd);

    case WM_DESTROY:
      RemovePropA(hwnd, FileOpenDlgInfosStr);
      return FALSE;

    case WM_NOTIFY:
    {
	LPNMHDR lpnmh = (LPNMHDR)lParam;
	UINT stringId = -1;

	/* set up the button tooltips strings */
	if(TTN_GETDISPINFOA == lpnmh->code )
	{
	    LPNMTTDISPINFOA lpdi = (LPNMTTDISPINFOA)lParam;
	    switch(lpnmh->idFrom )
	    {
		/* Up folder button */
		case FCIDM_TB_UPFOLDER:
		    stringId = IDS_UPFOLDER;
		    break;
		/* New folder button */
		case FCIDM_TB_NEWFOLDER:
		    stringId = IDS_NEWFOLDER;
		    break;
		/* List option button */
		case FCIDM_TB_SMALLICON:
		    stringId = IDS_LISTVIEW;
		    break;
		/* Details option button */
		case FCIDM_TB_REPORTVIEW:
		    stringId = IDS_REPORTVIEW;
		    break;
		/* Desktop button */
		case FCIDM_TB_DESKTOP:
		    stringId = IDS_TODESKTOP;
		    break;
		default:
		    stringId = 0;
	    }
	    lpdi->hinst = COMMDLG_hInstance32;
	    lpdi->lpszText =  (LPSTR) stringId;
	}
        return FALSE;
    }
    default :
      if(uMsg >= CDM_FIRST && uMsg <= CDM_LAST)
        return FILEDLG95_HandleCustomDialogMessages(hwnd, uMsg, wParam, lParam);
      return FALSE;
  }
}

/***********************************************************************
 *      FILEDLG95_InitControls
 *
 * WM_INITDIALOG message handler (before hook notification)
 */
static LRESULT FILEDLG95_InitControls(HWND hwnd)
{
  TBBUTTON tbb[] =
  {
   {0,                 0,                   TBSTATE_ENABLED, TBSTYLE_SEP, {0, 0}, 0, 0 },
   {VIEW_PARENTFOLDER, FCIDM_TB_UPFOLDER,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0 },
   {0,                 0,                   TBSTATE_ENABLED, TBSTYLE_SEP, {0, 0}, 0, 0 },
   {VIEW_NEWFOLDER+1,  FCIDM_TB_DESKTOP,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0 },
   {0,                 0,                   TBSTATE_ENABLED, TBSTYLE_SEP, {0, 0}, 0, 0 },
   {VIEW_NEWFOLDER,    FCIDM_TB_NEWFOLDER,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0 },
   {0,                 0,                   TBSTATE_ENABLED, TBSTYLE_SEP, {0, 0}, 0, 0 },
   {VIEW_LIST,         FCIDM_TB_SMALLICON,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0 },
   {VIEW_DETAILS,      FCIDM_TB_REPORTVIEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0 },
  };
  TBADDBITMAP tba[] =
  {
   { HINST_COMMCTRL, IDB_VIEW_SMALL_COLOR },
   { COMDLG32_hInstance, 800 } 			// desktop icon
  };

  RECT rectTB;

  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("%p\n", fodInfos);

  /* Get the hwnd of the controls */
  fodInfos->DlgInfos.hwndFileName = GetDlgItem(hwnd,IDC_FILENAME);
  fodInfos->DlgInfos.hwndFileTypeCB = GetDlgItem(hwnd,IDC_FILETYPE);
  fodInfos->DlgInfos.hwndLookInCB = GetDlgItem(hwnd,IDC_LOOKIN);

  /* construct the toolbar */
  GetWindowRect(GetDlgItem(hwnd,IDC_TOOLBARSTATIC),&rectTB);
  MapWindowPoints( 0, hwnd,(LPPOINT)&rectTB,2);

  fodInfos->DlgInfos.hwndTB = CreateWindowExA(0, TOOLBARCLASSNAMEA, (LPSTR) NULL,
        WS_CHILD | WS_GROUP | TBSTYLE_TOOLTIPS | CCS_NODIVIDER | CCS_NORESIZE,
        0, 0, 150, 26, hwnd, (HMENU) IDC_TOOLBAR, COMMDLG_hInstance32, NULL);

  SetWindowPos(fodInfos->DlgInfos.hwndTB, 0,
  	rectTB.left,rectTB.top, rectTB.right-rectTB.left, rectTB.bottom-rectTB.top,
	SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOZORDER );

  SendMessageA(fodInfos->DlgInfos.hwndTB, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);

/* fixme: use TB_LOADIMAGES when implemented */
/*  SendMessageA(fodInfos->DlgInfos.hwndTB, TB_LOADIMAGES, (WPARAM) IDB_VIEW_SMALL_COLOR, HINST_COMMCTRL);*/
  SendMessageA(fodInfos->DlgInfos.hwndTB, TB_ADDBITMAP, (WPARAM) 12, (LPARAM) &tba[0]);
  SendMessageA(fodInfos->DlgInfos.hwndTB, TB_ADDBITMAP, (WPARAM) 1, (LPARAM) &tba[1]);

  SendMessageA(fodInfos->DlgInfos.hwndTB, TB_ADDBUTTONSA, (WPARAM) 9,(LPARAM) &tbb);
  SendMessageA(fodInfos->DlgInfos.hwndTB, TB_AUTOSIZE, 0, 0);

  /* Set the window text with the text specified in the OPENFILENAME structure */
  if(fodInfos->ofnInfos->lpstrTitle)
  {
      SetWindowTextA(hwnd,fodInfos->ofnInfos->lpstrTitle);
  }
  else if (fodInfos->DlgInfos.dwDlgProp & FODPROP_SAVEDLG)
  {
      SetWindowTextA(hwnd,"Save");
  }

  /* Initialise the file name edit control */
  if(fodInfos->ofnInfos->lpstrFile)
  {
      LPSTR lpstrFile = PathFindFileNameA(fodInfos->ofnInfos->lpstrFile);
      SetDlgItemTextA(hwnd, IDC_FILENAME, lpstrFile);
  }

  /* Must the open as read only check box be checked ?*/
  if(fodInfos->ofnInfos->Flags & OFN_READONLY)
  {
    SendDlgItemMessageA(hwnd,IDC_OPENREADONLY,BM_SETCHECK,(WPARAM)TRUE,0);
  }

  /* Must the open as read only check box be hid ?*/
  if(fodInfos->ofnInfos->Flags & OFN_HIDEREADONLY)
  {
    ShowWindow(GetDlgItem(hwnd,IDC_OPENREADONLY),SW_HIDE);
  }

  /* Must the help button be hid ?*/
  if (!(fodInfos->ofnInfos->Flags & OFN_SHOWHELP))
  {
    ShowWindow(GetDlgItem(hwnd, pshHelp), SW_HIDE);
  }

  /* Resize the height, if open as read only checkbox ad help button
     are hidden and we are not using a custom template */
  if ( (fodInfos->ofnInfos->Flags & OFN_HIDEREADONLY) &&
       (!(fodInfos->ofnInfos->Flags &
         (OFN_SHOWHELP|OFN_ENABLETEMPLATE|OFN_ENABLETEMPLATEHANDLE))))
  {
    RECT rectDlg, rectHelp, rectCancel;
    GetWindowRect(hwnd, &rectDlg);
    GetWindowRect(GetDlgItem(hwnd, pshHelp), &rectHelp);
    GetWindowRect(GetDlgItem(hwnd, IDCANCEL), &rectCancel);
    /* subtract the height of the help button plus the space between
       the help button and the cancel button to the height of the dialog */
    SetWindowPos(hwnd, 0, 0, 0, rectDlg.right-rectDlg.left,
                 (rectDlg.bottom-rectDlg.top) - (rectHelp.bottom - rectCancel.bottom),
                 SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
  }

  /* change Open to Save FIXME: use resources */
  if (fodInfos->DlgInfos.dwDlgProp & FODPROP_SAVEDLG)
  {
      SetDlgItemTextA(hwnd,IDOK,"&Save");
      SetDlgItemTextA(hwnd,IDC_LOOKINSTATIC,"Save &in");
  }
  return 0;
}

/***********************************************************************
 *      FILEDLG95_FillControls
 *
 * WM_INITDIALOG message handler (after hook notification)
 */
static LRESULT FILEDLG95_FillControls(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  LPITEMIDLIST pidlItemId = NULL;
  CHAR *dopstr = NULL;

  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) lParam;

  TRACE("dir=%s file=%s\n",
  fodInfos->ofnInfos->lpstrInitialDir, fodInfos->ofnInfos->lpstrFile);

#ifdef __WIN32OS2__
  /*@PF
  How can Wine miss this?? Very common situation for Windows apps!
  If InitialDir is NULL and we have full blown path in lpstrFile we set
  directory to it.

  Q: What about Unicode?

  */

  if (fodInfos->ofnInfos->lpstrInitialDir == NULL && fodInfos->ofnInfos->lpstrFile &&
    !PathIsRelativeA(fodInfos->ofnInfos->lpstrFile) &&
    PathGetDriveNumberA(fodInfos->ofnInfos->lpstrFile) != -1)
  {
   dopstr = HEAP_strdupA(GetProcessHeap(), 0,fodInfos->ofnInfos->lpstrFile);
   *strrchr(dopstr,'\\') = '\0';
   fodInfos->ofnInfos->lpstrInitialDir = dopstr;
  }
#endif

  /* Get the initial directory pidl */
  if(!(pidlItemId = GetPidlFromName(fodInfos->Shell.FOIShellFolder,fodInfos->ofnInfos->lpstrInitialDir)))
  {
    char path[MAX_PATH];

    GetCurrentDirectoryA(MAX_PATH,path);
    pidlItemId = GetPidlFromName(fodInfos->Shell.FOIShellFolder, path);
  }
#ifdef __WIN32OS2__
  /* @PF Another WINE hole. If lpstrInitialDir is not a dir but a filename
   skip it and use current directory. */

  else
  {
     DWORD dwAttr = GetFileAttributesA(fodInfos->ofnInfos->lpstrInitialDir);
     if(dwAttr != -1 )
     {
       if (!(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
       {
          char path[MAX_PATH];

          GetCurrentDirectoryA(MAX_PATH,path);
          COMDLG32_SHFree(pidlItemId);
          pidlItemId = GetPidlFromName(fodInfos->Shell.FOIShellFolder, path);
       }
     }
  }
#endif
  /* Initialise shell objects */
  FILEDLG95_SHELL_Init(hwnd);

  /* Initialize the Look In combo box */
  FILEDLG95_LOOKIN_Init(fodInfos->DlgInfos.hwndLookInCB);

  /* Initialize the filter combo box */
  FILEDLG95_FILETYPE_Init(hwnd);

  /* Browse to the initial directory */
  IShellBrowser_BrowseObject(fodInfos->Shell.FOIShellBrowser,pidlItemId, SBSP_ABSOLUTE);

  /* Free pidlItem memory */
  COMDLG32_SHFree(pidlItemId);

#ifdef __WIN32OS2__
  /* Free dopstr */
  if (dopstr)
  {
   fodInfos->ofnInfos->lpstrInitialDir = NULL;
   HEAP_free(dopstr);
  }
#endif
  return TRUE;
}
/***********************************************************************
 *      FILEDLG95_Clean
 *
 * Regroups all the cleaning functions of the filedlg
 */
void FILEDLG95_Clean(HWND hwnd)
{
      FILEDLG95_FILETYPE_Clean(hwnd);
      FILEDLG95_LOOKIN_Clean(hwnd);
      FILEDLG95_SHELL_Clean(hwnd);
}
/***********************************************************************
 *      FILEDLG95_OnWMCommand
 *
 * WM_COMMAND message handler
 */
static LRESULT FILEDLG95_OnWMCommand(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  WORD wNotifyCode = HIWORD(wParam); /* notification code */
  WORD wID = LOWORD(wParam);         /* item, control, or accelerator identifier */
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  switch(wID)
  {
    /* OK button */
  case IDOK:
#ifndef __WIN32OS2__
      if(FILEDLG95_OnOpen(hwnd))
        SendCustomDlgNotificationMessage(hwnd,CDN_FILEOK);
    break;
#else
    {
      // 20011017 PH
      // some apps require the resources to be still there
      // for the CDN_ processing.
      BOOL fResult = FILEDLG95_OnOpen(hwnd);
      if(fResult)
        SendCustomDlgNotificationMessage(hwnd,CDN_FILEOK);
      break;
    }
#endif
    /* Cancel button */
  case IDCANCEL:
      FILEDLG95_Clean(hwnd);
      EndDialog(hwnd, FALSE);
    break;
    /* Filetype combo box */
  case IDC_FILETYPE:
    FILEDLG95_FILETYPE_OnCommand(hwnd,wNotifyCode);
    break;
    /* LookIn combo box */
  case IDC_LOOKIN:
    FILEDLG95_LOOKIN_OnCommand(hwnd,wNotifyCode);
    break;

  /* --- toolbar --- */
    /* Up folder button */
  case FCIDM_TB_UPFOLDER:
    FILEDLG95_SHELL_UpFolder(hwnd);
    break;
    /* New folder button */
  case FCIDM_TB_NEWFOLDER:
    FILEDLG95_SHELL_ExecuteCommand(hwnd,CMDSTR_NEWFOLDER);
    break;
    /* List option button */
  case FCIDM_TB_SMALLICON:
    FILEDLG95_SHELL_ExecuteCommand(hwnd,CMDSTR_VIEWLIST);
    break;
    /* Details option button */
  case FCIDM_TB_REPORTVIEW:
    FILEDLG95_SHELL_ExecuteCommand(hwnd,CMDSTR_VIEWDETAILS);
    break;
    /* Details option button */
  case FCIDM_TB_DESKTOP:
    FILEDLG95_SHELL_BrowseToDesktop(hwnd);
    break;

  case IDC_FILENAME:
    break;

  }
  /* Do not use the listview selection anymore */
  fodInfos->DlgInfos.dwDlgProp &= ~FODPROP_USEVIEW;
  return 0;
}

/***********************************************************************
 *      FILEDLG95_OnWMGetIShellBrowser
 *
 * WM_GETISHELLBROWSER message handler
 */
static LRESULT FILEDLG95_OnWMGetIShellBrowser(HWND hwnd)
{

  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("\n");

  SetWindowLongA(hwnd,DWL_MSGRESULT,(LONG)fodInfos->Shell.FOIShellBrowser);

  return TRUE;
}


/***********************************************************************
 *      FILEDLG95_OnOpenMultipleFiles
 *
 * Handles the opening of multiple files.
 *
 * FIXME
 *  check destination buffer size
 */
BOOL FILEDLG95_OnOpenMultipleFiles(HWND hwnd, LPSTR lpstrFileList, UINT nFileCount, UINT sizeUsed)
{
  CHAR   lpstrPathSpec[MAX_PATH] = "";
  LPSTR  lpstrFile;
  UINT   nCount, nSizePath;
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("\n");

  lpstrFile = fodInfos->ofnInfos->lpstrFile;
  lpstrFile[0] = '\0';

  SHGetPathFromIDListA( fodInfos->ShellInfos.pidlAbsCurrent, lpstrPathSpec );

  if ( !(fodInfos->ofnInfos->Flags & OFN_NOVALIDATE) &&
      ( fodInfos->ofnInfos->Flags & OFN_FILEMUSTEXIST))
  {
    LPSTR lpstrTemp = lpstrFileList;

    for ( nCount = 0; nCount < nFileCount; nCount++ )
    {
      LPITEMIDLIST pidl;

      pidl = GetPidlFromName(fodInfos->Shell.FOIShellFolder, lpstrTemp);
      if (!pidl)
      {
        CHAR lpstrNotFound[100];
        CHAR lpstrMsg[100];
        CHAR tmp[400];

        LoadStringA(COMMDLG_hInstance32, IDS_FILENOTFOUND, lpstrNotFound, 100);
        LoadStringA(COMMDLG_hInstance32, IDS_VERIFYFILE, lpstrMsg, 100);

        strcpy(tmp, lpstrTemp);
        strcat(tmp, "\n");
        strcat(tmp, lpstrNotFound);
        strcat(tmp, "\n");
        strcat(tmp, lpstrMsg);

        MessageBoxA(hwnd, tmp, fodInfos->ofnInfos->lpstrTitle, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
      }

      /* move to the next file in the list of files */
      lpstrTemp += strlen(lpstrTemp) + 1;
      COMDLG32_SHFree(pidl);
    }
  }

  nSizePath = strlen(lpstrPathSpec);
  strcpy( lpstrFile, lpstrPathSpec);
  memcpy( lpstrFile + nSizePath + 1, lpstrFileList, sizeUsed );

  fodInfos->ofnInfos->nFileOffset = nSizePath + 1;
  fodInfos->ofnInfos->nFileExtension = 0;

  /* clean and exit */
  FILEDLG95_Clean(hwnd);
  return EndDialog(hwnd,TRUE);
}

/***********************************************************************
 *      FILEDLG95_OnOpen
 *
 * Ok button WM_COMMAND message handler
 *
 * If the function succeeds, the return value is nonzero.
 */
#define ONOPEN_BROWSE 1
#define ONOPEN_OPEN   2
#define ONOPEN_SEARCH 3
static void FILEDLG95_OnOpenMessage(HWND hwnd, int idCaption, int idText)
{
  char strMsgTitle[MAX_PATH];
  char strMsgText [MAX_PATH];
  if (idCaption)
    LoadStringA(COMDLG32_hInstance, idCaption, strMsgTitle, sizeof(strMsgTitle));
  else
    strMsgTitle[0] = '\0';
  LoadStringA(COMDLG32_hInstance, idText, strMsgText, sizeof(strMsgText));
  MessageBoxA(hwnd,strMsgText, strMsgTitle, MB_OK | MB_ICONHAND);
}

BOOL FILEDLG95_OnOpen(HWND hwnd)
{
  char * lpstrFileList;
  UINT nFileCount = 0;
  UINT sizeUsed = 0;
  BOOL ret = TRUE;
  char lpstrPathAndFile[MAX_PATH];
  char lpstrTemp[MAX_PATH];
  LPSHELLFOLDER lpsf = NULL;
  int nOpenAction;
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("hwnd=0x%04x\n", hwnd);

  /* get the files from the edit control */
  nFileCount = FILEDLG95_FILENAME_GetFileNames(hwnd, &lpstrFileList, &sizeUsed);

  /* try if the user selected a folder in the shellview */
  if(nFileCount == 0)
  {
      BrowseSelectedFolder(hwnd);
      return FALSE;
  }

  if(nFileCount > 1)
  {
      ret = FILEDLG95_OnOpenMultipleFiles(hwnd, lpstrFileList, nFileCount, sizeUsed);
      goto ret;
  }

  TRACE("count=%u len=%u file=%s\n", nFileCount, sizeUsed, lpstrFileList);

/*
  Step 1:  Build a complete path name from the current folder and
  the filename or path in the edit box.
  Special cases:
  - the path in the edit box is a root path
    (with or without drive letter)
  - the edit box contains ".." (or a path with ".." in it)
*/

  /* Get the current directory name */
  if (!SHGetPathFromIDListA(fodInfos->ShellInfos.pidlAbsCurrent, lpstrPathAndFile))
  {
    /* we are in a special folder, default to desktop */
    if(FAILED(COMDLG32_SHGetFolderPathA(hwnd, CSIDL_DESKTOPDIRECTORY|CSIDL_FLAG_CREATE, 0, 0, lpstrPathAndFile)))
    {
      /* last fallback */
      GetCurrentDirectoryA(MAX_PATH, lpstrPathAndFile);
    }
  }
  PathAddBackslashA(lpstrPathAndFile);

  TRACE("current directory=%s\n", lpstrPathAndFile);

  /* if the user specifyed a fully qualified path use it */
  if(PathIsRelativeA(lpstrFileList))
  {
    strcat(lpstrPathAndFile, lpstrFileList);
  }
  else
  {
    /* does the path have a drive letter? */
    if (PathGetDriveNumberA(lpstrFileList) == -1)
      strcpy(lpstrPathAndFile+2, lpstrFileList);
    else
      strcpy(lpstrPathAndFile, lpstrFileList);
  }

  /* resolve "." and ".." */
  PathCanonicalizeA(lpstrTemp, lpstrPathAndFile );
  strcpy(lpstrPathAndFile, lpstrTemp);
  TRACE("canon=%s\n", lpstrPathAndFile);

  MemFree(lpstrFileList);

/*
  Step 2: here we have a cleaned up path

  We have to parse the path step by step to see if we have to browse
  to a folder if the path points to a directory or the last
  valid element is a directory.

  valid variables:
    lpstrPathAndFile: cleaned up path
 */

  nOpenAction = ONOPEN_BROWSE;

#ifdef __WIN32OS2__
  /* @@@AH 2001-06-02 moved out of !OFN_NONVALIDATE block - needs to be set */
  if (FAILED (SHGetDesktopFolder(&lpsf))) return FALSE;
#endif

  /* dont apply any checks with OFN_NOVALIDATE */
  if(!(fodInfos->ofnInfos->Flags & OFN_NOVALIDATE))
  {
    LPSTR lpszTemp, lpszTemp1;
    LPITEMIDLIST pidl = NULL;

    /* check for invalid chars */
    if(strpbrk(lpstrPathAndFile+3, "/:<>|") != NULL)
    {
      FILEDLG95_OnOpenMessage(hwnd, IDS_INVALID_FILENAME_TITLE, IDS_INVALID_FILENAME);
      ret = FALSE;
      goto ret;
    }
#ifndef __WIN32OS2__
    if (FAILED (SHGetDesktopFolder(&lpsf))) return FALSE;
#endif
    lpszTemp1 = lpszTemp = lpstrPathAndFile;
    while (lpszTemp1)
    {
      LPSHELLFOLDER lpsfChild;
      WCHAR lpwstrTemp[MAX_PATH];
      DWORD dwEaten, dwAttributes;

      lpszTemp = PathFindNextComponentA(lpszTemp);

      if (!lpszTemp) break; /* end of path */

      if(*lpszTemp)
      {
          DWORD len = MultiByteToWideChar( CP_ACP, 0, lpszTemp1, lpszTemp - lpszTemp1,
                                           lpwstrTemp, MAX_PATH );
          lpwstrTemp[len] = 0;
      }
      else
      {
          MultiByteToWideChar( CP_ACP, 0, lpszTemp1, -1,
                               lpwstrTemp, sizeof(lpwstrTemp)/sizeof(WCHAR) );

	/* if the last element is a wildcard do a search */
        if(strpbrk(lpszTemp1, "*?") != NULL)
        {
	  nOpenAction = ONOPEN_SEARCH;
	  break;
	}
      }
      lpszTemp1 = lpszTemp;

      TRACE("parse now=%s next=%s sf=%p\n",debugstr_w(lpwstrTemp), debugstr_a(lpszTemp), lpsf);

      if(lstrlenW(lpwstrTemp)==2) PathAddBackslashW(lpwstrTemp);

      dwAttributes = SFGAO_FOLDER;
      if(SUCCEEDED(IShellFolder_ParseDisplayName(lpsf, hwnd, NULL, lpwstrTemp, &dwEaten, &pidl, &dwAttributes)))
      {
        /* the path component is valid, we have a pidl of the next path component */
        TRACE("parse OK attr=0x%08lx pidl=%p\n", dwAttributes, pidl);
        if(dwAttributes & SFGAO_FOLDER)
        {
          if(FAILED(IShellFolder_BindToObject(lpsf, pidl, 0, &IID_IShellFolder, (LPVOID*)&lpsfChild)))
          {
            ERR("bind to failed\n"); /* should not fail */
            break;
          }
          IShellFolder_Release(lpsf);
          lpsf = lpsfChild;
          lpsfChild = NULL;
        }
        else
        {
          TRACE("value\n");

	  /* end dialog, return value */
          nOpenAction = ONOPEN_OPEN;
	  break;
        }
	COMDLG32_SHFree(pidl);
	pidl = NULL;
      }
      else
      {
	if(*lpszTemp)	/* points to trailing null for last path element */
        {
	  if(fodInfos->ofnInfos->Flags & OFN_PATHMUSTEXIST)
	  {
            FILEDLG95_OnOpenMessage(hwnd, 0, IDS_PATHNOTEXISTING);
	    break;
	  }
	}
        else
	{
          if(fodInfos->ofnInfos->Flags & OFN_FILEMUSTEXIST)
	  {
            FILEDLG95_OnOpenMessage(hwnd, 0, IDS_FILENOTEXISTING);
	    break;
	  }
	}
	/* change to the current folder */
        nOpenAction = ONOPEN_OPEN;
	break;
      }
    }
    if(pidl) COMDLG32_SHFree(pidl);
  }
#ifdef __WIN32OS2__
  else
  {
    // @@@AH 2001-06-02 when we are in OFN_NOVALIDATE mode, the action
    // must be to open the specified file when pressing open button
    TRACE("OFN_NOVALIDATE mode, set open action to ONOPEN_OPEN\n");
    nOpenAction = ONOPEN_OPEN;
  }
#endif

/*
  Step 3: here we have a cleaned up and validated path

  valid variables:
   lpsf:             ShellFolder bound to the rightmost valid path component
   lpstrPathAndFile: cleaned up path
   nOpenAction:      action to do
*/
  TRACE("end validate sf=%p\n", lpsf);

  switch(nOpenAction)
  {
    case ONOPEN_SEARCH:   /* set the current filter to the file mask and refresh */
      TRACE("ONOPEN_SEARCH %s\n", lpstrPathAndFile);
      {
        int iPos;
        LPSTR lpszTemp = PathFindFileNameA(lpstrPathAndFile);
        DWORD len;

        /* replace the current filter */
        if(fodInfos->ShellInfos.lpstrCurrentFilter)
	  MemFree((LPVOID)fodInfos->ShellInfos.lpstrCurrentFilter);
        len = MultiByteToWideChar( CP_ACP, 0, lpszTemp, -1, NULL, 0 );
        fodInfos->ShellInfos.lpstrCurrentFilter = MemAlloc(len * sizeof(WCHAR));
        MultiByteToWideChar( CP_ACP, 0, lpszTemp, -1,
                             fodInfos->ShellInfos.lpstrCurrentFilter, len );

        /* set the filter cb to the extension when possible */
        if(-1 < (iPos = FILEDLG95_FILETYPE_SearchExt(fodInfos->DlgInfos.hwndFileTypeCB, lpszTemp)))
        CBSetCurSel(fodInfos->DlgInfos.hwndFileTypeCB, iPos);
      }
      /* fall through */
    case ONOPEN_BROWSE:   /* browse to the highest folder we could bind to */
      TRACE("ONOPEN_BROWSE\n");
      {
	IPersistFolder2 * ppf2;
        if(SUCCEEDED(IShellFolder_QueryInterface( lpsf, &IID_IPersistFolder2, (LPVOID*)&ppf2)))
        {
          LPITEMIDLIST pidlCurrent;
          IPersistFolder2_GetCurFolder(ppf2, &pidlCurrent);
          IPersistFolder2_Release(ppf2);
	  if( ! COMDLG32_PIDL_ILIsEqual(pidlCurrent, fodInfos->ShellInfos.pidlAbsCurrent))
	  {
	    IShellBrowser_BrowseObject(fodInfos->Shell.FOIShellBrowser, pidlCurrent, SBSP_ABSOLUTE);
	  }
	  else if( nOpenAction == ONOPEN_SEARCH )
	  {
            IShellView_Refresh(fodInfos->Shell.FOIShellView);
	  }
          COMDLG32_SHFree(pidlCurrent);
        }
      }
      ret = FALSE;
      break;
    case ONOPEN_OPEN:   /* fill in the return struct and close the dialog */
      TRACE("ONOPEN_OPEN %s\n", lpstrPathAndFile);
      {
	/* add default extension */
	if (fodInfos->ofnInfos->lpstrDefExt)
	{
	  if (! *PathFindExtensionA(lpstrPathAndFile))
	  {
	    /* only add "." in case a default extension does exist */
	    if (*fodInfos->ofnInfos->lpstrDefExt != '\0')
	    {
	        strcat(lpstrPathAndFile, ".");
	        strcat(lpstrPathAndFile, fodInfos->ofnInfos->lpstrDefExt);
	    }
	  }
	}

        /* Check that the size of the file does not exceed buffer size.
             (Allow for extra \0 if OFN_MULTISELECT is set.) */
        if(strlen(lpstrPathAndFile) < fodInfos->ofnInfos->nMaxFile -
            ((fodInfos->ofnInfos->Flags & OFN_ALLOWMULTISELECT) ? 1 : 0))
        {
          LPSTR lpszTemp;

          /* fill destination buffer */
          strcpy(fodInfos->ofnInfos->lpstrFile, lpstrPathAndFile);
          if (fodInfos->ofnInfos->Flags & OFN_ALLOWMULTISELECT)
            fodInfos->ofnInfos->lpstrFile[strlen(fodInfos->ofnInfos->lpstrFile)
                + 1] = '\0';

          /* set filename offset */
          lpszTemp = PathFindFileNameA(lpstrPathAndFile);
          fodInfos->ofnInfos->nFileOffset = lpszTemp - lpstrPathAndFile;

          /* set extension offset */
          lpszTemp = PathFindExtensionA(lpstrPathAndFile);
          fodInfos->ofnInfos->nFileExtension = (*lpszTemp) ? lpszTemp - lpstrPathAndFile + 1 : 0;

          /* set the lpstrFileTitle */
          if(fodInfos->ofnInfos->lpstrFileTitle)
	  {
            LPSTR lpstrFileTitle = PathFindFileNameA(lpstrPathAndFile);
	    strncpy(fodInfos->ofnInfos->lpstrFileTitle, lpstrFileTitle, fodInfos->ofnInfos->nMaxFileTitle);
	  }

          /* ask the hook if we can close */
          if(IsHooked(fodInfos))
	  {
	    /* FIXME we are sending ASCII-structures. Does not work with NT */
	    /* first old style */
	    TRACE("---\n");
            CallWindowProcA((WNDPROC)fodInfos->ofnInfos->lpfnHook,
                            fodInfos->DlgInfos.hwndCustomDlg,
                            fodInfos->HookMsg.fileokstring, 0, (LPARAM)fodInfos->ofnInfos);
	    if (GetWindowLongA(hwnd, DWL_MSGRESULT))
	    {
	      TRACE("cancled\n");
	      ret = FALSE;
	      goto ret;
	    }
	  }

          TRACE("close\n");
	  FILEDLG95_Clean(hwnd);
          ret = EndDialog(hwnd, TRUE);
	}
	else
        {
          /* FIXME set error FNERR_BUFFERTOSMALL */
          FILEDLG95_Clean(hwnd);
          ret = EndDialog(hwnd, FALSE);
        }
        goto ret;
      }
      break;
  }

ret:
  if(lpsf) IShellFolder_Release(lpsf);
  return ret;
}

/***********************************************************************
 *      FILEDLG95_SHELL_Init
 *
 * Initialisation of the shell objects
 */
static HRESULT FILEDLG95_SHELL_Init(HWND hwnd)
{
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("\n");

  /*
   * Initialisation of the FileOpenDialogInfos structure
   */

  /* Shell */

  /*ShellInfos */
  fodInfos->ShellInfos.hwndOwner = hwnd;

  /* Disable multi-select if flag not set */
  if (!(fodInfos->ofnInfos->Flags & OFN_ALLOWMULTISELECT))
  {
     fodInfos->ShellInfos.folderSettings.fFlags |= FWF_SINGLESEL;
  }
  fodInfos->ShellInfos.folderSettings.fFlags |= FWF_AUTOARRANGE | FWF_ALIGNLEFT;
  fodInfos->ShellInfos.folderSettings.ViewMode = FVM_LIST;

  GetWindowRect(GetDlgItem(hwnd,IDC_SHELLSTATIC),&fodInfos->ShellInfos.rectView);
  ScreenToClient(hwnd,(LPPOINT)&fodInfos->ShellInfos.rectView.left);
  ScreenToClient(hwnd,(LPPOINT)&fodInfos->ShellInfos.rectView.right);

  /* Construct the IShellBrowser interface */
  fodInfos->Shell.FOIShellBrowser = IShellBrowserImpl_Construct(hwnd);

  return NOERROR;
}

/***********************************************************************
 *      FILEDLG95_SHELL_ExecuteCommand
 *
 * Change the folder option and refresh the view
 * If the function succeeds, the return value is nonzero.
 */
static BOOL FILEDLG95_SHELL_ExecuteCommand(HWND hwnd, LPCSTR lpVerb)
{
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  IContextMenu * pcm;
  TRACE("(0x%08x,%p)\n", hwnd, lpVerb);

  if(SUCCEEDED(IShellView_GetItemObject(fodInfos->Shell.FOIShellView,
					SVGIO_BACKGROUND,
					&IID_IContextMenu,
					(LPVOID*)&pcm)))
  {
    CMINVOKECOMMANDINFO ci;
    ZeroMemory(&ci, sizeof(CMINVOKECOMMANDINFO));
    ci.cbSize = sizeof(CMINVOKECOMMANDINFO);
    ci.lpVerb = lpVerb;
    ci.hwnd = hwnd;

    IContextMenu_InvokeCommand(pcm, &ci);
    IContextMenu_Release(pcm);
  }

  return FALSE;
}

/***********************************************************************
 *      FILEDLG95_SHELL_UpFolder
 *
 * Browse to the specified object
 * If the function succeeds, the return value is nonzero.
 */
static BOOL FILEDLG95_SHELL_UpFolder(HWND hwnd)
{
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("\n");

  if(SUCCEEDED(IShellBrowser_BrowseObject(fodInfos->Shell.FOIShellBrowser,
                                          NULL,
                                          SBSP_PARENT)))
  {
    return TRUE;
  }
  return FALSE;
}

/***********************************************************************
 *      FILEDLG95_SHELL_BrowseToDesktop
 *
 * Browse to the Desktop
 * If the function succeeds, the return value is nonzero.
 */
static BOOL FILEDLG95_SHELL_BrowseToDesktop(HWND hwnd)
{
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);
  LPITEMIDLIST pidl;
  HRESULT hres;

  TRACE("\n");

  SHGetSpecialFolderLocation(0,CSIDL_DESKTOP,&pidl);
  hres = IShellBrowser_BrowseObject(fodInfos->Shell.FOIShellBrowser, pidl, SBSP_ABSOLUTE);
  COMDLG32_SHFree(pidl);
  return SUCCEEDED(hres);
}
/***********************************************************************
 *      FILEDLG95_SHELL_Clean
 *
 * Cleans the memory used by shell objects
 */
static void FILEDLG95_SHELL_Clean(HWND hwnd)
{
    FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

    TRACE("\n");

    COMDLG32_SHFree(fodInfos->ShellInfos.pidlAbsCurrent);

    /* clean Shell interfaces */
    IShellView_DestroyViewWindow(fodInfos->Shell.FOIShellView);
    IShellView_Release(fodInfos->Shell.FOIShellView);
    IShellFolder_Release(fodInfos->Shell.FOIShellFolder);
    IShellBrowser_Release(fodInfos->Shell.FOIShellBrowser);
    if (fodInfos->Shell.FOIDataObject)
      IDataObject_Release(fodInfos->Shell.FOIDataObject);
}

/***********************************************************************
 *      FILEDLG95_FILETYPE_Init
 *
 * Initialisation of the file type combo box
 */
static HRESULT FILEDLG95_FILETYPE_Init(HWND hwnd)
{
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("\n");

  if(fodInfos->ofnInfos->lpstrFilter)
  {
    int nFilters = 0;	/* number of filters */
    LPSTR lpstrFilter;
    LPCSTR lpstrPos = fodInfos->ofnInfos->lpstrFilter;

    for(;;)
    {
      /* filter is a list...  title\0ext\0......\0\0
       * Set the combo item text to the title and the item data
       *  to the ext
       */
      LPCSTR lpstrDisplay;
      LPSTR lpstrExt;

      /* Get the title */
      if(! *lpstrPos) break;	/* end */
      lpstrDisplay = lpstrPos;
      lpstrPos += strlen(lpstrPos) + 1;

      /* Copy the extensions */
      if (! *lpstrPos) return E_FAIL;	/* malformed filter */
      if (!(lpstrExt = (LPSTR) MemAlloc(strlen(lpstrPos)+1))) return E_FAIL;
      strcpy(lpstrExt,lpstrPos);
      lpstrPos += strlen(lpstrPos) + 1;

      /* Add the item at the end of the combo */
      CBAddString(fodInfos->DlgInfos.hwndFileTypeCB, lpstrDisplay);
      CBSetItemDataPtr(fodInfos->DlgInfos.hwndFileTypeCB, nFilters, lpstrExt);
      nFilters++;
    }
    /*
     * Set the current filter to the one specified
     * in the initialisation structure
     * FIXME: lpstrCustomFilter not handled at all
     */

    /* set default filter index */
    if(fodInfos->ofnInfos->nFilterIndex == 0 && fodInfos->ofnInfos->lpstrCustomFilter == NULL)
      fodInfos->ofnInfos->nFilterIndex = 1;

    /* First, check to make sure our index isn't out of bounds. */
    if ( fodInfos->ofnInfos->nFilterIndex > nFilters )
      fodInfos->ofnInfos->nFilterIndex = nFilters;

    /* Set the current index selection. */
    CBSetCurSel(fodInfos->DlgInfos.hwndFileTypeCB, fodInfos->ofnInfos->nFilterIndex-1);

    /* Get the corresponding text string from the combo box. */
    lpstrFilter = (LPSTR) CBGetItemDataPtr(fodInfos->DlgInfos.hwndFileTypeCB,
                                             fodInfos->ofnInfos->nFilterIndex-1);

    if ((INT)lpstrFilter == CB_ERR)  /* control is empty */
      lpstrFilter = NULL;

    if(lpstrFilter)
    {
      DWORD len;
      _strlwr(lpstrFilter);	/* lowercase */
      len = MultiByteToWideChar( CP_ACP, 0, lpstrFilter, -1, NULL, 0 );
      fodInfos->ShellInfos.lpstrCurrentFilter = MemAlloc( len * sizeof(WCHAR) );
      MultiByteToWideChar( CP_ACP, 0, lpstrFilter, -1,
                           fodInfos->ShellInfos.lpstrCurrentFilter, len );
    }
  }
  return NOERROR;
}

/***********************************************************************
 *      FILEDLG95_FILETYPE_OnCommand
 *
 * WM_COMMAND of the file type combo box
 * If the function succeeds, the return value is nonzero.
 */
static BOOL FILEDLG95_FILETYPE_OnCommand(HWND hwnd, WORD wNotifyCode)
{
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  switch(wNotifyCode)
  {
    case CBN_SELENDOK:
    {
      LPSTR lpstrFilter;

      /* Get the current item of the filetype combo box */
      int iItem = CBGetCurSel(fodInfos->DlgInfos.hwndFileTypeCB);

      /* set the current filter index - indexed from 1 */
      fodInfos->ofnInfos->nFilterIndex = iItem + 1;

      /* Set the current filter with the current selection */
      if(fodInfos->ShellInfos.lpstrCurrentFilter)
         MemFree((LPVOID)fodInfos->ShellInfos.lpstrCurrentFilter);

      lpstrFilter = (LPSTR) CBGetItemDataPtr(fodInfos->DlgInfos.hwndFileTypeCB,
                                             iItem);
      if((int)lpstrFilter != CB_ERR)
      {
          DWORD len;
          _strlwr(lpstrFilter); /* lowercase */
          len = MultiByteToWideChar( CP_ACP, 0, lpstrFilter, -1, NULL, 0 );
          fodInfos->ShellInfos.lpstrCurrentFilter = MemAlloc( len * sizeof(WCHAR) );
          MultiByteToWideChar( CP_ACP, 0, lpstrFilter, -1,
                               fodInfos->ShellInfos.lpstrCurrentFilter, len );
          SendCustomDlgNotificationMessage(hwnd,CDN_TYPECHANGE);
      }

      /* Refresh the actual view to display the included items*/
      IShellView_Refresh(fodInfos->Shell.FOIShellView);
    }
  }
  return FALSE;
}
/***********************************************************************
 *      FILEDLG95_FILETYPE_SearchExt
 *
 * searches for a extension in the filetype box
 */
static int FILEDLG95_FILETYPE_SearchExt(HWND hwnd,LPCSTR lpstrExt)
{
  int i, iCount = CBGetCount(hwnd);

  TRACE("%s\n", lpstrExt);

  if(iCount != CB_ERR)
  {
    for(i=0;i<iCount;i++)
    {
      if(!strcasecmp(lpstrExt,(LPSTR)CBGetItemDataPtr(hwnd,i)))
          return i;
    }
  }
  return -1;
}

/***********************************************************************
 *      FILEDLG95_FILETYPE_Clean
 *
 * Clean the memory used by the filetype combo box
 */
static void FILEDLG95_FILETYPE_Clean(HWND hwnd)
{
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);
  int iPos;
  int iCount = CBGetCount(fodInfos->DlgInfos.hwndFileTypeCB);

  TRACE("\n");

  /* Delete each string of the combo and their associated data */
  if(iCount != CB_ERR)
  {
    for(iPos = iCount-1;iPos>=0;iPos--)
    {
      MemFree((LPSTR) CBGetItemDataPtr(fodInfos->DlgInfos.hwndFileTypeCB,iPos));
      CBDeleteString(fodInfos->DlgInfos.hwndFileTypeCB,iPos);
    }
  }
  /* Current filter */
  if(fodInfos->ShellInfos.lpstrCurrentFilter)
     MemFree(fodInfos->ShellInfos.lpstrCurrentFilter);

}

/***********************************************************************
 *      FILEDLG95_LOOKIN_Init
 *
 * Initialisation of the look in combo box
 */
static HRESULT FILEDLG95_LOOKIN_Init(HWND hwndCombo)
{
  IShellFolder	*psfRoot, *psfDrives;
  IEnumIDList	*lpeRoot, *lpeDrives;
  LPITEMIDLIST	pidlDrives, pidlTmp, pidlTmp1, pidlAbsTmp;

  LookInInfos *liInfos = MemAlloc(sizeof(LookInInfos));

  TRACE("\n");

  liInfos->iMaxIndentation = 0;

  SetPropA(hwndCombo, LookInInfosStr, (HANDLE) liInfos);

  /* set item height for both text field and listbox */
  CBSetItemHeight(hwndCombo,-1,GetSystemMetrics(SM_CYSMICON));
  CBSetItemHeight(hwndCombo,0,GetSystemMetrics(SM_CYSMICON));

  /* Initialise data of Desktop folder */
  SHGetSpecialFolderLocation(0,CSIDL_DESKTOP,&pidlTmp);
  FILEDLG95_LOOKIN_AddItem(hwndCombo, pidlTmp,LISTEND);
  COMDLG32_SHFree(pidlTmp);

  SHGetSpecialFolderLocation(0,CSIDL_DRIVES,&pidlDrives);

  SHGetDesktopFolder(&psfRoot);

  if (psfRoot)
  {
    /* enumerate the contents of the desktop */
    if(SUCCEEDED(IShellFolder_EnumObjects(psfRoot, hwndCombo, SHCONTF_FOLDERS, &lpeRoot)))
    {
      while (S_OK == IEnumIDList_Next(lpeRoot, 1, &pidlTmp, NULL))
      {
	FILEDLG95_LOOKIN_AddItem(hwndCombo, pidlTmp,LISTEND);

	/* special handling for CSIDL_DRIVES */
	if (COMDLG32_PIDL_ILIsEqual(pidlTmp, pidlDrives))
	{
	  if(SUCCEEDED(IShellFolder_BindToObject(psfRoot, pidlTmp, NULL, &IID_IShellFolder, (LPVOID*)&psfDrives)))
	  {
	    /* enumerate the drives */
	    if(SUCCEEDED(IShellFolder_EnumObjects(psfDrives, hwndCombo,SHCONTF_FOLDERS, &lpeDrives)))
	    {
	      while (S_OK == IEnumIDList_Next(lpeDrives, 1, &pidlTmp1, NULL))
	      {
	        pidlAbsTmp = COMDLG32_PIDL_ILCombine(pidlTmp, pidlTmp1);
	        FILEDLG95_LOOKIN_AddItem(hwndCombo, pidlAbsTmp,LISTEND);
	        COMDLG32_SHFree(pidlAbsTmp);
	        COMDLG32_SHFree(pidlTmp1);
	      }
	      IEnumIDList_Release(lpeDrives);
	    }
	    IShellFolder_Release(psfDrives);
	  }
	}
        COMDLG32_SHFree(pidlTmp);
      }
      IEnumIDList_Release(lpeRoot);
    }
  }

  IShellFolder_Release(psfRoot);
  COMDLG32_SHFree(pidlDrives);
  return NOERROR;
}

/***********************************************************************
 *      FILEDLG95_LOOKIN_DrawItem
 *
 * WM_DRAWITEM message handler
 */
static LRESULT FILEDLG95_LOOKIN_DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
  COLORREF crWin = GetSysColor(COLOR_WINDOW);
  COLORREF crHighLight = GetSysColor(COLOR_HIGHLIGHT);
  COLORREF crText = GetSysColor(COLOR_WINDOWTEXT);
  RECT rectText;
  RECT rectIcon;
  SHFILEINFOA sfi;
  HIMAGELIST ilItemImage;
  int iIndentation;
  TEXTMETRICA tm;
  LPSFOLDER tmpFolder;


  LookInInfos *liInfos = (LookInInfos *)GetPropA(pDIStruct->hwndItem,LookInInfosStr);

  TRACE("\n");

  if(pDIStruct->itemID == -1)
    return 0;

  if(!(tmpFolder = (LPSFOLDER) CBGetItemDataPtr(pDIStruct->hwndItem,
                            pDIStruct->itemID)))
    return 0;


  if(pDIStruct->itemID == liInfos->uSelectedItem)
  {
    ilItemImage = (HIMAGELIST) SHGetFileInfoA ((LPCSTR) tmpFolder->pidlItem,
                                               0,
                                               &sfi,
                                               sizeof (SHFILEINFOA),
                                               SHGFI_PIDL | SHGFI_SMALLICON |
                                               SHGFI_OPENICON | SHGFI_SYSICONINDEX    |
                                               SHGFI_DISPLAYNAME );
  }
  else
  {
    ilItemImage = (HIMAGELIST) SHGetFileInfoA ((LPCSTR) tmpFolder->pidlItem,
                                                  0,
                                                  &sfi,
                                                  sizeof (SHFILEINFOA),
                                                  SHGFI_PIDL | SHGFI_SMALLICON |
                                                  SHGFI_SYSICONINDEX |
                                                  SHGFI_DISPLAYNAME);
  }

  /* Is this item selected ? */
  if(pDIStruct->itemState & ODS_SELECTED)
  {
    SetTextColor(pDIStruct->hDC,(0x00FFFFFF & ~(crText)));
    SetBkColor(pDIStruct->hDC,crHighLight);
    FillRect(pDIStruct->hDC,&pDIStruct->rcItem,(HBRUSH)crHighLight);
  }
  else
  {
    SetTextColor(pDIStruct->hDC,crText);
    SetBkColor(pDIStruct->hDC,crWin);
    FillRect(pDIStruct->hDC,&pDIStruct->rcItem,(HBRUSH)crWin);
  }

  /* Do not indent item if drawing in the edit of the combo */
  if(pDIStruct->itemState & ODS_COMBOBOXEDIT)
  {
    iIndentation = 0;
    ilItemImage = (HIMAGELIST) SHGetFileInfoA ((LPCSTR) tmpFolder->pidlItem,
                                                0,
                                                &sfi,
                                                sizeof (SHFILEINFOA),
                                                SHGFI_PIDL | SHGFI_SMALLICON | SHGFI_OPENICON
                                                | SHGFI_SYSICONINDEX | SHGFI_DISPLAYNAME  );

  }
  else
  {
    iIndentation = tmpFolder->m_iIndent;
  }
  /* Draw text and icon */

  /* Initialise the icon display area */
  rectIcon.left = pDIStruct->rcItem.left + ICONWIDTH/2 * iIndentation;
  rectIcon.top = pDIStruct->rcItem.top;
  rectIcon.right = rectIcon.left + ICONWIDTH;
  rectIcon.bottom = pDIStruct->rcItem.bottom;

  /* Initialise the text display area */
  GetTextMetricsA(pDIStruct->hDC, &tm);
  rectText.left = rectIcon.right;
  rectText.top =
	  (pDIStruct->rcItem.top + pDIStruct->rcItem.bottom - tm.tmHeight) / 2;
  rectText.right = pDIStruct->rcItem.right + XTEXTOFFSET;
  rectText.bottom =
	  (pDIStruct->rcItem.top + pDIStruct->rcItem.bottom + tm.tmHeight) / 2;

  /* Draw the icon from the image list */
  ImageList_Draw(ilItemImage,
                 sfi.iIcon,
                 pDIStruct->hDC,
                 rectIcon.left,
                 rectIcon.top,
                 ILD_TRANSPARENT );

  /* Draw the associated text */
  if(sfi.szDisplayName)
    TextOutA(pDIStruct->hDC,rectText.left,rectText.top,sfi.szDisplayName,strlen(sfi.szDisplayName));


  return NOERROR;
}

/***********************************************************************
 *      FILEDLG95_LOOKIN_OnCommand
 *
 * LookIn combo box WM_COMMAND message handler
 * If the function succeeds, the return value is nonzero.
 */
static BOOL FILEDLG95_LOOKIN_OnCommand(HWND hwnd, WORD wNotifyCode)
{
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("%p\n", fodInfos);

  switch(wNotifyCode)
  {
    case CBN_SELENDOK:
    {
      LPSFOLDER tmpFolder;
      int iItem;

      iItem = CBGetCurSel(fodInfos->DlgInfos.hwndLookInCB);

      if(!(tmpFolder = (LPSFOLDER) CBGetItemDataPtr(fodInfos->DlgInfos.hwndLookInCB,
                                               iItem)))
	return FALSE;


      if(SUCCEEDED(IShellBrowser_BrowseObject(fodInfos->Shell.FOIShellBrowser,
                                              tmpFolder->pidlItem,
                                              SBSP_ABSOLUTE)))
      {
        return TRUE;
      }
      break;
    }

  }
  return FALSE;
}

/***********************************************************************
 *      FILEDLG95_LOOKIN_AddItem
 *
 * Adds an absolute pidl item to the lookin combo box
 * returns the index of the inserted item
 */
static int FILEDLG95_LOOKIN_AddItem(HWND hwnd,LPITEMIDLIST pidl, int iInsertId)
{
  LPITEMIDLIST pidlNext;
  SHFILEINFOA sfi;
  SFOLDER *tmpFolder;
  LookInInfos *liInfos;

  TRACE("%08x\n", iInsertId);

  if(!pidl)
    return -1;

  if(!(liInfos = (LookInInfos *)GetPropA(hwnd,LookInInfosStr)))
    return -1;

  tmpFolder = MemAlloc(sizeof(SFOLDER));
  tmpFolder->m_iIndent = 0;

  /* Calculate the indentation of the item in the lookin*/
  pidlNext = pidl;
  while( (pidlNext=COMDLG32_PIDL_ILGetNext(pidlNext)) )
  {
    tmpFolder->m_iIndent++;
  }

  tmpFolder->pidlItem = COMDLG32_PIDL_ILClone(pidl); /* FIXME: memory leak*/

  if(tmpFolder->m_iIndent > liInfos->iMaxIndentation)
    liInfos->iMaxIndentation = tmpFolder->m_iIndent;

  sfi.dwAttributes = SFGAO_FILESYSANCESTOR | SFGAO_FILESYSTEM;
  SHGetFileInfoA((LPSTR)pidl,
                  0,
                  &sfi,
                  sizeof(sfi),
                  SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX
                  | SHGFI_PIDL | SHGFI_SMALLICON | SHGFI_ATTRIBUTES | SHGFI_ATTR_SPECIFIED);

  TRACE("-- Add %s attr=%08lx\n", sfi.szDisplayName, sfi.dwAttributes);

  if((sfi.dwAttributes & SFGAO_FILESYSANCESTOR) || (sfi.dwAttributes & SFGAO_FILESYSTEM))
  {
    int iItemID;

    TRACE("-- Add %s at %u\n", sfi.szDisplayName, tmpFolder->m_iIndent);

    /* Add the item at the end of the list */
    if(iInsertId < 0)
    {
      iItemID = CBAddString(hwnd,sfi.szDisplayName);
    }
    /* Insert the item at the iInsertId position*/
    else
    {
      iItemID = CBInsertString(hwnd,sfi.szDisplayName,iInsertId);
    }

    CBSetItemDataPtr(hwnd,iItemID,tmpFolder);
    return iItemID;
  }

  MemFree( tmpFolder );
  return -1;

}

/***********************************************************************
 *      FILEDLG95_LOOKIN_InsertItemAfterParent
 *
 * Insert an item below its parent
 */
static int FILEDLG95_LOOKIN_InsertItemAfterParent(HWND hwnd,LPITEMIDLIST pidl)
{

  LPITEMIDLIST pidlParent = GetParentPidl(pidl);
  int iParentPos;

  TRACE("\n");

  iParentPos = FILEDLG95_LOOKIN_SearchItem(hwnd,(WPARAM)pidlParent,SEARCH_PIDL);

  if(iParentPos < 0)
  {
    iParentPos = FILEDLG95_LOOKIN_InsertItemAfterParent(hwnd,pidlParent);
  }

  /* Free pidlParent memory */
  COMDLG32_SHFree((LPVOID)pidlParent);

  return FILEDLG95_LOOKIN_AddItem(hwnd,pidl,iParentPos + 1);
}

/***********************************************************************
 *      FILEDLG95_LOOKIN_SelectItem
 *
 * Adds an absolute pidl item to the lookin combo box
 * returns the index of the inserted item
 */
int FILEDLG95_LOOKIN_SelectItem(HWND hwnd,LPITEMIDLIST pidl)
{
  int iItemPos;
  LookInInfos *liInfos;

  TRACE("\n");

  iItemPos = FILEDLG95_LOOKIN_SearchItem(hwnd,(WPARAM)pidl,SEARCH_PIDL);

  liInfos = (LookInInfos *)GetPropA(hwnd,LookInInfosStr);

  if(iItemPos < 0)
  {
    while(FILEDLG95_LOOKIN_RemoveMostExpandedItem(hwnd) > -1);
    iItemPos = FILEDLG95_LOOKIN_InsertItemAfterParent(hwnd,pidl);
  }

  else
  {
    SFOLDER *tmpFolder = (LPSFOLDER) CBGetItemDataPtr(hwnd,iItemPos);
    while(liInfos->iMaxIndentation > tmpFolder->m_iIndent)
    {
      int iRemovedItem;

      if(-1 == (iRemovedItem = FILEDLG95_LOOKIN_RemoveMostExpandedItem(hwnd)))
        break;
      if(iRemovedItem < iItemPos)
        iItemPos--;
    }
  }

  CBSetCurSel(hwnd,iItemPos);
  liInfos->uSelectedItem = iItemPos;

  return 0;

}

/***********************************************************************
 *      FILEDLG95_LOOKIN_RemoveMostExpandedItem
 *
 * Remove the item with an expansion level over iExpansionLevel
 */
static int FILEDLG95_LOOKIN_RemoveMostExpandedItem(HWND hwnd)
{
  int iItemPos;

  LookInInfos *liInfos = (LookInInfos *)GetPropA(hwnd,LookInInfosStr);

  TRACE("\n");

  if(liInfos->iMaxIndentation <= 2)
    return -1;

  if((iItemPos = FILEDLG95_LOOKIN_SearchItem(hwnd,(WPARAM)liInfos->iMaxIndentation,SEARCH_EXP)) >=0)
  {
    SFOLDER *tmpFolder = (LPSFOLDER) CBGetItemDataPtr(hwnd,iItemPos);
    COMDLG32_SHFree(tmpFolder->pidlItem);
    MemFree(tmpFolder);
    CBDeleteString(hwnd,iItemPos);
    liInfos->iMaxIndentation--;

    return iItemPos;
  }

  return -1;
}

/***********************************************************************
 *      FILEDLG95_LOOKIN_SearchItem
 *
 * Search for pidl in the lookin combo box
 * returns the index of the found item
 */
static int FILEDLG95_LOOKIN_SearchItem(HWND hwnd,WPARAM searchArg,int iSearchMethod)
{
  int i = 0;
  int iCount = CBGetCount(hwnd);

  TRACE("0x%08x 0x%x\n",searchArg, iSearchMethod);

  if (iCount != CB_ERR)
  {
    for(;i<iCount;i++)
    {
      LPSFOLDER tmpFolder = (LPSFOLDER) CBGetItemDataPtr(hwnd,i);

      if(iSearchMethod == SEARCH_PIDL && COMDLG32_PIDL_ILIsEqual((LPITEMIDLIST)searchArg,tmpFolder->pidlItem))
        return i;
      if(iSearchMethod == SEARCH_EXP && tmpFolder->m_iIndent == (int)searchArg)
        return i;
    }
  }

  return -1;
}

/***********************************************************************
 *      FILEDLG95_LOOKIN_Clean
 *
 * Clean the memory used by the lookin combo box
 */
static void FILEDLG95_LOOKIN_Clean(HWND hwnd)
{
    FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);
    int iPos;
    int iCount = CBGetCount(fodInfos->DlgInfos.hwndLookInCB);

    TRACE("\n");

    /* Delete each string of the combo and their associated data */
    if (iCount != CB_ERR)
    {
      for(iPos = iCount-1;iPos>=0;iPos--)
      {
        SFOLDER *tmpFolder = (LPSFOLDER) CBGetItemDataPtr(fodInfos->DlgInfos.hwndLookInCB,iPos);
        COMDLG32_SHFree(tmpFolder->pidlItem);
        MemFree(tmpFolder);
        CBDeleteString(fodInfos->DlgInfos.hwndLookInCB,iPos);
      }
    }

    /* LookInInfos structure */
    RemovePropA(fodInfos->DlgInfos.hwndLookInCB,LookInInfosStr);

}
/***********************************************************************
 * FILEDLG95_FILENAME_FillFromSelection
 *
 * fills the edit box from the cached DataObject
 */
void FILEDLG95_FILENAME_FillFromSelection (HWND hwnd)
{
    FileOpenDlgInfos *fodInfos;
    LPITEMIDLIST      pidl;
    UINT              nFiles = 0, nFileToOpen, nFileSelected, nLength = 0;
    char              lpstrTemp[MAX_PATH];
    LPSTR             lpstrAllFile = NULL, lpstrCurrFile = NULL;

    TRACE("\n");
    fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

    /* Count how many files we have */
    nFileSelected = GetNumSelected( fodInfos->Shell.FOIDataObject );

    /* calculate the string length, count files */
    if (nFileSelected >= 1)
    {
      nLength += 3;	/* first and last quotes, trailing \0 */
      for ( nFileToOpen = 0; nFileToOpen < nFileSelected; nFileToOpen++ )
      {
        pidl = GetPidlFromDataObject( fodInfos->Shell.FOIDataObject, nFileToOpen+1 );

        if (pidl)
	{
          /* get the total length of the selected file names*/
          lpstrTemp[0] = '\0';
          GetName( fodInfos->Shell.FOIShellFolder, pidl, SHGDN_INFOLDER, lpstrTemp );

          if ( ! IsPidlFolder(fodInfos->Shell.FOIShellFolder, pidl) ) /* Ignore folders */
	  {
            nLength += strlen( lpstrTemp ) + 3;
            nFiles++;
	  }
          COMDLG32_SHFree( pidl );
	}
      }
    }

    /* allocate the buffer */
    if (nFiles <= 1) nLength = MAX_PATH;
    lpstrAllFile = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nLength);
    lpstrAllFile[0] = '\0';

    /* Generate the string for the edit control */
    if(nFiles >= 1)
    {
      lpstrCurrFile = lpstrAllFile;
      for ( nFileToOpen = 0; nFileToOpen < nFileSelected; nFileToOpen++ )
      {
        pidl = GetPidlFromDataObject( fodInfos->Shell.FOIDataObject, nFileToOpen+1 );

        if (pidl)
	{
	  /* get the file name */
          lpstrTemp[0] = '\0';
          GetName( fodInfos->Shell.FOIShellFolder, pidl, SHGDN_INFOLDER, lpstrTemp );

          if (! IsPidlFolder(fodInfos->Shell.FOIShellFolder, pidl)) /* Ignore folders */
	  {
            if ( nFiles > 1)
	    {
              *lpstrCurrFile++ =  '\"';
              strcpy( lpstrCurrFile, lpstrTemp );
              lpstrCurrFile += strlen( lpstrTemp );
              strcpy( lpstrCurrFile, "\" " );
              lpstrCurrFile += 2;
	    }
	    else
	    {
              strcpy( lpstrAllFile, lpstrTemp );
	    }
	  }
          COMDLG32_SHFree( (LPVOID) pidl );
	}
      }
    }
#ifdef __WIN32OS2__
    //Selecting a directory previously erased the last selected file
    if(nFiles) {
        SetWindowTextA( fodInfos->DlgInfos.hwndFileName, lpstrAllFile );
    }
#else
    SetWindowTextA( fodInfos->DlgInfos.hwndFileName, lpstrAllFile );
#endif
    HeapFree(GetProcessHeap(),0, lpstrAllFile );
}


/* copied from shell32 to avoid linking to it */
static HRESULT COMDLG32_StrRetToStrNA (LPVOID dest, DWORD len, LPSTRRET src, LPITEMIDLIST pidl)
{
	switch (src->uType)
	{
	  case STRRET_WSTR:
	    WideCharToMultiByte(CP_ACP, 0, src->DUMMYUNIONNAME_DOT pOleStr, -1, (LPSTR)dest, len, NULL, NULL);
	    COMDLG32_SHFree(src->DUMMYUNIONNAME_DOT pOleStr);
	    break;

	  case STRRET_CSTR:
	    lstrcpynA((LPSTR)dest, src->DUMMYUNIONNAME_DOT cStr, len);
	    break;

	  case STRRET_OFFSET:
	    lstrcpynA((LPSTR)dest, ((LPCSTR)&pidl->mkid)+src->DUMMYUNIONNAME_DOT uOffset, len);
	    break;

	  default:
	    FIXME("unknown type!\n");
	    if (len)
	    {
	      *(LPSTR)dest = '\0';
	    }
	    return(FALSE);
	}
	return S_OK;
}

/***********************************************************************
 * FILEDLG95_FILENAME_GetFileNames
 *
 * copies the filenames to a 0-delimited string list (A\0B\0C\0\0)
 */
int FILEDLG95_FILENAME_GetFileNames (HWND hwnd, LPSTR * lpstrFileList, UINT * sizeUsed)
{
	FileOpenDlgInfos *fodInfos  = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);
	UINT nStrCharCount = 0;	/* index in src buffer */
	UINT nFileIndex = 0;	/* index in dest buffer */
	UINT nFileCount = 0;	/* number of files */
	UINT nStrLen = 0;	/* length of string in edit control */
	LPSTR lpstrEdit;	/* buffer for string from edit control */

	TRACE("\n");

	/* get the filenames from the edit control */
	nStrLen = SendMessageA(fodInfos->DlgInfos.hwndFileName, WM_GETTEXTLENGTH, 0, 0);
	lpstrEdit = MemAlloc(nStrLen+1);
	GetDlgItemTextA(hwnd, IDC_FILENAME, lpstrEdit, nStrLen+1);

	TRACE("nStrLen=%u str=%s\n", nStrLen, lpstrEdit);

	/* we might get single filename without any '"',
	 * so we need nStrLen + terminating \0 + end-of-list \0 */
	*lpstrFileList = MemAlloc(nStrLen+2);
	*sizeUsed = 0;

	/* build 0-delimited file list from filenames */
	while ( nStrCharCount <= nStrLen )
	{
	  if ( lpstrEdit[nStrCharCount]=='"' )
	  {
	    nStrCharCount++;
	    while ((lpstrEdit[nStrCharCount]!='"') && (nStrCharCount <= nStrLen))
	    {
	      (*lpstrFileList)[nFileIndex++] = lpstrEdit[nStrCharCount];
	      (*sizeUsed)++;
	      nStrCharCount++;
	    }
	    (*lpstrFileList)[nFileIndex++] = '\0';
	    (*sizeUsed)++;
	    nFileCount++;
	  }
	  nStrCharCount++;
	}

	/* single, unquoted string */
	if ((nStrLen > 0) && (*sizeUsed == 0) )
	{
	  strcpy(*lpstrFileList, lpstrEdit);
	  nFileIndex = strlen(lpstrEdit) + 1;
	  (*sizeUsed) = nFileIndex;
	  nFileCount = 1;
	}

	/* trailing \0 */
	(*lpstrFileList)[nFileIndex] = '\0';
	(*sizeUsed)++;

	MemFree(lpstrEdit);
	return nFileCount;
}

#define SETDefFormatEtc(fe,cf,med) \
{ \
    (fe).cfFormat = cf;\
    (fe).dwAspect = DVASPECT_CONTENT; \
    (fe).ptd =NULL;\
    (fe).tymed = med;\
    (fe).lindex = -1;\
};

/*
 * DATAOBJECT Helper functions
 */

/***********************************************************************
 * COMCTL32_ReleaseStgMedium
 *
 * like ReleaseStgMedium from ole32
 */
static void COMCTL32_ReleaseStgMedium (STGMEDIUM medium)
{
      if(medium.pUnkForRelease)
      {
        IUnknown_Release(medium.pUnkForRelease);
      }
      else
      {
        GlobalUnlock(medium.DUMMYUNIONNAME_DOT hGlobal);
        GlobalFree(medium.DUMMYUNIONNAME_DOT hGlobal);
      }
}

/***********************************************************************
 *          GetPidlFromDataObject
 *
 * Return pidl(s) by number from the cached DataObject
 *
 * nPidlIndex=0 gets the fully qualified root path
 */
LPITEMIDLIST GetPidlFromDataObject ( IDataObject *doSelected, UINT nPidlIndex)
{

    STGMEDIUM medium;
    FORMATETC formatetc;
    LPITEMIDLIST pidl = NULL;

    TRACE("sv=%p index=%u\n", doSelected, nPidlIndex);

    /* Set the FORMATETC structure*/
    SETDefFormatEtc(formatetc, RegisterClipboardFormatA(CFSTR_SHELLIDLIST), TYMED_HGLOBAL);

    /* Get the pidls from IDataObject */
    if(SUCCEEDED(IDataObject_GetData(doSelected,&formatetc,&medium)))
    {
      LPIDA cida = GlobalLock(medium.DUMMYUNIONNAME_DOT hGlobal);
      if(nPidlIndex <= cida->cidl)
      {
        pidl = COMDLG32_PIDL_ILClone((LPITEMIDLIST)(&((LPBYTE)cida)[cida->aoffset[nPidlIndex]]));
      }
      COMCTL32_ReleaseStgMedium(medium);
    }
    return pidl;
}

/***********************************************************************
 *          GetNumSelected
 *
 * Return the number of selected items in the DataObject.
 *
*/
UINT GetNumSelected( IDataObject *doSelected )
{
    UINT retVal = 0;
    STGMEDIUM medium;
    FORMATETC formatetc;

    TRACE("sv=%p\n", doSelected);

    if (!doSelected) return 0;

    /* Set the FORMATETC structure*/
    SETDefFormatEtc(formatetc, RegisterClipboardFormatA(CFSTR_SHELLIDLIST), TYMED_HGLOBAL);

    /* Get the pidls from IDataObject */
    if(SUCCEEDED(IDataObject_GetData(doSelected,&formatetc,&medium)))
    {
      LPIDA cida = GlobalLock(medium.DUMMYUNIONNAME_DOT hGlobal);
      retVal = cida->cidl;
      COMCTL32_ReleaseStgMedium(medium);
      return retVal;
    }
    return 0;
}

/*
 * TOOLS
 */

/***********************************************************************
 *      GetName
 *
 * Get the pidl's display name (relative to folder) and
 * put it in lpstrFileName.
 *
 * Return NOERROR on success,
 * E_FAIL otherwise
 */

HRESULT GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST pidl,DWORD dwFlags,LPSTR lpstrFileName)
{
  STRRET str;
  HRESULT hRes;

  TRACE("sf=%p pidl=%p\n", lpsf, pidl);

  if(!lpsf)
  {
    HRESULT hRes;
    SHGetDesktopFolder(&lpsf);
    hRes = GetName(lpsf,pidl,dwFlags,lpstrFileName);
    IShellFolder_Release(lpsf);
    return hRes;
  }

  /* Get the display name of the pidl relative to the folder */
  if (SUCCEEDED(hRes = IShellFolder_GetDisplayNameOf(lpsf, pidl, dwFlags, &str)))
  {
      return COMDLG32_StrRetToStrNA(lpstrFileName, MAX_PATH, &str, pidl);
  }
  return E_FAIL;
}

/***********************************************************************
 *      GetShellFolderFromPidl
 *
 * pidlRel is the item pidl relative
 * Return the IShellFolder of the absolute pidl
 */
IShellFolder *GetShellFolderFromPidl(LPITEMIDLIST pidlAbs)
{
  IShellFolder *psf = NULL,*psfParent;

  TRACE("%p\n", pidlAbs);

  if(SUCCEEDED(SHGetDesktopFolder(&psfParent)))
  {
    psf = psfParent;
    if(pidlAbs && pidlAbs->mkid.cb)
    {
      if(SUCCEEDED(IShellFolder_BindToObject(psfParent, pidlAbs, NULL, &IID_IShellFolder, (LPVOID*)&psf)))
      {
	IShellFolder_Release(psfParent);
        return psf;
      }
    }
    /* return the desktop */
    return psfParent;
  }
  return NULL;
}

/***********************************************************************
 *      GetParentPidl
 *
 * Return the LPITEMIDLIST to the parent of the pidl in the list
 */
LPITEMIDLIST GetParentPidl(LPITEMIDLIST pidl)
{
  LPITEMIDLIST pidlParent;

  TRACE("%p\n", pidl);

  pidlParent = COMDLG32_PIDL_ILClone(pidl);
  COMDLG32_PIDL_ILRemoveLastID(pidlParent);

  return pidlParent;
}

/***********************************************************************
 *      GetPidlFromName
 *
 * returns the pidl of the file name relative to folder
 * NULL if an error occurred
 */
LPITEMIDLIST GetPidlFromName(IShellFolder *lpsf,LPCSTR lpcstrFileName)
{
  LPITEMIDLIST pidl;
  ULONG ulEaten;
  WCHAR lpwstrDirName[MAX_PATH];

  TRACE("sf=%p file=%s\n", lpsf, lpcstrFileName);

  if(!lpcstrFileName) return NULL;
  if(!*lpcstrFileName) return NULL;

  MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,lpcstrFileName,-1,(LPWSTR)lpwstrDirName,MAX_PATH);

  if(!lpsf)
  {
    SHGetDesktopFolder(&lpsf);
    pidl = GetPidlFromName(lpsf, lpcstrFileName);
    IShellFolder_Release(lpsf);
  }
  else
  {
    IShellFolder_ParseDisplayName(lpsf, 0, NULL, (LPWSTR)lpwstrDirName, &ulEaten, &pidl, NULL);
  }
  return pidl;
}

/*
*/
BOOL IsPidlFolder (LPSHELLFOLDER psf, LPITEMIDLIST pidl)
{
	ULONG uAttr  = SFGAO_FOLDER | SFGAO_HASSUBFOLDER;
	HRESULT ret;

	TRACE("%p, %p\n", psf, pidl);

        ret = IShellFolder_GetAttributesOf( psf, 1, &pidl, &uAttr );

	TRACE("-- 0x%08lx 0x%08lx\n", uAttr, ret);
	/* see documentation shell 4.1*/
        return uAttr & (SFGAO_FOLDER | SFGAO_HASSUBFOLDER);
}

/***********************************************************************
 *      BrowseSelectedFolder
 */
static BOOL BrowseSelectedFolder(HWND hwnd)
{
  BOOL bBrowseSelFolder = FALSE;
  FileOpenDlgInfos *fodInfos = (FileOpenDlgInfos *) GetPropA(hwnd,FileOpenDlgInfosStr);

  TRACE("\n");

  if (GetNumSelected(fodInfos->Shell.FOIDataObject) == 1)
  {
      LPITEMIDLIST pidlSelection;

      /* get the file selected */
      pidlSelection  = GetPidlFromDataObject( fodInfos->Shell.FOIDataObject, 1);
      if (IsPidlFolder (fodInfos->Shell.FOIShellFolder, pidlSelection))
      {
          if ( FAILED( IShellBrowser_BrowseObject( fodInfos->Shell.FOIShellBrowser,
                         pidlSelection, SBSP_RELATIVE ) ) )
          {
               MessageBoxA( hwnd, "Path does not exist", fodInfos->ofnInfos->lpstrTitle,
                      MB_OK | MB_ICONEXCLAMATION );
          }

         bBrowseSelFolder = TRUE;
      }
      COMDLG32_SHFree( pidlSelection );
  }

  return bBrowseSelFolder;
}

/*
 * Memory allocation methods */
static void *MemAlloc(UINT size)
{
    return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);
}

static void MemFree(void *mem)
{
    if(mem)
    {
        HeapFree(GetProcessHeap(),0,mem);
    }
}

