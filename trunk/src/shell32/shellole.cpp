/* $Id: shellole.cpp,v 1.4 2000-03-26 16:34:51 cbratschi Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * handling of SHELL32.DLL OLE-Objects
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1998 Juergen Schmied  <juergen.schmied@metronet.de>
 *
 * Corel WINE 20000324 level
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <stdlib.h>
#include <string.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "wine/obj_base.h"
#include "wine/obj_shelllink.h"
#include "wine/obj_shellfolder.h"
#include "wine/obj_shellbrowser.h"
#include "wine/obj_contextmenu.h"
#include "wine/obj_shellextinit.h"
#include "wine/obj_extracticon.h"

#include "shlguid.h"
#include "winversion.h"
#include "winreg.h"
#include "winerror.h"
#include "debugtools.h"

#include "shell32_main.h"

#include <misc.h>


/*****************************************************************************
 * Implementation                                                            *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHELL32-SHELLOLE)


DWORD WINAPI SHCLSIDFromStringA (LPSTR clsid, CLSID *id);
extern IShellFolder * IShellFolder_Constructor(
	IShellFolder * psf,
	LPITEMIDLIST pidl);
extern HRESULT IFSFolder_Constructor(
	IUnknown * pUnkOuter,
	REFIID riid,
	LPVOID * ppv);

/*************************************************************************
 * SHCoCreateInstance [SHELL32.102]
 *
 * NOTES
 *     exported by ordinal
 */

ODINFUNCTION5(LRESULT, SHCoCreateInstance, LPSTR,     aclsid,
                                           REFCLSID,  clsid,
                                           IUnknown*, unknownouter,
                                           REFIID,    refiid,
                                           LPVOID*,   ppv)
{
   char  xclsid[48], xiid[48];
   DWORD hres;
   IID   iid;
   CLSID * myclsid = (CLSID*)clsid;

   WINE_StringFromCLSID(refiid,xiid);

   if (!clsid)
   {
     if (!aclsid) return REGDB_E_CLASSNOTREG;
     SHCLSIDFromStringA(aclsid, &iid);
     myclsid = &iid;
   }

   WINE_StringFromCLSID(myclsid,xclsid);
   WINE_StringFromCLSID(refiid,xiid);

   dprintf(("SHELL32:SHCoCreateInstance (%p,CLSID:%s unk:%s IID:%s,%p)\n",
            aclsid,
            xclsid,
            unknownouter,xiid,ppv));

   if IsEqualCLSID(myclsid, &CLSID_ShellFSFolder)
   {
     hres = IFSFolder_Constructor(unknownouter, refiid, ppv);
   }
   else
   {
     hres = CoCreateInstance(myclsid, unknownouter, CLSCTX_INPROC_SERVER, refiid, ppv);
   }

   if(hres!=S_OK)
   {
     dprintf(("SHELL32: SHCoCreateInstance failed (0x%08lx) to create CLSID:%s IID:%s\n",
              hres,
              xclsid,
              xiid));
     dprintf(("SHELL32: SHCoCreateInstance: class not found in registry\n"));
     dprintf(("SHELL32: SHCoCreateInstance you might need to import the winedefault.reg\n"));
   }

   return hres;
}

/*************************************************************************
 * SHELL32_DllGetClassObject   [SHELL32.128]
 */

ODINFUNCTION3(HRESULT, SHELL32_DllGetClassObject, REFCLSID, rclsid,
                                                  REFIID,   iid,
                                                  LPVOID*,  ppv)
{
   HRESULT  hres = E_OUTOFMEMORY;
   LPCLASSFACTORY lpclf;

   char xclsid[50],xiid[50];
   WINE_StringFromCLSID((LPCLSID)rclsid,xclsid);
   WINE_StringFromCLSID((LPCLSID)iid,xiid);
   dprintf(("SHELL32:SHELL32_DllGetClassObject CLSID:%s, IID:%s\n",
            xclsid,
            xiid));

   *ppv = NULL;

   if(IsEqualCLSID(rclsid, &CLSID_ShellDesktop)||
      IsEqualCLSID(rclsid, &CLSID_ShellLink))
   {
     lpclf = IClassFactory_Constructor( rclsid );

     if(lpclf)
     {
       hres = IClassFactory_QueryInterface(lpclf,iid, ppv);
       IClassFactory_Release(lpclf);
     }
   }
   else
   {
     dprintf(("SHELL32:SHELL32_DllGetClassObject -- CLSID not found\n"));
     hres = CLASS_E_CLASSNOTAVAILABLE;
   }
   dprintf(("SHELL32:SHELL32_DllGetClassObject -- pointer to class factory: %p\n",
            *ppv));
   return hres;
}

