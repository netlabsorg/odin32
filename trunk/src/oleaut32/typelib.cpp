/* $Id: typelib.cpp,v 1.20 2001-05-30 17:43:39 sandervl Exp $ */
/* 
 * ITypelib interface
 * 
 * 1/12/99
 * 
 * Copyright 1999 David J. Raison
 * Some portions based on original work on Wine
 *   Copyright 1997 Marcus Meissner
 	       1999 Rein Klazes
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * NB: Logging from this module is piped into TLIB_n.LOG via
 * the private logging interface.
 */


#define PRIVATE_LOGGING	// Private logfile
#define DEBUG2

#include "oleaut32.h"
#include "olectl.h"
#include "oList.h"	// linked list template
#include "itypelib.h"

static FILE *_privateLogFile = NULL;

// ----------------------------------------------------------------------
// OpenPrivateLogFileTypelib
// ----------------------------------------------------------------------
void OpenPrivateLogFileTypelib()
{
#ifdef DEBUG
    char logname[1024];

    sprintf(logname, "tlib_%d.log", loadNr);
    _privateLogFile = fopen(logname, "w");
    if(_privateLogFile == NULL)
    {
	sprintf(logname, "%stlib_%d.log", oleaut32Path, loadNr);
	_privateLogFile = fopen(logname, "w");
    }
    dprintfGlobal(("TLIB LOGFILE : %s", logname));
#endif
}

// ----------------------------------------------------------------------
// ClosePrivateLogFileTypelib
// ----------------------------------------------------------------------
void ClosePrivateLogFileTypelib()
{
#ifdef DEBUG
    if(_privateLogFile)
    {
	fclose(_privateLogFile);
	_privateLogFile = NULL;
    }
#endif
}

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

    dprintfGlobal(("OLEAUT32: QueryPathOfRegTypeLib()"));

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
	dprintfGlobal(("OLEAUT32: QueryPathOfRegTypeLib(%s,%d,%d,0x%04lx,%p) - stub!",
	    xguid, wMaj, wMin, (DWORD)lcid, path));
	return E_FAIL;
    }
    plen = sizeof(pathname);
    if (RegQueryValueA(HKEY_LOCAL_MACHINE, typelibkey, pathname, (LPLONG)&plen))
    {
	/* try again without lang specific id */
	if (SUBLANGID(lcid))
	    return QueryPathOfRegTypeLib(guid, wMaj, wMin, PRIMARYLANGID(lcid), path);

	dprintfGlobal(("OLEAUT32: QueryPathOfRegTypeLib() - key \"%s\" not found", typelibkey));
	return E_FAIL;
    }
    *path = HEAP_strdupAtoW(GetProcessHeap(), 0, pathname);
    return S_OK;
}

/******************************************************************************
 * CreateTypeLib [OLEAUT32]  creates a typelib
 *
 * RETURNS
 *    Success: S_OK
 *    Failure: Status
 */
HRESULT WINAPI CreateTypeLib(
	SYSKIND syskind, LPCOLESTR szFile, ICreateTypeLib** ppctlib
) {
    dprintfGlobal(("ERROR: CreateTypeLib STUB"));
//    FIXME("(%d,%s,%p), stub!\n",syskind,debugstr_w(szFile),ppctlib);
    return E_FAIL;
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
    dprintfGlobal(("OLEAUT32: LoadTypeLibEx(%s)", szFileA));

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
	dprintfGlobal(("  Invalid typelib file"));
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

    dprintfGlobal(("OLEAUT32: LoadTypeLib()"));

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

    dprintfGlobal(("OLEAUT32: LoadRegTypeLib()"));

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
    HRESULT res;
    TLIBATTR *attr;
    OLECHAR guid[80];
    LPSTR guidA;
    CHAR keyName[120];
    HKEY key, subKey;

#ifdef __WIN32OS2__
    dprintfGlobal(("OLEAUT32: RegisterTypeLib() %x %ls %ls", ptlib, szFullPath, szHelpDir));
#endif

    if (ptlib == NULL || szFullPath == NULL)
        return E_INVALIDARG;

    if (!SUCCEEDED(ITypeLib_GetLibAttr(ptlib, &attr)))
        return E_FAIL;

    StringFromGUID2(&attr->guid, guid, 80);
    guidA = HEAP_strdupWtoA(GetProcessHeap(), 0, guid);
#ifdef __WIN32OS2__
    sprintf(keyName, "SOFTWARE\\Classes\\TypeLib\\%s\\%x.%x",
            guidA, attr->wMajorVerNum, attr->wMinorVerNum);
#else
    snprintf(keyName, sizeof(keyName), "TypeLib\\%s\\%x.%x",
             guidA, attr->wMajorVerNum, attr->wMinorVerNum);
#endif
    HeapFree(GetProcessHeap(), 0, guidA);

    res = S_OK;
#ifdef __WIN32OS2__
    if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, keyName, 0, NULL, 0,
#else
    if (RegCreateKeyExA(HKEY_CLASSES_ROOT, keyName, 0, NULL, 0,
#endif
        KEY_WRITE, NULL, &key, NULL) == ERROR_SUCCESS)
    {
        LPOLESTR doc;

        if (SUCCEEDED(ITypeLib_GetDocumentation(ptlib, -1, NULL, &doc, NULL, NULL)))
        {
            if (RegSetValueExW(key, NULL, 0, REG_SZ,
                (BYTE *)doc, lstrlenW(doc) * sizeof(OLECHAR)) != ERROR_SUCCESS)
                res = E_FAIL;

            SysFreeString(doc);
        }
        else
            res = E_FAIL;

        /* FIXME: This *seems* to be 0 always, not sure though */
        if (res == S_OK && RegCreateKeyExA(key, "0\\win32", 0, NULL, 0,
            KEY_WRITE, NULL, &subKey, NULL) == ERROR_SUCCESS)
        {
            if (RegSetValueExW(subKey, NULL, 0, REG_SZ,
                (BYTE *)szFullPath, lstrlenW(szFullPath) * sizeof(OLECHAR)) != ERROR_SUCCESS)
                res = E_FAIL;

            RegCloseKey(subKey);
        }
        else
            res = E_FAIL;

        if (res == S_OK && RegCreateKeyExA(key, "FLAGS", 0, NULL, 0,
            KEY_WRITE, NULL, &subKey, NULL) == ERROR_SUCCESS)
        {
            CHAR buf[20];
            /* FIXME: is %u correct? */
#ifdef __WIN32OS2__
            sprintf(buf, "%u", attr->wLibFlags);
            if (RegSetValueExA(subKey, NULL, 0, REG_SZ,
                (LPBYTE)buf, lstrlenA(buf) + 1) != ERROR_SUCCESS)
#else
            snprintf(buf, strlen(buf), "%u", attr->wLibFlags);
            if (RegSetValueExA(subKey, NULL, 0, REG_SZ,
                buf, lstrlenA(buf) + 1) != ERROR_SUCCESS)
#endif
                res = E_FAIL;
        }
        RegCloseKey(key);
    }
    else
        res = E_FAIL;

    ITypeLib_ReleaseTLibAttr(ptlib, attr);
    return res;
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
    OLECHAR guid[80];
    LPSTR guidA;
    CHAR keyName[120];
    HKEY key, subKey;

    StringFromGUID2(libid, guid, 80);
    guidA = HEAP_strdupWtoA(GetProcessHeap(), 0, guid);
    sprintf(keyName, "SOFTWARE\\Classes\\TypeLib\\%s\\%x.%x",
            guidA, wVerMajor, wVerMinor);
    RegDeleteKeyA(HKEY_LOCAL_MACHINE, keyName);
    HeapFree(GetProcessHeap(), 0, guidA);

    return S_OK;
}

