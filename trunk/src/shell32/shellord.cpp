/* $Id: shellord.cpp,v 1.10 2000-08-18 02:01:22 phaller Exp $ */
/*
 * The parameters of many functions changes between different OS versions
 * (NT uses Unicode strings, 95 uses ASCII strings)
 *
 * Copyright 1997 Marcus Meissner
 *           1998 Jürgen Schmied
 *
 * Corel WINE 20000324 level
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <string.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "winerror.h"
#include "winreg.h"
#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "heap.h"

#include "shellapi.h"
#include "shlobj.h"
#include "shell32_main.h"
#include "wine/undocshell.h"
#include "shpolicy.h"

#include <heapstring.h>
#include <misc.h>
#include <ctype.h>
#include <wctype.h>
#include <wcstr.h>


/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(shell32-shellord)


/*************************************************************************
 * ParseField                                   [SHELL32.58]
 *
 */
DWORD WINAPI ParseFieldA(LPCSTR src, DWORD field, LPSTR dst, DWORD len)
{       WARN("('%s',0x%08lx,%p,%ld) semi-stub.\n",src,field,dst,len);

        if (!src || !src[0] || !dst || !len)
          return 0;

        if (field >1)
        { field--;
          while (field)
          { if (*src==0x0) return FALSE;
            if (*src==',') field--;
            src++;
          }
        }

        while (*src!=0x00 && *src!=',' && len>0)
        { *dst=*src; dst++, src++; len--;
        }
        *dst=0x0;

        return TRUE;
}


/*************************************************************************
 * GetFileNameFromBrowse                        [SHELL32.63]
 *
 */
ODINFUNCTION7(BOOL,   GetFileNameFromBrowse,
              HWND,   hwndOwner,
              LPSTR,  lpstrFile,
              DWORD,  nMaxFile,
              LPCSTR, lpstrInitialDir,
              LPCSTR, lpstrDefExt,
              LPCSTR, lpstrFIlter,
              LPCSTR, lpstrTitle)
{
  dprintf(("SHELL32: GetFileNameFromBrowse not implemented"));

    /* puts up a Open Dialog and requests input into targetbuf */
    /* OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST|OFN_unknown */
  lstrcpynA(lpstrFile,
            "x:\\dummy.exe",
            nMaxFile);
  return 1;
}

/*************************************************************************
 * SHGetSettings                                [SHELL32.68]
 *
 * NOTES
 *  the registry path are for win98 (tested)
 *  and possibly are the same in nt40
 */
void WINAPI SHGetSettings(LPSHELLFLAGSTATE lpsfs, DWORD dwMask, DWORD dwx)
{
        HKEY    hKey;
        DWORD   dwData;
        DWORD   dwDataSize = sizeof (DWORD);

        TRACE("(%p 0x%08lx 0x%08lx)\n",lpsfs,dwMask, dwx);

        if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                                 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, 0))
          return;

        if ( (SSF_SHOWEXTENSIONS & dwMask) && !RegQueryValueExA(hKey, "HideFileExt", 0, 0, (LPBYTE)&dwData, &dwDataSize))
          lpsfs->fShowExtensions  = ((dwData == 0) ?  0 : 1);

        if ( (SSF_SHOWINFOTIP & dwMask) && !RegQueryValueExA(hKey, "ShowInfoTip", 0, 0, (LPBYTE)&dwData, &dwDataSize))
          lpsfs->fShowInfoTip  = ((dwData == 0) ?  0 : 1);

        if ( (SSF_DONTPRETTYPATH & dwMask) && !RegQueryValueExA(hKey, "DontPrettyPath", 0, 0, (LPBYTE)&dwData, &dwDataSize))
          lpsfs->fDontPrettyPath  = ((dwData == 0) ?  0 : 1);

        if ( (SSF_HIDEICONS & dwMask) && !RegQueryValueExA(hKey, "HideIcons", 0, 0, (LPBYTE)&dwData, &dwDataSize))
          lpsfs->fHideIcons  = ((dwData == 0) ?  0 : 1);

        if ( (SSF_MAPNETDRVBUTTON & dwMask) && !RegQueryValueExA(hKey, "MapNetDrvBtn", 0, 0, (LPBYTE)&dwData, &dwDataSize))
          lpsfs->fMapNetDrvBtn  = ((dwData == 0) ?  0 : 1);

        if ( (SSF_SHOWATTRIBCOL & dwMask) && !RegQueryValueExA(hKey, "ShowAttribCol", 0, 0, (LPBYTE)&dwData, &dwDataSize))
          lpsfs->fShowAttribCol  = ((dwData == 0) ?  0 : 1);

        if (((SSF_SHOWALLOBJECTS | SSF_SHOWSYSFILES) & dwMask) && !RegQueryValueExA(hKey, "Hidden", 0, 0, (LPBYTE)&dwData, &dwDataSize))
        { if (dwData == 0)
          { if (SSF_SHOWALLOBJECTS & dwMask)    lpsfs->fShowAllObjects  = 0;
            if (SSF_SHOWSYSFILES & dwMask)      lpsfs->fShowSysFiles  = 0;
          }
          else if (dwData == 1)
          { if (SSF_SHOWALLOBJECTS & dwMask)    lpsfs->fShowAllObjects  = 1;
            if (SSF_SHOWSYSFILES & dwMask)      lpsfs->fShowSysFiles  = 0;
          }
          else if (dwData == 2)
          { if (SSF_SHOWALLOBJECTS & dwMask)    lpsfs->fShowAllObjects  = 0;
            if (SSF_SHOWSYSFILES & dwMask)      lpsfs->fShowSysFiles  = 1;
          }
        }
        RegCloseKey (hKey);

        TRACE("-- 0x%04x\n", *(WORD*)lpsfs);
}

