/* $Id: folders.cpp,v 1.3 2000-03-29 15:24:03 cbratschi Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Corel WINE 20000324 level
 */

/*
 *      Copyright 1997  Marcus Meissner
 *      Copyright 1998  Juergen Schmied
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <odin.h>
#include <odinwrap.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "wine/obj_base.h"
#include "wine/obj_extracticon.h"
#include "wine/undocshell.h"

#include "debugtools.h"
#include "winerror.h"

#include "pidl.h"
#include "shell32_main.h"

#include <misc.h>


ODINDEBUGCHANNEL(SHELL32-FOLDERS)


/***********************************************************************
*   IExtractIconA implementation
*/

typedef struct
{       ICOM_VTABLE(IExtractIconA)*     lpvtbl;
        DWORD   ref;
        ICOM_VTABLE(IPersistFile)*      lpvtblPersistFile;
        LPITEMIDLIST    pidl;
} IExtractIconAImpl;

//static struct ICOM_VTABLE(IExtractIconA) eivt;
//static struct ICOM_VTABLE(IPersistFile) pfvt;

#define _IPersistFile_Offset ((int)(&(((IExtractIconAImpl*)0)->lpvtblPersistFile)))
#define _ICOM_THIS_From_IPersistFile(class, name) class* This = (class*)(((char*)name)-_IPersistFile_Offset);

/**************************************************************************
 *  IExtractIconA_QueryInterface
 */
static HRESULT WINAPI IExtractIconA_fnQueryInterface( IExtractIconA * iface, REFIID riid, LPVOID *ppvObj)
{
        ICOM_THIS(IExtractIconAImpl,iface);

        char    xriid[50];
         WINE_StringFromCLSID((LPCLSID)riid,xriid);
         dprintf(("SHELL32:folders IExtractIconA_fnQueryInterface(%p)->(\n\tIID:\t%s,%p)\n",
           This,
           xriid,
           ppvObj));

        *ppvObj = NULL;

        if(IsEqualIID(riid, &IID_IUnknown))             /*IUnknown*/
        { *ppvObj = This;
        }
        else if(IsEqualIID(riid, &IID_IPersistFile))    /*IExtractIcon*/
        {    *ppvObj = (IPersistFile*)&(This->lpvtblPersistFile);
        }
        else if(IsEqualIID(riid, &IID_IExtractIconA))   /*IExtractIcon*/
        {    *ppvObj = (IExtractIconA*)This;
        }

        if(*ppvObj)
        { IExtractIconA_AddRef((IExtractIconA*) *ppvObj);
          dprintf(("SHELL32:folders IExtractIconA_fnQueryInterface-- Interface: (%p)->(%p)\n",ppvObj,*ppvObj));
          return S_OK;
        }
        dprintf(("SHELL32:folders IExtractIconA_fnQueryInterface-- Interface: E_NOINTERFACE\n"));
        return E_NOINTERFACE;
}