// ======================================================================
// Local functions.
// ======================================================================

#if defined(DEBUG2)

// ----------------------------------------------------------------------
// dprintfLine2
// ----------------------------------------------------------------------
static void dprintfLine2(void)
{
    dprintf((LOG, "========================================"));
}

// ----------------------------------------------------------------------
// dprintfLine
// ----------------------------------------------------------------------
static void dprintfLine(void)
{
    dprintf((LOG, "----------------------------------------"));
}

// ----------------------------------------------------------------------
// TypeKindAsString
// ----------------------------------------------------------------------
char * TypeKindAsString(int typekind)
{
    char *	p;
    switch (typekind)
    {
	case TKIND_ENUM: p = "TKIND_ENUM"; break;
	case TKIND_RECORD: p = "TKIND_RECORD"; break;
	case TKIND_MODULE: p = "TKIND_MODULE"; break;
	case TKIND_INTERFACE: p = "TKIND_INTERFACE"; break;
	case TKIND_DISPATCH: p = "TKIND_DISPATCH"; break;
	case TKIND_COCLASS: p = "TKIND_COCLASS"; break;
	case TKIND_ALIAS: p = "TKIND_ALIAS"; break;
	case TKIND_UNION: p = "TKIND_UNION"; break;
	case TKIND_MAX: p = "TKIND_MAX"; break;
	default: p = "*UNKNOWN*"; break;
    }
    return p;
}

// ----------------------------------------------------------------------
// FuncKindAsString
// ----------------------------------------------------------------------
char * FuncKindAsString(int funckind)
{
    char *	p;
    switch (funckind)
    {
	case FUNC_VIRTUAL: p = "FUNC_VIRTUAL"; break;
	case FUNC_PUREVIRTUAL: p = "FUNC_PUREVIRTUAL"; break;
	case FUNC_NONVIRTUAL: p = "FUNC_NONVIRTUAL"; break;
	case FUNC_STATIC: p = "FUNC_STATIC"; break;
	case FUNC_DISPATCH: p = "FUNC_DISPATCH"; break;
	default: p = "*UNKNOWN*"; break;
    }
    return p;
}

// ----------------------------------------------------------------------
// InvKindAsString
// ----------------------------------------------------------------------
char * InvKindAsString(int invkind)
{
    char *	p;
    switch (invkind)
    {
	case INVOKE_FUNC: p = "INVOKE_FUNC"; break;
	case INVOKE_PROPERTYGET: p = "INVOKE_PROPERTYGET"; break;
	case INVOKE_PROPERTYPUT: p = "INVOKE_PROPERTYPUT"; break;
	case INVOKE_PROPERTYPUTREF: p = "INVOKE_PROPERTYPUTREF"; break;
	default: p = "*UNKNOWN*"; break;
    }
    return p;
}

// ----------------------------------------------------------------------
// VarKindAsString
// ----------------------------------------------------------------------
char * VarKindAsString(int varkind)
{
    char *	p;
    switch (varkind)
    {
	case VAR_PERINSTANCE: p = "VAR_PERINSTANCE"; break;
	case VAR_STATIC: p = "VAR_STATIC"; break;
	case VAR_CONST: p = "VAR_CONST"; break;
	case VAR_DISPATCH: p = "VAR_DISPATCH"; break;
	default: p = "*UNKNOWN*"; break;
    }
    return p;
}

// ----------------------------------------------------------------------
// CallConvAsString
// ----------------------------------------------------------------------
char * CallConvAsString(int callconv)
{
    char *	p;
    switch (callconv)
    {
	case CC_CDECL: p = "CC_CDECL"; break; 
//	case CC_MSCPASCAL: p = "CC_MSCPASCAL"; break; 
	case CC_PASCAL: p = "CC_PASCAL"; break; 
	case CC_MACPASCAL: p = "CC_MACPASCAL"; break; 
	case CC_STDCALL: p = "CC_STDCALL"; break; 
	case CC_RESERVED: p = "CC_RESERVED"; break; 
	case CC_SYSCALL: p = "CC_SYSCALL"; break; 
	case CC_MPWCDECL: p = "CC_MPWCDECL"; break; 
	case CC_MPWPASCAL: p = "CC_MPWPASCAL"; break; 
	case CC_MAX: p = "CC_MAX"; break; 

	default: p = "*UNKNOWN*"; break;
    }
    return p;
}

// ----------------------------------------------------------------------
// VariantAsString
// ----------------------------------------------------------------------
char * VariantTypeAsString(int vt)
{
    char *	p;

    switch(vt & VT_TYPEMASK)
    {
	case VT_EMPTY: p = "VT_EMPTY"; break;
	case VT_NULL: p = "VT_NULL"; break;
	case VT_I2: p = "VT_I2"; break;
	case VT_I4: p = "VT_I4"; break;
	case VT_R4: p = "VT_R4"; break;
	case VT_ERROR: p = "VT_ERROR"; break;
	case VT_BOOL: p = "VT_BOOL"; break;
	case VT_I1: p = "VT_I1"; break;
	case VT_UI1: p = "VT_UI1"; break;
	case VT_UI2: p = "VT_UI2"; break;
	case VT_UI4: p = "VT_UI4"; break;
	case VT_INT: p = "VT_INT"; break;
	case VT_UINT: p = "VT_UINT"; break;
	case VT_VOID: p = "VT_VOID"; break;
	case VT_HRESULT: p = "VT_HRESULT"; break;
	case VT_R8: p = "VT_R8"; break;
	case VT_CY: p = "VT_CY"; break;
	case VT_DATE: p = "VT_DATE"; break;
	case VT_I8: p = "VT_I8"; break;
	case VT_UI8: p = "VT_UI8"; break;
	case VT_DECIMAL: p = "VT_DECIMAL"; break;
	case VT_FILETIME: p = "VT_FILETIME"; break;
	case VT_BSTR: p = "VT_BSTR"; break;
	case VT_DISPATCH: p = "VT_DISPATCH"; break;
	case VT_VARIANT: p = "VT_VARIANT"; break;
	case VT_UNKNOWN: p = "VT_UNKNOWN"; break;
	case VT_PTR: p = "VT_PTR"; break;
	case VT_SAFEARRAY: p = "VT_SAFEARRAY"; break;
	case VT_CARRAY: p = "VT_CARRAY"; break;
	case VT_USERDEFINED: p = "VT_USERDEFINED"; break;
	case VT_LPSTR: p = "VT_LPSTR"; break;
	case VT_LPWSTR: p = "VT_LPWSTR"; break;
	case VT_BLOB: p = "VT_BLOB"; break;
	case VT_STREAM: p = "VT_STREAM"; break;
	case VT_STORAGE: p = "VT_STORAGE"; break;
	case VT_STREAMED_OBJECT: p = "VT_STREAMED_OBJECT"; break;
	case VT_STORED_OBJECT: p = "VT_STORED_OBJECT"; break;
	case VT_BLOB_OBJECT: p = "VT_BLOB_OBJECT"; break;
	case VT_CF: p = "VT_CF"; break;
	case VT_CLSID: p = "VT_CLSID"; break;
	default: p = "*UNKNOWN*"; break;
    }

    return p;
}