/*************************************************************************
 * SHShellFolderView_Message                    [SHELL32.73]
 *
 * PARAMETERS
 *  hwndCabinet defines the explorer cabinet window that contains the
 *              shellview you need to communicate with
 *  uMsg        identifying the SFVM enum to perform
 *  lParam
 *
 * NOTES
 *  Message SFVM_REARRANGE = 1
 *    This message gets sent when a column gets clicked to instruct the
 *    shell view to re-sort the item list. lParam identifies the column
 *    that was clicked.
 */
ODINFUNCTION3(int, SHShellFolderView_Message,
              HWND, hwndCabinet,
              DWORD, dwMessage,
              DWORD, dwParam)
{
  dprintf(("SHELL32: SHShellFolderView_Message not implemented"));
  return 0;
}


/*************************************************************************
 * RegisterShellHook                            [SHELL32.181]
 *
 * PARAMS
 *      hwnd [I]  window handle
 *      y    [I]  flag ????
 *
 * NOTES
 *     exported by ordinal
 */
ODINFUNCTION2(BOOL,  RegisterShellHook,
              HWND,  hWnd,
              DWORD, dwType)
{
  dprintf(("SHELL32: RegisterShellHook not implemented"));
  return FALSE;
}
/*************************************************************************
 * ShellMessageBoxW                             [SHELL32.182]
 *
 * Format and output errormessage.
 *
 * idText       resource ID of title or LPSTR
 * idTitle      resource ID of title or LPSTR
 *
 * NOTES
 *     exported by ordinal
 */
INT __cdecl
ShellMessageBoxW(HMODULE hmod,HWND hwnd,DWORD idText,DWORD idTitle,DWORD uType,LPCVOID arglist)
{       WCHAR   szText[100],szTitle[100],szTemp[256];
        LPWSTR   pszText = &szText[0], pszTitle = &szTitle[0];
        LPVOID  args = &arglist;

        TRACE("(%08lx,%08lx,%08lx,%08lx,%08lx,%p)\n",(DWORD)hmod,(DWORD)hwnd,idText,idTitle,uType,arglist);

        if (!HIWORD (idTitle))
          LoadStringW(hmod,idTitle,pszTitle,100);
        else
          pszTitle = (LPWSTR)idTitle;

        if (! HIWORD (idText))
          LoadStringW(hmod,idText,pszText,100);
        else
          pszText = (LPWSTR)idText;

        FormatMessageW(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                szText,
                0,
                0,
                szTemp,
                256,
                (LPDWORD)args);
        return MessageBoxW(hwnd,szTemp,szTitle,uType);
}

/*************************************************************************
 * ShellMessageBoxA                             [SHELL32.183]
 */
INT __cdecl
ShellMessageBoxA(HMODULE hmod,HWND hwnd,DWORD idText,DWORD idTitle,DWORD uType,LPCVOID arglist)
{       char    szText[100],szTitle[100],szTemp[256];
        LPSTR   pszText = &szText[0], pszTitle = &szTitle[0];
        LPVOID  args = &arglist;

        TRACE("(%08lx,%08lx,%08lx,%08lx,%08lx,%p)\n", (DWORD)hmod,(DWORD)hwnd,idText,idTitle,uType,arglist);

        if (!HIWORD (idTitle))
          LoadStringA(hmod,idTitle,pszTitle,100);
        else
          pszTitle = (LPSTR)idTitle;

        if (! HIWORD (idText))
          LoadStringA(hmod,idText,pszText,100);
        else
          pszText = (LPSTR)idText;

        FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY ,pszText,0,0,szTemp,256,(LPDWORD)args);
        return MessageBoxA(hwnd,szTemp,pszTitle,uType);
}


/*************************************************************************
 * SHFree                                       [SHELL32.195]
 *
 * NOTES
 *     free_ptr() - frees memory using IMalloc
 *     exported by ordinal
 */
#define MEM_DEBUG 0
void WINAPI SHFree(LPVOID x)
{
#if MEM_DEBUG
  WORD len = *(LPWORD)(x-2);

  if ( *(LPWORD)(x+len) != 0x7384)
    ERR("MAGIC2!\n");

  if ( (*(LPWORD)(x-4)) != 0x8271)
    ERR("MAGIC1!\n");
  else
    memset(x-4, 0xde, len+6);

  TRACE("%p len=%u\n",x, len);

  x -= 4;
#else
  TRACE("%p\n",x);
#endif
  
  HeapFree(GetProcessHeap(), 0, x);
}

/*************************************************************************
 * SHAlloc                                      [SHELL32.196]
 *
 * NOTES
 *     void *task_alloc(DWORD len), uses SHMalloc allocator
 *     exported by ordinal
 */
LPVOID WINAPI SHAlloc(DWORD len)
{
        LPBYTE ret;

#if MEM_DEBUG
        ret = (LPBYTE) HeapAlloc(GetProcessHeap(),0,len+6);
#else
        ret = (LPBYTE) HeapAlloc(GetProcessHeap(),0,len);
#endif

#if MEM_DEBUG
        *(LPWORD)(ret) = 0x8271;
        *(LPWORD)(ret+2) = (WORD)len;
        *(LPWORD)(ret+4+len) = 0x7384;
        ret += 4;
        memset(ret, 0xdf, len);
#endif
        TRACE("%lu bytes at %p\n",len, ret);
        return (LPVOID)ret;
}

