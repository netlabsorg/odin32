/* $Id: dataobject.cpp,v 1.2 2000-03-24 17:17:26 cbratschi Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*
 *      IEnumFORMATETC, IDataObject
 *
 * selecting and droping objects within the shell and/or common dialogs
 *
 *      Copyright 1998, 1999    <juergen.schmied@metronet.de>
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
#define CINTERFACE 1

#include "winerror.h"
#include "debugtools.h"

#include "oleidl.h"
#include "pidl.h"
#include "winerror.h"
#include "shell32_main.h"
#include "debugtools.h"
#include "wine/undocshell.h"
#include "wine/obj_dataobject.h"

#include <misc.h>


ODINDEBUGCHANNEL(shell32-dataobject)


/***********************************************************************
*   IEnumFORMATETC implementation
*/

typedef struct
{
    /* IUnknown fields */
    ICOM_VTABLE(IEnumFORMATETC)* lpvtbl;
    DWORD                        ref;
    /* IEnumFORMATETC fields */
    UINT        posFmt;
    UINT        countFmt;
    LPFORMATETC pFmt;
} IEnumFORMATETCImpl;

static HRESULT WINAPI IEnumFORMATETC_fnQueryInterface(LPENUMFORMATETC iface, REFIID riid, LPVOID* ppvObj);
static ULONG WINAPI IEnumFORMATETC_fnAddRef(LPENUMFORMATETC iface);
static ULONG WINAPI IEnumFORMATETC_fnRelease(LPENUMFORMATETC iface);
static HRESULT WINAPI IEnumFORMATETC_fnNext(LPENUMFORMATETC iface, ULONG celt, FORMATETC* rgelt, ULONG* pceltFethed);
static HRESULT WINAPI IEnumFORMATETC_fnSkip(LPENUMFORMATETC iface, ULONG celt);
static HRESULT WINAPI IEnumFORMATETC_fnReset(LPENUMFORMATETC iface);
static HRESULT WINAPI IEnumFORMATETC_fnClone(LPENUMFORMATETC iface, LPENUMFORMATETC* ppenum);

static struct ICOM_VTABLE(IEnumFORMATETC) efvt =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
        IEnumFORMATETC_fnQueryInterface,
        IEnumFORMATETC_fnAddRef,
    IEnumFORMATETC_fnRelease,
    IEnumFORMATETC_fnNext,
    IEnumFORMATETC_fnSkip,
    IEnumFORMATETC_fnReset,
    IEnumFORMATETC_fnClone
};

LPENUMFORMATETC IEnumFORMATETC_Constructor(UINT cfmt, const FORMATETC afmt[])
{
        IEnumFORMATETCImpl* ef;
        DWORD size=cfmt * sizeof(FORMATETC);

  dprintf(("SHELL32:dataobject:IEnumFORMATETC_Constructor(%08xh,%08xh)\n",
           cfmt,
           afmt));

        ef=(IEnumFORMATETCImpl*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IEnumFORMATETCImpl));

        if(ef)
        {
          ef->ref=1;
          ef->lpvtbl=&efvt;

          ef->countFmt = cfmt;
          ef->pFmt = (FORMATETC*)SHAlloc (size);

          if (ef->pFmt)
          {
            memcpy(ef->pFmt, afmt, size);
          }

          shell32_ObjCount++;
        }

        TRACE("(%p)->()\n",ef);
        return (LPENUMFORMATETC)ef;
}


