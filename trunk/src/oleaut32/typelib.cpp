/* $Id: typelib.cpp,v 1.6 1999-12-08 19:59:58 davidr Exp $ */

typedef enum tagREGKIND
{
    REGKIND_DEFAULT,
    REGKIND_REGISTER,
    REGKIND_NONE
} REGKIND;

#include "oleaut32.h"
#include "olectl.h"
#include "oList.h"	// linked list template
#include "itypelib.h"

// ======================================================================
// Local Data
// ======================================================================
static TYPEDESC stndTypeDesc[VT_LPWSTR+1] =
{   /* VT_LPWSTR is largest type that */
    /* may appear in type description*/
    {{0}, 0},{{0}, 1},{{0}, 2},{{0}, 3},{{0}, 4},
    {{0}, 5},{{0}, 6},{{0}, 7},{{0}, 8},{{0}, 9},
    {{0},10},{{0},11},{{0},12},{{0},13},{{0},14},
    {{0},15},{{0},16},{{0},17},{{0},18},{{0},19},
    {{0},20},{{0},21},{{0},22},{{0},23},{{0},24},
    {{0},25},{{0},26},{{0},27},{{0},28},{{0},29},
    {{0},30},{{0},31}
};

// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
// QueryPathOfRegTypeLib()				[OLEAUT32.164]
//
// Retrieves the path of a registered type library.
// ----------------------------------------------------------------------
HRESULT WINAPI QueryPathOfRegTypeLib(
    REFGUID		guid,	/* [in] referenced guid */
    WORD		wMaj,	/* [in] major version */
    WORD		wMin,	/* [in] minor version */
    LCID		lcid,	/* [in] locale id */
    LPBSTR		path)	/* [out] path of typelib */
{
    char	xguid[80];
    char	typelibkey[100];
    char	pathname[260];
    DWORD	plen;

    dprintf(("OLEAUT32: QueryPathOfRegTypeLib()"));

    if (HIWORD(guid))
    {
	WINE_StringFromCLSID(guid, xguid);
	sprintf(typelibkey,
	    "SOFTWARE\\Classes\\Typelib\\%s\\%d.%d\\%lx\\win32",
		xguid, wMaj, wMin, lcid);
    }
    else
    {
	sprintf(xguid, "<guid 0x%08lx>", (DWORD)guid);
	dprintf(("OLEAUT32: QueryPathOfRegTypeLib(%s,%d,%d,0x%04lx,%p) - stub!",
	    xguid, wMaj, wMin, (DWORD)lcid, path));
	return E_FAIL;
    }
    plen = sizeof(pathname);
    if (RegQueryValueA(HKEY_LOCAL_MACHINE, typelibkey, pathname, (LPLONG)&plen))
    {
	/* try again without lang specific id */
	if (SUBLANGID(lcid))
	    return QueryPathOfRegTypeLib(guid, wMaj, wMin, PRIMARYLANGID(lcid), path);

	dprintf(("OLEAUT32: QueryPathOfRegTypeLib() - key \"%s\" not found", typelibkey));
	return E_FAIL;
    }
    *path = HEAP_strdupAtoW(GetProcessHeap(), 0, pathname);
    return S_OK;
}

// ----------------------------------------------------------------------
// LoadTypeLibEx()					[OLEAUT32.183]
//
// Loads a type library & optionally registers it in the system registry.
// * If file is a standalone type library - load it directly.
// * If the file is a DLL or EXE then load the module, and extract the TYPELIB
//   resource from it. The default is the first TYPELIB resource - subsequent
//   TYPELIBs may be accessed by appending an integer index to szFile
// * If neither of the above, the library is parsed into a filebased Moniker. 
//   No idea of what/when/where/how to do this yet ;-)
//
// If the library (guid, etc) is already loaded return the loaded ITypeLib
// and incr. its ref count.
// ----------------------------------------------------------------------
HRESULT WINAPI LoadTypeLibEx(
    LPCOLESTR		szFile,
    REGKIND		regkind,
    ITypeLib * *	ppTlib)
{
    HANDLE		hHeap = GetProcessHeap();
    char *		szFileA;
    HRESULT		rc;

    szFileA = HEAP_strdupWtoA(hHeap, 0, szFile);
    dprintf(("OLEAUT32: LoadTypeLibEx(%s)", szFileA));

    // Sanity check...
    if (ppTlib == 0)
    {
	HeapFree(hHeap, 0, szFileA);
    	return E_POINTER;
    }

    *ppTlib = 0;

    TypeLibExtract	extractor(szFileA);

    if (!extractor.Valid())
    {
	HeapFree(hHeap, 0, szFileA);
	dprintf(("  Invalid typelib file"));
	return E_FAIL;
    }

    rc = extractor.MakeITypeLib((ITypeLibImpl * *)ppTlib);

    HeapFree(hHeap, 0, szFileA); 

    return rc;
}

// ----------------------------------------------------------------------
// LoadTypeLib()					[OLEAUT32.161]
//
// Loads a type library & registers it in the system registry.
// ----------------------------------------------------------------------
HRESULT WINAPI LoadTypeLib(
    LPCOLESTR		szFile,		/* [in] Name of file to load from */
    ITypeLib * *	ppTLib)		/* [out] Pointer to pointer to loaded type library */
{
    HRESULT		res;

    dprintf(("OLEAUT32: LoadTypeLib()"));

    return LoadTypeLibEx(szFile, REGKIND_DEFAULT, ppTLib);
}