/*************************************************************************
 * SHRegisterDragDrop                           [SHELL32.86]
 *
 * NOTES
 *     exported by ordinal
 */
ODINFUNCTION2(HRESULT,      SHRegisterDragDrop,
              HWND,         hWnd,
              LPDROPTARGET, lpDropTarget)
{
  dprintf(("SHELL32: SHRegisterDragDrop not correctly implemented"));
  return     RegisterDragDrop(hWnd, lpDropTarget);
}

/*************************************************************************
 * SHRevokeDragDrop                             [SHELL32.87]
 *
 * NOTES
 *     exported by ordinal
 */
ODINFUNCTION1(HRESULT, SHRevokeDragDrop,
              HWND,    hWnd)
{
  dprintf(("SHELL32: SHRevokeDragDrop not implemented"));
  return 0;
}

/*************************************************************************
 * SHDoDragDrop                                 [SHELL32.88]
 *
 * NOTES
 *     exported by ordinal
 */
ODINFUNCTION5(HRESULT,      SHDoDragDrop,
              HWND,         hWnd,
              LPDATAOBJECT, lpDataObject,
              LPDROPSOURCE, lpDropSource,
              DWORD,        dwOKEffect,
              LPDWORD,      pdwEffect)
{
  dprintf(("SHELL32: SHDoDragDrop not implemented"));
  return 0;
}


/*************************************************************************
 * ArrangeWindows                               [SHELL32.184]
 *
 */
ODINFUNCTION5(WORD,        ArrangeWindows,
              HWND,        hwndParent,
              DWORD,       dwReserved,
              LPCRECT,     lpRect,
              WORD,        cKids,
              CONST HWND*, lpKids)
{
  dprintf(("SHELL32: ArrangeWindows not implemented"));
  return 0;
}

/*************************************************************************
 * SignalFileOpen                               [SHELL32.103]
 *
 * NOTES
 *     exported by ordinal
 */
DWORD WINAPI
SignalFileOpen (DWORD dwParam1)
{
    FIXME("(0x%08lx):stub.\n", dwParam1);

    return 0;
}

/*************************************************************************
 * SHAddToRecentDocs                            [SHELL32.234]
 *
 * PARAMETERS
 *   uFlags  [IN] SHARD_PATH or SHARD_PIDL
 *   pv      [IN] string or pidl, NULL clears the list
 *
 * NOTES
 *     exported by name
 */
DWORD WINAPI SHAddToRecentDocs (UINT uFlags,LPCVOID pv)
{ if (SHARD_PIDL==uFlags)
  { FIXME("(0x%08x,pidl=%p):stub.\n", uFlags,pv);
        }
        else
        { FIXME("(0x%08x,%s):stub.\n", uFlags,(char*)pv);
        }
  return 0;
}

/*************************************************************************
 * SHChangeNotify                               [SHELL32.239]
 *
 * NOTES
 *     exported by name
 */
DWORD WINAPI SHChangeNotify (
    INT   wEventId,  /* [IN] flags that specifies the event*/
    UINT  uFlags,   /* [IN] the meaning of dwItem[1|2]*/
                LPCVOID dwItem1,
                LPCVOID dwItem2)
{ FIXME("(0x%08x,0x%08ux,%p,%p):stub.\n", wEventId,uFlags,dwItem1,dwItem2);
  return 0;
}
/*************************************************************************
 * SHCreateShellFolderViewEx                    [SHELL32.174]
 *
 * NOTES
 *  see IShellFolder::CreateViewObject
 */
ODINFUNCTION2(HRESULT, SHCreateShellFolderViewEx,
              LPCSHELLFOLDERVIEWINFO, pshfvi,
              LPSHELLVIEW*, ppshv)
{
  IShellView * psf;
  HRESULT hRes;

  psf = IShellView_Constructor(pshfvi->pshf);

  if (!psf)
    return E_OUTOFMEMORY;

  IShellView_AddRef(psf);
  hRes = IShellView_QueryInterface(psf, &IID_IShellView, (LPVOID *)ppshv);
  IShellView_Release(psf);

  return hRes;
}
/*************************************************************************
 *  SHWinHelp                                   [SHELL32.127]
 *
 */
HRESULT WINAPI SHWinHelp (DWORD v, DWORD w, DWORD x, DWORD z)
{       FIXME("0x%08lx 0x%08lx 0x%08lx 0x%08lx stub\n",v,w,x,z);
        return 0;
}
/*************************************************************************
 *  SHRunControlPanel [SHELL32.161]
 *
 */
HRESULT WINAPI SHRunControlPanel (DWORD x, DWORD z)
{       FIXME("0x%08lx 0x%08lx stub\n",x,z);
        return 0;
}
/*************************************************************************
 * ShellExecuteEx                               [SHELL32.291]
 *
 */
BOOL WINAPI ShellExecuteExAW (LPVOID sei)
{       if (VERSION_OsIsUnicode())
          return ShellExecuteExW ((LPSHELLEXECUTEINFOW)sei);
        return ShellExecuteExA ((LPSHELLEXECUTEINFOA)sei);
}
/*************************************************************************
 * ShellExecuteExA				[SHELL32.292]
 *
 * placeholder in the commandline:
 *	%1 file
 *	%2 printer
 *	%3 driver
 *	%4 port
 *	%I adress of a global item ID (explorer switch /idlist)
 *	%L ??? path/url/current file ???
 *	%S ???
 *	%* all following parameters (see batfile)
 */