/**************************************************************************
*  IExtractIconA_AddRef
*/
static ULONG WINAPI IExtractIconA_fnAddRef(IExtractIconA * iface)
{
        ICOM_THIS(IExtractIconAImpl,iface);

        dprintf(("SHELL32:folders IExtractIconA_fnAddRef(%p)->(count=%lu)\n",
          This,
          This->ref));

        shell32_ObjCount++;

        return ++(This->ref);
}
/**************************************************************************
*  IExtractIconA_Release
*/
static ULONG WINAPI IExtractIconA_fnRelease(IExtractIconA * iface)
{
        ICOM_THIS(IExtractIconAImpl,iface);

        dprintf(("SHELL32:folders IExtractIconA_fnRelease(%p)->()\n",
          This));

        shell32_ObjCount--;

        if (!--(This->ref))
        { dprintf(("SHELL32:folders  IExtractIconA_fnRelease destroying IExtractIcon(%p)\n",
            This));
          SHFree(This->pidl);
          HeapFree(GetProcessHeap(),0,This);
          return 0;
        }
        return This->ref;
}
/**************************************************************************
*  IExtractIconA_GetIconLocation
*
* mapping filetype to icon
*/
static HRESULT WINAPI IExtractIconA_fnGetIconLocation(
        IExtractIconA * iface,
        UINT uFlags,
        LPSTR szIconFile,
        UINT cchMax,
        int * piIndex,
        UINT * pwFlags)
{
        ICOM_THIS(IExtractIconAImpl,iface);

        char    sTemp[MAX_PATH];
        DWORD   dwNr;
        GUID const * riid;
        LPITEMIDLIST    pSimplePidl = ILFindLastID(This->pidl);

        dprintf(("SHELL32:folders IExtractIconA_fnGetIconLocation(%p) (flags=%u %p %u %p %p)\n",
          This,
          uFlags,
          szIconFile,
          cchMax,
          piIndex,
          pwFlags));

        if (pwFlags)
          *pwFlags = 0;

        if (_ILIsDesktop(pSimplePidl))
        {
          lstrcpynA(szIconFile, "shell32.dll", cchMax);
          *piIndex = 34;
        }

        /* my computer and other shell extensions */
        else if (riid = _ILGetGUIDPointer(pSimplePidl))
        {
          char xriid[50];
          strcpy(xriid,"CLSID\\");
          WINE_StringFromCLSID((LPCLSID)riid,&xriid[strlen(xriid)]);

          if (HCR_GetDefaultIcon(xriid, sTemp, MAX_PATH, &dwNr))
          {
            lstrcpynA(szIconFile, sTemp, cchMax);
            *piIndex = dwNr;
          }
          else
          {
            lstrcpynA(szIconFile, "shell32.dll", cchMax);
            *piIndex = 15;
          }

        }
        else if (_ILIsDrive (pSimplePidl))
        {
          if (HCR_GetDefaultIcon("Drive", sTemp, MAX_PATH, &dwNr))
          {
            lstrcpynA(szIconFile, sTemp, cchMax);
            *piIndex = dwNr;
          }
          else
          {
            lstrcpynA(szIconFile, "shell32.dll", cchMax);
            *piIndex = 8;
          }
        }
        else if (_ILIsFolder (pSimplePidl))
        {
          if (HCR_GetDefaultIcon("Folder", sTemp, MAX_PATH, &dwNr))
          {
            lstrcpynA(szIconFile, sTemp, cchMax);
            *piIndex = dwNr;
          }
          else
          {
            lstrcpynA(szIconFile, "shell32.dll", cchMax);
            *piIndex = 3;
          }
        }
        else    /* object is file */
        {
          if (_ILGetExtension (pSimplePidl, sTemp, MAX_PATH))
          {
            if (HCR_MapTypeToValue(sTemp, sTemp, MAX_PATH, TRUE)
              && HCR_GetDefaultIcon(sTemp, sTemp, MAX_PATH, &dwNr))
            {
              if (!strcmp("%1",sTemp))            /* icon is in the file */
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                dwNr = 0;
              }
              lstrcpynA(szIconFile, sTemp, cchMax);
              *piIndex = dwNr;
            } else
            {
              //icon is in the file/file is icon
              if (stricmp(sTemp,"EXE") == 0) //CB: add more
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                dwNr = 0;
                lstrcpynA(szIconFile, sTemp, cchMax);
                *piIndex = dwNr;
              } else //default icon
              {
                lstrcpynA(szIconFile, "shell32.dll", cchMax);
                *piIndex = 0;
              }
            }
          } else                                  /* default icon */
          {
            lstrcpynA(szIconFile, "shell32.dll", cchMax);
            *piIndex = 0;
          }
        }

        dprintf(("SHELL32:folders IExtractIconA_fnGetIconLocation -- %s %x\n",
          szIconFile,
          *piIndex));
        return NOERROR;
}
/**************************************************************************
*  IExtractIconA_Extract
*/
static HRESULT WINAPI IExtractIconA_fnExtract(IExtractIconA * iface, LPCSTR pszFile, UINT nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize)
{
        ICOM_THIS(IExtractIconAImpl,iface);

        dprintf(("SHELL32:folders IExtractIconA_fnExtract (%p) (file=%p index=%u %p %p size=%u) semi-stub\n",
          This,
          pszFile,
          nIconIndex,
          phiconLarge,
          phiconSmall,
          nIconSize));

        if (phiconLarge)
          *phiconLarge = pImageList_GetIcon(ShellBigIconList, nIconIndex, ILD_TRANSPARENT);

        if (phiconSmall)
          *phiconSmall = pImageList_GetIcon(ShellSmallIconList, nIconIndex, ILD_TRANSPARENT);

        return S_OK;
}

static struct ICOM_VTABLE(IExtractIconA) eivt =
{
        ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
        IExtractIconA_fnQueryInterface,
        IExtractIconA_fnAddRef,
        IExtractIconA_fnRelease,
        IExtractIconA_fnGetIconLocation,
        IExtractIconA_fnExtract
};


/************************************************************************
 * IEIPersistFile_QueryInterface (IUnknown)
 */
