/* $Id: itypelib.h,v 1.1 2001-08-10 19:24:50 sandervl Exp $ */
/* 
 * OLE Typelib functions private header
 *
 * 18/11/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#if !defined(ITYPELIB_INCLUDED)
#define ITYPELIB_INCLUDED

#include "oList.h"

#define TLBMAGIC2 "MSFT"
#define TLBMAGIC1 "SLTG"
#define HELPDLLFLAG (0x0100)
#define DO_NOT_SEEK (-1)

#define HREFTYPE_INTHISFILE(href) (!((href) & 3))
#define HREFTYPE_INDEX(href) ((href) /sizeof(TLBTypeInfoBase))

typedef struct TLBCustData
{
    GUID			guid;
    VARIANT			data;
} TLBCustData;

/* internal Parameter data */
typedef struct TLBParDesc
{
    PCHAR			szName;
    oList<TLBCustData *>	pCustData;
} TLBParDesc;

/* internal Function data */
typedef struct TLBFuncDesc
{
    FUNCDESC			funcdesc;	/* info on the function and its attributes. */
    PCHAR			szName;		/* the name of this function */
    TLBParDesc *		pParamDesc;	/* array with name and custom data */
    ULONG			helpcontext;
    ULONG			lHelpStringContext;
    PCHAR			szHelpString;
    PCHAR			szEntry;	/* if its Hiword==0, it numeric; -1 is not present*/
    oList<TLBCustData *>	pCustData;
} TLBFuncDesc;

/* internal Variable data */
typedef struct TLBVarDesc
{
    VARDESC			vardesc;          /* info on the variable and its attributes. */
    PCHAR			szName;             /* the name of this variable */
    ULONG			lHelpContext;
    ULONG			lHelpStringContext;  /* fixme: where? */
    PCHAR			szHelpString;
    oList<TLBCustData *>	pCustData;
} TLBVarDesc;

/* data structure for import typelibs */
typedef struct TLBImpLib
{
    ULONG			offset;                 /* offset in the file */
    GUID			guid;                  	/* libid */
    PCHAR			name;                 	/* name; */
    struct ITypeLibImpl *	pImpTypeLib; 		/* pointer to loaded typelib */
} TLBImpLib;

/* data for refernced types in a coclass, or an inherited interface */
typedef struct TLBRefType 
{
    GUID			guid;           	/* guid of the referenced type */
							/* (important if its a imported type) */
    HREFTYPE			reference;
    ULONG			flags;
    oList<TLBCustData *>	pCustData;
    TLBImpLib *			pImpTLInfo;
} TLBRefType;

/* internal TypeInfo data */
typedef struct ITypeInfoImpl
{
    ICOM_VTABLE(ITypeInfo2) *	lpvtbl;
    ULONG			ref;			/* Reference count */
    TYPEATTR			TypeAttr ;         	/* _lots_ of type information. */
    ITypeLib *			pTypeLib; 		/* back pointer to typelib */
    ULONG			index;                  /* index in this typelib; */
    PCHAR			szName;
    PCHAR			szDocString;
    ULONG			lHelpContext;
    ULONG			lHelpStringContext;

    oList<TLBFuncDesc *>	pFunctions;    		/* Function descriptions */
    oList<TLBVarDesc *>		pVariables;    		/* Variable descriptions */
    oList<TLBRefType *>		pImplements;     	/* Implemented Interfaces  */
    oList<TLBCustData *>	pCustData;	     	/* Custom Data  */

} ITypeInfoImpl;

/* internal TypeLib data */
typedef struct ITypeLibImpl
{
    ICOM_VTABLE(ITypeLib2) *	lpvtbl;
    ULONG			ref;			// Reference count...

    TLIBATTR			LibAttr;               	/* guid,lcid,syskind,version,flags */
    PCHAR			szName;
    PCHAR			szDocString;
    PCHAR			szHelpFile;
    PCHAR			szHelpStringDll;
    ULONG 			lHelpContext;

    oList<ITypeInfoImpl *>	pTypeInfo;	     	/* Type info data */
    oList<TLBCustData *>	pCustData;	    	/* Custom data */
    oList<TLBImpLib *>		pImpLibs;	     	/* Imported typelibs */
    oList<TYPEDESC *>		pTypedesc;	       	/* array of TypeDescriptions found in the libary */
} ITypeLibImpl;