#include "process.h" /* we can get rid of it hopefully */
#include "task.h"
BOOL WINAPI ShellExecuteExA (LPSHELLEXECUTEINFOA sei)
{ 	CHAR szApplicationName[MAX_PATH],szCommandline[MAX_PATH],szPidl[20];
	LPSTR pos;
	int gap, len;
	STARTUPINFOA  startup;
	PROCESS_INFORMATION info;

	WARN("mask=0x%08lx hwnd=0x%04x verb=%s file=%s parm=%s dir=%s show=0x%08x class=%s incomplete\n",
		sei->fMask, sei->hwnd, sei->lpVerb, sei->lpFile,
		sei->lpParameters, sei->lpDirectory, sei->nShow,
		(sei->fMask & SEE_MASK_CLASSNAME) ? sei->lpClass : "not used");

	ZeroMemory(szApplicationName,MAX_PATH);
	if (sei->lpFile)
	  strcpy(szApplicationName, sei->lpFile);

	ZeroMemory(szCommandline,MAX_PATH);
	if (sei->lpParameters)
	  strcpy(szCommandline, sei->lpParameters);

	if (sei->fMask & (SEE_MASK_CLASSKEY | SEE_MASK_INVOKEIDLIST | SEE_MASK_ICON | SEE_MASK_HOTKEY |
			  SEE_MASK_CONNECTNETDRV | SEE_MASK_FLAG_DDEWAIT |
			  SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI | SEE_MASK_UNICODE |
			  SEE_MASK_NO_CONSOLE | SEE_MASK_ASYNCOK | SEE_MASK_HMONITOR ))
	{
	  FIXME("flags ignored: 0x%08lx\n", sei->fMask);
	}

	/* launch a document by fileclass like 'Wordpad.Document.1' */
	if (sei->fMask & SEE_MASK_CLASSNAME)
	{
	  /* the commandline contains 'c:\Path\wordpad.exe "%1"' */
	  HCR_GetExecuteCommand(sei->lpClass, (sei->lpVerb) ? sei->lpVerb : "open", szCommandline, 256);
	  /* fixme: get the extension of lpFile, check if it fits to the lpClass */
	  TRACE("SEE_MASK_CLASSNAME->'%s'\n", szCommandline);
	}

	/* process the IDList */
	if ( (sei->fMask & SEE_MASK_INVOKEIDLIST) == SEE_MASK_INVOKEIDLIST) /*0x0c*/
	{
	  SHGetPathFromIDListA ((LPCITEMIDLIST)sei->lpIDList,szApplicationName);
	  TRACE("-- idlist=%p (%s)\n", sei->lpIDList, szApplicationName);
	}
	else
	{
	  if (sei->fMask & SEE_MASK_IDLIST )
	  {
	    pos = strstr(szCommandline, "%I");
	    if (pos)
	    {
	      LPVOID pv;
	      HGLOBAL hmem = SHAllocShared ( (LPITEMIDLIST)sei->lpIDList, ILGetSize((LPITEMIDLIST)sei->lpIDList), 0);
	      pv = SHLockShared(hmem,0);
	      sprintf(szPidl,":%p",pv );
	      SHUnlockShared(pv);

	      gap = strlen(szPidl);
	      len = strlen(pos)-2;
	      memmove(pos+gap,pos+2,len);
	      memcpy(pos,szPidl,gap);

	    }
	  }
	}

	TRACE("execute:'%s','%s'\n",szApplicationName, szCommandline);

	strcat(szApplicationName, " ");
	strcat(szApplicationName, szCommandline);

	ZeroMemory(&startup,sizeof(STARTUPINFOA));
	startup.cb = sizeof(STARTUPINFOA);

	if (! CreateProcessA(NULL, szApplicationName,
			 NULL, NULL, FALSE, 0,
			 NULL, NULL, &startup, &info))
	{
	  sei->hInstApp = GetLastError();
	  return FALSE;
	}

        sei->hInstApp = 33;

    	/* Give 30 seconds to the app to come up */
	if ( WaitForInputIdle ( info.hProcess, 30000 ) ==  0xFFFFFFFF )
	  ERR("WaitForInputIdle failed: Error %ld\n", GetLastError() );

	if(sei->fMask & SEE_MASK_NOCLOSEPROCESS)
	  sei->hProcess = info.hProcess;
        else
          CloseHandle( info.hProcess );
        CloseHandle( info.hThread );
	return TRUE;
}

/*************************************************************************
 * ShellExecuteExW				[SHELL32.293]
 *
 */