// ----------------------------------------------------------------------
// LoadRegTypeLib()					[OLEAUT32.162]
//
// Use system registry information to locate and load a type library.
// ----------------------------------------------------------------------
HRESULT WINAPI LoadRegTypeLib(
    REFGUID		rguid,		/* [in] referenced guid */
    WORD		wVerMajor,	/* [in] major version */
    WORD		wVerMinor,	/* [in] minor version */
    LCID		lcid,		/* [in] locale id */
    ITypeLib * *	ppTLib		/* [out] path of typelib */
)
{
    BSTR		bstr = NULL;
    HRESULT		res;

    dprintf(("OLEAUT32: LoadRegTypeLib()"));

    res = QueryPathOfRegTypeLib( rguid, wVerMajor, wVerMinor, lcid, &bstr);

    if (SUCCEEDED(res))
    {
        res = LoadTypeLibEx(bstr, REGKIND_NONE, ppTLib);
        SysFreeString(bstr);
    }

    return res;
}

// ----------------------------------------------------------------------
// RegisterTypeLib()					[OLEAUT32.163]
//
// Adds information about a type library to the System Registry
// ----------------------------------------------------------------------
HRESULT WINAPI RegisterTypeLib(
    ITypeLib *		ptlib,      	/* [in] Pointer to the library*/
    OLECHAR *		szFullPath, 	/* [in] full Path of the library*/
    OLECHAR *		szHelpDir)  	/* [in] dir to the helpfile for the library,
							 may be NULL*/
{
    dprintf(("OLEAUT32: RegisterTypeLib() - stub\n"));

    return S_OK;	/* FIXME: pretend everything is OK */
}

// ----------------------------------------------------------------------
// UnRegisterTypeLib()					[OLEAUT32.186]
// 
// Removes information about a type library from the System Registry
// ----------------------------------------------------------------------
HRESULT WINAPI UnRegisterTypeLib(
    REFGUID		libid,		/* [in] Guid of the library */
    WORD		wVerMajor,	/* [in] major version */
    WORD		wVerMinor,	/* [in] minor version */
    LCID		lcid,		/* [in] locale id */
    SYSKIND		syskind)
{
    dprintf(("OLEAUT32: UnRegisterTypeLib() - stub"));

    return S_OK;	/* FIXME: pretend everything is OK */
}

// ======================================================================
// Class functions.
// ======================================================================