/* internal TypeComp data */
typedef struct ITypeCompImpl
{
    ICOM_VTABLE(ITypeComp) *	lpvtbl;
    ULONG			ref;			// Reference count...
} ITypeCompImpl;

/*-------------------------FILE STRUCTURES-----------------------------------*/

/*
 * structure of the typelib type2 header
 * it is at the beginning of a type lib file
 *  
 */
typedef struct TLB2Header
{
/*0x00*/ULONG			magic1;       	/* 0x5446534D "MSFT" */
        ULONG   		magic2;       	/* 0x00010002 version nr? */
        ULONG   		posguid;     	/* position of libid in guid table  */
						/* (should be,  else -1) */
        ULONG   		lcid;         	/* locale id */
/*0x10*/ULONG   		lcid2;
        ULONG   		varflags;     	/* (largely) unknown flags ,seems to be always 41 */
						/* becomes 0x51 with a helpfile defined */
						/* if help dll defined its 0x151 */
						/* update : the lower nibble is syskind */
        ULONG   		version;      	/* set with SetVersion() */
        ULONG   		flags;        	/* set with SetFlags() */
/*0x20*/ULONG   		nrtypeinfos;  	/* number of typeinfo's (till so far) */
        ULONG   		helpstring;   	/* position of help string in stringtable */
        ULONG   		helpstringcontext;
        ULONG   		helpcontext;
/*0x30*/ULONG   		nametablecount; /* number of names in name table */
        ULONG   		nametablechars; /* nr of characters in name table */
        ULONG   		nameOffset;     /* offset of name in name table */
        ULONG   		helpfile;       /* position of helpfile in stringtable */
/*0x40*/ULONG   		customDataOffset; /* if -1 no custom data, else it is offset */
						/* in customer data/guid offset table */
        ULONG   		res44;          /* unknown always: 0x20 */
        ULONG   		res48;          /* unknown always: 0x80 */
        ULONG   		dispatchpos;    /* gets a value (1+n*0x0c) Idispatch interfaces */
/*0x50*/ULONG   		res50;          /* is zero becomes one when an interface is derived */
} TLB2Header;

/* segments in the type lib file have a structure like this: */
typedef struct _tp 
{
        ULONG   		offset;		/* absolute offset in file */
        ULONG   		length;		/* length of segment */
        ULONG   		res08;		/* unknown always -1 */
        ULONG   		res0c;		/* unknown always 0x0f in the header */
						/* 0x03 in the typeinfo_data */
} pSeg;

/* layout of the main segment directory */
typedef struct TLBSegDir
{
/*1*/pSeg			pTypeInfoTab;	/* each type info get an entry of 0x64 bytes */
						/* (25 ints) */
/*2*/pSeg			pImpInfo;     	/* table with info for imported types */
/*3*/pSeg 			pImpFiles;    	/* import libaries */
/*4*/pSeg 			pRefTab;      	/* References table */
/*5*/pSeg			pLibtab;      	/* always exists, alway same size (0x80) */
						/* hash table w offsets to guid????? */
/*6*/pSeg			pGuidTab;     	/* all guids are stored here together with  */
						/* offset in some table???? */
/*7*/pSeg			pRes7;        	/* always created, alway same size (0x200) */
						/* purpose largely unknown */
/*8*/pSeg			pNameTab;     	/* name tables */
/*9*/pSeg			pStringTab;   	/*string table */
/*A*/pSeg			pTypedescTab;  	/* table with type descriptors */
/*B*/pSeg			pArrayDescriptions;
/*C*/pSeg			pCustData;    	/*  data table, used for custom data and default */
						/* parameter values */
/*D*/pSeg			pCDGuids;     	/* table with offsets for the guids and into the customer data table */
/*E*/pSeg			pResE;          /* unknown */
/*F*/pSeg			pResF;          /* unknown  */
} TLBSegDir;