// ----------------------------------------------------------------------
// VariantTypeAsString2
// ----------------------------------------------------------------------
char * VariantTypeAsString2(int vt)
{
    char *	p;

    switch(vt & VT_TYPEMASK)
    {
	case VT_EMPTY: p = "(empty)"; break;
	case VT_NULL: p = "(null)"; break;
	case VT_I2: p = "short"; break;
	case VT_I4: p = "long"; break;
	case VT_R4: p = "float"; break;
	case VT_ERROR: p = "(error)"; break;
	case VT_BOOL: p = "boolean"; break;
	case VT_I1: p = "char"; break;
	case VT_UI1: p = "unsigned char"; break;
	case VT_UI2: p = "unsigned short"; break;
	case VT_UI4: p = "unsigned long"; break;
	case VT_INT: p = "int"; break;
	case VT_UINT: p = "unsigned int"; break;
	case VT_VOID: p = "void"; break;
	case VT_HRESULT: p = "HRESULT"; break;
	case VT_R8: p = "double"; break;
	case VT_CY: p = "currency"; break;
	case VT_DATE: p = "date"; break;
	case VT_I8: p = "long long"; break;
	case VT_UI8: p = "unsigned long long"; break;
	case VT_DECIMAL: p = "decimal"; break;
	case VT_FILETIME: p = "FILETIME"; break;
	case VT_BSTR: p = "BSTR"; break;
	case VT_DISPATCH: p = "IDISPATCH"; break;
	case VT_VARIANT: p = "VARIANT"; break;
	case VT_UNKNOWN: p = "IUNKNOWN"; break;
	case VT_PTR: p = "*"; break;
	case VT_SAFEARRAY: p = "SAFEARRAY"; break;
	case VT_CARRAY: p = "CARRAY"; break;
	case VT_USERDEFINED: p = "USERDEFINED"; break;
	case VT_LPSTR: p = "LPSTR"; break;
	case VT_LPWSTR: p = "LPWSTR"; break;
	case VT_BLOB: p = "BLOB"; break;
	case VT_STREAM: p = "STREAM"; break;
	case VT_STORAGE: p = "STORAGE"; break;
	case VT_STREAMED_OBJECT: p = "STREAMED_OBJECT"; break;
	case VT_STORED_OBJECT: p = "STORED_OBJECT"; break;
	case VT_BLOB_OBJECT: p = "BLOB_OBJECT"; break;
	case VT_CF: p = "CF"; break;
	case VT_CLSID: p = "CLSID"; break;
	default: p = "*UNKNOWN*"; break;
    }

    return p;
}
// ----------------------------------------------------------------------
// dprintTypeDesc
// ----------------------------------------------------------------------
size_t sprintfTypeDesc(char * buf, TYPEDESC * pDesc)
{
    LONG	ii;
    size_t	len = 0;

    if (pDesc->vt & VT_ARRAY)
    {
	ARRAYDESC *	pArray = V_UNION(pDesc, lpadesc);
	
	len += sprintfTypeDesc(buf + len, &pArray->tdescElem);
	for (ii = 0; ii < pArray->cDims; ii++)
	{
	    if (ii)
		len += sprintf(buf + len, ", ");

	    len += sprintf(buf + len, "[%ld..%ld]", pArray->rgbounds[ii].lLbound,
				pArray->rgbounds[ii].lLbound + pArray->rgbounds[ii].cElements);
	}

    }
    else if (pDesc->vt == VT_PTR)
    {
	len += sprintfTypeDesc(buf + len, V_UNION(pDesc, lptdesc)) ;	// 'ware recursion...!
	len += sprintf(buf + len,  "%s ", VariantTypeAsString2(pDesc->vt));
    }
    else if (pDesc->vt == VT_USERDEFINED)
    {
	len += sprintf(buf + len, "(%ld) ", V_UNION(pDesc, hreftype)  );
    }
    else
    {
	len += sprintf(buf + len, "%s ",  VariantTypeAsString2(pDesc->vt));
    }
    return len;
}

typedef struct FlagSt
{
    char *		name;
    ULONG		value;
} FLAGST;

static FLAGST FKCCICFlagData[] =
{
    {"custom data",	    0x80},
    {"default values",	    0x1000},
    {"numeric entry",	    0x2000},
    {"has retval",	    0x4000},	// Set for methods with a retval parameter
    {"0x8000",		    0x8000},
    {0, 0}
};

static FLAGST VarFlagData[] =
{
    {"readonly",	    0x1},
    {"source",		    0x2},
    {"bindable",	    0x4},
    {"requestedit ",	    0x8},
    {"displaybind ",	    0x10},
    {"defaultbinD ",	    0x20},
    {"hidden",		    0x40},
    {"restricted",	    0x80},
    {"defaultcollElem",	    0x100},
    {"uidefault ",	    0x200},
    {"nonbrowsable",	    0x400},
    {"replaceable ",	    0x800},
    {"immediatebind ",	    0x1000},
    {0, 0}
};

static FLAGST ParamFlagData[] =
{
    {"in",		    0x01},
    {"out",		    0x02},
    {"lcid",		    0x04},
    {"retval",		    0x08},
    {"opt",		    0x10},
    {"hasdefault",	    0x20},
    {0, 0}
};

static FLAGST FuncFlagData[] =
{
    {"restricted",    	    0x1},
    {"source",        	    0x2},
    {"bindable",      	    0x4},
    {"requestedit",   	    0x8},
    {"displaybind",   	    0x10},
    {"defaultbind",   	    0x20},
    {"hidden",        	    0x40},
    {"usesgetlasterror",    0x80},
    {"defaultcollelem",     0x100},
    {"uidefault",     	    0x200},
    {"nonbrowsable",  	    0x400},
    {"replaceable",   	    0x800},
    {"immediatebind", 	    0x1000},
    {0, 0}
};