static HRESULT WINAPI IEnumFORMATETC_fnQueryInterface(LPENUMFORMATETC iface, REFIID riid, LPVOID* ppvObj)
{
        ICOM_THIS(IEnumFORMATETCImpl,iface);
        char    xriid[50];
        WINE_StringFromCLSID((LPCLSID)riid,xriid);

  dprintf(("SHELL32:dataobject:IEnumFORMATETC_fnQueryInterface(%08xh,%08xh,%08xh)\n",
           This,
           riid,
           xriid));

                        *ppvObj = NULL;

        if(IsEqualIID(riid, &IID_IUnknown))
        { *ppvObj = This;
        }
        else if(IsEqualIID(riid, &IID_IEnumFORMATETC))
        { *ppvObj = (IDataObject*)This;
        }

        if(*ppvObj)
        { IEnumFORMATETC_AddRef((IEnumFORMATETC*)*ppvObj);
          TRACE("-- Interface: (%p)->(%p)\n",ppvObj,*ppvObj);
          return S_OK;
        }
        TRACE("-- Interface: E_NOINTERFACE\n");
        return E_NOINTERFACE;

}


static ULONG WINAPI IEnumFORMATETC_fnAddRef(LPENUMFORMATETC iface)
{
        ICOM_THIS(IEnumFORMATETCImpl,iface);

  dprintf(("SHELL32:dataobject:IEnumFORMATETC_fnAddRef(%08xh,%08xh)\n",
           This,
           This->ref));

        shell32_ObjCount++;
        return ++(This->ref);
}


static ULONG WINAPI IEnumFORMATETC_fnRelease(LPENUMFORMATETC iface)
{
        ICOM_THIS(IEnumFORMATETCImpl,iface);

  dprintf(("SHELL32:dataobject:IEnumFORMATETC_fnRelease(%08xh)\n",
           This));

        shell32_ObjCount--;

        if (!--(This->ref))
        { TRACE(" destroying IEnumFORMATETC(%p)\n",This);
          if (This->pFmt)
          { SHFree (This->pFmt);
          }
          HeapFree(GetProcessHeap(),0,This);
          return 0;
        }
        return This->ref;
}


static HRESULT WINAPI IEnumFORMATETC_fnNext(LPENUMFORMATETC iface, ULONG celt, FORMATETC *rgelt, ULONG *pceltFethed)
{
        ICOM_THIS(IEnumFORMATETCImpl,iface);
        UINT cfetch;
        HRESULT hres = S_FALSE;

  dprintf(("SHELL32:dataobject:IEnumFORMATETC_fnNext(%08xh,%08xh,%08xh,%08xh)\n",
           This,
           celt,
           rgelt,
           pceltFethed));

        if (This->posFmt < This->countFmt)
        {
          cfetch = This->countFmt - This->posFmt;
          if (cfetch >= celt)
          {
            cfetch = celt;
            hres = S_OK;
          }
          memcpy(rgelt, &This->pFmt[This->posFmt], cfetch * sizeof(FORMATETC));
          This->posFmt += cfetch;
        }
        else
        {
          cfetch = 0;
        }

        if (pceltFethed)
        {
          *pceltFethed = cfetch;
        }

        return hres;
}


static HRESULT WINAPI IEnumFORMATETC_fnSkip(LPENUMFORMATETC iface, ULONG celt)
{
        ICOM_THIS(IEnumFORMATETCImpl,iface);

  dprintf(("SHELL32:dataobject:IEnumFORMATETC_fnSkip(%08xh,%08xh) to be fixed\n",
           This,
           celt));

        This->posFmt += celt;
        if (This->posFmt > This->countFmt)
        {
          This->posFmt = This->countFmt;
          return S_FALSE;
        }
        return S_OK;
}
static HRESULT WINAPI IEnumFORMATETC_fnReset(LPENUMFORMATETC iface)
{
        ICOM_THIS(IEnumFORMATETCImpl,iface);

  dprintf(("SHELL32:dataobject:IEnumFORMATETC_fnReset(%08xh) to be fixed\n",
           This));

        This->posFmt = 0;
        return S_OK;
}


static HRESULT WINAPI IEnumFORMATETC_fnClone(LPENUMFORMATETC iface, LPENUMFORMATETC* ppenum)
{
        ICOM_THIS(IEnumFORMATETCImpl,iface);

  dprintf(("SHELL32:dataobject:IEnumFORMATETC_fnClone(%08xh) not implemented\n",
           This,
           ppenum));

        return E_NOTIMPL;
}

