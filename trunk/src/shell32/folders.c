/* $Id: folders.c,v 1.8 2001-04-28 13:33:44 sandervl Exp $ */
/*
 *  Copyright 1997  Marcus Meissner
 *  Copyright 1998  Juergen Schmied
 *
 */
#ifdef __WIN32OS2__
#define ICOM_CINTERFACE 1
#include <odin.h>
#include "shellicon.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wine/obj_base.h"
#include "wine/obj_extracticon.h"
#include "wine/undocshell.h"
#include "shlguid.h"

#include "debugtools.h"
#include "winerror.h"

#include "pidl.h"
#include "shell32_main.h"

DEFAULT_DEBUG_CHANNEL(shell)


/***********************************************************************
*   IExtractIconA implementation
*/

typedef struct
{   ICOM_VFIELD(IExtractIconA);
    DWORD   ref;
    ICOM_VTABLE(IPersistFile)*  lpvtblPersistFile;
    LPITEMIDLIST    pidl;
} IExtractIconAImpl;

static struct ICOM_VTABLE(IExtractIconA) eivt;
static struct ICOM_VTABLE(IPersistFile) pfvt;

#define _IPersistFile_Offset ((int)(&(((IExtractIconAImpl*)0)->lpvtblPersistFile)))
#define _ICOM_THIS_From_IPersistFile(class, name) class* This = (class*)(((char*)name)-_IPersistFile_Offset);

/**************************************************************************
*  IExtractIconA_Constructor
*/
IExtractIconA* IExtractIconA_Constructor(LPCITEMIDLIST pidl)
{
    IExtractIconAImpl* ei;

    ei=(IExtractIconAImpl*)HeapAlloc(GetProcessHeap(),0,sizeof(IExtractIconAImpl));
    ei->ref=1;
    ICOM_VTBL(ei) = &eivt;
    ei->lpvtblPersistFile = &pfvt;
    ei->pidl=ILClone(pidl);

    pdump(pidl);

    TRACE("(%p)\n",ei);
    shell32_ObjCount++;
    return (IExtractIconA *)ei;
}
/**************************************************************************
 *  IExtractIconA_QueryInterface
 */
static HRESULT WINAPI IExtractIconA_fnQueryInterface( IExtractIconA * iface, REFIID riid, LPVOID *ppvObj)
{
    ICOM_THIS(IExtractIconAImpl,iface);

     TRACE("(%p)->(\n\tIID:\t%s,%p)\n",This,debugstr_guid(riid),ppvObj);

    *ppvObj = NULL;

    if(IsEqualIID(riid, &IID_IUnknown))     /*IUnknown*/
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
      TRACE("-- Interface: (%p)->(%p)\n",ppvObj,*ppvObj);
      return S_OK;
    }
    TRACE("-- Interface: E_NOINTERFACE\n");
    return E_NOINTERFACE;
}