static HRESULT WINAPI IEIPersistFile_fnQueryInterface(
        IPersistFile    *iface,
        REFIID          iid,
        LPVOID          *ppvObj)
{
        _ICOM_THIS_From_IPersistFile(IExtractIconA, iface);

        return IShellFolder_QueryInterface((IExtractIconA*)This, iid, ppvObj);
}

/************************************************************************
 * IEIPersistFile_AddRef (IUnknown)
 */
static ULONG WINAPI IEIPersistFile_fnAddRef(
        IPersistFile    *iface)
{
        _ICOM_THIS_From_IPersistFile(IExtractIconA, iface);

        return IExtractIconA_AddRef((IExtractIconA*)This);
}

/************************************************************************
 * IEIPersistFile_Release (IUnknown)
 */
static ULONG WINAPI IEIPersistFile_fnRelease(
        IPersistFile    *iface)
{
        _ICOM_THIS_From_IPersistFile(IExtractIconA, iface);

        return IExtractIconA_Release((IExtractIconA*)This);
}

/************************************************************************
 * IEIPersistFile_GetClassID (IPersist)
 */
static HRESULT WINAPI IEIPersistFile_fnGetClassID(
        IPersistFile    *iface,
        LPCLSID         lpClassId)
{
        CLSID StdFolderID = { 0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };

        if (lpClassId==NULL)
          return E_POINTER;

        memcpy(lpClassId, &StdFolderID, sizeof(StdFolderID));

        return S_OK;
}

/************************************************************************
 * IEIPersistFile_IsDirty (IPersistFile)
 */
static HRESULT WINAPI IEIPersistFile_fnIsDirty(IPersistFile* iface)
{
  dprintf(("SHELL32: Folders: IEIPersistFile_fnIsDirty not implemented.\n"));
  return E_NOTIMPL;
}


/************************************************************************
 * IEIPersistFile_Load (IPersistFile)
 */
static HRESULT WINAPI IEIPersistFile_fnLoad(IPersistFile* iface, LPCOLESTR pszFileName, DWORD dwMode)
{
        _ICOM_THIS_From_IPersistFile(IExtractIconA, iface);
        FIXME("%p\n", This);
        return E_NOTIMPL;

}


/************************************************************************
 * IEIPersistFile_Save (IPersistFile)
 */
static HRESULT WINAPI IEIPersistFile_fnSave(IPersistFile* iface, LPCOLESTR pszFileName, DWORD dwMode)
{
  dprintf(("SHELL32: Folders: IEIPersistFile_fnSave not implemented.\n"));
  return E_NOTIMPL;
}


/************************************************************************
 * IEIPersistFile_SaveCompleted (IPersistFile)
 */
static HRESULT WINAPI IEIPersistFile_fnSaveCompleted(IPersistFile* iface, LPCOLESTR pszFileName)
{
  dprintf(("SHELL32: Folders: IEIPersistFile_fnSaveCompleted not implemented.\n"));
  return E_NOTIMPL;
}


/************************************************************************
 * IEIPersistFile_GetCurFile (IPersistFile)
 */
static HRESULT WINAPI IEIPersistFile_fnGetCurFile(IPersistFile* iface, LPOLESTR* pszFileName)
{
  dprintf(("SHELL32: Folders: IEIPersistFile_fnGetCurFile not implemented.\n"));
  return E_NOTIMPL;
}

static struct ICOM_VTABLE(IPersistFile) pfvt =
{
        ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
        IEIPersistFile_fnQueryInterface,
        IEIPersistFile_fnAddRef,
        IEIPersistFile_fnRelease,
        IEIPersistFile_fnGetClassID,
        IEIPersistFile_fnIsDirty,       /* IEIPersistFile_fnIsDirty */
        IEIPersistFile_fnLoad,
        IEIPersistFile_fnSave,          /* IEIPersistFile_fnSave */
        IEIPersistFile_fnSaveCompleted, /* IEIPersistFile_fnSaveCompleted */
        IEIPersistFile_fnGetCurFile     /* IEIPersistFile_fnGetCurFile */
};

/**************************************************************************
*  IExtractIconA_Constructor
*/
IExtractIconA* IExtractIconA_Constructor(LPCITEMIDLIST pidl)
{
        IExtractIconAImpl* ei;

        ei=(IExtractIconAImpl*)HeapAlloc(GetProcessHeap(),0,sizeof(IExtractIconAImpl));
        ei->ref=1;
        ei->lpvtbl = &eivt;
        ei->lpvtblPersistFile = &pfvt;
        ei->pidl=ILClone(pidl);

        pdump(pidl);

        dprintf(("SHELL32:folders IExtractIconA_Constructor(%p)\n",
          ei));
        shell32_ObjCount++;
        return (IExtractIconA *)ei;
}