/* base type info data */
typedef struct TLBTypeInfoBase
{
/*000*/ INT			typekind;       /*  it is the TKIND_xxx */
						/* some byte alignment stuf */
        INT     		memoffset;      /* points past the file, if no elements */
        INT     		res2;           /* zero if no element, N*0x40 */
        INT     		res3;           /* -1 if no lement, (N-1)*0x38 */
/*010*/ INT     		res4;           /* always? 3 */
        INT     		res5;           /* always? zero */
        INT     		cElement;       /* counts elements, HI=cVars, LO=cFuncs */
        INT     		res7;           /* always? zero */
/*020*/ INT     		res8;           /* always? zero */
        INT     		res9;           /* always? zero */
        INT     		resA;           /* always? zero */
        INT     		posguid;        /* position in guid table */
/*030*/ INT     		flags;          /* Typeflags */
        INT     		NameOffset;     /* offset in name table */
        INT     		version;        /* element version */
        INT     		docstringoffs;  /* offset of docstring in string tab */
/*040*/ INT     		helpstringcontext;  /*  */
        INT     		helpcontext;    /* */
        INT     		oCustData;      /* offset in customer data table */
        INT16   		cImplTypes;     /* nr of implemented interfaces */
        INT16   		cbSizeVft;      /* virtual table size, not including inherits */
/*050*/ INT     		size;           /* size in bytes, at least for structures */
						/* fixme: name of this field */
        INT     		datatype1;      /* position in type description table */
						/* or in base intefaces */
						/* if coclass: offset in reftable */
						/* if interface: reference to inherited if */
        INT     		datatype2;      /* if 0x8000, entry above is valid */
						/* actually dunno */
						/* else it is zero? */
        INT     		res18;          /* always? 0 */
/*060*/ INT     		res19;          /* always? -1 */
} TLBTypeInfoBase;

/* layout of an entry with information on imported types */
typedef struct TLBImpInfo
{
    INT     			res0;           /* unknown */
    INT     			oImpFile;       /* offset inthe Import File table */
    INT     			oGuid;          /* offset in Guid table */
} TLBImpInfo;

/* function description data */
typedef struct
{
    INT16   			recsize;	// record size including some xtra stuff 
    INT16   			index;		// 99.12.19 This must be the function index :-)
    INT   			DataType;       /* data type of memeber, eg return of function */
    INT   			Flags;          /* something to do with attribute flags (LOWORD) */
    INT16 			VtableOffset;   /* offset in vtable */
    INT16 			res3;           /* some offset into dunno what */
    INT   			FKCCIC;         /* bit string with the following  */
						/* meaning (bit 0 is the msb): */
						/* bit 2 indicates that oEntry is numeric */
						/* bit 3 that parameter has default values */
						/* calling convention (bits 4-7 ) */
						/* bit 8 indicates that custom data is present */
						/* Invokation kind (bits 9-12 ) */
						/* function kind (eg virtual), bits 13-15  */
    INT16 			nrargs;         /* number of arguments (including optional ????) */
    INT16 			nroargs;        /* nr of optional arguments */
						/* optional attribute fields, number variable */
    INT   			OptAttr[1];
/*
0*  INT   helpcontext;
1*  INT   oHelpString;
2*  INT   oEntry;       // either offset in string table or numeric as it is //
3*  INT   res9;         // unknown (-1) //
4*  INT   resA;         // unknown (-1) //
5*  INT   HelpStringContext;
    // these are controlled by a bit set in the FKCCIC field  //
6*  INT   oCustData;        // custom data for function //
7*  INT   oArgCustData[1];  // custom data per argument //
*/
} TLBFuncRecord;

/* after this may follow an array with default value pointers if the 
 * appropriate bit in the FKCCIC field has been set: 
 * INT   oDefautlValue[nrargs];
 */

/* Parameter info one per argument*/
typedef struct
{
    INT   			DataType;
    INT   			oName;
    INT   			Flags;
} TLBParameterInfo;