BOOL WINAPI ShellExecuteExW (LPSHELLEXECUTEINFOW sei)
{	SHELLEXECUTEINFOA seiA;
	DWORD ret;

	TRACE("%p\n", sei);

	memcpy(&seiA, sei, sizeof(SHELLEXECUTEINFOA));

        if (sei->lpVerb)
	  seiA.lpVerb = HEAP_strdupWtoA( GetProcessHeap(), 0, sei->lpVerb);

        if (sei->lpFile)
	  seiA.lpFile = HEAP_strdupWtoA( GetProcessHeap(), 0, sei->lpFile);

        if (sei->lpParameters)
	  seiA.lpParameters = HEAP_strdupWtoA( GetProcessHeap(), 0, sei->lpParameters);

	if (sei->lpDirectory)
	  seiA.lpDirectory = HEAP_strdupWtoA( GetProcessHeap(), 0, sei->lpDirectory);

        if ((sei->fMask & SEE_MASK_CLASSNAME) && sei->lpClass)
	  seiA.lpClass = HEAP_strdupWtoA( GetProcessHeap(), 0, sei->lpClass);
	else
	  seiA.lpClass = NULL;

	ret = ShellExecuteExA(&seiA);

        if (seiA.lpVerb)	HeapFree( GetProcessHeap(), 0, (LPSTR) seiA.lpVerb );
	if (seiA.lpFile)	HeapFree( GetProcessHeap(), 0, (LPSTR) seiA.lpFile );
	if (seiA.lpParameters)	HeapFree( GetProcessHeap(), 0, (LPSTR) seiA.lpParameters );
	if (seiA.lpDirectory)	HeapFree( GetProcessHeap(), 0, (LPSTR) seiA.lpDirectory );
	if (seiA.lpClass)	HeapFree( GetProcessHeap(), 0, (LPSTR) seiA.lpClass );

 	return ret;
}

static LPUNKNOWN SHELL32_IExplorerInterface=0;
/*************************************************************************
 * SHSetInstanceExplorer                        [SHELL32.176]
 *
 * NOTES
 *  Sets the interface
 */
HRESULT WINAPI SHSetInstanceExplorer (LPUNKNOWN lpUnknown)
{       TRACE("%p\n", lpUnknown);
        SHELL32_IExplorerInterface = lpUnknown;
        return (HRESULT) lpUnknown;
}
/*************************************************************************
 * SHGetInstanceExplorer                        [SHELL32.256]
 *
 * NOTES
 *  gets the interface pointer of the explorer and a reference
 */
HRESULT WINAPI SHGetInstanceExplorer (LPUNKNOWN * lpUnknown)
{       TRACE("%p\n", lpUnknown);

        *lpUnknown = SHELL32_IExplorerInterface;

        if (!SHELL32_IExplorerInterface)
          return E_FAIL;

        IUnknown_AddRef(SHELL32_IExplorerInterface);
        return NOERROR;
}
/*************************************************************************
 * SHFreeUnusedLibraries                        [SHELL32.123]
 *
 * NOTES
 *  exported by name
 */
ODINPROCEDURE0(SHFreeUnusedLibraries)
{ 
  dprintf(("SHELL32: SHFreeUnusedLibraries not implemented"));
}
/*************************************************************************
 * DAD_SetDragImage                             [SHELL32.136]
 *
 * NOTES
 *  exported by name
 */
ODINFUNCTION2(BOOL, DAD_SetDragImage,
              HIMAGELIST, himlTrack,
              LPPOINT, lppt)
{ 
  dprintf(("SHELL32: DAD_SetDragImage not implemented"));
  return 0;
}
/*************************************************************************
 * DAD_ShowDragImage                            [SHELL32.137]
 *
 * NOTES
 *  exported by name
 */
ODINFUNCTION1(BOOL, DAD_ShowDragImage,
              BOOL, bShow)
{ 
  dprintf(("SHELL32: DAD_ShowDragImage not implemented"));
  return 0;
}

/*************************************************************************
 * ReadCabinetState                             [NT 4.0:SHELL32.651]
 *
 */
HRESULT WINAPI ReadCabinetState(DWORD u, DWORD v)
{       FIXME("0x%04lx 0x%04lx stub\n",u,v);
        return 0;
}
/*************************************************************************
 * WriteCabinetState                            [NT 4.0:SHELL32.652]
 *
 */
HRESULT WINAPI WriteCabinetState(DWORD u)
{       FIXME("0x%04lx stub\n",u);
        return 0;
}
/*************************************************************************
 * FileIconInit                                 [SHELL32.660]
 *
 */
BOOL WINAPI FileIconInit(BOOL bFullInit)
{       FIXME("(%s)\n", bFullInit ? "true" : "false");
        return 0;
}
/*************************************************************************
 * IsUserAdmin                                  [NT 4.0:SHELL32.680]
 *
 */
HRESULT WINAPI IsUserAdmin(void)
{       FIXME("stub\n");
        return TRUE;
}
/*************************************************************************
 * StrRetToStrN                                 [SHELL32.96]
 *
 * converts a STRRET to a normal string
 *
 * NOTES
 *  the pidl is for STRRET OFFSET
 */
HRESULT WINAPI StrRetToBufA (LPSTRRET src, LPITEMIDLIST pidl, LPSTR dest, DWORD len)
{
        return StrRetToStrNA(dest, len, src, pidl);
}

HRESULT WINAPI StrRetToBufW (LPSTRRET src, LPITEMIDLIST pidl, LPWSTR dest, DWORD len)
{
        return StrRetToStrNW(dest, len, src, pidl);
}


/*************************************************************************
 * StrChrA                                      [NT 4.0:SHELL32.651]
 *
 */
LPSTR WINAPI StrChrA (LPSTR str, CHAR x )
{       LPSTR ptr=str;

        do
        {  if (*ptr==x)
           { return ptr;
           }
           ptr++;
        } while (*ptr);
        return NULL;
}

/*************************************************************************
 * StrChrW                                      [NT 4.0:SHELL32.651]
 *
 */
