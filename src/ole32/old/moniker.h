/* $Id: moniker.h,v 1.1 2001-04-26 19:26:12 sandervl Exp $ */
/*
 * moniker.h [OLE32]
 *
 * Private include for Running Object Table (ROT) implementation class
 */

#if !defined(MONIKER_INCLUDED)
#define MONIKER_INCLUDED

#define ROT_BLOCK_SIZE	50

// ROT Element
struct RunObject
{
    IUnknown *	pObj;			// object
    IMoniker *	pmkObj;			// moniker who identifies this object
    FILETIME	lastModifObj;		// Last modified date/time
    DWORD	identRegObj;		// registration key relative to this object
    DWORD      	regTypeObj;		// registration type : strong or weak
};

// Implementation structure for ROT
struct RunningObjectTableImpl
{
    ICOM_VTABLE(IRunningObjectTable)*  lpvtbl;
    ULONG      	ref;

    RunObject * runObjTab;            	// First object
    DWORD      	runObjTabSize;		// ROT size
    DWORD      	runObjTabLastIndx;	// First free ROT entry
    DWORD      	runObjTabRegister;	// Registration key of the next registred object
};

// Initialisation
extern HRESULT RunningObjectTableImpl_Initialize();
extern HRESULT RunningObjectTableImpl_UnInitialize();

static HRESULT Destroy();
static HRESULT GetObjectIndex(RunningObjectTableImpl * This, DWORD identReg, IMoniker * pmk, DWORD * indx);

static HRESULT WIN32API RunningObjectTableImpl_QueryInterface
    (IRunningObjectTable * iface,REFIID riid,void** ppvObject);
static ULONG   WIN32API RunningObjectTableImpl_AddRef
    (IRunningObjectTable * iface);
static ULONG   WIN32API RunningObjectTableImpl_Release
    (IRunningObjectTable * iface);
static HRESULT WIN32API RunningObjectTableImpl_Register
    (IRunningObjectTable * iface, DWORD grfFlags, IUnknown * punkObject,
     IMoniker * pmkObjectName, DWORD* pdwRegister);
static HRESULT WIN32API RunningObjectTableImpl_Revoke
    (IRunningObjectTable * iface, DWORD dwRegister);
static HRESULT WIN32API RunningObjectTableImpl_IsRunning
    (IRunningObjectTable * iface, IMoniker * pmkObjectName);
static HRESULT WIN32API RunningObjectTableImpl_GetObject
    (IRunningObjectTable * iface, IMoniker * pmkObjectName, IUnknown * * ppunkObject);
static HRESULT WIN32API RunningObjectTableImpl_NoteChangeTime
    (IRunningObjectTable * iface, DWORD dwRegister, FILETIME * pfiletime);
static HRESULT WIN32API RunningObjectTableImpl_GetTimeOfLastChange
    (IRunningObjectTable * iface, IMoniker * pmkObjectName, FILETIME * pfiletime);
static HRESULT WIN32API RunningObjectTableImpl_EnumRunning
    (IRunningObjectTable * iface, IEnumMoniker * * ppenumMoniker);

#endif // MONIKER_INCLUDED