/* Variable description data */
typedef struct
{
    INT16   			recsize;	// record size including some xtra stuff 
    INT16   			unk02;		// 
    INT   			DataType;     /* data type of the variable */
    INT   			Flags;        /* VarFlags (LOWORD) */
    INT16 			VarKind;      /* VarKind */
    INT16 			res3;         /* some offset into dunno what */
    INT   			OffsValue;    /* value of the variable or the offset  */

    INT   			OptAttr[1];

//                        /* in the data structure */
//    /* optional attribute fields, the number of them is variable */
//  /* controlled by record length */
//    INT   			HelpContext;
//    INT   			oHelpString;
//    INT   			res9;         /* unknown (-1) */
//    INT   			oCustData;        /* custom data for variable */
//    INT   			HelpStringContext;
} TLBVarRecord;

/* Structure of the reference data  */
typedef struct
{
    INT   			reftype;    /* either offset in type info table, then its */
					    /* a multiple of 64 */
					    /* or offset in the external reference table */
					    /* with an offset of 1 */
    INT   			flags;
    INT   			oCustData;  /* custom data */
    INT   			onext;      /* next offset, -1 if last */
} TLBRefRecord;

/* this is how a guid is stored */
typedef struct
{
    GUID 			guid;
    INT   			unk10;       /* differntiate with libid, classid etc? */
					     /* its -2 for a libary */
					     /* it's 0 for an interface */
    INT   			unk14;       /* always? -1 */
} TLBGuidEntry;

/* some data preceding entries in the name table */
typedef struct
{
    INT   			unk00;        /* sometimes -1 (lib, parameter) ,
          			                 sometimes 0 (interface, func) */
    INT   			unk02;        /* sometimes -1 (lib) , sometimes 0 (interface, func),
						 sometime 0x10 (par) */
    BYTE   			namelen;
    BYTE   			unk04;
    INT16   			unk05;
    char			name[1];
} TLBName;

// String
typedef struct
{
    INT16   			stringlen;
    char			string[1];
} TLBString;

/* the custom data table directory has enties like this */
typedef struct
{
    INT   			GuidOffset;
    INT   			DataOffset;
    INT   			next;     /* next offset in the table, -1 if its the last */
} TLBCDGuid;

// Typedef
typedef struct
{
    INT16		rec0;
    INT16		rec1;
    INT16		rec2;
    INT16		rec3;
} TLBTypedesc;

// Array
typedef struct
{
    INT16		rec0;
    INT16		rec1;
    INT16		rec2;
    INT16		rec3;
} TLBArray;

// ImpFile
typedef struct
{
    INT			offGuid;
    INT			unk4;
    INT			unk8;
    INT16		size;
    char		name[1];
} TLBImpFile;

/*---------------------------END--------------------------------------------*/

// Access macros

// ======================================================================
// Prototypes.
// ======================================================================

class TypeLibExtract
{
public:
    TypeLibExtract(char * szName);
    virtual ~TypeLibExtract();

    ULONG	Type() const;
    BOOL	Valid() const
	{ return m_fValid; }

    HRESULT MakeITypeLib(ITypeLibImpl * * ppObject);

private:

    ITypeLibImpl *	m_pITypeLib;		// Pointer to typelib resource...
    BOOL		m_fFree;		// Set if we need to free m_pITypeLib
    BOOL		m_fValid;
    HANDLE		m_hHeap;		// Heap Handle
    void *		m_pTypeLib;
    TLB2Header *        m_pHeader;
    ULONG *             m_pHelpStringOff;
    ULONG *             m_pTypeInfoDir;
    TLBSegDir *         m_pSegDir;

    TLBTypeInfoBase *	m_pTypeInfo;		// Ptrs into the typelib resource
    TLBImpInfo *	m_pImpInfo;
    TLBImpFile *	m_pImpFile;
    TLBRefRecord *	m_pRef;
    GUID *         	m_pGUID;
    TLBName *		m_pName;
    TLBString *		m_pString;
    TLBTypedesc *	m_pTypedesc;
    TLBArray *		m_pArray;
    void *		m_pCustData;
    TLBCDGuid *		m_pCDGuid;
    void *		m_pRes7;
    void *		m_pResE;
    void *		m_pResF;