LPWSTR WINAPI StrChrW (LPWSTR str, WCHAR x )
{       LPWSTR ptr=str;

        TRACE("%s 0x%04x\n",debugstr_w(str),x);
        do
        {  if (*ptr==x)
           { return ptr;
           }
           ptr++;
        } while (*ptr);
        return NULL;
}

/*************************************************************************
 * StrCmpNIW                                    [NT 4.0:SHELL32.*]
 *
 */
INT WINAPI StrCmpNIW ( LPWSTR wstr1, LPWSTR wstr2, INT len)
{       FIXME("%s %s %i stub\n", debugstr_w(wstr1),debugstr_w(wstr2),len);
        return 0;
}

/*************************************************************************
 * StrCmpNIA                                    [NT 4.0:SHELL32.*]
 *
 */
INT WINAPI StrCmpNIA ( LPSTR wstr1, LPSTR wstr2, INT len)
{       FIXME("%s %s %i stub\n", wstr1,wstr2,len);
        return 0;
}

/*************************************************************************
 * SHAllocShared                                [SHELL32.520]
 *
 * NOTES
 *  parameter1 is return value from HeapAlloc
 *  parameter2 is equal to the size allocated with HeapAlloc
 *  parameter3 is return value from GetCurrentProcessId
 *
 *  the return value is posted as lParam with 0x402 (WM_USER+2) to somewhere
 *  WM_USER+2 could be the undocumented CWM_SETPATH
 *  the allocated memory contains a pidl
 */
HGLOBAL WINAPI SHAllocShared(LPVOID psrc, DWORD size, DWORD procID)
{       HGLOBAL hmem;
        LPVOID pmem;

        TRACE("ptr=%p size=0x%04lx procID=0x%04lx\n",psrc,size,procID);
        hmem = GlobalAlloc(GMEM_FIXED, size);
        if (!hmem)
          return 0;

        pmem =  GlobalLock (hmem);

        if (! pmem)
          return 0;

        memcpy (pmem, psrc, size);
        GlobalUnlock(hmem);
        return hmem;
}
/*************************************************************************
 * SHLockShared                                 [SHELL32.521]
 *
 * NOTES
 *  parameter1 is return value from SHAllocShared
 *  parameter2 is return value from GetCurrentProcessId
 *  the receiver of (WM_USER+2) trys to lock the HANDLE (?)
 *  the returnvalue seems to be a memoryadress
 */
LPVOID WINAPI SHLockShared(HANDLE hmem, DWORD procID)
{       TRACE("handle=0x%04x procID=0x%04lx\n",hmem,procID);
        return GlobalLock(hmem);
}
/*************************************************************************
 * SHUnlockShared                               [SHELL32.522]
 *
 * NOTES
 *  parameter1 is return value from SHLockShared
 */
ODINFUNCTION1(BOOL, SHUnlockShared,
              LPVOID, pv)
{       
  TRACE("handle=0x%08x\n",pv);
  //@@@PH rather VirtualUnlock ?
  return GlobalUnlock((HANDLE)pv);
}
/*************************************************************************
 * SHFreeShared                                 [SHELL32.523]
 *
 * NOTES
 *  parameter1 is return value from SHAllocShared
 *  parameter2 is return value from GetCurrentProcessId
 */
HANDLE WINAPI SHFreeShared(HANDLE hmem, DWORD procID)
{       TRACE("handle=0x%04x 0x%04lx\n",hmem,procID);
        return GlobalFree(hmem);
}

/*************************************************************************
 * SetAppStartingCursor                         [SHELL32.99]
 *
 */
HRESULT WINAPI SetAppStartingCursor(HWND u, DWORD v)
{       FIXME("hwnd=0x%04x 0x%04lx stub\n",u,v );
        return 0;
}
/*************************************************************************
 * SHLoadOLE                                    [SHELL32.151]
 *
 */
HRESULT WINAPI SHLoadOLE(DWORD u)
{       FIXME("0x%04lx stub\n",u);
        return S_OK;
}
/*************************************************************************
 * DriveType                                    [SHELL32.64]
 *
 */
HRESULT WINAPI DriveType(DWORD u)
{       FIXME("0x%04lx stub\n",u);
        return 0;
}
/*************************************************************************
 * SHAbortInvokeCommand                         [SHELL32.198]
 *
 */
HRESULT WINAPI SHAbortInvokeCommand(void)
{       FIXME("stub\n");
        return 1;
}
/*************************************************************************
 * SHOutOfMemoryMessageBox                      [SHELL32.126]
 *
 */
ODINFUNCTION3(int,    SHOutOfMemoryMessageBox,
              HWND,   hwndOwner,
              LPCSTR, lpCaption,
              UINT,   uType)
{ 
  dprintf(("SHELL32: SHOutOfMemoryMessageBox not implemented."));
  return 0;
}
/*************************************************************************
 * SHFlushClipboard                             [SHELL32.121]
 *
 */
HRESULT WINAPI SHFlushClipboard(void)
{       FIXME("stub\n");
        return 1;
}
/*************************************************************************
 * StrRChrA                                     [SHELL32.346]
 *
 */
LPSTR WINAPI StrRChrA(LPCSTR lpStart, LPCSTR lpEnd, DWORD wMatch)
{
        if (!lpStart)
            return NULL;

        /* if the end not given, search*/
        if (!lpEnd)
        { lpEnd=lpStart;
          while (*lpEnd)
            lpEnd++;
        }

        for (--lpEnd;lpStart <= lpEnd; lpEnd--)
            if (*lpEnd==(char)wMatch)
                return (LPSTR)lpEnd;

        return NULL;
}
/*************************************************************************
 * StrRChrW                                     [SHELL32.320]
 *
 */