/*************************************************************************
 * SHCLSIDFromString                              [SHELL32.147]
 *
 * NOTES
 *     exported by ordinal
 */
ODINFUNCTION2(DWORD, SHCLSIDFromStringA, LPSTR,  clsid,
                                         CLSID*, id)
{
   return CLSIDFromStringA(clsid, id);
}


ODINFUNCTION2(DWORD, SHCLSIDFromStringW, LPWSTR, clsid,
                                         CLSID*, id)
{
   return CLSIDFromString(clsid, id);
}


ODINFUNCTION2(DWORD, SHCLSIDFromStringAW, LPVOID, clsid,
                                          CLSID*, id)
{
   if (VERSION_OsIsUnicode())
     return SHCLSIDFromStringW ((LPWSTR)clsid, id);
   return SHCLSIDFromStringA ((LPSTR)clsid, id);
}


/*************************************************************************
 *        SHGetMalloc                          [SHELL32.220]
 * returns the interface to shell malloc.
 *
 * [SDK header win95/shlobj.h:
 * equivalent to:  #define SHGetMalloc(ppmem)   CoGetMalloc(MEMCTX_TASK, ppmem)
 * ]
 * What we are currently doing is not very wrong, since we always use the same
 * heap (ProcessHeap).
 */

ODINFUNCTION1(DWORD, SHGetMalloc, LPMALLOC*, lpmal)
{
  return CoGetMalloc(0,lpmal);
}


/*************************************************************************
 * SHGetDesktopFolder                          [SHELL32.216]
 */
LPSHELLFOLDER pdesktopfolder=NULL;

ODINFUNCTION1(DWORD, SHGetDesktopFolder, IShellFolder**, psf)
{
   HRESULT  hres = S_OK;
   LPCLASSFACTORY lpclf;
   dprintf(("SHELL32: SHGetDesktopFolder %p->(%p)\n",psf,*psf));

   *psf=NULL;

   if (!pdesktopfolder)
   {
     lpclf = IClassFactory_Constructor(&CLSID_ShellDesktop);
     if(lpclf)
     {
       hres = IClassFactory_CreateInstance(lpclf,NULL,(REFIID)&IID_IShellFolder, (LPVOID*)&pdesktopfolder);
       IClassFactory_Release(lpclf);
     }
   }

   if (pdesktopfolder)
   {
     /* even if we create the folder, add a ref so the application can´t destroy the folder*/
     IShellFolder_AddRef(pdesktopfolder);
     *psf = pdesktopfolder;
   }

   dprintf(("SHELL32: SHGetDesktopFolder-- %p->(%p)\n",psf, *psf));
   return hres;
}

/**************************************************************************
*  IClassFactory Implementation
*/

typedef struct
{
    /* IUnknown fields */
    ICOM_VTABLE(IClassFactory)* lpvtbl;
    DWORD                       ref;
    CLSID         *rclsid;
} IClassFactoryImpl;

//static ICOM_VTABLE(IClassFactory) clfvt;

/**************************************************************************
 *  IClassFactory_QueryInterface
 */