/**************************************************************************
 *  IDLList "Item ID List List"
 *
 *  NOTES
 *   interal data holder for IDataObject
 */
#define STDMETHOD(xfn) HRESULT (CALLBACK *fn##xfn)
#define STDMETHOD_(type,xfn) type (CALLBACK *fn##xfn)
#define THIS_ THIS,

typedef struct tagLPIDLLIST *LPIDLLIST, IDLList;

#define THIS LPIDLLIST me
typedef enum
{ State_UnInit=1,
  State_Init=2,
  State_OutOfMem=3
} IDLListState;

typedef struct IDLList_VTable
{ STDMETHOD_(UINT, GetState)(THIS);
  STDMETHOD_(LPITEMIDLIST, GetElement)(THIS_ UINT nIndex);
  STDMETHOD_(UINT, GetCount)(THIS);
  STDMETHOD_(BOOL, StoreItem)(THIS_ LPITEMIDLIST pidl);
  STDMETHOD_(BOOL, AddItems)(THIS_ LPITEMIDLIST *apidl, UINT cidl);
  STDMETHOD_(BOOL, InitList)(THIS);
  STDMETHOD_(void, CleanList)(THIS);
} IDLList_VTable,*LPIDLLIST_VTABLE;

struct tagLPIDLLIST
{ LPIDLLIST_VTABLE  lpvtbl;
  HDPA  dpa;
  UINT  uStep;
};

extern LPIDLLIST IDLList_Constructor (UINT uStep);
extern void IDLList_Destructor(LPIDLLIST me);
#undef THIS



/**************************************************************************
 *  IDLList "Item ID List List"
 *
 */
static UINT WINAPI IDLList_GetState(LPIDLLIST THIS);
static LPITEMIDLIST WINAPI IDLList_GetElement(LPIDLLIST THIS, UINT nIndex);
static UINT WINAPI IDLList_GetCount(LPIDLLIST THIS);
static BOOL WINAPI IDLList_StoreItem(LPIDLLIST THIS, LPITEMIDLIST pidl);
static BOOL WINAPI IDLList_AddItems(LPIDLLIST THIS, LPITEMIDLIST *apidl, UINT cidl);
static BOOL WINAPI IDLList_InitList(LPIDLLIST THIS);
static void WINAPI IDLList_CleanList(LPIDLLIST THIS);

static IDLList_VTable idllvt =
{       IDLList_GetState,
        IDLList_GetElement,
        IDLList_GetCount,
        IDLList_StoreItem,
        IDLList_AddItems,
        IDLList_InitList,
        IDLList_CleanList
};

LPIDLLIST IDLList_Constructor (UINT uStep)
{
        LPIDLLIST lpidll;
        lpidll = (LPIDLLIST)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IDLList));

  dprintf(("SHELL32:dataobject:IDLList_Constructor(%08xh, %08xh) not implemented\n",
           lpidll,
           uStep));

        if (lpidll)
        {
          lpidll->lpvtbl=&idllvt;
          lpidll->uStep=uStep;
        }

        return lpidll;
}


void IDLList_Destructor(LPIDLLIST THIS)
{
  dprintf(("SHELL32:dataobject:IDLList_Destructor(%08xh) not implemented\n",
           THIS));

        IDLList_CleanList(THIS);
}

static UINT WINAPI IDLList_GetState(LPIDLLIST THIS)
{
  dprintf(("SHELL32:dataobject:IDLList_GetState((%p)->(uStep=%u dpa=%p)\n",
           THIS,
           THIS->uStep,
           THIS->dpa));

        if (THIS->uStep == 0)
        {
          if (THIS->dpa)
            return(State_Init);

          return(State_OutOfMem);
        }
        return(State_UnInit);
}