LPWSTR WINAPI StrRChrW(LPWSTR lpStart, LPWSTR lpEnd, DWORD wMatch)
{       LPWSTR wptr=NULL;
        TRACE("%s %s 0x%04x\n",debugstr_w(lpStart),debugstr_w(lpEnd), (WCHAR)wMatch );

        /* if the end not given, search*/
        if (!lpEnd)
        { lpEnd=lpStart;
          while (*lpEnd)
            lpEnd++;
        }

        do
        { if (*lpStart==(WCHAR)wMatch)
            wptr = lpStart;
          lpStart++;
        } while ( lpStart<=lpEnd );
        return wptr;
}
/*************************************************************************
* StrFormatByteSize                             [SHLWAPI]
*/
LPSTR WINAPI StrFormatByteSizeA ( DWORD dw, LPSTR pszBuf, UINT cchBuf )
{       char buf[64];
        TRACE("%lx %p %i\n", dw, pszBuf, cchBuf);
        if ( dw<1024L )
        { sprintf (buf,"%3.0f bytes", (FLOAT)dw);
        }
        else if ( dw<1048576L)
        { sprintf (buf,"%3.2f KB", (FLOAT)dw/1024);
        }
        else if ( dw < 1073741824L)
        { sprintf (buf,"%3.2f MB", (FLOAT)dw/1048576L);
        }
        else
        { sprintf (buf,"%3.2f GB", (FLOAT)dw/1073741824L);
        }
        lstrcpynA (pszBuf, buf, cchBuf);
        return pszBuf;
}
LPWSTR WINAPI StrFormatByteSizeW ( DWORD dw, LPWSTR pszBuf, UINT cchBuf )
{       char buf[64];
        TRACE("%lx %p %i\n", dw, pszBuf, cchBuf);
        if ( dw<1024L )
        { sprintf (buf,"%3.0f bytes", (FLOAT)dw);
        }
        else if ( dw<1048576L)
        { sprintf (buf,"%3.2f KB", (FLOAT)dw/1024);
        }
        else if ( dw < 1073741824L)
        { sprintf (buf,"%3.2f MB", (FLOAT)dw/1048576L);
        }
        else
        { sprintf (buf,"%3.2f GB", (FLOAT)dw/1073741824L);
        }
        lstrcpynAtoW (pszBuf, buf, cchBuf);
        return pszBuf;
}
/*************************************************************************
 * SHWaitForFileToOpen                          [SHELL32.97]
 *
 */
ODINFUNCTION3(BOOL, SHWaitForFileToOpen,
              LPCITEMIDLIST, pidl,
              DWORD, dwFlags,
              DWORD, dwTimeout)
{ 
  dprintf(("SHELL32: SHWaitForFileToOpen not implemented"));
  return 0;
}
/*************************************************************************
 * Control_FillCache_RunDLL                     [SHELL32.8]
 *
 */
HRESULT WINAPI Control_FillCache_RunDLL(HWND hWnd, HANDLE hModule, DWORD w, DWORD x)
{       FIXME("0x%04x 0x%04x 0x%04lx 0x%04lx stub\n",hWnd, hModule,w,x);
        return 0;
}
/*************************************************************************
 * RunDLL_CallEntry16                           [SHELL32.122]
 * the name is propably wrong
 */
HRESULT WINAPI RunDLL_CallEntry16(DWORD v, DWORD w, DWORD x, DWORD y, DWORD z)
{       FIXME("0x%04lx 0x%04lx 0x%04lx 0x%04lx 0x%04lx stub\n",v,w,x,y,z);
        return 0;
}

/************************************************************************
 *      shell32_654                             [SHELL32.654]
 *
 * NOTES: first parameter seems to be a pointer (same as passed to WriteCabinetState)
 * second one could be a size (0x0c). The size is the same as the structure saved to
 * HCU\Software\Microsoft\Windows\CurrentVersion\Explorer\CabinetState
 * I'm (js) guessing: this one is just ReadCabinetState ;-)
 */
HRESULT WINAPI shell32_654 (DWORD x, DWORD y)
{       FIXME("0x%08lx 0x%08lx stub\n",x,y);
        return 0;
}

/************************************************************************
 *      RLBuildListOfPaths                      [SHELL32.146]
 *
 * NOTES
 *   builds a DPA
 */
DWORD WINAPI RLBuildListOfPaths (void)
{       FIXME("stub\n");
        return 0;
}

/************************************************************************
 *      SHValidateUNC                           [SHELL32.173]
 *
 */
HRESULT WINAPI SHValidateUNC (DWORD x, DWORD y, DWORD z)
{
        FIXME("0x%08lx 0x%08lx 0x%08lx stub\n",x,y,z);
        return 0;
}

/************************************************************************
 *      DoEnvironmentSubstW                     [SHELL32.53]
 *
 */
HRESULT WINAPI DoEnvironmentSubstA(LPSTR x, LPSTR y)
{
        FIXME("%p(%s) %p(%s) stub\n", x, x, y, y);
        return 0;
}

HRESULT WINAPI DoEnvironmentSubstW(LPWSTR x, LPWSTR y)
{
        FIXME("%p(%s) %p(%s) stub\n", x, debugstr_w(x), y, debugstr_w(y));
        return 0;
}

