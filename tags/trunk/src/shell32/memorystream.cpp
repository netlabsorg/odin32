/* $Id: memorystream.cpp,v 1.3 1999-10-22 12:18:46 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * this class implements a pure IStream object
 * and can be used for many purposes
 *
 * the main reason for implementing this was
 * a cleaner implementation of IShellLink which
 * needs to be able to load lnk's from a IStream
 * interface so it was obvious to capsule the file
 * access in a IStream to.
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "wine/obj_storage.h"
#include "heap.h"
#include "winerror.h"
#include "debugtools.h"
#include "shell32_main.h"

#include <misc.h>


ODINDEBUGCHANNEL(SHELL32-MEMORYSTREAM)


/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/

static HRESULT WINAPI IStream_fnQueryInterface(IStream *iface, REFIID riid, LPVOID *ppvObj);
static ULONG WINAPI IStream_fnAddRef(IStream *iface);
static ULONG WINAPI IStream_fnRelease(IStream *iface);
static HRESULT WINAPI IStream_fnRead (IStream * iface, void* pv, ULONG cb, ULONG* pcbRead);
static HRESULT WINAPI IStream_fnWrite (IStream * iface, const void* pv, ULONG cb, ULONG* pcbWritten);
static HRESULT WINAPI IStream_fnSeek (IStream * iface, LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition);
static HRESULT WINAPI IStream_fnSetSize (IStream * iface, ULARGE_INTEGER libNewSize);
static HRESULT WINAPI IStream_fnCopyTo (IStream * iface, IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten);
static HRESULT WINAPI IStream_fnCommit (IStream * iface, DWORD grfCommitFlags);
static HRESULT WINAPI IStream_fnRevert (IStream * iface);
static HRESULT WINAPI IStream_fnLockRegion (IStream * iface, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
static HRESULT WINAPI IStream_fnUnlockRegion (IStream * iface, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
static HRESULT WINAPI IStream_fnStat (IStream * iface, STATSTG*   pstatstg, DWORD grfStatFlag);
static HRESULT WINAPI IStream_fnClone (IStream * iface, IStream** ppstm);

static ICOM_VTABLE(IStream) stvt =
{
   ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
   IStream_fnQueryInterface,
   IStream_fnAddRef,
   IStream_fnRelease,
   IStream_fnRead,
   IStream_fnWrite,
   IStream_fnSeek,
   IStream_fnSetSize,
   IStream_fnCopyTo,
   IStream_fnCommit,
   IStream_fnRevert,
   IStream_fnLockRegion,
   IStream_fnUnlockRegion,
   IStream_fnStat,
   IStream_fnClone

};

typedef struct
{  ICOM_VTABLE(IStream)                  *lpvtst;
   DWORD    ref;
   LPBYTE      pImage;
   HANDLE      hMapping;
   DWORD    dwLength;
   DWORD    dwPos;
} ISHFileStream;

/**************************************************************************
 *   CreateStreamOnFile()
 *
 *   similar to CreateStreamOnHGlobal
 */
HRESULT CreateStreamOnFile (LPCSTR pszFilename, IStream ** ppstm)
{
   ISHFileStream* fstr;
   OFSTRUCT ofs;
   HFILE    hFile = OpenFile( pszFilename, &ofs, OF_READ );
   HRESULT     ret = E_FAIL;

   dprintf(("SHELL32:MemoryStream CreateStreamOnFile(%s,%08xh)\n",
            pszFilename,
            ppstm));

   fstr = (ISHFileStream*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(ISHFileStream));
   fstr->lpvtst=&stvt;
   fstr->ref = 1;
   fstr->dwLength = GetFileSize (hFile, NULL);

   shell32_ObjCount++;

   if (!(fstr->hMapping = CreateFileMappingA(hFile,NULL,PAGE_READONLY|SEC_COMMIT,0,0,NULL)))
   {
     dprintf(("failed to create filemap.\n"));
     goto end_2;
   }

   if (!(fstr->pImage = (BYTE*)MapViewOfFile(fstr->hMapping,FILE_MAP_READ,0,0,0)))
   {
     dprintf(("failed to mmap filemap.\n"));
     goto end_3;
   }

   ret = S_OK;
   goto end_1;

end_3:   CloseHandle(fstr->hMapping);
end_2:   HeapFree(GetProcessHeap(), 0, fstr);
   fstr = NULL;

end_1:   _lclose(hFile);
   (*ppstm) = (IStream*)fstr;
   return ret;
}