    BOOL	Read(HANDLE hFile, void * buffer, DWORD count, DWORD * pBytesRead, long where);
    HRESULT	Load(char * szName);
    HRESULT	EstablishPointers();

    HRESULT	Parse();

    // Helpers...
    void	ParseGuid(int offset, GUID * pGuid);
    void	ParseName(int offset, char * * ppName);
    void	ParseString(int offset, char * * ppString);
    void	ParseValue(int offset, VARIANT * pVar);
    void	ParseArray(int offset, TLBArray * pArray);
    void	ParseReference(int offset, TLBRefType * pNew);
    void	ParseCustomData(int offset, oList<TLBCustData *> * pCustomData);
    void	ParseTypeInfo(ITypeInfoImpl * pTypeInfo, TLBTypeInfoBase * pBase);
    void	ParseMembers(ITypeInfoImpl * pTypeInfo, TLBTypeInfoBase * pBase);
    void	ParseImplemented(ITypeInfoImpl * pTypeInfo, TLBTypeInfoBase * pBase);
    void	GetTypedesc(int type, TYPEDESC * pTypedesc);

};

// ----------------------------------------------------------------------
// ITypeLibImpl
// ----------------------------------------------------------------------
ITypeLibImpl *	ITypeLibImpl_Constructor();
void	ITypeLibImpl_Destructor(ITypeLibImpl * This);

HRESULT WIN32API ITypeLibImpl_QueryInterface(ITypeLib2 * iface,
				REFIID riid, VOID **ppvObject);
ULONG   WIN32API ITypeLibImpl_AddRef(ITypeLib2 * iface);
ULONG   WIN32API ITypeLibImpl_Release(ITypeLib2 * iface);
UINT    WIN32API ITypeLibImpl_GetTypeInfoCount(ITypeLib2 * iface);
HRESULT WIN32API ITypeLibImpl_GetTypeInfo(ITypeLib2 * iface,
				UINT index, ITypeInfo **ppTInfo);
HRESULT WIN32API ITypeLibImpl_GetTypeInfoType(ITypeLib2 * iface,
				UINT index, TYPEKIND *pTKind);
HRESULT WIN32API ITypeLibImpl_GetTypeInfoOfGuid(ITypeLib2 * iface,
				REFGUID guid, ITypeInfo **ppTinfo);
HRESULT WIN32API ITypeLibImpl_GetLibAttr(ITypeLib2 * iface,
				LPTLIBATTR *ppTLibAttr);
HRESULT WIN32API ITypeLibImpl_GetTypeComp(ITypeLib2 * iface,
				ITypeComp **ppTComp);
HRESULT WIN32API ITypeLibImpl_GetDocumentation(ITypeLib2 * iface,
				INT index, BSTR *pBstrName, BSTR *pBstrDocString,
				DWORD *pdwHelpContext, BSTR *pBstrHelpFile);
HRESULT WIN32API ITypeLibImpl_IsName(ITypeLib2 * iface,
				LPOLESTR szNameBuf, ULONG lHashVal, BOOL *pfName);
HRESULT WIN32API ITypeLibImpl_FindName(ITypeLib2 * iface,
				LPOLESTR szNameBuf, ULONG lHashVal, ITypeInfo **ppTInfo,
				MEMBERID *rgMemId, UINT16 *pcFound);
VOID WIN32API ITypeLibImpl_ReleaseTLibAttr(ITypeLib2 * iface,
				TLIBATTR *pTLibAttr);
HRESULT WIN32API ITypeLib2Impl_GetCustData(ITypeLib2 * iface,
				REFGUID guid, VARIANT *pVarVal);
HRESULT WIN32API ITypeLib2Impl_GetLibStatistics(ITypeLib2 * iface,
				ULONG *pcUniqueNames, ULONG *pcchUniqueNames);
HRESULT WIN32API ITypeLib2Impl_GetDocumentation2(ITypeLib2 * iface,
				INT index, LCID lcid, BSTR *pbstrHelpString,
				DWORD *pdwHelpStringContext, BSTR *pbstrHelpStringDll);