static HRESULT WINAPI IClassFactory_fnQueryInterface(
  LPCLASSFACTORY iface, REFIID riid, LPVOID *ppvObj)
{
   ICOM_THIS(IClassFactoryImpl,iface);
   char  xriid[50];
   WINE_StringFromCLSID((LPCLSID)riid,xriid);

   dprintf(("SHELL32: IClassFactory_fnQueryInterface (%p)->(\n\tIID:\t%s)\n",This,xriid));

   *ppvObj = NULL;

   if(IsEqualIID(riid, &IID_IUnknown))          /*IUnknown*/
   { *ppvObj = This;
   }
   else if(IsEqualIID(riid, &IID_IClassFactory))  /*IClassFactory*/
   { *ppvObj = (IClassFactory*)This;
   }

   if(*ppvObj)
   { IUnknown_AddRef((LPUNKNOWN)*ppvObj);
     dprintf(("SHELL32: IClassFactory_fnQueryInterface -- Interface: (%p)->(%p)\n",ppvObj,*ppvObj));
     return S_OK;
   }
   dprintf(("SHELL32: IClassFactory_fnQueryInterface -- Interface: %s E_NOINTERFACE\n", xriid));
   return E_NOINTERFACE;
}
/******************************************************************************
 * IClassFactory_AddRef
 */
static ULONG WINAPI IClassFactory_fnAddRef(LPCLASSFACTORY iface)
{
   ICOM_THIS(IClassFactoryImpl,iface);
   dprintf(("SHELL32: IClassFactory_fnAddRef (%p)->(count=%lu)\n",This,This->ref));

   shell32_ObjCount++;
   return ++(This->ref);
}
/******************************************************************************
 * IClassFactory_Release
 */
static ULONG WINAPI IClassFactory_fnRelease(LPCLASSFACTORY iface)
{
   ICOM_THIS(IClassFactoryImpl,iface);
   dprintf(("SHELL32: IClassFactory_fnRelease (%p)->(count=%lu)\n",This,This->ref));

   shell32_ObjCount--;
   if (!--(This->ref))
   { dprintf(("SHELL32: IClassFactory_fnRelease -- destroying IClassFactory(%p)\n",This));
      HeapFree(GetProcessHeap(),0,This);
      return 0;
   }
   return This->ref;
}
/******************************************************************************
 * IClassFactory_CreateInstance
 */
static HRESULT WINAPI IClassFactory_fnCreateInstance(
  LPCLASSFACTORY iface, LPUNKNOWN pUnknown, REFIID riid, LPVOID *ppObject)
{
   ICOM_THIS(IClassFactoryImpl,iface);
   IUnknown *pObj = NULL;
   HRESULT hres;
   char  xriid[50];

   WINE_StringFromCLSID((LPCLSID)riid,xriid);
   dprintf(("SHELL32: IClassFactory_fnCreateInstance %p->(%p,\n\tIID:\t%s,%p)\n",This,pUnknown,xriid,ppObject));

   *ppObject = NULL;

   if(pUnknown)
   {
     return(CLASS_E_NOAGGREGATION);
   }

   if (IsEqualCLSID(This->rclsid, &CLSID_ShellDesktop))
   {
     pObj = (IUnknown *)ISF_Desktop_Constructor();
   }
   else if (IsEqualCLSID(This->rclsid, &CLSID_ShellLink))
   {
     pObj = (IUnknown *)IShellLink_Constructor(FALSE);
   }
   else
   {
     dprintf(("SHELL32: IClassFactory_fnCreateInstance unknown IID requested\n\tIID:\t%s\n",xriid));
     return(E_NOINTERFACE);
   }

   if (!pObj)
   {
     return(E_OUTOFMEMORY);
   }

   hres = IUnknown_QueryInterface(pObj,riid, ppObject);
   IUnknown_Release(pObj);

   dprintf(("SHELL32: IClassFactory_fnCreateInstance -- Object created: (%p)->%p\n",This,*ppObject));

   return hres;
}
/******************************************************************************
 * IClassFactory_LockServer
 */
static HRESULT WINAPI IClassFactory_fnLockServer(LPCLASSFACTORY iface, BOOL fLock)
{
   ICOM_THIS(IClassFactoryImpl,iface);
   dprintf(("SHELL32: IClassFactory_fnLockServer %p->(0x%x), not implemented\n",This, fLock));
   return E_NOTIMPL;
}