static LPITEMIDLIST WINAPI IDLList_GetElement(LPIDLLIST THIS, UINT nIndex)
{
  dprintf(("SHELL32:dataobject:IDLList_GetElement((%p)->(index=%u)\n",
           THIS,
           nIndex));

        return((LPITEMIDLIST)pDPA_GetPtr(THIS->dpa, nIndex));
}


static UINT WINAPI IDLList_GetCount(LPIDLLIST THIS)
{
  dprintf(("SHELL32:dataobject:IDLList_GetCount(%p)\n",
           THIS));

        return(IDLList_GetState(THIS)==State_Init ? DPA_GetPtrCount(THIS->dpa) : 0);
}


static BOOL WINAPI IDLList_StoreItem(LPIDLLIST THIS, LPITEMIDLIST pidl)
{
  dprintf(("SHELL32:dataobject:IDLList_StoreItem((%p)->(pidl=%p)\n",
           THIS,
           pidl));

        if (pidl)
        { if (IDLList_InitList(THIS) && pDPA_InsertPtr(THIS->dpa, 0x7fff, (LPSTR)pidl)>=0)
            return(TRUE);
          ILFree(pidl);
        }
        IDLList_CleanList(THIS);
        return(FALSE);
}


static BOOL WINAPI IDLList_AddItems(LPIDLLIST THIS, LPITEMIDLIST *apidl, UINT cidl)
{
        INT i;

  dprintf(("SHELL32:dataobject:IDLList_AddItems((%p)->(apidl=%p cidl=%u)\n",
           THIS,
           apidl,
           cidl));

        for (i=0; i<cidl; ++i)
        { if (!IDLList_StoreItem(THIS, ILClone((LPCITEMIDLIST)apidl[i])))
            return(FALSE);
        }
        return(TRUE);
}


static BOOL WINAPI IDLList_InitList(LPIDLLIST THIS)
{
  dprintf(("SHELL32:dataobject:IDLList_InitList(%p)\n",
           THIS));

        switch (IDLList_GetState(THIS))
        { case State_Init:
            return(TRUE);

          case State_OutOfMem:
            return(FALSE);

          case State_UnInit:
          default:
            THIS->dpa = pDPA_Create(THIS->uStep);
            THIS->uStep = 0;
            return(IDLList_InitList(THIS));
        }
}


static void WINAPI IDLList_CleanList(LPIDLLIST THIS)
{
        INT i;

  dprintf(("SHELL32:dataobject:IDLList_CleanList(%p)\n",
           THIS));

        if (THIS->uStep != 0)
        { THIS->dpa = NULL;
          THIS->uStep = 0;
          return;
        }

        if (!THIS->dpa)
        { return;
        }

        for (i=DPA_GetPtrCount(THIS->dpa)-1; i>=0; --i)
        { ILFree(IDLList_GetElement(THIS,i));
        }

        pDPA_Destroy(THIS->dpa);
        THIS->dpa=NULL;
}


/***********************************************************************
*   IDataObject implementation
*/
/* number of supported formats */
#define MAX_FORMATS 1

typedef struct
{
        /* IUnknown fields */
        ICOM_VTABLE(IDataObject)* lpvtbl;
        DWORD           ref;

        /* IDataObject fields */
        LPIDLLIST       lpill;       /* the data of the dataobject */
        LPITEMIDLIST    pidl;

        FORMATETC       pFormatEtc[MAX_FORMATS];
        UINT            cfShellIDList;
        UINT            cfFileGroupDesc;
        UINT            cfFileContents;

} IDataObjectImpl;

//static struct ICOM_VTABLE(IDataObject) dtovt;