HRESULT WINAPI DoEnvironmentSubstAW(LPVOID x, LPVOID y)
{
        if (VERSION_OsIsUnicode())
          return DoEnvironmentSubstW((LPWSTR)x, (LPWSTR)y);
        return DoEnvironmentSubstA((LPSTR)x, (LPSTR)y);
}

/*************************************************************************
 *      shell32_243                             [SHELL32.243]
 *
 * Win98+ by-ordinal routine.  In Win98 this routine returns zero and
 * does nothing else.  Possibly this does something in NT or SHELL32 5.0?
 *
 */

BOOL WINAPI shell32_243(DWORD a, DWORD b)
{
  return FALSE;
}


/*****************************************************************************
 * Name      : StrChrIA
 * Purpose   : Searches a string for the first occurrence of a character that
 *             matches the specified character. The comparison is not case sensitive.
 * Parameters: LPCSTR lpStart Address of the string to be searched.
 *             TCHAR  wMatch  Character to be used for comparison.
 * Variables :
 * Result    : Returns the address of the first occurrence of the character in
 *             the string if successful, or NULL otherwise.
 * Remark    : SHELL32.
 * Status    : COMPLETELY IMPLEMENTED UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPSTR,  StrChrIA,
              LPCSTR, lpStart,
              CHAR,   wMatch)
{
  LPSTR lpRes;

  wMatch = tolower(wMatch);
  lpRes = strchr(lpStart, wMatch);    // lower case comparsion
  if (NULL == lpRes)
  {
    wMatch = toupper(wMatch);
    lpRes = strchr(lpStart, wMatch);  // upper case comparsion
  }

  return lpRes;
}


/*****************************************************************************
 * Name      : StrChrIW
 * Purpose   : Searches a string for the first occurrence of a character that
 *             matches the specified character. The comparison is not case sensitive.
 * Parameters: LPCSTR lpStart Address of the string to be searched.
 *             TCHAR  wMatch  Character to be used for comparison.
 * Variables :
 * Result    : Returns the address of the first occurrence of the character in
 *             the string if successful, or NULL otherwise.
 * Remark    : SHELL32.
 * Status    : COMPLETELY IMPLEMENTED UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPWSTR,   StrChrIW,
              LPCWSTR, lpStart,
              WCHAR,    wMatch)
{
  LPWSTR lpRes;

  wMatch = towlower(wMatch);
  lpRes = (WCHAR*)wcschr((const wchar_t*)lpStart, wMatch);    // lower case comparsion
  if (NULL == lpRes)
  {
    wMatch = towupper(wMatch);
    lpRes = (WCHAR*)wcschr((const wchar_t*)lpStart, wMatch);  // upper case comparsion
  }

  return lpRes;
}


/*****************************************************************************
 * Name      : StrStrIA
 * Purpose   : Finds the first occurrence of a substring within a string. The
 *             comparison is not case sensitive.
 * Parameters: LPCSTR lpFirst
 *             LPCSTR lpSrch
 * Variables :
 * Result    : Returns the address of the first occurrence of the matching
 *             substring if successful, or NULL otherwise.
 * Remark    : SHELL32.
 * Status    : COMPLETELY IMPLEMENTED UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPSTR,  StrStrIA,
              LPCSTR, lpFirst,
              LPCSTR, lpSrch)
{
  char  ch = lpSrch[0];          // look for 1st character
  LONG  lLen = lstrlenA(lpSrch); // length of search string
  int   iRes;                    // comparsion result

  do
  {
    lpFirst = StrChrIA(lpFirst, // find first matching character
                       ch);
    if (NULL == lpFirst)        // not found
      return NULL;

    iRes   = StrCmpNIA((LPSTR)lpFirst, // compare search string
                       (LPSTR)lpSrch,
                       lLen);

    if (0 == iRes)              // Found!
      return (LPSTR)lpFirst;

    lpFirst = CharNextA(lpFirst); // skip to next character
  }
  while (*lpFirst != 0);        // safe termination

  return NULL;                  // default result
}



/*****************************************************************************
 * Name      : StrStrIW
 * Purpose   : Finds the first occurrence of a substring within a string. The
 *             comparison is not case sensitive.
 * Parameters: LPCWSTR lpFirst
 *             LPCWSTR lpSrch
 * Variables :
 * Result    : Returns the address of the first occurrence of the matching
 *             substring if successful, or NULL otherwise.
 * Remark    : SHELL32.
 * Status    : COMPLETELY IMPLEMENTED UNTESTED UNKNOWN
 *
 * Author    : Patrick Haller [Wed, 1999/12/29 09:00]
 *****************************************************************************/

ODINFUNCTION2(LPWSTR,  StrStrIW,
              LPCWSTR, lpFirst,
              LPCWSTR, lpSrch)
{
  WCHAR  ch = lpSrch[0];          // look for 1st character
  LONG   lLen = lstrlenW(lpSrch); // length of search string
  int   iRes;                    // comparsion result

  do
  {
    lpFirst = StrChrIW(lpFirst, // find first matching character
                       ch);
    if (NULL == lpFirst)        // not found
      return NULL;

    iRes   = StrCmpNIW((LPWSTR)lpFirst, // compare search string
                       (LPWSTR)lpSrch,
                       lLen);

    if (0 == iRes)              // Found!
      return (LPWSTR)lpFirst;

    lpFirst = CharNextW(lpFirst); // skip to next character
  }
  while (*lpFirst != 0);        // safe termination

  return NULL;                  // default result
}