static ICOM_VTABLE(IClassFactory) clfvt =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IClassFactory_fnQueryInterface,
    IClassFactory_fnAddRef,
  IClassFactory_fnRelease,
  IClassFactory_fnCreateInstance,
  IClassFactory_fnLockServer
};

/**************************************************************************
 *  IClassFactory_Constructor
 */

LPCLASSFACTORY IClassFactory_Constructor(REFCLSID rclsid)
{
   IClassFactoryImpl* lpclf;

   lpclf= (IClassFactoryImpl*)HeapAlloc(GetProcessHeap(),0,sizeof(IClassFactoryImpl));
   lpclf->ref = 1;
   lpclf->lpvtbl = &clfvt;
   lpclf->rclsid = (CLSID*)rclsid;

   dprintf(("SHELL32: IClassFactory_Constructor (%p)->()\n",lpclf));
   shell32_ObjCount++;
   return (LPCLASSFACTORY)lpclf;
}


/**************************************************************************
 * Default ClassFactory Implementation
 *
 * SHCreateDefClassObject
 *
 * NOTES
 *  helper function for dll's without a own classfactory
 *  a generic classfactory is returned
 *  when the CreateInstance of the cf is called the callback is executed
 */
typedef HRESULT (CALLBACK * LPFNCREATEINSTANCE)(IUnknown* pUnkOuter, REFIID riid, LPVOID* ppvObject);

typedef struct
{
    ICOM_VTABLE(IClassFactory)* lpvtbl;
    DWORD                       ref;
    CLSID         *rclsid;
    LPFNCREATEINSTANCE                      lpfnCI;
    const IID *                             riidInst;
    UINT *        pcRefDll; /* pointer to refcounter in external dll (ugrrr...) */
} IDefClFImpl;

//static ICOM_VTABLE(IClassFactory) dclfvt;

/**************************************************************************
 *  IDefClF_fnQueryInterface
 */
static HRESULT WINAPI IDefClF_fnQueryInterface(
  LPCLASSFACTORY iface, REFIID riid, LPVOID *ppvObj)
{
   ICOM_THIS(IDefClFImpl,iface);
   char  xriid[50];
   WINE_StringFromCLSID((LPCLSID)riid,xriid);
   dprintf(("SHELL32: IDefClF_fnQueryInterface (%p)->(\n\tIID:\t%s)\n",This,xriid));

   *ppvObj = NULL;

   if(IsEqualIID(riid, &IID_IUnknown))          /*IUnknown*/
   { *ppvObj = This;
   }
   else if(IsEqualIID(riid, &IID_IClassFactory))  /*IClassFactory*/
   { *ppvObj = (IClassFactory*)This;
   }

   if(*ppvObj)
   { IUnknown_AddRef((LPUNKNOWN)*ppvObj);
     dprintf(("SHELL32: IDefClF_fnQueryInterface -- Interface: (%p)->(%p)\n",ppvObj,*ppvObj));
     return S_OK;
   }
   dprintf(("SHELL32: IDefClF_fnQueryInterface -- Interface: %s E_NOINTERFACE\n", xriid));
   return E_NOINTERFACE;
}
/******************************************************************************
 * IDefClF_fnAddRef
 */
static ULONG WINAPI IDefClF_fnAddRef(LPCLASSFACTORY iface)
{
   ICOM_THIS(IDefClFImpl,iface);
   dprintf(("SHELL32: IDefClF_fnAddRef (%p)->(count=%lu)\n",This,This->ref));

   shell32_ObjCount++;

   return ++(This->ref);
}
/******************************************************************************
 * IDefClF_fnRelease
 */
static ULONG WINAPI IDefClF_fnRelease(LPCLASSFACTORY iface)
{
   ICOM_THIS(IDefClFImpl,iface);
   dprintf(("SHELL32: IDefClF_fnRelease (%p)->(count=%lu)\n",This,This->ref));

   shell32_ObjCount--;

   if (!--(This->ref))
   {
     if (This->pcRefDll)
       (*This->pcRefDll)--;

     dprintf(("SHELL32: IDefClF_fn -- destroying IClassFactory(%p)\n",This));
     HeapFree(GetProcessHeap(),0,This);
     return 0;
   }
   return This->ref;
}
/******************************************************************************
 * IDefClF_fnCreateInstance
 */