/***************************************************************************
*  IDataObject_QueryInterface
*/
static HRESULT WINAPI IDataObject_fnQueryInterface(LPDATAOBJECT iface, REFIID riid, LPVOID * ppvObj)
{
        ICOM_THIS(IDataObjectImpl,iface);
        char    xriid[50];
        WINE_StringFromCLSID((LPCLSID)riid,xriid);

  dprintf(("SHELL32:dataobject:IDataObject_fnQueryInterface((%p)->(\n\tIID:\t%s,%p)\n",
           This,
           xriid,
           ppvObj));

        *ppvObj = NULL;

        if(IsEqualIID(riid, &IID_IUnknown))          /*IUnknown*/
        { *ppvObj = This;
        }
        else if(IsEqualIID(riid, &IID_IDataObject))  /*IDataObject*/
        { *ppvObj = (IDataObject*)This;
        }

        if(*ppvObj)
        { IDataObject_AddRef((IDataObject*)*ppvObj);
          TRACE("-- Interface: (%p)->(%p)\n",ppvObj,*ppvObj);
          return S_OK;
        }
        TRACE("-- Interface: E_NOINTERFACE\n");
        return E_NOINTERFACE;
}
/**************************************************************************
*  IDataObject_AddRef
*/
static ULONG WINAPI IDataObject_fnAddRef(LPDATAOBJECT iface)
{
        ICOM_THIS(IDataObjectImpl,iface);

  dprintf(("SHELL32:dataobject:IDataObject_fnAddRef((%p)->(count=%lu)\n",
           This,
           This->ref));

        shell32_ObjCount++;
        return ++(This->ref);
}
/**************************************************************************
*  IDataObject_Release
*/
static ULONG WINAPI IDataObject_fnRelease(LPDATAOBJECT iface)
{
        ICOM_THIS(IDataObjectImpl,iface);
  dprintf(("SHELL32:dataobject:IDataObject_fnRelease(%p)\n",
           This));
        shell32_ObjCount--;

        if (!--(This->ref))
        { TRACE(" destroying IDataObject(%p)\n",This);
          IDLList_Destructor(This->lpill);
          HeapFree(GetProcessHeap(),0,This);
          return 0;
        }
        return This->ref;
}

/**************************************************************************
* IDataObject_fnGetData
*/
static HRESULT WINAPI IDataObject_fnGetData(LPDATAOBJECT iface, LPFORMATETC pformatetcIn, STGMEDIUM *pmedium)
{
        ICOM_THIS(IDataObjectImpl,iface);

        char    szTemp[256];
        UINT    cItems;
        DWORD   sizeCIDA, sizePidl, nOffset, nSize;
        LPCIDA  pcida;
        HGLOBAL hmem;
        int     i;
        LPITEMIDLIST    pidl;

        GetClipboardFormatNameA (pformatetcIn->cfFormat, szTemp, 256);

  dprintf(("SHELL32:dataobject:IDataObject_fnGetData(%p)->(%p %p format=%s)\n",
           This,
           pformatetcIn,
           pmedium,
           szTemp));

        /* test expected format */
        if (!(pformatetcIn->cfFormat == This->cfShellIDList))
        {
          FIXME("-- clipformat not implemented\n");
          return (E_INVALIDARG);
        }

        if (pformatetcIn->ptd==NULL
                && (pformatetcIn->dwAspect & DVASPECT_CONTENT)
                && pformatetcIn->lindex==-1
                && (pformatetcIn->tymed&TYMED_HGLOBAL))
        {
          cItems = This->lpill->lpvtbl->fnGetCount(This->lpill);
          if (cItems < 1) return(E_UNEXPECTED);

          sizeCIDA = sizeof(CIDA) + sizeof (UINT)*(cItems);     /* without any pidl */
          sizePidl = ILGetSize (This->pidl);                    /* add root pidl */

          nSize = sizeCIDA + sizePidl;
          hmem = GlobalAlloc(GHND|GMEM_SHARE, nSize);
          if (!hmem) return(E_OUTOFMEMORY);
          pcida = (CIDA*)GlobalLock (hmem);

          nOffset = sizeCIDA;                           /* start after the CIDA */
          pcida->cidl = cItems;

          pcida->aoffset[0] = nOffset;                  /* first element */
          memcpy(((LPBYTE)pcida)+nOffset, This->pidl, sizePidl);
          nOffset += sizePidl;
          pdump(This->pidl);

          for (i=0; i< cItems; i++)
          {
            pidl = This->lpill->lpvtbl->fnGetElement(This->lpill, i);
            sizePidl = ILGetSize (pidl);
            nSize += sizePidl;                          /* size of the structure */
            pdump(pidl);

            GlobalUnlock(hmem);                         /* grow memory */
            hmem = GlobalReAlloc(hmem, nSize, GHND|GMEM_SHARE);
            if (!hmem) return(E_OUTOFMEMORY);
            pcida = (CIDA*)GlobalLock (hmem);

            pcida->aoffset[i+1] = nOffset;              /* copy element */
            memcpy(((LPBYTE)pcida)+nOffset, pidl, sizePidl);
            nOffset += sizePidl;
          }

          GlobalUnlock(hmem);

          pmedium->tymed = TYMED_HGLOBAL;
          pmedium->u.hGlobal = hmem;
          pmedium->pUnkForRelease = NULL;

          TRACE("(%p)->(cida at %p)\n", This, pcida);
          return S_OK;
        }

        FIXME("-- can't serve format\n");
        return (E_INVALIDARG);
}