// ----------------------------------------------------------------------
// sprintfFlags
// ----------------------------------------------------------------------
size_t sprintfFlags(char * buf, FLAGST const * pData, ULONG flags)
{
    size_t	len = 0;

    len += sprintf(buf + len, "[");

    while (pData->name != 0)
    {
	if (flags & pData->value)
	{
	    if (len > 1)
		len += sprintf(buf + len, ", ");

	    len += sprintf(buf + len, pData->name);
	}
	pData++;
    }
    len += sprintf(buf + len, "]");

    return len;
}


#endif

// ======================================================================
// Class functions.
// ======================================================================

// ----------------------------------------------------------------------
// TypeLibExtract::TypeLibExtract
// ----------------------------------------------------------------------
TypeLibExtract::TypeLibExtract(char * szName)
    : m_fValid(0), m_hHeap(GetProcessHeap()), m_fFree(FALSE)
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

    if (m_fFree)
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

    dprintf((LOG, "header:"));
    dprintf((LOG, "magic1=0x%08x ,magic2=0x%08x\n", m_pHeader->magic1, m_pHeader->magic2));

    if (memcmp(&m_pHeader->magic1,TLBMAGIC2,4)) {
	dprintf((LOG, "Header type magic 0x%08x not supported.",m_pHeader->magic1));
	return E_FAIL;
    }

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
        dprintf((LOG, "  Segment directory sanity check failed! pTypeInfo %x, pImpInfo %x", m_pSegDir->pTypeInfoTab.res0c, m_pSegDir->pImpInfo.res0c));
	return E_FAIL;
    }

    // Decode the segment offsets...
    dprintf((LOG, "Segment Offsets:"));

    if (m_pSegDir->pImpInfo.offset != 0xffffffff)
    {
	m_pImpInfo = (TLBImpInfo *)((char *)m_pTypeLib + m_pSegDir->pImpInfo.offset);
	dprintf((LOG, "  ImpInfo       0x%08lx", m_pSegDir->pImpInfo.offset));
    }
    else
    {
	m_pImpInfo = 0;
    }

    if (m_pSegDir->pRefTab.offset != 0xffffffff)
    {
	m_pRef = (TLBRefRecord *)((char *)m_pTypeLib + m_pSegDir->pRefTab.offset);
	dprintf((LOG, "  RefTab        0x%08lx", m_pSegDir->pRefTab.offset));
    }
    else
    {
	m_pRef = 0;
    }

    if (m_pSegDir->pGuidTab.offset != 0xffffffff)
    {
	m_pGUID = (GUID *)((char *)m_pTypeLib + m_pSegDir->pGuidTab.offset);
	dprintf((LOG, "  GuidTab       0x%08lx", m_pSegDir->pGuidTab.offset));
    }
    else
    {
	m_pGUID = 0;
    }

    if (m_pSegDir->pNameTab.offset != 0xffffffff)
    {
	m_pName = (TLBName *)((char *)m_pTypeLib + m_pSegDir->pNameTab.offset);
	dprintf((LOG, "  NameTab       0x%08lx", m_pSegDir->pNameTab.offset));
    }
    else
    {
	m_pName = 0;
    }

    if (m_pSegDir->pStringTab.offset != 0xffffffff)
    {
	m_pString = (TLBString *)((char *)m_pTypeLib + m_pSegDir->pStringTab.offset);
	dprintf((LOG, "  StringTab     0x%08lx", m_pSegDir->pStringTab.offset));
    }
    else
    {
	m_pString = 0;
    }

    if (m_pSegDir->pTypedescTab.offset != 0xffffffff)
    {
	m_pTypedesc = (TLBTypedesc *)((char *)m_pTypeLib + m_pSegDir->pTypedescTab.offset);
	dprintf((LOG, "  TypedescTab   0x%08lx", m_pSegDir->pTypedescTab.offset));
    }
    else
    {
	m_pTypedesc = 0;
    }

    if (m_pSegDir->pCustData.offset != 0xffffffff)
    {
	m_pCustData = (void *)((char *)m_pTypeLib + m_pSegDir->pCustData.offset);
	dprintf((LOG, "  CustData      0x%08lx", m_pSegDir->pCustData.offset));
    }
    else
    {
	m_pCustData = 0;
    }

    if (m_pSegDir->pCDGuids.offset != 0xffffffff)
    {
        m_pCDGuid = (TLBCDGuid *)((char *)m_pTypeLib + m_pSegDir->pCDGuids.offset);
	dprintf((LOG, "  CDGuids       0x%08lx", m_pSegDir->pCDGuids.offset));
    }
    else
    {
	m_pCDGuid = 0;
    }

    if (m_pSegDir->pImpFiles.offset != 0xffffffff)
    {
        m_pImpFile = (TLBImpFile *)((char *)m_pTypeLib + m_pSegDir->pImpFiles.offset);
	dprintf((LOG, "  ImpFiles      0x%08lx", m_pSegDir->pImpFiles.offset));
    }
    else
    {
	m_pImpFile = 0;
    }

    if (m_pSegDir->pTypeInfoTab.offset != 0xffffffff)
    {
        m_pTypeInfo = (TLBTypeInfoBase *)((char *)m_pTypeLib + m_pSegDir->pTypeInfoTab.offset);
	dprintf((LOG, "  TypeInfoTab   0x%08lx", m_pSegDir->pTypeInfoTab.offset));
    }
    else
    {
	m_pTypeInfo = 0;
    }

    if (m_pSegDir->pArrayDescriptions.offset != 0xffffffff)
    {
        m_pArray = (TLBArray *)((char *)m_pTypeLib + m_pSegDir->pArrayDescriptions.offset);
	dprintf((LOG, "  ArrayDesc     0x%08lx", m_pSegDir->pArrayDescriptions.offset));
    }
    else
    {
	m_pArray = 0;
    }

    if (m_pSegDir->pRes7.offset != 0xffffffff)
    {
        m_pRes7 = (void *)((char *)m_pTypeLib + m_pSegDir->pRes7.offset);
	dprintf((LOG, "  Res7          0x%08lx", m_pSegDir->pRes7.offset));
    }
    else
    {
	m_pRes7 = 0;
    }

    if (m_pSegDir->pResE.offset != 0xffffffff)
    {
        m_pResE = (void *)((char *)m_pTypeLib + m_pSegDir->pResE.offset);
	dprintf((LOG, "  ResE          0x%08lx", m_pSegDir->pResE.offset));
    }
    else
    {
	m_pResE = 0;
    }

    if (m_pSegDir->pResF.offset != 0xffffffff)
    {
        m_pResF = (void *)((char *)m_pTypeLib + m_pSegDir->pResF.offset);
	dprintf((LOG, "  ResF          0x%08lx", m_pSegDir->pResF.offset));
    }
    else
    {
	m_pResF = 0;
    }

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
	    // SvL: *pStringLen == -1 in MS Office 2k install
	    // DjR: *pStringLen == -1 is used to denote VBA.vbNullString
	    //      *pStringLen == 0 for a string that happens to be 0 length
	    //      *pStringLen > 0 for a string with defined content.
            if(*pStringLen == -1)
		*pStringLen = 0;

	    V_UNION(pVar, bstrVal) = SysAllocStringLen(NULL, *pStringLen);

	    if (*pStringLen != 0)	// Save cluttering up logfile
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
            dprintf((LOG,
            	     " VARTYPE %d (%s) is not supported yet, setting to NULL\n",
            	     pVar->vt,
            	     VariantTypeAsString(pVar->vt)));
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

    while (offset >= 0)		// Some offsets in VBA are 0x8c000000 - Special meaning??
    {
	pCDGuid = (TLBCDGuid *)((char *)m_pCDGuid + offset);
	pNew = new TLBCustData;
        memset(pNew, 0, sizeof(*pNew));	// Ensure that unset fields get nulled out.

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

    /*  FIXME: */
    /*    IDLDESC  idldescType; *//* never saw this one ! =  zero  */

    /* name, eventually add to a hash table */
    ParseName(pBase->NameOffset, &pTypeInfo->szName);

    /* help info */
    ParseString(pBase->docstringoffs, &pTypeInfo->szDocString);
    pTypeInfo->lHelpStringContext = pBase->helpstringcontext;
    pTypeInfo->lHelpContext = pBase->helpcontext;
    /* note: InfoType's Help file and HelpStringDll come from the containing
     * library. Further HelpString and Docstring appear to be the same thing :(
     */

    // ImplTypes {DR: not sure this is OK yet}
    switch(pTypeInfo->TypeAttr.typekind)
    {
	case TKIND_ALIAS:
	    GetTypedesc(pBase->datatype1, &pTypeInfo->TypeAttr.tdescAlias) ;
	    break;

	case TKIND_COCLASS:
	    ParseImplemented(pTypeInfo, pBase);
	    break;

	case TKIND_DISPATCH:
	    break;

	default:
	    if (pBase->datatype1 != 0xffffffff)
	    {
		TLBRefType *	pNew = new TLBRefType;
		memset(pNew, 0, sizeof(*pNew));	// Ensure that unset fields get nulled out.

		ParseReference(pBase->datatype1, pNew);
		pTypeInfo->pImplements.AddAtEnd(pNew);
	    }
	
    }

#if defined(DEBUG2)	    
    dprintfLine2();
    dprintf((LOG, "TypeInfo        %s", pTypeInfo->szName));
    dprintf((LOG, "  .typeKind:    %08x (%s)", pTypeInfo->TypeAttr.typekind,
						TypeKindAsString(pTypeInfo->TypeAttr.typekind)));
    char	guid[128];
    char	buf[1024];
    WINE_StringFromCLSID(&pTypeInfo->TypeAttr.guid, guid);
    dprintf((LOG, "  .GUID:        %s", guid));
    dprintf((LOG, "  .lcid:        %08x", pTypeInfo->TypeAttr.lcid));
    dprintf((LOG, "  .memidCon:    %08x", pTypeInfo->TypeAttr.memidConstructor));
    dprintf((LOG, "  .memidDest:   %08x", pTypeInfo->TypeAttr.memidDestructor));
    dprintf((LOG, "  .SizeInst:    %08x", pTypeInfo->TypeAttr.cbSizeInstance));
    dprintf((LOG, "  .cFuncs:      %08x", pTypeInfo->TypeAttr.cFuncs));
    dprintf((LOG, "  .cVars:       %08x", pTypeInfo->TypeAttr.cVars));
    dprintf((LOG, "  .cbAlignment: %08x", pTypeInfo->TypeAttr.cbAlignment));
    dprintf((LOG, "  .TypeFlags:   %08x", pTypeInfo->TypeAttr.wTypeFlags));
    dprintf((LOG, "  .MajorVerNum: %08x", pTypeInfo->TypeAttr.wMajorVerNum));
    dprintf((LOG, "  .MinorVerNum: %08x", pTypeInfo->TypeAttr.wMinorVerNum));
    dprintf((LOG, "  .cImplTypes:  %08x", pTypeInfo->TypeAttr.cImplTypes));
    dprintf((LOG, "  .cbSizeVft:   %08x", pTypeInfo->TypeAttr.cbSizeVft));
    if (pTypeInfo->TypeAttr.typekind == TKIND_ALIAS)
    {
	sprintfTypeDesc(buf, &pTypeInfo->TypeAttr.tdescAlias);
	dprintf((LOG, "  .tdesc        %s", buf));
    }
    dprintf((LOG, "  .DocString    %s", pTypeInfo->szDocString));
    dprintf((LOG, "  .HelpStrCtx   %08x", pTypeInfo->lHelpStringContext));
    dprintf((LOG, "  .HelpCtx      %08x", pTypeInfo->lHelpContext));

    dprintf((LOG, "pBase"));
    dprintf((LOG, "  .res2         %08x", pBase->res2));
    dprintf((LOG, "  .res3         %08x", pBase->res3));
    dprintf((LOG, "  .res4         %08x", pBase->res4));
    dprintf((LOG, "  .res5         %08x", pBase->res5));
    dprintf((LOG, "  .res7         %08x", pBase->res7));
    dprintf((LOG, "  .res8         %08x", pBase->res8));
    dprintf((LOG, "  .res9         %08x", pBase->res9));
    dprintf((LOG, "  .resA         %08x", pBase->resA));
    dprintf((LOG, "  .datatype1    %08x", pBase->datatype1));
    dprintf((LOG, "  .datatype2    %08x", pBase->datatype2));
    dprintf((LOG, "  .res18        %08x", pBase->res18));
    dprintf((LOG, "  .res19        %08x", pBase->res19));

#endif

    // Functions/
    ParseMembers(pTypeInfo, pBase);


    // Load Custom data
    ParseCustomData(pBase->oCustData, &pTypeInfo->pCustData);

#if defined(DEBUG2)	    
    dprintfLine();
#endif

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
	dprintf((LOG, " WARNING: Cannot find a reference\n"));
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
        memset(pNew, 0, sizeof(*pNew));	// Ensure that unset fields get nulled out.

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
    char * 		pMemberRec;
    TLBFuncRecord *	pFuncRec;
    TLBVarRecord *	pVarRec;
    int			ii;
    int			jj;
    int			iAttrCount;
    INT *		pInfoLen;
    INT *		pNameOff;
    INT *		pMemberID;
    INT *		pMemberOff;
    INT *		pDefaultOff;
    INT *		pCustomOff;
    LONG		lMembers = pTypeInfo->TypeAttr.cFuncs + pTypeInfo->TypeAttr.cVars;

    // Map ptrs to sections of the typeinfo record...
    pInfoLen = (INT *)((char *)m_pTypeLib + pBase->memoffset);
    pMemberRec = (char *)(pInfoLen + 1);
    pMemberID = (INT *)(pMemberRec + *pInfoLen);
    pNameOff = pMemberID + lMembers;
    pMemberOff = pNameOff + lMembers;

    // loop through each function...
    for (ii = 0; ii < pTypeInfo->TypeAttr.cFuncs; ii++)
    {
        // Get record...
        pFuncRec = (TLBFuncRecord *)(pMemberRec + pMemberOff[ii]);

	TLBFuncDesc *	pNew;

        pNew = new TLBFuncDesc;
        memset(pNew, 0, sizeof(*pNew));	// Ensure that unset fields get nulled out.

        ParseName(pNameOff[ii], &pNew->szName);
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

	// Calc remaining attributes to be decoded.
	iAttrCount = pFuncRec->recsize
			- 24		// OFFSETOF(OptAttr[0])
			- (pFuncRec->nrargs * sizeof(TLBParameterInfo));

	iAttrCount /= sizeof(INT);

	// If the 'default values' flag is set
	// allow for a default value for each argument
	// (include the return value)
	if (pFuncRec->FKCCIC & 0x1000)
	    iAttrCount -= pFuncRec->nrargs;

	// If the 'custom data' flag is set
	// allow for one dataoffset for the method
	// plus one for each argument
	// (include the return value)
	if (pFuncRec->FKCCIC & 0x0080)
	    iAttrCount -= pFuncRec->nrargs + 1;

	// Loop through 'size specified' block to rip info...
	for (jj = 0; jj < iAttrCount; jj++)
	{
	    switch(jj)
	    {
		case 0:
		    pNew->helpcontext = pFuncRec->OptAttr[jj];
		    break;

		case 1:
		    ParseString(pFuncRec->OptAttr[jj], &pNew->szHelpString);
		    break;

		case 2:
		    if (pFuncRec->FKCCIC & 0x2000)
			pNew->szEntry = (char *)(pFuncRec->OptAttr[jj]);
		    else
			ParseString(pFuncRec->OptAttr[jj], &pNew->szEntry);
		    break;

		case 5:
		    pNew->lHelpStringContext = pFuncRec->OptAttr[jj];
		    break;

		default:
		    if (pFuncRec->OptAttr[jj] != 0xffffffff)
			dprintf((LOG, "- skipping pFuncRec->OptAttr[%lx] (%08x)...",
				jj, pFuncRec->OptAttr[jj]));
	    }
	}

	// Default & custom data come after size specified' block...
	pCustomOff = pDefaultOff = pFuncRec->OptAttr + iAttrCount;

	// Only advance custom data if there are defaults...
	if (pFuncRec->FKCCIC & 0x1000)
	    pCustomOff += pFuncRec->nrargs;

	// Rip the method custom data...
	if (pFuncRec->FKCCIC & 0x80)
	    ParseCustomData(pCustomOff[0], &pNew->pCustData);

#if defined(DEBUG2)
	{
	dprintfLine();
        char	 buf[1024];
	dprintf((LOG, "Method          \"%s\"\n", pNew->szName));
	dprintf((LOG, "  .memid        %08x", pNew->funcdesc.memid));
        dprintf((LOG, "  .funckind     %08x (%s)",
				    pNew->funcdesc.funckind,
				    FuncKindAsString(pNew->funcdesc.funckind)));
        dprintf((LOG, "  .invkind      %08x (%s)",
				    pNew->funcdesc.invkind,
				    InvKindAsString(pNew->funcdesc.invkind)));
        dprintf((LOG, "  .callconv     %08x (%s)",
				    pNew->funcdesc.callconv,
				    CallConvAsString(pNew->funcdesc.callconv)));
        dprintf((LOG, "  .cParams      %08x", pNew->funcdesc.cParams));
        dprintf((LOG, "  .cParamsOpt   %08x", pNew->funcdesc.cParamsOpt));
        dprintf((LOG, "  .oVft         %08x", pNew->funcdesc.oVft));
        sprintfFlags(buf, FuncFlagData, pNew->funcdesc.wFuncFlags);
        dprintf((LOG, "  .wFuncFlags   %s", buf));
        sprintfTypeDesc(buf, &pNew->funcdesc.elemdescFunc.tdesc);
        dprintf((LOG, "  .tdesc        %s", buf));
        dprintf((LOG, "  .helpcontext  %08x", pNew->helpcontext));
        dprintf((LOG, "  .helpstring   %s", pNew->szHelpString));
        if (pFuncRec->FKCCIC & 0x2000)
	    dprintf((LOG, "  .entry        %08x", pNew->szHelpString));
        else
	    dprintf((LOG, "  .entry        %s", pNew->szHelpString));
#if 0
        dprintf((LOG, "pFuncRec"));
        dprintf((LOG, "  .recsize      %08lx", pFuncRec->recsize));
        dprintf((LOG, "  .index        %08lx", pFuncRec->index));
        dprintf((LOG, "  .Datatype     %08lx", pFuncRec->DataType));
        dprintf((LOG, "  .Flags        %08lx", pFuncRec->Flags));
        dprintf((LOG, "  .res3         %08lx", pFuncRec->res3));

        sprintfFlags(buf, FKCCICFlagData, pFuncRec->FKCCIC);
        dprintf((LOG, "  .FKCCIC       %08lx (%s)", pFuncRec->FKCCIC, buf));
        dprintf((LOG, "  .nrargs       %04lx", pFuncRec->nrargs));
        dprintf((LOG, "  .nroargs      %04lx", pFuncRec->nroargs));
	for (jj = 0;
		(pFuncRec->recsize
		    - (jj * sizeof(INT))
		    - 24
		    - (pFuncRec->nrargs * sizeof(TLBParameterInfo))) > 0 ;
		jj++)
	    dprintf((LOG, "  .optattr[%d]   %08lx", jj, pFuncRec->OptAttr[jj]));

#endif // 0
	}
#endif // defined(DEBUG2)

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

                // default value
                if ((PARAMFLAG_FHASDEFAULT & V_UNION(&(pNew->funcdesc.
			lprgelemdescParam[jj]),paramdesc.wParamFlags)) )
		{
		    if (pDefaultOff[jj] != 0xffffffff)	// jic
		    {
			PARAMDESC * pParamDesc =  &V_UNION(&(pNew->funcdesc.
				lprgelemdescParam[jj]), paramdesc);
			pParamDesc->pparamdescex  = new PARAMDESCEX;
			pParamDesc->pparamdescex->cBytes = sizeof(PARAMDESCEX);
			ParseValue(pDefaultOff[jj], &(pParamDesc->pparamdescex->varDefaultValue));
		    }
                }

                // custom info
                if (pFuncRec->FKCCIC & 0x80)
                    ParseCustomData(pCustomOff[1 + jj], &pNew->pParamDesc[jj].pCustData);

#if defined(DEBUG2)
	dprintfLine();
	dprintf((LOG, "Parameter       \"%s\"\n", pNew->pParamDesc[jj].szName));
	char	buf[1024];
	sprintfTypeDesc(buf, &pNew->funcdesc.lprgelemdescParam[jj].tdesc);
	dprintf((LOG, "  .tdesc        %s", buf));
	sprintfFlags(buf, ParamFlagData,
		V_UNION(&pNew->funcdesc.lprgelemdescParam[jj], paramdesc.wParamFlags));
	dprintf((LOG, "  .wParamFlags  %s", buf));
#endif


                pParam++;	// Next record.
            }
	}

	/* scode is not used: archaic win16 stuff FIXME: right? */
        pNew->funcdesc.cScodes = 0 ;
        pNew->funcdesc.lprgscode = NULL ;

	// Store
	pTypeInfo->pFunctions.AddAtEnd(pNew);

    }

    for (ii = 0; ii < pTypeInfo->TypeAttr.cVars; ii++)
    {
        // Get record...
        pVarRec = (TLBVarRecord *)(pMemberRec + pMemberOff[pTypeInfo->TypeAttr.cFuncs + ii]);

	TLBVarDesc *	pNew;

	pNew = new TLBVarDesc;
        memset(pNew, 0, sizeof(*pNew));	// Ensure that unset fields get nulled out.

	// name, eventually add to a hash table 
	ParseName(pNameOff[ii], &pNew->szName);

	// Calc remaining attributes to be decoded.
	iAttrCount = pVarRec->recsize - 20;

	iAttrCount /= sizeof(INT);

	// Optional data
	for (jj = 0; jj < iAttrCount; jj++)
	{
	    switch(jj)
	    {
		case 0:
		    pNew->lHelpContext = pVarRec->OptAttr[jj];
		    break;

		case 1:
		    ParseString(pVarRec->OptAttr[jj], &pNew->szHelpString);
		    break;

		case 4:
		    pNew->lHelpStringContext = pVarRec->OptAttr[jj];
		    break;

		default:
		    if (pVarRec->OptAttr[jj] != 0xffffffff)
			dprintf((LOG, "- skipping pVarRec->OptAttr[%lx] (%08x)...",
				jj, pVarRec->OptAttr[jj]));

	    }
	}

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

#if defined(DEBUG2)
	{
	dprintfLine();
        char	 buf[1024];
	dprintf((LOG, "Variable        \"%s\"\n", pNew->szName));
	dprintf((LOG, "  .MemberID     %08x", pNew->vardesc.memid));
	dprintf((LOG, "  .VarKind      %08x (%s)",
					pNew->vardesc.varkind,
					VarKindAsString(pNew->vardesc.varkind)));
        sprintfFlags(buf, FuncFlagData, pNew->vardesc.wVarFlags);
	dprintf((LOG, "  .VarFlags     %08x %s", pNew->vardesc.wVarFlags, buf));
	sprintfTypeDesc(buf, &pNew->vardesc.elemdescVar.tdesc);
	dprintf((LOG, "  .TDesc        %s", buf));
	dprintf((LOG, "  .HelpContext  %08x", pNew->lHelpContext));
        dprintf((LOG, "  .HelpStrCtx   %08x", pNew->lHelpStringContext));
        dprintf((LOG, "  .HelpStr      %s", pNew->szHelpString));

#if 0
        dprintf((LOG, "pVarRec"));
	for (jj = 0; jj < iAttrCount; jj++)
	    dprintf((LOG, "  .optattr[%d]   %08lx", jj, pVarRec->OptAttr[jj]));

#endif

        }

#endif // defined(DEBUG2)

	// Store
	pTypeInfo->pVariables.AddAtEnd(pNew);
    }
}