/**************************************************************************
*  IExtractIconA_AddRef
*/
static ULONG WINAPI IExtractIconA_fnAddRef(IExtractIconA * iface)
{
    ICOM_THIS(IExtractIconAImpl,iface);

    TRACE("(%p)->(count=%lu)\n",This, This->ref );

    shell32_ObjCount++;

    return ++(This->ref);
}
/**************************************************************************
*  IExtractIconA_Release
*/
static ULONG WINAPI IExtractIconA_fnRelease(IExtractIconA * iface)
{
    ICOM_THIS(IExtractIconAImpl,iface);

    TRACE("(%p)->()\n",This);

    shell32_ObjCount--;

    if (!--(This->ref))
    { TRACE(" destroying IExtractIcon(%p)\n",This);
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

    TRACE("(%p) (flags=%u %p %u %p %p)\n", This, uFlags, szIconFile, cchMax, piIndex, pwFlags);

    if (pwFlags)
      *pwFlags = 0;

    if (_ILIsDesktop(pSimplePidl))
    {
      lstrcpynA(szIconFile, "shell32.dll", cchMax);
#ifdef __WIN32OS2__
      *piIndex = SHLICON_DESKTOP;
#else
      *piIndex = 34;
#endif
    }

    /* my computer and other shell extensions */
    else if ( (riid = _ILGetGUIDPointer(pSimplePidl)) )
    {
      char xriid[50];
          sprintf( xriid, "CLSID\\{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                   riid->Data1, riid->Data2, riid->Data3,
                   riid->Data4[0], riid->Data4[1], riid->Data4[2], riid->Data4[3],
                   riid->Data4[4], riid->Data4[5], riid->Data4[6], riid->Data4[7] );

      if (HCR_GetDefaultIcon(xriid, sTemp, MAX_PATH, &dwNr))
      {
        lstrcpynA(szIconFile, sTemp, cchMax);
        *piIndex = dwNr;
      }
      else
      {
#ifdef __WIN32OS2__
        /* Not correct. Originally location is explorer.exe, index 1 */
        lstrcpynA(szIconFile, "shell32.dll", cchMax);
        *piIndex = -SHLICON_MYCOMPUTER;

        /* 15 is absolutely wrong! ( Another computer in the network ) */
#else
        lstrcpynA(szIconFile, "shell32.dll", cchMax);
        *piIndex = 15;
#endif
      }
    }
    else if (_ILIsDrive (pSimplePidl))
    {
#ifdef __WIN32OS2__
      lstrcpynA(szIconFile, "shell32.dll", cchMax);
      *piIndex = SHLICON_HARDDISK;

      if ( _ILGetDrive( pSimplePidl, sTemp, cchMax ) )
      {
        if ( ( sTemp[ 0 ] == 'A' ) || ( sTemp[ 0 ] == 'a' ) ||
             ( sTemp[ 0 ] == 'B' ) || ( sTemp[ 0 ] == 'b' ) )
        {
            /* FIXME determine 5.25 Floppy */
            *piIndex = SHLICON_FLOPPY35;
        }
        else
        {
            UINT nType = GetDriveTypeA( sTemp );
            switch ( nType )
            {
                case DRIVE_REMOVABLE:
                    *piIndex = SHLICON_REMOVABLE_DISK;
                    break;

                case DRIVE_FIXED:
                    *piIndex = SHLICON_HARDDISK;
                    break;

                case DRIVE_REMOTE:
                {
                    /* FIXME: connected / disconnected state */
                    BOOL connected = TRUE;
                    if ( connected )
                        *piIndex = SHLICON_NETDRIVE_CONN;
                    else
                        *piIndex = SHLICON_NETDRIVE_DISCON;
                    break;
                }
                case DRIVE_CDROM:
                    *piIndex = SHLICON_CDROM_DRIVE;
                    break;

                case DRIVE_RAMDISK:
                    *piIndex = SHLICON_RAMDRIVE;
                    break;

                case DRIVE_UNKNOWN:
                case DRIVE_NO_ROOT_DIR:
                default:
                    *piIndex = SHLICON_HARDDISK;
                    break;
            }
        }
      }

      if ( ( *piIndex == ( SHLICON_HARDDISK ) ) &&
           HCR_GetDefaultIcon( "Drive", sTemp, MAX_PATH, &dwNr ) )
      {
        /* kso: Are there special registry keys for particular drives? */
        lstrcpynA(szIconFile, sTemp, cchMax);
        *piIndex = dwNr;
      }
#else
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
#endif
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
#ifdef __WIN32OS2__
        *piIndex = (uFlags & GIL_OPENICON)
				 ? SHLICON_FOLDER_OPEN : SHLICON_FOLDER_CLOSED;
#else
        *piIndex = (uFlags & GIL_OPENICON)? 4 : 3;
#endif
      }
    }
    else    /* object is file */
    {
#ifdef __WIN32OS2__
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
              if ((stricmp(sTemp,"EXE") == 0) ||
                  (stricmp(sTemp,"ICO") == 0))
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile, sTemp, cchMax);
                *piIndex = 0;
              }
              else if (stricmp(sTemp,"FND") == 0)
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile, "shell32.dll", cchMax);
                *piIndex = SHLICON_COMPUTERS;
              }
              else if (stricmp(sTemp,"COM") == 0)
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile, "shell32.dll", cchMax);
                *piIndex = SHLICON_APPLICATION;
              }