static HRESULT WINAPI IDataObject_fnGetDataHere(LPDATAOBJECT iface, LPFORMATETC pformatetc, STGMEDIUM *pmedium)
{
        ICOM_THIS(IDataObjectImpl,iface);
  dprintf(("SHELL32:dataobject:IDataObject_fnGetDataHere(%p)->(%p %p) not implemented\n",
           This,
           pformatetc,
           pmedium));

        return E_NOTIMPL;
}


static HRESULT WINAPI IDataObject_fnQueryGetData(LPDATAOBJECT iface, LPFORMATETC pformatetc)
{
        ICOM_THIS(IDataObjectImpl,iface);
        UINT i;

  dprintf(("SHELL32:dataobject:IDataObject_fnQueryGetData((%p)->(fmt=0x%08x tym=0x%08lx)\n",
           This,
           pformatetc->cfFormat,
           pformatetc->tymed));

        if(!(DVASPECT_CONTENT & pformatetc->dwAspect))
          return DV_E_DVASPECT;

        /* check our formats table what we have */
        for (i=0; i<MAX_FORMATS; i++)
        {
          if ((This->pFormatEtc[i].cfFormat == pformatetc->cfFormat)
           && (This->pFormatEtc[i].tymed == pformatetc->tymed))
          {
            return S_OK;
          }
        }

        return DV_E_TYMED;
}


static HRESULT WINAPI IDataObject_fnGetCanonicalFormatEtc(LPDATAOBJECT iface, LPFORMATETC pformatetcIn, LPFORMATETC pformatetcOut)
{
        ICOM_THIS(IDataObjectImpl,iface);

  dprintf(("SHELL32:dataobject:IDataObject_fnGetCanonicalFormatEtc((%p)->(%p %p) not implemented\n",
           This,
           pformatetcIn,
           pformatetcOut));

        return E_NOTIMPL;
}


static HRESULT WINAPI IDataObject_fnSetData(LPDATAOBJECT iface, LPFORMATETC pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
        ICOM_THIS(IDataObjectImpl,iface);
  dprintf(("SHELL32:dataobject:IDataObject_fnSetData((%p)->(%p %p %08xh) not implemented\n",
           This,
           pformatetc,
           pmedium,
           fRelease));
        return E_NOTIMPL;
}