typedef struct LoadSt
{
    ULONG	cLibs;
    VOID * *	ppLib;
} LoadSt;

// ----------------------------------------------------------------------
// LoadSub
//
// Callback handler function for the EnumResourceNamesA function...
// ----------------------------------------------------------------------
static BOOL	WIN32API LoadSub(HANDLE hMod, LPCTSTR pResType, LPTSTR pResName, LONG lParm)
{
    LoadSt *	pLoad = (LoadSt *)lParm;

    if (--pLoad->cLibs == 0)
    {

	HRSRC	hRsrc;
	HGLOBAL	hData;

	hRsrc = FindResourceA(hMod, pResName, pResType);
	hData =  LoadResource(hMod, hRsrc);
	*(pLoad->ppLib) = LockResource(hData);
	return FALSE;
    }
    return TRUE;
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
    HINSTANCE		hInst;
    LoadSt		loadData;

    dprintf((LOG, "TypeLibExtract::Load(%s)", szFile));

    // Open file
    if ((hFile = OpenFile(szFile, &ofStruct, OF_READ)) != HFILE_ERROR)
    {
	lStart = 0;

	// Read first four byts of file to identify it...
	if (!Read(hFile, (void *)buf, 4, &lReadLen, lStart))
	{
	    CloseHandle(hFile);
	    dprintf((LOG, "  Failed to read file start bytes"));
	    return E_FAIL;
	}

	// Check to see if this is a type 1 typelib...
	if ((buf[0] == 'S') && (buf[1] == 'L') && (buf[2] == 'T') && (buf[3] == 'G'))
	{
	    CloseHandle(hFile);
	    dprintf((LOG, "  File identified as TYPE1 TYPELIB - not supported yet :-("));
	    return E_FAIL;
	}

	// Check to see if this is a type 2 typelib...
	if ((buf[0] == 'M') && (buf[1] == 'S') && (buf[2] == 'F') && (buf[3] == 'T'))
	{
	    // Get typelib file size...
	    lFileSize = GetFileSize(hFile, NULL);
	    dprintf((LOG, "  File identified as TYPE2 TYPELIB - Loading image (%lu bytes)", lFileSize));

	    m_pTypeLib = HeapAlloc(m_hHeap, 0, lFileSize);
	    if (!m_pTypeLib)
	    {
		CloseHandle(hFile);
		dprintf((LOG, "  Failed to allocate a memory pool for typelib image"));
		return E_OUTOFMEMORY;
	    }

	    // Read whole file into memory...
	    if (!Read(hFile, m_pTypeLib, lFileSize, &lReadLen, lStart))
	    {
		CloseHandle(hFile);
		dprintf((LOG, "  Failed to read typelib"));
		HeapFree(m_hHeap, 0, m_pTypeLib);
		return E_FAIL;
	    }

	    // Return buffer...
	    CloseHandle(hFile);
	    m_fFree = TRUE;
	    return S_OK;
	}

	// Done with file handle...
	CloseHandle(hFile);
    }
    else
    {
	// Failed to open file - Check to see if this is a request for a given resource
	HRSRC			hRsrc;
	HGLOBAL			hData;
	char *       		szLibName = HEAP_strdupA(m_hHeap, 0, szFile);
	char *			p = strrchr(szLibName, '\\');
	ULONG			lResId;

	if (p)
	{
	    // Filename of form {drive:}{\\path\\}file\\resid
	    *p = 0;
	    lResId = atoi((p + 1));

            LPOLESTR libnameW = (LPOLESTR)HEAP_strdupAtoW(m_hHeap, 0, szLibName);
	    hInst = CoLoadLibrary(libnameW, TRUE);
	    HeapFree(m_hHeap, 0, libnameW);
	    HeapFree(m_hHeap, 0, szLibName);
	    if (hInst != 0)
	    {
		HRSRC	hRsrc;
		HGLOBAL	hData;

		if ((hRsrc = FindResourceA(hInst, (LPTSTR)lResId, "TYPELIB")) != 0)
		{
		    hData =  LoadResource(hInst, hRsrc);
		    m_pTypeLib = LockResource(hData);
		    return S_OK;
		}
		return E_FAIL;
	    }
	}
	HeapFree(m_hHeap, 0, szLibName);
    }

    // Check to see if this is a module...
    LPOLESTR libnameW = (LPOLESTR)HEAP_strdupAtoW(m_hHeap, 0, szFile);
    hInst = CoLoadLibrary(libnameW, TRUE);
    HeapFree(m_hHeap, 0, libnameW);
    if (hInst)
    {
	loadData.cLibs = 1;
	loadData.ppLib = &m_pTypeLib;
	// Yup - use EnumResourceNames to locate & load 1st resource.
	EnumResourceNamesA(hInst, "TYPELIB", LoadSub, (LONG)&loadData);
	return m_pTypeLib ? S_OK : E_FAIL;
    }

    return E_FAIL;
}

