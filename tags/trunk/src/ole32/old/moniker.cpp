/* $Id: moniker.cpp,v 1.1 2001-04-26 19:26:11 sandervl Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * COM Monikers.
 * 
 * 1/7/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Some portions from Wine Implementation
 *   Copyright 1998  Marcus Meissner
 *   Copyright 1999  Noomen Hamza
 */

#include "ole32.h"

#include "moniker.h"

// ======================================================================
// DATA
// ======================================================================

// Singleton instance of ROT
RunningObjectTableImpl * runningObjectTableInstance = 0;

// VTABLE instance
static ICOM_VTABLE(IRunningObjectTable) VT_RunningObjectTableImpl =
{
    RunningObjectTableImpl_QueryInterface, 
    RunningObjectTableImpl_AddRef, 
    RunningObjectTableImpl_Release, 
    RunningObjectTableImpl_Register, 
    RunningObjectTableImpl_Revoke, 
    RunningObjectTableImpl_IsRunning, 
    RunningObjectTableImpl_GetObject, 
    RunningObjectTableImpl_NoteChangeTime, 
    RunningObjectTableImpl_GetTimeOfLastChange, 
    RunningObjectTableImpl_EnumRunning
};

// ======================================================================
// Local Methods
// ======================================================================

// ----------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------
HRESULT RunningObjectTableImpl_Initialize()
{
    // create singleton ROT
    runningObjectTableInstance = (RunningObjectTableImpl *)
			    HeapAlloc(GetProcessHeap(), 0, sizeof(RunningObjectTableImpl));

    if (runningObjectTableInstance == 0)
	return E_OUTOFMEMORY;

    /* initialize the virtual table function */
    runningObjectTableInstance->lpvtbl = &VT_RunningObjectTableImpl;
    runningObjectTableInstance->ref = 1;
    runningObjectTableInstance->runObjTabSize = ROT_BLOCK_SIZE;
    runningObjectTableInstance->runObjTabRegister = 0;
    runningObjectTableInstance->runObjTabLastIndx = 0;
    runningObjectTableInstance->runObjTab = (RunObject *)
			HeapAlloc(GetProcessHeap(), 0, sizeof(RunObject[ROT_BLOCK_SIZE]));

    if (runningObjectTableInstance->runObjTab == NULL)
	return E_OUTOFMEMORY;

    return S_OK;
}

// ----------------------------------------------------------------------
// UnInitialize
// ----------------------------------------------------------------------
HRESULT RunningObjectTableImpl_UnInitialize()
{
    RunningObjectTableImpl_Release((IRunningObjectTable * )runningObjectTableInstance);
    Destroy();

    return S_OK;
}

// ----------------------------------------------------------------------
// Destroy
// ----------------------------------------------------------------------
static HRESULT Destroy()
{
    HeapFree(GetProcessHeap(), 0, runningObjectTableInstance->runObjTab);
    HeapFree(GetProcessHeap(), 0, runningObjectTableInstance);

    return S_OK;
}

// ----------------------------------------------------------------------
// GetObjectIndex
// ----------------------------------------------------------------------
static HRESULT GetObjectIndex(RunningObjectTableImpl * This, DWORD identReg,
						 IMoniker * pmk, DWORD * indx)
{
    DWORD i;

    if (pmk != NULL)
        /* search object identified by a moniker*/
        for(i = 0; (i < This->runObjTabLastIndx) && (!IMoniker_IsEqual(This->runObjTab[i].pmkObj, pmk) == S_OK); i++)
	    ;
    else
        /* search object identified by a register identifier*/
        for(i = 0; ((i<This->runObjTabLastIndx) && (This->runObjTab[i].identRegObj != identReg)); i++)
	    ;
    
    if (i == This->runObjTabLastIndx)
	return S_FALSE;

    if (indx != NULL)
	*indx = i;

    return S_OK;
}

// ======================================================================
// COM INTERFACE FUNCTIONS
// ======================================================================