/**************************************************************************
*  IStream_fnQueryInterface
*/
static HRESULT WINAPI IStream_fnQueryInterface(IStream *iface, REFIID riid, LPVOID *ppvObj)
{
   ICOM_THIS(ISHFileStream, iface);

   char    xriid[50];
   WINE_StringFromCLSID((LPCLSID)riid,xriid);

   dprintf(("SHELL32:MemoryStream IStream_fnQueryInterface(%p)->(\n\tIID:\t%s,%p)\n",
            This,xriid,ppvObj));

   *ppvObj = NULL;

   if(IsEqualIID(riid, &IID_IUnknown) ||
      IsEqualIID(riid, &IID_IStream))
   {
     *ppvObj = This;
   }

   if(*ppvObj)
   {
     IStream_AddRef((IStream*)*ppvObj);
     dprintf(("Interface: (%p)->(%p)\n",ppvObj,*ppvObj));
     return S_OK;
   }
   dprintf(("-- Interface: E_NOINTERFACE\n"));
   return E_NOINTERFACE;
}

/**************************************************************************
*  IStream_fnAddRef
*/
static ULONG WINAPI IStream_fnAddRef(IStream *iface)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnAddRef (%p)->(count=%lu)\n",
            This, This->ref));

   shell32_ObjCount++;
   return ++(This->ref);
}

/**************************************************************************
*  IStream_fnRelease
*/
static ULONG WINAPI IStream_fnRelease(IStream *iface)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnRelease(%p)->()\n",
            This));

   shell32_ObjCount--;

   if (!--(This->ref))
   { dprintf((" destroying SHFileStream (%p)\n",This));

     UnmapViewOfFile(This->pImage);
     CloseHandle(This->hMapping);

     HeapFree(GetProcessHeap(),0,This);
     return 0;
   }
   return This->ref;
}

static HRESULT WINAPI IStream_fnRead (IStream * iface, void* pv, ULONG cb, ULONG* pcbRead)
{
   DWORD dwBytesToRead, dwBytesLeft;

   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnRead(%p)->(%p,%p,%p)\n",
            This, pv, cb, pcbRead));

   if ( !pv )
     return STG_E_INVALIDPOINTER;

   dwBytesLeft = This->dwLength - This->dwPos;

   if ( 0 >= dwBytesLeft )                              /* end of buffer */
     return S_FALSE;

   dwBytesToRead = ( cb > dwBytesLeft) ? dwBytesLeft : cb;

   memmove ( pv, (This->pImage) + (This->dwPos), dwBytesToRead);

   This->dwPos += dwBytesToRead;                     /* adjust pointer */

   if (pcbRead)
     *pcbRead = dwBytesToRead;

   return S_OK;
}
static HRESULT WINAPI IStream_fnWrite (IStream * iface, const void* pv, ULONG cb, ULONG* pcbWritten)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnWrite (%p) not implemented\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnSeek (IStream * iface, LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnSeek (%p) not implemented\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnSetSize (IStream * iface, ULARGE_INTEGER libNewSize)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnSetSize (%p) not implemented\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnCopyTo (IStream * iface, IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnCopyTo (%p) not implemented\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnCommit (IStream * iface, DWORD grfCommitFlags)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnCommit (%p) not implemented\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnRevert (IStream * iface)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnRevert (%p) not implemented\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnLockRegion (IStream * iface, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnLockRegion (%p) not implemented\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnUnlockRegion (IStream * iface, ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnUnlockRegoin (%p) not implemented.\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnStat (IStream * iface, STATSTG*   pstatstg, DWORD grfStatFlag)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnStat (%p) not implemented.\n",This));

   return E_NOTIMPL;
}
static HRESULT WINAPI IStream_fnClone (IStream * iface, IStream** ppstm)
{
   ICOM_THIS(ISHFileStream, iface);

   dprintf(("SHELL32:MemoryStream IStream_fnClone (%p) not implemented.\n",This));

   return E_NOTIMPL;
}