static HRESULT WINAPI IDataObject_fnEnumFormatEtc(LPDATAOBJECT iface, DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
        ICOM_THIS(IDataObjectImpl,iface);

  dprintf(("SHELL32:dataobject:IDataObject_fnEnumFormatEtc((%p)->(%08xh %p)\n",
           This,
           dwDirection,
           ppenumFormatEtc));

        *ppenumFormatEtc=NULL;

        /* only get data */
        if (DATADIR_GET == dwDirection)
        {
          *ppenumFormatEtc = IEnumFORMATETC_Constructor(MAX_FORMATS, This->pFormatEtc);
          return (*ppenumFormatEtc) ? S_OK : E_FAIL;
        }

        return E_NOTIMPL;
}
static HRESULT WINAPI IDataObject_fnDAdvise(LPDATAOBJECT iface, FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
        ICOM_THIS(IDataObjectImpl,iface);
  dprintf(("SHELL32:dataobject:IDataObject_fnDAdvise((%p)->(%p %08xh %p %p) not implemented.\n",
           This,
           pformatetc,
           advf,
           pAdvSink,
           pdwConnection));

        return E_NOTIMPL;
}


static HRESULT WINAPI IDataObject_fnDUnadvise(LPDATAOBJECT iface, DWORD dwConnection)
{
        ICOM_THIS(IDataObjectImpl,iface);
  dprintf(("SHELL32:dataobject:IDataObject_fnDUnadvise((%p)->(%08xh) not implemented.\n",
           This,
           dwConnection));
        return E_NOTIMPL;
}
static HRESULT WINAPI IDataObject_fnEnumDAdvise(LPDATAOBJECT iface, IEnumSTATDATA **ppenumAdvise)
{
        ICOM_THIS(IDataObjectImpl,iface);
  dprintf(("SHELL32:dataobject:IDataObject_fnEnumDAdvise((%p)->(%p) not implemented.\n",
           This,
           ppenumAdvise));

        return E_NOTIMPL;
}

static struct ICOM_VTABLE(IDataObject) dtovt =
{
        ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
        IDataObject_fnQueryInterface,
        IDataObject_fnAddRef,
        IDataObject_fnRelease,
        IDataObject_fnGetData,
        IDataObject_fnGetDataHere,
        IDataObject_fnQueryGetData,
        IDataObject_fnGetCanonicalFormatEtc,
        IDataObject_fnSetData,
        IDataObject_fnEnumFormatEtc,
        IDataObject_fnDAdvise,
        IDataObject_fnDUnadvise,
        IDataObject_fnEnumDAdvise
};

/**************************************************************************
*  IDataObject_Constructor
*/
LPDATAOBJECT IDataObject_Constructor(HWND hwndOwner, LPITEMIDLIST pMyPidl, LPITEMIDLIST * apidl, UINT cidl)
{
        IDataObjectImpl* dto;

        dto = (IDataObjectImpl*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IDataObjectImpl));

        if (dto)
        {
          dto->ref=1;
          dto->lpvtbl=&dtovt;
          dto->pidl=ILClone(pMyPidl);

          /* fill the ItemID List List */
          dto->lpill = IDLList_Constructor (8);
          if (! dto->lpill )
            return NULL;

          dto->lpill->lpvtbl->fnAddItems(dto->lpill, apidl, cidl);

          /* */
          dto->cfShellIDList = RegisterClipboardFormatA(CFSTR_SHELLIDLIST);
/*        dto->cfFileGroupDesc = RegisterClipboardFormatA(CFSTR_FILEDESCRIPTORA);
          dto->cfFileContents = RegisterClipboardFormatA(CFSTR_FILECONTENTS);
*/
          InitFormatEtc(dto->pFormatEtc[0], dto->cfShellIDList, TYMED_HGLOBAL);

          shell32_ObjCount++;
        }

  dprintf(("SHELL32:dataobject:IDataObject_Constructor((%p)->(apidl=%p cidl=%u)\n",
           dto,
           apidl,
           cidl));
        return (LPDATAOBJECT)dto;
}