// ----------------------------------------------------------------------
// RunningObjectTableImpl_QueryInterface
// ----------------------------------------------------------------------
HRESULT WIN32API RunningObjectTableImpl_QueryInterface
   (IRunningObjectTable * iface,
   REFIID riid,
   void**ppvObject)
{
    ICOM_THIS(RunningObjectTableImpl, iface);

    dprintf(("OLE32: RunningObjectTableImpl_QueryInterface(%p, %p, %p)", This, riid, ppvObject));

    /* validate arguments*/
    if (This == 0)
        return CO_E_NOTINITIALIZED;

    if (ppvObject == 0)
        return E_INVALIDARG;

    * ppvObject = 0;

    if (IsEqualIID(&IID_IUnknown, riid))
        * ppvObject = (IRunningObjectTable * )This;
    else
        if (IsEqualIID(&IID_IRunningObjectTable, riid))
            * ppvObject = (IRunningObjectTable * )This;

    if ((* ppvObject) == 0)
        return E_NOINTERFACE;

    RunningObjectTableImpl_AddRef(iface);

    return S_OK;
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_AddRef
// ----------------------------------------------------------------------
ULONG   WIN32API RunningObjectTableImpl_AddRef
   (IRunningObjectTable * iface)
{
    ICOM_THIS(RunningObjectTableImpl, iface);

    dprintf(("OLE32: RunningObjectTableImpl_AddRef(%p)", This));

    return ++(This->ref);
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_Release
// ----------------------------------------------------------------------
ULONG   WIN32API RunningObjectTableImpl_Release
   (IRunningObjectTable * iface)
{
    ICOM_THIS(RunningObjectTableImpl, iface);

    DWORD	i;

    dprintf(("OLE32: RunningObjectTableImpl_Release(%p)", This));

    This->ref--;

    /* unitialize ROT structure if there's no more reference to it*/
    if (This->ref == 0){

        /* release all registred objects */
        for( i = 0; i < This->runObjTabLastIndx; i++)
        {
            if (( This->runObjTab[i].regTypeObj & ROTFLAGS_REGISTRATIONKEEPSALIVE) != 0)
                IUnknown_Release(This->runObjTab[i].pObj);
 
            IMoniker_Release(This->runObjTab[i].pmkObj);
        }

       /*
        * RunningObjectTable data structure will be not destroyed here !
        * The destruction will be done only
        * when RunningObjectTableImpl_UnInitialize function is called
        */

        /* there's no more elements in the table */
        This->runObjTabRegister = 0;
        This->runObjTabLastIndx = 0;

        return 0;
    }

    return This->ref;
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_Register
// ----------------------------------------------------------------------
HRESULT WIN32API RunningObjectTableImpl_Register
   (IRunningObjectTable * iface, 
    DWORD grfFlags,           /* Registration options */
    IUnknown * punkObject,     /* Pointer to the object being registered */
    IMoniker * pmkObjectName,  /* Pointer to the moniker of the object being registered */
    DWORD * pdwRegister)       /* Pointer to the value identifying the  registration */
{
    ICOM_THIS(RunningObjectTableImpl, iface);

    dprintf(("OLE32: RunningObjectTableImpl_Register(%p, %ld, %p, %p, %p)", This, grfFlags, punkObject, pmkObjectName, pdwRegister));

    HRESULT res = S_OK;

    /* there's only two types of register : strong and or weak registration (only one must be passed on parameter) */
    if ( ( (grfFlags & ROTFLAGS_REGISTRATIONKEEPSALIVE) || !(grfFlags & ROTFLAGS_ALLOWANYCLIENT)) &&
         (!(grfFlags & ROTFLAGS_REGISTRATIONKEEPSALIVE) ||  (grfFlags & ROTFLAGS_ALLOWANYCLIENT)) &&
         (grfFlags) )
        return E_INVALIDARG;

    if ((punkObject == NULL) || (pmkObjectName == NULL) || (pdwRegister == NULL))
        return E_INVALIDARG;

    /* verify if the object to be registred was registred befor */
    if (GetObjectIndex(This, -1, pmkObjectName, NULL) == S_OK)
        res = MK_S_MONIKERALREADYREGISTERED;

    /* put the new registred object in the first free element in the table */
    This->runObjTab[This->runObjTabLastIndx].pObj = punkObject;
    This->runObjTab[This->runObjTabLastIndx].pmkObj = pmkObjectName;
    This->runObjTab[This->runObjTabLastIndx].regTypeObj = grfFlags;
    This->runObjTab[This->runObjTabLastIndx].identRegObj = This->runObjTabRegister;
    CoFileTimeNow(&(This->runObjTab[This->runObjTabLastIndx].lastModifObj));
    
    /* gives a registration identifier to the registred object*/
    (* pdwRegister)= This->runObjTabRegister;

    if (This->runObjTabRegister == 0xFFFFFFFF)
    {
        WriteLog("OLE32: Rot_Impl_Register() - runObjTabRegister: %ld overflow! ", This->runObjTabRegister);
	return E_FAIL;
    }
    This->runObjTabRegister++;
    This->runObjTabLastIndx++;
    
    if (This->runObjTabLastIndx == This->runObjTabSize)
    { /* table is full ! so it must be resized */

        This->runObjTabSize += ROT_BLOCK_SIZE; /* newsize table */
        This->runObjTab = (RunObject *)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, This->runObjTab, 
                        This->runObjTabSize * sizeof(RunObject));
        if (!This->runObjTab)
            return E_OUTOFMEMORY;
    }
    /* add a reference to the object in the strong registration case */
    if ((grfFlags & ROTFLAGS_REGISTRATIONKEEPSALIVE) != 0 )
        IUnknown_AddRef(punkObject);

    IMoniker_AddRef(pmkObjectName);
    
    return res;
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_Revoke
// ----------------------------------------------------------------------
HRESULT WIN32API RunningObjectTableImpl_Revoke
   (IRunningObjectTable * iface, 
    DWORD dwRegister)  /* Value identifying registration to be revoked*/
{

    ICOM_THIS(RunningObjectTableImpl, iface);

    dprintf(("OLE32: RunningObjectTableImpl_Revoke(%p, %ld)", This, dwRegister));

    DWORD index, j;

    /* verify if the object to be revoked was registred befor or not */
    if (GetObjectIndex(This, dwRegister, NULL, &index) == S_FALSE)
        return E_INVALIDARG;

    /* release the object if it was registred with a strong registrantion option */
    if ((This->runObjTab[index].regTypeObj & ROTFLAGS_REGISTRATIONKEEPSALIVE) != 0)
        IUnknown_Release(This->runObjTab[index].pObj);

    IMoniker_Release(This->runObjTab[index].pmkObj);
    
    /* remove the object from the table */
    for(j = index; j < This->runObjTabLastIndx - 1; j++)
        This->runObjTab[j]= This->runObjTab[j + 1];
    
    This->runObjTabLastIndx--;

    return S_OK;
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_IsRunning
// ----------------------------------------------------------------------
HRESULT WIN32API RunningObjectTableImpl_IsRunning
   (IRunningObjectTable * iface, 
    IMoniker * pmkObjectName)  /* Pointer to the moniker of the object whose status is desired */
{
    ICOM_THIS(RunningObjectTableImpl, iface);

    dprintf(("OLE32: RunningObjectTableImpl_IsRunning(%p, %p)", This, pmkObjectName));

    return GetObjectIndex(This, -1, pmkObjectName, NULL);
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_GetObject
// ----------------------------------------------------------------------
HRESULT WIN32API RunningObjectTableImpl_GetObject
   (IRunningObjectTable * iface, 
    IMoniker * pmkObjectName, /* Pointer to the moniker on the object */
    IUnknown **ppunkObject) /* Address of output variable that receives the IUnknown interface pointer */
{
    ICOM_THIS(RunningObjectTableImpl, iface);

    dprintf(("OLE32: RunningObjectTableImpl_GetObject(%p, %p, %p)", This, pmkObjectName, ppunkObject));

    DWORD index;

    if (ppunkObject == NULL)
        return E_POINTER;
    
    *ppunkObject = 0;

    /* verify if the object was registred befor or not */
    if (GetObjectIndex(This, -1, pmkObjectName, &index) == S_FALSE)
        return MK_E_UNAVAILABLE;

    /* add a reference to the object then set output object argument */
    IUnknown_AddRef(This->runObjTab[index].pObj);
    *ppunkObject = This->runObjTab[index].pObj;

    return S_OK;
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_NoteChangeTime
// ----------------------------------------------------------------------
HRESULT WIN32API RunningObjectTableImpl_NoteChangeTime
   (IRunningObjectTable * iface, 
    DWORD dwRegister,  /* Value identifying registration being updated */
    FILETIME * pfiletime) /* Pointer to structure containing object's last change time */
{
    ICOM_THIS(RunningObjectTableImpl, iface);

    dprintf(( "OLE32: RunningObjectTableImpl_NoteChangeTime(%p, %ld, %p)", This, dwRegister, pfiletime));

    DWORD index=-1;

    /* verify if the object to be changed was registred befor or not */
    if (GetObjectIndex(This, dwRegister, NULL, &index) == S_FALSE)
        return E_INVALIDARG;

    /* set the new value of the last time change */
    This->runObjTab[index].lastModifObj = (* pfiletime);

    return S_OK;
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_GetTimeOfLastChange
// ----------------------------------------------------------------------
HRESULT WIN32API RunningObjectTableImpl_GetTimeOfLastChange
   (IRunningObjectTable * iface, 
    IMoniker * pmkObjectName,  /* Pointer to moniker on the object whose status is desired */
    FILETIME * pfiletime)       /* Pointer to structure that receives object's last change time */
{
    ICOM_THIS(RunningObjectTableImpl, iface);

    dprintf(( "OLE32: RunningObjectTableImpl_GetTimeOfLastChange(%p, %p, %p)", This, pmkObjectName, pfiletime));

    DWORD index = -1;

    if (pmkObjectName == NULL || pfiletime == NULL)
        return E_INVALIDARG;

    /* verify if the object was registred befor or not */
    if (GetObjectIndex(This, -1, pmkObjectName, &index) == S_FALSE)
        return MK_E_UNAVAILABLE;;

    (* pfiletime)= This->runObjTab[index].lastModifObj;

    return S_OK;
}

// ----------------------------------------------------------------------
// RunningObjectTableImpl_EnumRunning
// ----------------------------------------------------------------------
HRESULT WIN32API RunningObjectTableImpl_EnumRunning
   (IRunningObjectTable * iface, 
    IEnumMoniker **ppenumMoniker) /* Address of output variable that receives the IEnumMoniker interface pointer */
{
    dprintf(( "OLE32: RunningObjectTableImpl_EnumRunning - stub"));
    return E_NOTIMPL;
}

// ======================================================================
// API Methods
// ======================================================================

// ----------------------------------------------------------------------
// GetRunningObjectTable
// ----------------------------------------------------------------------
HRESULT WIN32API GetRunningObjectTable(DWORD reserved, LPRUNNINGOBJECTTABLE *pprot)
{
    IID		riid = IID_IRunningObjectTable;
    HRESULT	hr;

    dprintf(("OLE32: GetRunningObjectTable"));

    if (reserved != 0)
	return E_UNEXPECTED;

    if (runningObjectTableInstance == NULL)
	return CO_E_NOTINITIALIZED;

    hr = RunningObjectTableImpl_QueryInterface(
		    (IRunningObjectTable*)runningObjectTableInstance, &riid, (void**)pprot);

    return hr;
}