HRESULT WIN32API ITypeLib2Impl_GetAllCustData(ITypeLib2 * iface,
				CUSTDATA *pCustData);

// ----------------------------------------------------------------------
// ITypeInfoImpl
// ----------------------------------------------------------------------
ITypeInfoImpl *	ITypeInfoImpl_Constructor();
void	ITypeInfoImpl_Destructor(ITypeInfoImpl * This);

HRESULT WIN32API ITypeInfoImpl_QueryInterface(ITypeInfo2 * iface,
				REFIID riid, VOID **ppvObject);
ULONG   WIN32API ITypeInfoImpl_AddRef(ITypeInfo2 * iface);
ULONG   WIN32API ITypeInfoImpl_Release(ITypeInfo2 * iface);
HRESULT WIN32API ITypeInfoImpl_GetTypeAttr(ITypeInfo2 * iface,
				LPTYPEATTR  *ppTypeAttr);
HRESULT WIN32API ITypeInfoImpl_GetTypeComp(ITypeInfo2 * iface,
				ITypeComp  * *ppTComp);
HRESULT WIN32API ITypeInfoImpl_GetFuncDesc(ITypeInfo2 * iface,
				UINT index, LPFUNCDESC  *ppFuncDesc);
HRESULT WIN32API ITypeInfoImpl_GetVarDesc(ITypeInfo2 * iface,
				UINT index, LPVARDESC  *ppVarDesc);
HRESULT WIN32API ITypeInfoImpl_GetNames(ITypeInfo2 * iface,
				MEMBERID memid, BSTR  *rgBstrNames,
				UINT cMaxNames, UINT  *pcNames);
HRESULT WIN32API ITypeInfoImpl_GetRefTypeOfImplType(ITypeInfo2 * iface,
				UINT index, HREFTYPE  *pRefType);
HRESULT WIN32API ITypeInfoImpl_GetImplTypeFlags(ITypeInfo2 * iface,
				UINT index, INT  *pImplTypeFlags);
HRESULT WIN32API ITypeInfoImpl_GetIDsOfNames(ITypeInfo2 * iface,
				LPOLESTR  *rgszNames, UINT cNames, MEMBERID  *pMemId);
HRESULT WIN32API ITypeInfoImpl_Invoke(ITypeInfo2 * iface,
				VOID  *pIUnk, MEMBERID memid, UINT16 dwFlags,
				DISPPARAMS  *pDispParams, VARIANT *pVarResult,
				EXCEPINFO  *pExcepInfo, UINT  *pArgErr);
HRESULT WIN32API ITypeInfoImpl_GetDocumentation(ITypeInfo2 * iface,
				MEMBERID memid, BSTR  *pBstrName, BSTR  *pBstrDocString,
				DWORD  *pdwHelpContext, BSTR  *pBstrHelpFile);
HRESULT WIN32API ITypeInfoImpl_GetDllEntry(ITypeInfo2 * iface,
				MEMBERID memid, INVOKEKIND invKind, BSTR  *pBstrDllName,
				BSTR  *pBstrName, WORD  *pwOrdinal);
HRESULT WIN32API ITypeInfoImpl_GetRefTypeInfo(ITypeInfo2 * iface,
				HREFTYPE hRefType, ITypeInfo  * *ppTInfo);
HRESULT WIN32API ITypeInfoImpl_AddressOfMember(ITypeInfo2 * iface,
				MEMBERID memid, INVOKEKIND invKind, PVOID *ppv);
HRESULT WIN32API ITypeInfoImpl_CreateInstance(ITypeInfo2 * iface,
				IUnknown *pUnk, REFIID riid, VOID  * *ppvObj);
HRESULT WIN32API ITypeInfoImpl_GetMops(ITypeInfo2 * iface,
				MEMBERID memid, BSTR  *pBstrMops);
HRESULT WIN32API ITypeInfoImpl_GetContainingTypeLib(ITypeInfo2 * iface,
				ITypeLib  * *ppTLib, UINT  *pIndex);
HRESULT WIN32API ITypeInfoImpl_ReleaseTypeAttr(ITypeInfo2 * iface,
				TYPEATTR *pTypeAttr);