// ----------------------------------------------------------------------
// TypeLibExtract::TypeLibExtract
// ----------------------------------------------------------------------
TypeLibExtract::TypeLibExtract(char * szName) : m_fValid(0), m_hHeap(GetProcessHeap())
{
    if ((Load(szName) == S_OK)
	&& (EstablishPointers() == S_OK))
    {
	m_fValid = TRUE;
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::~TypeLibExtract
// ----------------------------------------------------------------------
TypeLibExtract::~TypeLibExtract()
{
    m_fValid = 0;

    if (m_pTypeLib)
	HeapFree(m_hHeap, 0, m_pTypeLib);

    m_hHeap = 0;
}

// ----------------------------------------------------------------------
// TypeLibExtract::EstablishPointers()
// ----------------------------------------------------------------------
HRESULT TypeLibExtract::EstablishPointers()
{
    // Locate segment directory...
    m_pHeader = (TLB2Header *)m_pTypeLib;
    if (m_pHeader->varflags & HELPDLLFLAG)
    {
	m_pHelpStringOff = (ULONG *)(m_pHeader + 1);
	m_pTypeInfoDir = m_pHelpStringOff + 1;
    }
    else
    {
	m_pHelpStringOff = 0;
	m_pTypeInfoDir = (ULONG *)(m_pHeader + 1);
    }
    m_pSegDir = (TLBSegDir *)(m_pTypeInfoDir + m_pHeader->nrtypeinfos);

    // Segment directory sanity check...
    if (m_pSegDir->pTypeInfoTab.res0c != 0x0F || m_pSegDir->pImpInfo.res0c != 0x0F)
    {
	dprintf(("  Segment directory sanity check failed!"));
	return E_FAIL;
    }

//    m_pLib
    if (m_pSegDir->pImpInfo.offset > 0)
	m_pImpInfo = (TLBImpInfo *)((char *)m_pTypeLib + m_pSegDir->pImpInfo.offset);
    else
	m_pImpInfo = 0;

    if (m_pSegDir->pRefTab.offset > 0)
	m_pRef = (TLBRefRecord *)((char *)m_pTypeLib + m_pSegDir->pRefTab.offset);
    else
	m_pRef = 0;

    if (m_pSegDir->pGuidTab.offset > 0)
	m_pGUID = (GUID *)((char *)m_pTypeLib + m_pSegDir->pGuidTab.offset);
    else
	m_pGUID = 0;

    if (m_pSegDir->pNameTab.offset > 0)
	m_pName = (TLBName *)((char *)m_pTypeLib + m_pSegDir->pNameTab.offset);
    else
	m_pName = 0;

    if (m_pSegDir->pStringTab.offset > 0)
	m_pString = (TLBString *)((char *)m_pTypeLib + m_pSegDir->pStringTab.offset);
    else
	m_pString = 0;

    if (m_pSegDir->pTypedescTab.offset > 0)
	m_pTypedesc = (TLBTypedesc *)((char *)m_pTypeLib + m_pSegDir->pTypedescTab.offset);
    else
	m_pTypedesc = 0;

    if (m_pSegDir->pTypedescTab.offset > 0)
	m_pArray = (TLBArray *)((char *)m_pTypeLib + m_pSegDir->pTypedescTab.offset);
    else
	m_pArray = 0;

    if (m_pSegDir->pCustData.offset > 0)
	m_pCustData = (void *)((char *)m_pTypeLib + m_pSegDir->pCustData.offset);
    else
	m_pCustData = 0;

    if (m_pSegDir->pCDGuids.offset > 0)
        m_pCDGuid = (TLBCDGuid *)((char *)m_pTypeLib + m_pSegDir->pCDGuids.offset);
    else
	m_pCDGuid = 0;

    if (m_pSegDir->pImpFiles.offset > 0)
        m_pImpFile = (TLBImpFile *)((char *)m_pTypeLib + m_pSegDir->pImpFiles.offset);
    else
	m_pImpFile = 0;

    if (m_pSegDir->pTypeInfoTab.offset > 0)
        m_pTypeInfo = (TLBTypeInfoBase *)((char *)m_pTypeLib + m_pSegDir->pTypeInfoTab.offset);
    else
	m_pTypeInfo = 0;

    return S_OK;
}

// ----------------------------------------------------------------------
// TypeLibExtract::Read
//
// If 'where' is != DO_NOT_SEEK then seek to 'where'.
// Read 'count' bytes from 'hfile' into 'buffer'
// ----------------------------------------------------------------------
BOOL TypeLibExtract::Read(HANDLE hFile, void * buffer, DWORD count, DWORD * pBytesRead, long where)
{
    if (where != DO_NOT_SEEK)
    {
	if (0xffffffff == SetFilePointer(hFile, where, 0, FILE_BEGIN))
	    return FALSE;
    }
    return ReadFile(hFile, buffer, count, pBytesRead, NULL);
}

// ----------------------------------------------------------------------
// TypeLibExtract::GetTypedesc
// ----------------------------------------------------------------------
void TypeLibExtract::GetTypedesc(int type, TYPEDESC * pTd)
{
    if (type < 0)
        pTd->vt = type & VT_TYPEMASK;
    else
        *pTd = *m_pITypeLib->pTypedesc[type / (sizeof(TLBTypedesc))];
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseGuid
// ----------------------------------------------------------------------
void TypeLibExtract::ParseGuid(int offset, GUID * pGuid)
{
    if (offset < 0)
    {
        memset(pGuid, 0, sizeof(GUID));
    }
    else
    {
	memcpy(pGuid, (char *)m_pGUID + offset, sizeof(GUID));
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseArray
// ----------------------------------------------------------------------
void TypeLibExtract::ParseArray(int offset, TLBArray * pArray)
{
    if (offset < 0)
    {
        memset(pArray, 0, sizeof(TLBArray));
    }
    else
    {
	memcpy(pArray, (char *)m_pGUID + offset, sizeof(TLBArray));
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseName
// ----------------------------------------------------------------------
void TypeLibExtract::ParseName(int offset, char * * ppName)
{
    char *	p;

    if (offset < 0)
    {
	*ppName = 0;
    }
    else
    {
	TLBName *	pName = (TLBName *)((char *)m_pName + offset);
	int		nameLen = pName->namelen & 0xff;

	p = (char *)HeapAlloc(m_hHeap, 0, nameLen + 1);
	memcpy(p, pName->name, nameLen);
	p[nameLen] = 0;

	*ppName = p;
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseString
// ----------------------------------------------------------------------
void TypeLibExtract::ParseString(int offset, char * * ppString)
{
    char *	p;

    if (offset < 0)
    {
	*ppString = 0;
    }
    else
    {
	TLBString *	pString = (TLBString *)((char *)m_pString + offset);
	int		stringLen = pString->stringlen;

	p = (char *)HeapAlloc(m_hHeap, 0, stringLen + 1);

	memcpy(p, pString->string, stringLen);
	p[stringLen] = 0;

	*ppString = p;
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseValue
// ----------------------------------------------------------------------
void TypeLibExtract::ParseValue(int offset, VARIANT * pVar)
{
    int		size;

    if (offset < 0) /* data is packed in here */
    {
        pVar->vt = (offset & 0x7c000000) >> 26;
        V_UNION(pVar, iVal) = offset & 0xffff;
        return;
    }

    VARTYPE *	pVarType = (VARTYPE *)((char *)m_pCustData + offset);

    pVar->vt = *pVarType;

    switch(pVar->vt)
    {
        case VT_EMPTY:  /* FIXME: is this right? */
        case VT_NULL:   /* FIXME: is this right? */
        case VT_I2  :   /* this should not happen */
        case VT_I4  :
        case VT_R4  :
        case VT_ERROR   :
        case VT_BOOL    :
        case VT_I1  :
        case VT_UI1 :
        case VT_UI2 :
        case VT_UI4 :
        case VT_INT :
        case VT_UINT    :
        case VT_VOID    : /* FIXME: is this right? */
        case VT_HRESULT :
            size=4;
            break;

        case VT_R8  :
        case VT_CY  :
        case VT_DATE    :
        case VT_I8  :
        case VT_UI8 :
        case VT_DECIMAL :  /* FIXME: is this right? */
        case VT_FILETIME :
            size=8;
            break;

            /* pointer types with known behaviour */
        case VT_BSTR    :
        {
            INT *	pStringLen = (INT *)(pVarType + 1);
            char *	pString = (char *)(pStringLen + 1);

	    // Allocate BSTR container and then convert directly into it...
//???A Fix
	    V_UNION(pVar, bstrVal) = SysAllocStringLen(NULL, *pStringLen);
	    AsciiToUnicodeN(pString, V_UNION(pVar, bstrVal), *pStringLen);
	    return;
	}

    /* FIXME: this will not work AT ALL when the variant contains a pointer */
        case VT_DISPATCH :
        case VT_VARIANT :
        case VT_UNKNOWN :
        case VT_PTR :
        case VT_SAFEARRAY :
        case VT_CARRAY  :
        case VT_USERDEFINED :
        case VT_LPSTR   :
        case VT_LPWSTR  :
        case VT_BLOB    :
        case VT_STREAM  :
        case VT_STORAGE :
        case VT_STREAMED_OBJECT :
        case VT_STORED_OBJECT   :
        case VT_BLOB_OBJECT :
        case VT_CF  :
        case VT_CLSID   :
        default:
            size = 0;
            dprintf(("OLEAUT32: VARTYPE %d is not supported, setting pointer to NULL\n", pVar->vt));
    }

    if (size > 0) /* (big|small) endian correct? */
	memcpy(&(V_UNION(pVar, iVal)), (pVarType + 1), size);

}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseCustomData
// ----------------------------------------------------------------------
void TypeLibExtract::ParseCustomData(int offset, oList<TLBCustData *> * pCustomData)
{
    TLBCDGuid *		pCDGuid;
    TLBCustData *	pNew;

    while (offset >=0)
    {
	pCDGuid = (TLBCDGuid *)((char *)m_pCDGuid + offset);
	pNew = new TLBCustData;

	ParseGuid(pCDGuid->GuidOffset, &(pNew->guid));
	ParseValue(pCDGuid->DataOffset, &(pNew->data));

	pCustomData->AddAtEnd(pNew);
	offset  =  pCDGuid->next;
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseTypeInfo
// ----------------------------------------------------------------------
void TypeLibExtract::ParseTypeInfo(ITypeInfoImpl * pTypeInfo, TLBTypeInfoBase * pBase)
{
    /* fill in the typeattr fields */
    ParseGuid(pBase->posguid, &pTypeInfo->TypeAttr.guid);
    pTypeInfo->TypeAttr.lcid = m_pITypeLib->LibAttr.lcid;   /* FIXME: correct? */
    pTypeInfo->TypeAttr.memidConstructor = MEMBERID_NIL ;/* FIXME */
    pTypeInfo->TypeAttr.memidDestructor = MEMBERID_NIL ; /* FIXME */
    pTypeInfo->TypeAttr.lpstrSchema = NULL;              /* reserved */
    pTypeInfo->TypeAttr.cbSizeInstance = pBase->size;
    pTypeInfo->TypeAttr.typekind = (tagTYPEKIND)(pBase->typekind & 0xF);
    pTypeInfo->TypeAttr.cFuncs = LOWORD(pBase->cElement);
    pTypeInfo->TypeAttr.cVars = HIWORD(pBase->cElement);
    pTypeInfo->TypeAttr.cbAlignment = (pBase->typekind >> 11 )& 0x1F; /* more flags here ? */
    pTypeInfo->TypeAttr.wTypeFlags = pBase->flags;
    pTypeInfo->TypeAttr.wMajorVerNum = LOWORD(pBase->version);
    pTypeInfo->TypeAttr.wMinorVerNum = HIWORD(pBase->version);
    pTypeInfo->TypeAttr.cImplTypes = pBase->cImplTypes;
    pTypeInfo->TypeAttr.cbSizeVft = pBase->cbSizeVft; // FIXME: this is only the non inherited part
    if (pTypeInfo->TypeAttr.typekind  == TKIND_ALIAS)
        GetTypedesc(pBase->datatype1, &pTypeInfo->TypeAttr.tdescAlias) ;

    /*  FIXME: */
    /*    IDLDESC  idldescType; *//* never saw this one ! =  zero  */

    /* name, eventually add to a hash table */
    ParseName(pBase->NameOffset, &pTypeInfo->szName);

    dprintf(("  -> typeinfo \"%s\"\n", pTypeInfo->szName));

    /* help info */
    ParseString(pBase->docstringoffs, &pTypeInfo->szDocString);
    pTypeInfo->lHelpStringContext = pBase->helpstringcontext;
    pTypeInfo->lHelpContext = pBase->helpcontext;
    /* note: InfoType's Help file and HelpStringDll come from the containing
     * library. Further HelpString and Docstring appear to be the same thing :(
     */

    // Functions/
    ParseMembers(pTypeInfo, pBase);

    // ImplTypes {DR: not sure this is OK yet}
    if (pTypeInfo->TypeAttr.typekind == TKIND_COCLASS)
	ParseImplemented(pTypeInfo, pBase);

    // RefTypes {DR: not sure this is OK yet}
    else if (pTypeInfo->TypeAttr.typekind != TKIND_DISPATCH)
    {
	TLBRefType *	pNew = new TLBRefType;
        ParseReference(pBase->datatype1, pNew);
        pTypeInfo->pImplements.AddAtEnd(pNew);

    }

    // Load Custom data
    ParseCustomData(pBase->oCustData, &pTypeInfo->pCustData);
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseReference
// ----------------------------------------------------------------------
void TypeLibExtract::ParseReference(int offset, TLBRefType * pNew)
{
    int j;

    if(!HREFTYPE_INTHISFILE( offset))
    {
        /* external typelib */
        TLBImpInfo *	pImpInfo = (TLBImpInfo *)((char *)m_pImpInfo + (offset & 0xfffffffc));
        oListIter<TLBImpLib *>	itrImpLib(m_pITypeLib->pImpLibs);

        // We have already loaded the external typelibs so
        // loop thru them to find the one we want.
        for (itrImpLib.MoveStart(); itrImpLib.IsValid(); itrImpLib.MoveNext())
        {

	    if (itrImpLib.Element()->offset == pImpInfo->oImpFile)
	    {
		pNew->reference = offset;
		pNew->pImpTLInfo = itrImpLib.Element();
		ParseGuid(pImpInfo->oGuid, &pNew->guid);
		return;
	    }
        }
	dprintf((" WARNING: Cannot find a reference\n"));
	pNew->reference = -1;
	pNew->pImpTLInfo = (TLBImpLib *)-1;
    }
    else
    {
        /* in this typelib */
        pNew->reference = offset;
        pNew->pImpTLInfo = (TLBImpLib *)-2;
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseImplemented
//
// Process Implemented Interfaces of a com class
// ----------------------------------------------------------------------
void TypeLibExtract::ParseImplemented(ITypeInfoImpl * pTypeInfo, TLBTypeInfoBase * pBase)
{
    TLBRefRecord *	pRefRec;
    TLBRefType *	pNew;
    int			ii;
    int			offset;

    offset = pBase->datatype1;
    for (ii = 0; ii < pTypeInfo->TypeAttr.cImplTypes; ii++)
    {
        if (offset < 0)
	    break; /* paranoia */

	pRefRec = (TLBRefRecord *)((char *)m_pRef + offset);
	pNew = new TLBRefType;

        ParseReference(pRefRec->reftype, pNew);
        pNew->flags = pRefRec->flags;

        // Custom data
        ParseCustomData(pRefRec->oCustData, &pNew->pCustData);

        pTypeInfo->pImplements.AddAtEnd(pNew);

        offset = pRefRec->onext;
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::ParseMembers
//
// Member information is stored in a data structure at offset
// indicated by the memoffset field of the typeinfo structure
// There are several distinctive parts.
// the first part starts with a field that holds the total length
// of this (first) part excluding this field. Then follow the records,
// for each member there is one record.
//
// First entry is always the length of the record (excluding this
// length word).
// Rest of the record depends on the type of the member. If there is
// a field indicating the member type (function variable intereface etc)
// I have not found it yet. At this time we depend on the information
// in the type info and the usual order how things are stored.
//
// Second follows an array sized nrMEM*sizeof(INT) with a memeber id
// for each member;
//
// Third is a equal sized array with file offsets to the name entry
// of each member.
//
// Forth and last (?) part is an array with offsets to the records in the
// first part of this file segment.
//
// ----------------------------------------------------------------------
void	TypeLibExtract::ParseMembers(ITypeInfoImpl * pTypeInfo, TLBTypeInfoBase * pBase)
{
    TLBFuncRecord *	pFuncRec;
    TLBVarRecord *	pVarRec;
    int			ii;
    int			jj;
    int			iAttrCount;
    INT *		pInfoLen;
    INT *		pNameOff;
    INT *		pMemberID;
    INT *		pMemberOff;

    // Map ptrs to sections of the typeinfo record...
    pInfoLen = (INT *)((char *)m_pTypeLib + pBase->memoffset);
    pFuncRec = (TLBFuncRecord *)(pInfoLen + 1);
    pMemberID = (INT *)((char *)pFuncRec + *pInfoLen);
    pNameOff = pMemberID + pTypeInfo->TypeAttr.cFuncs + pTypeInfo->TypeAttr.cVars;
    pMemberOff = pNameOff + pTypeInfo->TypeAttr.cFuncs + pTypeInfo->TypeAttr.cVars;

    // loop through each function...
    for (ii = 0; ii < pTypeInfo->TypeAttr.cFuncs; ii++)
    {
	TLBFuncDesc *	pNew;

        pNew = new TLBFuncDesc;
        ParseName(pNameOff[ii], &pNew->szName);

	dprintf(("    -> function \"%s\"\n", pNew->szName));

	// decode optional attributes
        iAttrCount = (pFuncRec->recsize
			- pFuncRec->nrargs * 3 * sizeof(int) - 0x18) / sizeof(int);

        if (iAttrCount > 0)
            pNew->helpcontext = pFuncRec->OptAttr[0] ;

	if (iAttrCount > 1)
	    ParseString(pFuncRec->OptAttr[1], &pNew->szHelpString);

	if (iAttrCount > 2)
	{
	    if (pFuncRec->FKCCIC & 0x2000)
		pNew->szEntry = (char *) pFuncRec->OptAttr[2] ;
	    else
		ParseString(pFuncRec->OptAttr[2], &pNew->szEntry);
	}

	if (iAttrCount > 5 )
	    pNew->lHelpStringContext = pFuncRec->OptAttr[5] ;

	if (iAttrCount > 6 && pFuncRec->FKCCIC & 0x80)
	    ParseCustomData(pFuncRec->OptAttr[6], &pNew->pCustData);

	// fill the FuncDesc Structure
	pNew->funcdesc.memid = pMemberID[ii];
        pNew->funcdesc.funckind = (tagFUNCKIND)((pFuncRec->FKCCIC) & 0x7);
        pNew->funcdesc.invkind = (tagINVOKEKIND)(((pFuncRec->FKCCIC) >>3) & 0xF);
        pNew->funcdesc.callconv = (tagCALLCONV)((pFuncRec->FKCCIC) >>8 & 0xF);
        pNew->funcdesc.cParams = pFuncRec->nrargs ;
        pNew->funcdesc.cParamsOpt = pFuncRec->nroargs ;
        pNew->funcdesc.oVft = pFuncRec->VtableOffset ;
        pNew->funcdesc.wFuncFlags = LOWORD(pFuncRec->Flags)  ;
        GetTypedesc(pFuncRec->DataType, &pNew->funcdesc.elemdescFunc.tdesc) ;

        // do the parameters/arguments 
        if (pFuncRec->nrargs)
        {
            TLBParameterInfo *	pParam;

            pNew->funcdesc.lprgelemdescParam = new ELEMDESC[pFuncRec->nrargs];
            pNew->pParamDesc = new TLBParDesc[pFuncRec->nrargs];

	    pParam = (TLBParameterInfo *)((char *)pFuncRec + pFuncRec->recsize -
                pFuncRec->nrargs * sizeof(TLBParameterInfo));

            for(jj = 0 ; jj < pFuncRec->nrargs; jj++)
            {
                GetTypedesc(pParam->DataType, &pNew->funcdesc.lprgelemdescParam[jj].tdesc);
                    
                V_UNION(&(pNew->funcdesc.lprgelemdescParam[jj]),
		    paramdesc.wParamFlags)  =  pParam->Flags;
		// Name
                ParseName(pParam->oName, &pNew->pParamDesc[jj].szName);
		dprintf(("      -> param \"%s\"\n", pNew->pParamDesc[jj].szName));

                // default value
                if ((PARAMFLAG_FHASDEFAULT & V_UNION(&(pNew->funcdesc.
                    lprgelemdescParam[jj]),paramdesc.wParamFlags)) &&
                    ((pFuncRec->FKCCIC) & 0x1000))
		{
                    INT *pInt = (INT *)((char *)pFuncRec + pFuncRec->recsize -
                        (pFuncRec->nrargs * 4 + 1) * sizeof(INT) );
                    PARAMDESC * pParamDesc =  &V_UNION(&(pNew->funcdesc.
                        lprgelemdescParam[jj]),paramdesc);
                    pParamDesc->pparamdescex  = new PARAMDESCEX;
                    pParamDesc->pparamdescex->cBytes = sizeof(PARAMDESCEX);
                    ParseValue(pInt[jj], &(pParamDesc->pparamdescex->varDefaultValue));
                }

                // custom info
                if (iAttrCount > 7 + jj && pFuncRec->FKCCIC & 0x80)
                    ParseCustomData(pFuncRec->OptAttr[7 + jj], &pNew->pParamDesc[jj].pCustData);

                pParam++;	// Next record.
            }
	}

	/* scode is not used: archaic win16 stuff FIXME: right? */
        pNew->funcdesc.cScodes = 0 ;
        pNew->funcdesc.lprgscode = NULL ;

	// Store
	pTypeInfo->pFunctions.AddAtEnd(pNew);

        // Next record...
        pFuncRec = (TLBFuncRecord *)((char *)pFuncRec + pFuncRec->recsize);
    }

    // Wine calc's for this ptr were b**ll***s
    pVarRec = (TLBVarRecord *)pFuncRec;

    for (ii = 0; ii < pTypeInfo->TypeAttr.cVars; ii++)
    {
	TLBVarDesc *	pNew;

	pNew = new TLBVarDesc;

	// name, eventually add to a hash table 
	ParseName(pNameOff[ii], &pNew->szName);
	dprintf(("    -> variable \"%s\"\n", pNew->szName));

	// Optional data
        if (pVarRec->recsize > (6 * sizeof(INT)) )
            pNew->lHelpContext = pVarRec->HelpContext;

        if (pVarRec->recsize > (7 * sizeof(INT)) )
            ParseString(pVarRec->oHelpString, &pNew->szHelpString);

        if (pVarRec->recsize > (8 * sizeof(INT)) )
	    ;

        if (pVarRec->recsize > (9 * sizeof(INT)) )
            pNew->lHelpStringContext = pVarRec->HelpStringContext;

	// fill the VarDesc Structure
        pNew->vardesc.memid = pMemberID[ii];
        pNew->vardesc.varkind = (tagVARKIND)pVarRec->VarKind;
        pNew->vardesc.wVarFlags = pVarRec->Flags;
        GetTypedesc(pVarRec->DataType, &pNew->vardesc.elemdescVar.tdesc) ;
	/*   pNew->vardesc.lpstrSchema; is reserved (SDK) fixme?? */
        if (pVarRec->VarKind == VAR_CONST )
        {
            V_UNION(&(pNew->vardesc), lpvarValue) = new VARIANT;
            ParseValue(pVarRec->OffsValue, V_UNION(&(pNew->vardesc), lpvarValue));
        }
        else
            V_UNION(&(pNew->vardesc), oInst) = pVarRec->OffsValue;

	// Store
	pTypeInfo->pVariables.AddAtEnd(pNew);

        // Next record...
        pVarRec = (TLBVarRecord *)((char *)pVarRec + pVarRec->recsize);
    }
}

// ----------------------------------------------------------------------
// LoadSub
//
// Callback handler function for the EnumResourceNamesA function...
// ----------------------------------------------------------------------
static BOOL	WIN32API LoadSub(HANDLE hMod, LPCTSTR pResType, LPTSTR pResName, LONG lParm)
{
    dprintf((" LoadSub called"));
    HRSRC	hRsrc;
    HGLOBAL	hData;

    hRsrc = FindResourceA(hMod, pResName, pResType);
    hData =  LoadResource(hMod, hRsrc);
    *((void **)lParm) = LockResource(hData);
    return FALSE;
}

// ----------------------------------------------------------------------
// TypeLibExtract::Load
//
// Search for & load the TYPELIB into memory.
// ----------------------------------------------------------------------
HRESULT TypeLibExtract::Load(char * szFile)
{
    ULONG		lStart;			// Start of typelib in file
    ULONG		lReadLen;		// Size of data read from file
    ULONG		lFileSize;		// Size of file
    char		buf[5];
    OFSTRUCT		ofStruct;
    HANDLE		hFile;
    HRESULT		rc;

    dprintf(("OLEAUT32: TypeLibExtract::Load()"));

    // Open file
    if ((hFile = OpenFile(szFile, &ofStruct, OF_READ)) == HFILE_ERROR)
    {
	dprintf(("  Error opening file - 0x%08x", GetLastError()));
	return E_ACCESSDENIED;
    }

    lStart = 0;

    // Read first four byts of file to identify it...
    if (!Read(hFile, (void *)buf, 4, &lReadLen, lStart))
    {
	CloseHandle(hFile);
	dprintf(("  Failed to read file start bytes"));
	return E_FAIL;
    }

    // Check to see if this is a type 1 typelib...
    if ((buf[0] == 'S') && (buf[1] == 'L') && (buf[2] == 'T') && (buf[3] == 'G'))
    {
	dprintf(("  File identified as TYPE1 TYPELIB - not supported yet :-("));
	CloseHandle(hFile);
	return E_FAIL;
    }

    // Check to see if this is a type 2 typelib...
    if ((buf[0] == 'M') && (buf[1] == 'S') && (buf[2] == 'F') && (buf[3] == 'T'))
    {
	// Get typelib file size...
	lFileSize = GetFileSize(hFile, NULL);
	dprintf(("  File identified as TYPE2 TYPELIB - Loading image (%lu bytes)...", lFileSize));

	m_pTypeLib = HeapAlloc(m_hHeap, 0, lFileSize);
	if (!m_pTypeLib)
	{
	    CloseHandle(hFile);
	    dprintf(("  Failed to allocate a memory pool for typelib image"));
	    return E_OUTOFMEMORY;
	}

	// Read whole file into memory...
	if (!Read(hFile, m_pTypeLib, lFileSize, &lReadLen, lStart))
	{
	    CloseHandle(hFile);
	    dprintf(("  Failed to read typelib"));
	    HeapFree(m_hHeap, 0, m_pTypeLib);
	    return E_FAIL;
	}

	// Return buffer...
	CloseHandle(hFile);
	return S_OK;
    }

    // Done with file handle...
    CloseHandle(hFile);

    HINSTANCE		hInst;

    // Check to see if this is a module...
    hInst = CoLoadLibrary(szFile, TRUE);
    if (hInst)
    {
	// Yup - use EnumResourceNames to locate & load resource.
	EnumResourceNamesA(hInst, "TYPELIB", LoadSub, (LONG)&m_pTypeLib);
    }

    if (!m_pTypeLib)
    {
	dprintf(("  No TYPELIB resources found..."));
	return E_FAIL;
    }

    return S_OK;
}

// ----------------------------------------------------------------------
// TypeLibExtract::DumpHeader
//
// Drop a debug print of the header content to the ODIN log.
// ----------------------------------------------------------------------
void TypeLibExtract::DumpHeader()
{
    dprintf(("TYPELIB HEADER:"));
    if (m_fValid)
    {
	dprintf(("  Magic1            - 0x%08lx", m_pHeader->magic1));
	dprintf(("  Magic2            - 0x%08lx", m_pHeader->magic2));
	dprintf(("  posguid           - 0x%08lx", m_pHeader->posguid));
	dprintf(("  lcid              - 0x%08lx", m_pHeader->lcid));
	dprintf(("  lcid2             - 0x%08lx", m_pHeader->lcid2));
	dprintf(("  varflags          - 0x%08lx", m_pHeader->varflags));
	dprintf(("  version           - 0x%08lx", m_pHeader->version));
	dprintf(("  flags             - 0x%08lx", m_pHeader->flags));
	dprintf(("  nrtypeinfos       - 0x%08lx", m_pHeader->nrtypeinfos));
	dprintf(("  helpstring        - 0x%08lx", m_pHeader->helpstring));
	dprintf(("  helpstringcontext - 0x%08lx", m_pHeader->helpstringcontext));
	dprintf(("  helpcontext       - 0x%08lx", m_pHeader->helpcontext));
	dprintf(("  nametablecount    - 0x%08lx", m_pHeader->nametablecount));
	dprintf(("  nametablechars    - 0x%08lx", m_pHeader->nametablechars));
	dprintf(("  nameoffset        - 0x%08lx", m_pHeader->nameOffset));
	dprintf(("  helpfile          - 0x%08lx", m_pHeader->helpfile));
	dprintf(("  custdataoffset    - 0x%08lx", m_pHeader->customDataOffset));
	dprintf(("  res44             - 0x%08lx", m_pHeader->res44));
	dprintf(("  res48             - 0x%08lx", m_pHeader->res48));
	dprintf(("  dispatchpos       - 0x%08lx", m_pHeader->dispatchpos));
	dprintf(("  res50             - 0x%08lx", m_pHeader->res50));
    }
    else
    {
	dprintf(("  INVALID"));
    }
}

// ----------------------------------------------------------------------
// TypeLibExtract::MakeITypeLib
// ----------------------------------------------------------------------
HRESULT TypeLibExtract::MakeITypeLib(ITypeLibImpl * * ppObject)
{
    dprintf(("OLEAUT32: TypeLibExtract::MakeITypeLib"));

    HRESULT		rc;

    // Create new typelib object...
    if ((m_pITypeLib = ITypeLibImpl_Constructor()) == 0)
    	return E_OUTOFMEMORY;

    // Parse the typelib into storeage independent format.
    rc = Parse();
    if (rc != S_OK)
    {
	ITypeLibImpl_Destructor(m_pITypeLib);
	return rc;
    }

    // Return new object
    *ppObject = m_pITypeLib;
    return S_OK;
}

// ----------------------------------------------------------------------
// TypeLibExtract::Parse
// ----------------------------------------------------------------------
HRESULT TypeLibExtract::Parse()
{

    dprintf(("OLEAUT32: TypeLibExtract::Parse"));

    // Load header info...
    ParseGuid(m_pHeader->posguid, &m_pITypeLib->LibAttr.guid);

    m_pITypeLib->LibAttr.lcid = m_pHeader->lcid;
    m_pITypeLib->LibAttr.syskind = (tagSYSKIND)(m_pHeader->varflags & 0x0f); /* check the mask */
    m_pITypeLib->LibAttr.wMajorVerNum = LOWORD(m_pHeader->version);
    m_pITypeLib->LibAttr.wMinorVerNum = HIWORD(m_pHeader->version);
    m_pITypeLib->LibAttr.wLibFlags = (WORD) m_pHeader->flags & 0xffff;	/* check mask */

    // name, eventually add to a hash table
    ParseName(m_pHeader->nameOffset, &m_pITypeLib->szName);
    dprintf((" reading library: \"%s\"", m_pITypeLib->szName));

    // help info
    m_pITypeLib->lHelpContext = m_pHeader->helpstringcontext;
    ParseString(m_pHeader->helpstring, &m_pITypeLib->szDocString);
    ParseString(m_pHeader->helpfile, &m_pITypeLib->szHelpFile);
    if (m_pHelpStringOff)
	ParseString(*m_pHelpStringOff, &m_pITypeLib->szHelpStringDll);

    // Load Custom data
    ParseCustomData(m_pHeader->customDataOffset, &m_pITypeLib->pCustData);

    // Load typedescriptions
    if (m_pSegDir->pTypedescTab.length > 0)
    {
	TLBTypedesc *	pTd;
	TYPEDESC *	pNew;
	TLBArray *	pArray;
	ULONG		clTd;
	ULONG		ii;
	ULONG		jj;
	
	clTd = m_pSegDir->pTypedescTab.length / sizeof(TLBTypedesc);

	// Preload (in case of forward references)
	for (ii = 0; ii < clTd; ii++)
	{
	    pNew = new TYPEDESC;
	    m_pITypeLib->pTypedesc.AddAtEnd(pNew);
	}

	// Populate
	for (ii = 0, pTd = m_pTypedesc; ii < clTd; ii++, pTd++)
	{
	    pNew = m_pITypeLib->pTypedesc[ii];

	    pNew->vt = pTd->rec0 & VT_TYPEMASK;
	    if ((pNew->vt == VT_PTR) || (pNew->vt == VT_SAFEARRAY))
	    {
		if (pTd->rec3 < 0)
		    V_UNION(pNew, lptdesc) = &stndTypeDesc[pTd->rec2];
		else
		    V_UNION(pNew, lptdesc) = m_pITypeLib->pTypedesc[pTd->rec3 / 8];
	    }
	    else if (pNew->vt == VT_CARRAY) 
	    {
		pArray = (TLBArray *)((char *)m_pArray + pTd->rec2);

		V_UNION(pNew, lpadesc) =
		    (tagARRAYDESC*) HeapAlloc(m_hHeap, 0, (sizeof(ARRAYDESC)
			+ sizeof(SAFEARRAYBOUND) * (pArray->rec3 - 1)));

		if (pArray->rec1 < 0)
		    V_UNION(pNew,lpadesc)->tdescElem.vt= pArray->rec0 & VT_TYPEMASK;
		else
		    V_UNION(pNew,lpadesc)->tdescElem = stndTypeDesc[pArray->rec0 / 8];

		V_UNION(pNew,lpadesc)->cDims = pArray->rec2;

		INT *	pInt = (INT *)(pArray + 1);

		for (jj = 0; jj < pArray->rec2; jj++)
		{
		    V_UNION(pNew,lpadesc)->rgbounds[jj].cElements = *pInt++;
		    V_UNION(pNew,lpadesc)->rgbounds[jj].lLbound = *pInt++;
		}
	    }
	    else if (pNew->vt == VT_USERDEFINED)
	    {
		V_UNION(pNew, hreftype) = MAKELONG(pTd->rec2, pTd->rec3);
	    }
	}
    }

    // Load Imported typelibs
    if (m_pSegDir->pImpFiles.length > 0)
    {
	TLBImpFile *	pImpFile = m_pImpFile;
	TLBImpLib *	pNew;
        ULONG		recLen = 0;
        ULONG		totLen = 0;
        ULONG 		size;

        while(totLen < m_pSegDir->pImpFiles.length)
        {
	    pNew = new TLBImpLib;
            pNew->offset = totLen;

            ParseGuid(pImpFile->offGuid, &(pNew->guid));
            
            /* we are skipping some unknown info here */

            size = pImpFile->size >> 2;

            pNew->name = (char *)HeapAlloc(m_hHeap, 0, size + 1);
	    memcpy(pNew->name, pImpFile->name, size);

	    // Add to list
	    m_pITypeLib->pImpLibs.AddAtEnd(pNew);

            // Align to next 4 byte boundary...
            recLen = (sizeof(TLBImpFile) + size + 3) & 0xfffffffc;
            totLen += recLen;
            pImpFile = (TLBImpFile *)((char *)pImpFile + recLen);
        }
    }

    // Load Type info data
    if (m_pHeader->nrtypeinfos >=0 )
    {
	ULONG			ii;
	ITypeInfoImpl *		pNew;
	TLBTypeInfoBase *	pBase = m_pTypeInfo;

	for (ii = 0; ii < m_pHeader->nrtypeinfos; ii++)
	{
	    TLBTypeInfoBase *	pBase = (TLBTypeInfoBase *)
					((char *)m_pTypeInfo + m_pTypeInfoDir[ii]);
	    pNew = ITypeInfoImpl_Constructor();
	    pNew->pTypeLib = (ITypeLib *)m_pITypeLib;
	    pNew->index = ii;

	    ParseTypeInfo(pNew, pBase);

	    m_pITypeLib->pTypeInfo.AddAtEnd(pNew);
	}
    }

    return S_OK;
}

// ----------------------------------------------------------------------
// DupAtoBstr
// ----------------------------------------------------------------------
extern BSTR DupAtoBstr(char * pAscii)
{
    int		len;
    BSTR	bstr;
    DWORD *	pl;

    if (!pAscii)
        return NULL;

    len = strlen(pAscii);
    pl  =  (DWORD *)HeapAlloc(GetProcessHeap(), 0, (len+3)*sizeof(OLECHAR));
    pl[0] = (len)*sizeof(OLECHAR);
    bstr = (BSTR)&( pl[1]);
    lstrcpyAtoW( bstr, pAscii);
    return bstr;
}