static HRESULT WINAPI IDefClF_fnCreateInstance(
  LPCLASSFACTORY iface, LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObject)
{
   ICOM_THIS(IDefClFImpl,iface);
   char  xriid[50];

   WINE_StringFromCLSID((LPCLSID)riid,xriid);
   dprintf(("SHELL32: IDefClF_fnCreateInterface %p->(%p,\n\tIID:\t%s,%p)\n",This,pUnkOuter,xriid,ppvObject));

   *ppvObject = NULL;

   if(pUnkOuter)
     return(CLASS_E_NOAGGREGATION);

   if ( This->riidInst==NULL ||
        IsEqualCLSID(riid, This->riidInst) ||
        IsEqualCLSID(riid, &IID_IUnknown) )
   {
     return This->lpfnCI(pUnkOuter, riid, ppvObject);
   }

   dprintf(("SHELL32: IDefClF_fn unknown IID requested\n\tIID:\t%s\n",xriid));
   return E_NOINTERFACE;
}
/******************************************************************************
 * IDefClF_fnLockServer
 */
static HRESULT WINAPI IDefClF_fnLockServer(LPCLASSFACTORY iface, BOOL fLock)
{
   ICOM_THIS(IDefClFImpl,iface);
   dprintf(("SHELL32: IDefClF_fnLockServer %p->(0x%x), not implemented\n",This, fLock));
   return E_NOTIMPL;
}

static ICOM_VTABLE(IClassFactory) dclfvt =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IDefClF_fnQueryInterface,
    IDefClF_fnAddRef,
  IDefClF_fnRelease,
  IDefClF_fnCreateInstance,
  IDefClF_fnLockServer
};

/**************************************************************************
 *  IDefClF_fnConstructor
 */

IClassFactory * IDefClF_fnConstructor(LPFNCREATEINSTANCE lpfnCI, UINT * pcRefDll, REFIID riidInst)
{
   IDefClFImpl* lpclf;
   char  xriidInst[50];

   WINE_StringFromCLSID((LPCLSID)riidInst,xriidInst);

   lpclf = (IDefClFImpl*)HeapAlloc(GetProcessHeap(),0,sizeof(IDefClFImpl));
   lpclf->ref = 1;
   lpclf->lpvtbl = &dclfvt;
   lpclf->lpfnCI = lpfnCI;
   lpclf->pcRefDll = pcRefDll;

   if (pcRefDll)
     (*pcRefDll)++;

   lpclf->riidInst = riidInst;

   dprintf(("SHELL32: IDefClF_fnConstructor (%p)\n\tIID:\t%s\n",lpclf, xriidInst));
   shell32_ObjCount++;
   return (LPCLASSFACTORY)lpclf;
}


/******************************************************************************
 * SHCreateDefClassObject                      [SHELL32.70]
 */

ODINFUNCTION5(HRESULT, SHCreateDefClassObject, REFIID,            riid,
                                               LPVOID*,           ppv,
                                               LPFNCREATEINSTANCE,lpfnCI,
                                               UINT*,             pcRefDll,
                                               REFIID,            riidInst)
{

   char xriid[50],xriidInst[50];
   WINE_StringFromCLSID((LPCLSID)riid,xriid);
   WINE_StringFromCLSID((LPCLSID)riidInst,xriidInst);

   dprintf(("SHELL32: SHCreateDefClassObject \n\tIID:\t%s %p %p %p \n\tIIDIns:\t%s\n",
   xriid, ppv, lpfnCI, pcRefDll, xriidInst));

   if ( IsEqualCLSID(riid, &IID_IClassFactory) )
   {
     IClassFactory * pcf = IDefClF_fnConstructor(lpfnCI, pcRefDll, riidInst);
     if (pcf)
     {
       *ppv = pcf;
       return NOERROR;
     }
     return E_OUTOFMEMORY;
   }
   return E_NOINTERFACE;
}