HRESULT WIN32API ITypeInfoImpl_ReleaseFuncDesc(ITypeInfo2 * iface,
				FUNCDESC *pFuncDesc);
HRESULT WIN32API ITypeInfoImpl_ReleaseVarDesc(ITypeInfo2 * iface,
				VARDESC *pVarDesc);
HRESULT WIN32API ITypeInfo2Impl_GetTypeKind(ITypeInfo2 * iface,
				TYPEKIND *pTypeKind);
HRESULT WIN32API ITypeInfo2Impl_GetTypeFlags(ITypeInfo2 * iface,
				UINT *pTypeFlags);
HRESULT WIN32API ITypeInfo2Impl_GetFuncIndexOfMemId(ITypeInfo2 * iface,
				MEMBERID memid, INVOKEKIND invKind, UINT *pFuncIndex);
HRESULT WIN32API ITypeInfo2Impl_GetVarIndexOfMemId(ITypeInfo2 * iface,
				MEMBERID memid, UINT *pVarIndex);
HRESULT WIN32API ITypeInfo2Impl_GetCustData(ITypeInfo2 * iface,
				REFGUID guid, VARIANT *pVarVal);
HRESULT WIN32API ITypeInfo2Impl_GetFuncCustData(ITypeInfo2 * iface,
				UINT index, REFGUID guid, VARIANT *pVarVal);
HRESULT WIN32API ITypeInfo2Impl_GetParamCustData(ITypeInfo2 * iface,
				UINT indexFunc, UINT indexParam, REFGUID guid, VARIANT *pVarVal);
HRESULT WIN32API ITypeInfo2Impl_GetVarCustData(ITypeInfo2 * iface,
				UINT index, REFGUID guid, VARIANT *pVarVal);
HRESULT WIN32API ITypeInfo2Impl_GetImplTypeCustData(ITypeInfo2 * iface,
				UINT index, REFGUID guid, VARIANT *pVarVal);
HRESULT WIN32API ITypeInfo2Impl_GetDocumentation2(ITypeInfo2 * iface,
				MEMBERID memid, LCID lcid, BSTR *pbstrHelpString,
				DWORD *pdwHelpStringContext, BSTR *pbstrHelpStringDll);
HRESULT WIN32API ITypeInfo2Impl_GetAllCustData(ITypeInfo2 * iface,
				CUSTDATA *pCustData);
HRESULT WIN32API ITypeInfo2Impl_GetAllFuncCustData(ITypeInfo2 * iface,
				UINT index, CUSTDATA *pCustData);
HRESULT WIN32API ITypeInfo2Impl_GetAllParamCustData(ITypeInfo2 * iface,
				UINT indexFunc, UINT indexParam, CUSTDATA *pCustData);
HRESULT WIN32API ITypeInfo2Impl_GetAllVarCustData(ITypeInfo2 * iface,
				UINT index, CUSTDATA *pCustData);
HRESULT WIN32API ITypeInfo2Impl_GetAllImplTypeCustData(ITypeInfo2 * iface,
				UINT index, CUSTDATA *pCustData);

HRESULT WIN32API ITypeCompImpl_QueryInterface(LPTYPECOMP iface,
				REFIID riid, VOID **ppvObject);
ULONG   WIN32API ITypeCompImpl_AddRef(LPTYPECOMP iface);
ULONG   WIN32API ITypeCompImpl_Release(LPTYPECOMP iface);
HRESULT	WIN32API ITypeCompImpl_Bind(LPTYPECOMP iface,
				LPOLESTR szName, ULONG lHashVal, WORD wFlags,
				ITypeInfo** ppTInfo, DESCKIND* pDescKind, BINDPTR* pBindPtr);
HRESULT	WIN32API ITypeCompImpl_BindType(LPTYPECOMP iface,
				LPOLESTR szName, ULONG lHashVal, ITypeInfo** ppTInfo,
				ITypeComp** ppTComp);

// Helper functions
extern BSTR DupAtoBstr(char * pAscii);

#endif /* ITYPELIB_INCLUDED */