// ----------------------------------------------------------------------
// TypeLibExtract::MakeITypeLib
// ----------------------------------------------------------------------
HRESULT TypeLibExtract::MakeITypeLib(ITypeLibImpl * * ppObject)
{
    dprintf((LOG, "TypeLibExtract::MakeITypeLib"));

    HRESULT		rc;

    // Create new typelib object...
    if ((m_pITypeLib = ITypeLibImpl_Constructor()) == 0)
    {
	dprintfGlobal(("OLEAUT32: TypeLibExtract::MakeITypeLib: E_OUTOFMEMORY"));
    	return E_OUTOFMEMORY;
    }

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

    dprintf((LOG, "TypeLibExtract::Parse"));

    // name, eventually add to a hash table
    ParseName(m_pHeader->nameOffset, &m_pITypeLib->szName);

    // Load header info...
    ParseGuid(m_pHeader->posguid, &m_pITypeLib->LibAttr.guid);

    m_pITypeLib->LibAttr.lcid = m_pHeader->lcid;
    m_pITypeLib->LibAttr.syskind = (tagSYSKIND)(m_pHeader->varflags & 0x0f); /* check the mask */
    m_pITypeLib->LibAttr.wMajorVerNum = LOWORD(m_pHeader->version);
    m_pITypeLib->LibAttr.wMinorVerNum = HIWORD(m_pHeader->version);
    m_pITypeLib->LibAttr.wLibFlags = (WORD) m_pHeader->flags & 0xffff;	/* check mask */

    // help info
    m_pITypeLib->lHelpContext = m_pHeader->helpstringcontext;
    ParseString(m_pHeader->helpstring, &m_pITypeLib->szDocString);
    ParseString(m_pHeader->helpfile, &m_pITypeLib->szHelpFile);
    if (m_pHelpStringOff)
	ParseString(*m_pHelpStringOff, &m_pITypeLib->szHelpStringDll);

#if defined(DEBUG2)

    dprintfLine2();
    dprintf((LOG, "Library:        \"%s\"", m_pITypeLib->szName));
    char	guid[128];
    WINE_StringFromCLSID(&m_pITypeLib->LibAttr.guid, guid);
    dprintf((LOG, "  .GUID:        %s", guid));
    dprintf((LOG, "  .lcid:        %08x", m_pITypeLib->LibAttr.lcid));
    dprintf((LOG, "  .syskind:     %08x", m_pITypeLib->LibAttr.syskind));
    dprintf((LOG, "  .MajorVer:    %08x", m_pITypeLib->LibAttr.wMajorVerNum));
    dprintf((LOG, "  .MinorVer:    %08x", m_pITypeLib->LibAttr.wMinorVerNum));
    dprintf((LOG, "  .LibFlags:    %08x", m_pITypeLib->LibAttr.wLibFlags));

#endif

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
	    memset(pNew, 0, sizeof(*pNew));	// Ensure that unset fields get nulled out.
	    m_pITypeLib->pTypedesc.AddAtEnd(pNew);
	}

	// Populate
	for (ii = 0, pTd = m_pTypedesc; ii < clTd; ii++, pTd++)
	{
	    pNew = m_pITypeLib->pTypedesc[ii];

	    pNew->vt = pTd->rec0 & VT_TYPEMASK;
	    if (pNew->vt == VT_PTR)
	    {
		// >< 99.12.18 DJR Changed test
		if ((pTd->rec3 & 0x8000))
		    V_UNION(pNew, lptdesc) = &stndTypeDesc[pTd->rec2];
		else
		    V_UNION(pNew, lptdesc) = m_pITypeLib->pTypedesc[pTd->rec2 / 8];
	    }
	    if (pNew->vt == VT_SAFEARRAY)
	    {
		dprintf((LOG, "VT_SAFEARRAY - NIY"));
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

#if defined(DEBUG2)	    
	    dprintfLine2();
	    dprintf((LOG, "TypeDesc:       %i", ii));
	    char	buf[1024];
	    sprintfTypeDesc(buf, pNew);
	    dprintf((LOG, "  desc          %s", buf));
	    dprintf((LOG, "  pTd->rec0:    %04x", pTd->rec0));
	    dprintf((LOG, "  pTd->rec1:    %04x", pTd->rec1));
	    dprintf((LOG, "  pTd->rec2:    %04x", pTd->rec2));
	    dprintf((LOG, "  pTd->rec3:    %04x", pTd->rec3));
	    if (pNew->vt == VT_CARRAY) 
	    {
		dprintf((LOG, "  pArray->rec0: %04x", pArray->rec0));
		dprintf((LOG, "  pArray->rec1: %04x", pArray->rec1));
		dprintf((LOG, "  pArray->rec2: %04x", pArray->rec2));
		dprintf((LOG, "  pArray->rec3: %04x", pArray->rec3));
	    }
#endif
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
	    memset(pNew, 0, sizeof(*pNew));	// Ensure that unset fields get nulled out.
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
#if defined(DEBUG2)	    
	    dprintfLine2();
	    dprintf((LOG, "ImpLib %i", pNew->offset));
	    char	guid[128];
	    WINE_StringFromCLSID(&(pNew->guid), guid);
	    dprintf((LOG, "  .GUID:        %s", guid));
	    dprintf((LOG, "  .Name:        %s", pNew->name));
#endif
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