#if 0
    // icons not yet in resources

              else if ((stricmp(sTemp,"INI") == 0) ||
                       (stricmp(sTemp,"INF") == 0))
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile, "shell32.dll", cchMax);
                *piIndex = -151;
              }
              else if (stricmp(sTemp,"TXT") == 0)
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile, "shell32.dll", cchMax);
                *piIndex = -152;
              }
              else if ((stricmp(sTemp,"BAT") == 0) ||
                       (stricmp(sTemp,"CMD") == 0))
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile, "shell32.dll", cchMax);
                *piIndex = -153;
              }
              else if ((stricmp(sTemp,"DLL") == 0) ||
                       (stricmp(sTemp,"SYS") == 0) ||
                       (stricmp(sTemp,"VXD") == 0) ||
                       (stricmp(sTemp,"DRV") == 0) ||
                       (stricmp(sTemp,"CPL") == 0))
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile, "shell32.dll", cchMax);
                *piIndex = -154;
              }
              else if (stricmp(sTemp,"FON") == 0)
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile, "shell32.dll", cchMax);
                *piIndex = -155;
              }
              else if (stricmp(sTemp,"TTF") == 0)
              {
                SHGetPathFromIDListA(This->pidl, sTemp);
                lstrcpynA(szIconFile,"shell32.dll", cchMax);
                *piIndex = -156;
              }
#endif
              else //default icon
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
#else
      if (_ILGetExtension (pSimplePidl, sTemp, MAX_PATH)
          && HCR_MapTypeToValue(sTemp, sTemp, MAX_PATH, TRUE)
          && HCR_GetDefaultIcon(sTemp, sTemp, MAX_PATH, &dwNr))
      {
        if (!strcmp("%1",sTemp))        /* icon is in the file */
        {
          SHGetPathFromIDListA(This->pidl, sTemp);
          dwNr = 0;
        }
        lstrcpynA(szIconFile, sTemp, cchMax);
        *piIndex = dwNr;
      }
      else                  /* default icon */
      {
        lstrcpynA(szIconFile, "shell32.dll", cchMax);
        *piIndex = 0;
      }
    }
#endif
    TRACE("-- %s %x\n", szIconFile, *piIndex);
    return NOERROR;
}
/**************************************************************************
*  IExtractIconA_Extract
*/
static HRESULT WINAPI IExtractIconA_fnExtract(IExtractIconA * iface, LPCSTR pszFile, UINT nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize)
{
    ICOM_THIS(IExtractIconAImpl,iface);

    FIXME("(%p) (file=%p index=%u %p %p size=%u) semi-stub\n", This, pszFile, nIconIndex, phiconLarge, phiconSmall, nIconSize);

    if (phiconLarge)
      *phiconLarge = ImageList_GetIcon(ShellBigIconList, nIconIndex, ILD_TRANSPARENT);

    if (phiconSmall)
      *phiconSmall = ImageList_GetIcon(ShellSmallIconList, nIconIndex, ILD_TRANSPARENT);

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
    REFIID      iid,
    LPVOID      *ppvObj)
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
    LPCLSID     lpClassId)
{
    CLSID StdFolderID = { 0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };

    if (lpClassId==NULL)
      return E_POINTER;

    memcpy(lpClassId, &StdFolderID, sizeof(StdFolderID));

    return S_OK;
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

#ifdef __WIN32OS2__
/************************************************************************
 * IEIPersistFile_IsDirty (IPersistFile)
 */
static HRESULT WINAPI IEIPersistFile_fnIsDirty(IPersistFile* iface)
{
  dprintf(("SHELL32: Folders: IEIPersistFile_fnIsDirty not implemented.\n"));
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
#endif

static struct ICOM_VTABLE(IPersistFile) pfvt =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IEIPersistFile_fnQueryInterface,
    IEIPersistFile_fnAddRef,
    IEIPersistFile_fnRelease,
    IEIPersistFile_fnGetClassID,
#ifdef __WIN32OS2__
        IEIPersistFile_fnIsDirty,       /* IEIPersistFile_fnIsDirty */
#else
    (void *) 0xdeadbeef /* IEIPersistFile_fnIsDirty */,
#endif
    IEIPersistFile_fnLoad,
#ifdef __WIN32OS2__
        IEIPersistFile_fnSave,          /* IEIPersistFile_fnSave */
        IEIPersistFile_fnSaveCompleted, /* IEIPersistFile_fnSaveCompleted */
        IEIPersistFile_fnGetCurFile     /* IEIPersistFile_fnGetCurFile */
#else
    (void *) 0xdeadbeef /* IEIPersistFile_fnSave */,
    (void *) 0xdeadbeef /* IEIPersistFile_fnSaveCompleted */,
    (void *) 0xdeadbeef /* IEIPersistFile_fnGetCurFile */
#endif
};

