/*
 *	TYPELIB
 *
 *	Copyright 1997	Marcus Meissner
 *		      1999  Rein Klazes
 *		      2000  Francois Jacques
 * --------------------------------------------------------------------------------------
 * Known problems (2000, Francois Jacques)
 *
 * - Tested using OLEVIEW (Platform SDK tool) only.
 *   
 * - dual interface dispinterfaces. vtable-interface ITypeInfo instances are 
 *   creating by doing a straight copy of the dispinterface instance and just changing 
 *   its typekind. Pointed structures aren't copied - only the address of the pointers.
 *   So when you release the dispinterface, you delete the vtable-interface structures
 *   as well... fortunately, clean up of structures is not implemented.
 * 
 * - locale stuff is partially implemented but hasn't been tested.
 *
 * - imported typelib should be stored together in a linked list instead of having
 *   independant TLBImpLib strucutures in each ITypeInfo. This way, imported libraries
 *   are just imported once (major optimization)
 *
 * - typelib file is still read it's in entirety, but it is released now.
 * - some garbage is read from function names on some very rare occasion
 *
 * --------------------------------------------------------------------------------------
 *  Known problems left from previous implementation (1999, Rein Klazes) :
 *
 * -. Only one format of typelibs is supported
 * -. Data structures are straightforward, but slow for look-ups.
 * -. (related) nothing is hashed
 * -. there are a number of stubs in ITypeLib and ITypeInfo interfaces. Most
 *      of them I don't know yet how to implement them.
 * -. Most error return values are just guessed not checked with windows
 *      behaviour.
 * -. didn't bother with a c++ interface
 * -. lousy fatal error handling
 * -. some methods just return pointers to internal data structures, this is
 *      partly laziness, partly I want to check how windows does it.
 * 
 */
#ifdef __WIN32OS2__
#define HAVE_FLOAT_H
#define WINE_LARGE_INTEGER
#include "oleaut32.h"
#include <win\pe_image.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "winerror.h"
#include "winreg.h"         /* for HKEY_LOCAL_MACHINE */
#include "winnls.h"         /* for PRIMARYLANGID */
#include "ole.h"
#include "heap.h"
#include "wine/obj_base.h"
#include "debugtools.h"
#include "typelib.h"

DEFAULT_DEBUG_CHANNEL(ole);
DECLARE_DEBUG_CHANNEL(typelib);

#ifndef __WIN32OS2__
/****************************************************************************
 *		QueryPathOfRegTypeLib16	[TYPELIB.14]
 *
 * the path is "Classes\Typelib\<guid>\<major>.<minor>\<lcid>\win16\"
 * RETURNS
 *	path of typelib
 */
HRESULT WINAPI
QueryPathOfRegTypeLib16(	
	REFGUID guid,	/* [in] referenced guid */
	WORD wMaj,	/* [in] major version */
	WORD wMin,	/* [in] minor version */
	LCID lcid,	/* [in] locale id */
	LPBSTR16 path	/* [out] path of typelib */
) {
	char	xguid[80];
	char	typelibkey[100],pathname[260];
	DWORD	plen;

       	TRACE("\n");

	if (HIWORD(guid)) {
            sprintf( typelibkey, "SOFTWARE\\Classes\\Typelib\\{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\\%d.%d\\%lx\\win16",
                     guid->Data1, guid->Data2, guid->Data3,
                     guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
                     guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7],
                     wMaj,wMin,lcid);
	} else {
		sprintf(xguid,"<guid 0x%08lx>",(DWORD)guid);
		FIXME("(%s,%d,%d,0x%04lx,%p),can't handle non-string guids.\n",xguid,wMaj,wMin,(DWORD)lcid,path);
		return E_FAIL;
	}
	plen = sizeof(pathname);
	if (RegQueryValueA(HKEY_LOCAL_MACHINE,typelibkey,pathname,&plen)) {
		/* try again without lang specific id */
		if (SUBLANGID(lcid))
			return QueryPathOfRegTypeLib16(guid,wMaj,wMin,PRIMARYLANGID(lcid),path);
		FIXME("key %s not found\n",typelibkey);
		return E_FAIL;
	}
	*path = SysAllocString16(pathname);
	return S_OK;
}
#endif
 
/****************************************************************************
 *		QueryPathOfRegTypeLib	[OLEAUT32.164]
 * RETURNS
 *	path of typelib
 */
HRESULT WINAPI
QueryPathOfRegTypeLib(	
	REFGUID guid,	/* [in] referenced guid */
	WORD wMaj,	/* [in] major version */
	WORD wMin,	/* [in] minor version */
	LCID lcid,	/* [in] locale id */
	LPBSTR path )	/* [out] path of typelib */
{
    /* don't need to ZeroMemory those arrays since sprintf and RegQueryValue add
       string termination character on output strings */

    HRESULT hr        = E_FAIL;

    DWORD   dwPathLen = _MAX_PATH;
    LCID    myLCID    = lcid;

    char    szXGUID[80];
    char    szTypeLibKey[100];
#ifdef __WIN32OS2__
    //since when is this valid C code??????
    char    szPath[_MAX_PATH];
#else
    char    szPath[dwPathLen];
#endif

    if ( !HIWORD(guid) )
    {
        sprintf(szXGUID,
            "<guid 0x%08lx>",
            (DWORD) guid);

        FIXME("(%s,%d,%d,0x%04lx,%p),stub!\n", szXGUID, wMaj, wMin, (DWORD)lcid, path);
        return E_FAIL;
    }

    while (hr != S_OK)
    {
        sprintf(szTypeLibKey,
            "SOFTWARE\\Classes\\Typelib\\{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\\%d.%d\\%lx\\win32",
            guid->Data1,    guid->Data2,    guid->Data3,
            guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3], 
            guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7],
            wMaj,
            wMin,
            myLCID);

        if (RegQueryValueA(HKEY_LOCAL_MACHINE, szTypeLibKey, szPath, &dwPathLen))
        {
            if (!lcid)
                break;
            else if (myLCID == lcid)
            {
                /* try with sub-langid */
                myLCID = SUBLANGID(lcid);
            }
            else if ((myLCID == SUBLANGID(lcid)) && myLCID)
            {
                /* try with system langid */
                myLCID = 0;
            }
            else
            {
                break;
            }
        }
        else
        {
            DWORD len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szPath, dwPathLen, NULL, 0 );
            BSTR bstrPath = SysAllocStringLen(NULL,len);

            MultiByteToWideChar(CP_ACP,
                                MB_PRECOMPOSED,
                                szPath,
                                dwPathLen,
                                bstrPath,
                                len);
           *path = bstrPath;
           hr = S_OK;
        }
    }
	
    if (hr != S_OK)
		TRACE_(typelib)("%s not found\n", szTypeLibKey);
	
    return hr;
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
    FIXME("(%d,%s,%p), stub!\n",syskind,debugstr_w(szFile),ppctlib);
    return E_FAIL;
}
#ifndef __WIN32OS2__
/******************************************************************************
 * LoadTypeLib [TYPELIB.3]  Loads and registers a type library
 * NOTES
 *    Docs: OLECHAR FAR* szFile
 *    Docs: iTypeLib FAR* FAR* pptLib
 *
 * RETURNS
 *    Success: S_OK
 *    Failure: Status
 */
HRESULT WINAPI LoadTypeLib16(
    LPOLESTR szFile, /* [in] Name of file to load from */
    ITypeLib** pptLib) /* [out] Pointer to pointer to loaded type library */
{
    FIXME("('%s',%p): stub\n",debugstr_w((LPWSTR)szFile),pptLib);

    if (pptLib!=0)
      *pptLib=0;

    return E_FAIL;
}
#endif

/******************************************************************************
 *		LoadTypeLib	[OLEAUT32.161]
 * Loads and registers a type library
 * NOTES
 *    Docs: OLECHAR FAR* szFile
 *    Docs: iTypeLib FAR* FAR* pptLib
 *
 * RETURNS
 *    Success: S_OK
 *    Failure: Status
 */
int TLB_ReadTypeLib(PCHAR file, ITypeLib2 **ppTypelib);

HRESULT WINAPI LoadTypeLib(
    const OLECHAR *szFile,/* [in] Name of file to load from */
    ITypeLib * *pptLib)   /* [out] Pointer to pointer to loaded type library */
{
    TRACE("\n");
    return LoadTypeLibEx(szFile, REGKIND_DEFAULT, pptLib);
}

/******************************************************************************
 *		LoadTypeLibEx	[OLEAUT32.183]
 * Loads and optionally registers a type library
 *
 * RETURNS
 *    Success: S_OK
 *    Failure: Status
 */
HRESULT WINAPI LoadTypeLibEx(
    LPCOLESTR szFile,  /* [in] Name of file to load from */
    REGKIND  regkind,  /* [in] Specify kind of registration */
    ITypeLib **pptLib) /* [out] Pointer to pointer to loaded type library */
{
    LPSTR p;
    HRESULT res;
    TRACE("(%s,%d,%p)\n",debugstr_w(szFile), regkind, pptLib);
    
    p=HEAP_strdupWtoA(GetProcessHeap(),0,szFile);
    
    if(regkind != REGKIND_NONE)
        FIXME ("registration of typelibs not supported yet!\n");

    res= TLB_ReadTypeLib(p, (ITypeLib2**)pptLib);
    HeapFree(GetProcessHeap(),0,p);
    TRACE(" returns %08lx\n",res);

    return res;
}

/******************************************************************************
 *		LoadRegTypeLib	[OLEAUT32.162]
 */
HRESULT WINAPI LoadRegTypeLib(	
	REFGUID rguid,		/* [in] referenced guid */
	WORD wVerMajor,		/* [in] major version */
	WORD wVerMinor,		/* [in] minor version */
	LCID lcid,		/* [in] locale id */
	ITypeLib **ppTLib)	/* [out] path of typelib */
{
    BSTR bstr=NULL;
    HRESULT res=QueryPathOfRegTypeLib( rguid, wVerMajor, wVerMinor, lcid, &bstr);

    if(SUCCEEDED(res))
    {
        res= LoadTypeLib(bstr, ppTLib);
        SysFreeString(bstr);
    }

    TRACE("(IID: %s) load %s (%p)\n",debugstr_guid(rguid), SUCCEEDED(res)? "SUCCESS":"FAILED", *ppTLib);

    return res;
}	


/******************************************************************************
 *		RegisterTypeLib	[OLEAUT32.163]
 * Adds information about a type library to the System Registry           
 * NOTES
 *    Docs: ITypeLib FAR * ptlib
 *    Docs: OLECHAR FAR* szFullPath
 *    Docs: OLECHAR FAR* szHelpDir
 *
 * RETURNS
 *    Success: S_OK
 *    Failure: Status
 */
HRESULT WINAPI RegisterTypeLib(
     ITypeLib * ptlib,     /* [in] Pointer to the library*/
     OLECHAR * szFullPath, /* [in] full Path of the library*/
     OLECHAR * szHelpDir)  /* [in] dir to the helpfile for the library,
							 may be NULL*/
{   FIXME("(%p,%s,%s): stub\n",ptlib, debugstr_w(szFullPath),debugstr_w(szHelpDir));
    return S_OK;	/* FIXME: pretend everything is OK */
}


/******************************************************************************
 *	UnRegisterTypeLib	[OLEAUT32.186]
 * Removes information about a type library from the System Registry           
 * NOTES
 *
 * RETURNS
 *    Success: S_OK
 *    Failure: Status
 */
HRESULT WINAPI UnRegisterTypeLib(
    REFGUID libid,	/* [in] Guid of the library */
	WORD wVerMajor,	/* [in] major version */
	WORD wVerMinor,	/* [in] minor version */
	LCID lcid,	/* [in] locale id */
	SYSKIND syskind)
{   
    TRACE("(IID: %s): stub\n",debugstr_guid(libid));
    return S_OK;	/* FIXME: pretend everything is OK */
}

#ifndef __WIN32OS2__
/****************************************************************************
 *	OaBuildVersion				(TYPELIB.15)
 *
 * known TYPELIB.DLL versions:
 *
 * OLE 2.01 no OaBuildVersion() avail	1993	--	---
 * OLE 2.02				1993-94	02     3002
 * OLE 2.03					23	730
 * OLE 2.03					03     3025
 * OLE 2.03 W98 SE orig. file !!	1993-95	10     3024
 * OLE 2.1   NT				1993-95	??	???
 * OLE 2.3.1 W95				23	700
 * OLE2 4.0  NT4SP6			1993-98	40     4277
 */
DWORD WINAPI OaBuildVersion16(void)
{
    /* FIXME: I'd like to return the highest currently known version value
     * in case the user didn't force a --winver, but I don't know how
     * to retrieve the "versionForced" info from misc/version.c :(
     * (this would be useful in other places, too) */
    FIXME("Please report to a.mohr@mailto.de if you get version error messages !\n");
    switch(GetVersion() & 0x8000ffff)  /* mask off build number */
    {
    case 0x80000a03:  /* WIN31 */
		return MAKELONG(3027, 3); /* WfW 3.11 */
    case 0x80000004:  /* WIN95 */
		return MAKELONG(700, 23); /* Win95A */
    case 0x80000a04:  /* WIN98 */
		return MAKELONG(3024, 10); /* W98 SE */
    case 0x00000004:  /* NT4 */
		return MAKELONG(4277, 40); /* NT4 SP6 */
    default:
	FIXME("Version value not known yet. Please investigate it !");
		return 0;
    }
}

/* for better debugging info leave the static out for the time being */
#define static
#endif

/*======================= ITypeLib implementation =======================*/

typedef struct tagTLBCustData 
{
    GUID guid;
    VARIANT data;
    struct tagTLBCustData* next;
} TLBCustData;

/* data structure for import typelibs */
typedef struct tagTLBImpLib
{
    int offset;                 /* offset in the file */
    GUID guid;                  /* libid */
    BSTR name;                  /* name; */

	LCID lcid;					/* lcid of imported typelib */

	WORD wVersionMajor;         /* major version number */
    WORD wVersionMinor;         /* minor version number */

    struct tagITypeLibImpl *pImpTypeLib; /* pointer to loaded typelib */
    struct tagTLBImpLib * next;
} TLBImpLib;

/* internal ITypeLib data */
typedef struct tagITypeLibImpl
{
    ICOM_VFIELD(ITypeLib2);
    UINT ref;
    TLIBATTR LibAttr;            /* guid,lcid,syskind,version,flags */
    
    /* strings can be stored in tlb as multibyte strings BUT they are *always*
	 * exported to the application as a UNICODE string.
	 */
	BSTR Name;
    BSTR DocString;
    BSTR HelpFile;
    BSTR HelpStringDll;
    unsigned long  dwHelpContext;
    int TypeInfoCount;          /* nr of typeinfo's in librarry */
    struct tagITypeInfoImpl *pTypeInfo;   /* linked list of type info data */
    int ctCustData;             /* number of items in cust data list */
    TLBCustData * pCustData;    /* linked list to cust data; */
    TLBImpLib   * pImpLibs;     /* linked list to all imported typelibs */
    TYPEDESC * pTypeDesc;       /* array of TypeDescriptions found in the libary */
} ITypeLibImpl;

static struct ICOM_VTABLE(ITypeLib2) tlbvt;

/* ITypeLib methods */
static ITypeLib2* ITypeLib2_Constructor(LPVOID pLib, DWORD dwTLBLength);

/*======================= ITypeInfo implementation =======================*/

/* data for refernced types in a coclass, or an inherited interface */
typedef struct tagTLBRefType
{
    GUID guid;              /* guid of the referenced type */
                            /* (important if its a imported type) */
    HREFTYPE reference;
    int flags;
    int ctCustData;
    TLBCustData * pCustData;/* linked list to custom data; */
    TLBImpLib *pImpTLInfo;
    struct tagTLBRefType * next;
} TLBRefType;

/* internal Parameter data */
typedef struct tagTLBParDesc
{
    BSTR Name;
    int ctCustData;
    TLBCustData * pCustData;        /* linked list to cust data; */
	TLBRefType * pRefType;			/* linked list to referenced types */
} TLBParDesc;

/* internal Function data */
typedef struct tagTLBFuncDesc
{
    FUNCDESC funcdesc;      /* lots of info on the function and its attributes. */
    BSTR Name;             /* the name of this function */
    TLBParDesc *pParamDesc; /* array with name and custom data */
    int helpcontext;
    int HelpStringContext;
    BSTR HelpString;
    BSTR Entry;            /* if its Hiword==0, it numeric; -1 is not present*/
    int ctCustData;
    TLBCustData * pCustData;        /* linked list to cust data; */
    struct tagTLBFuncDesc * next; 
} TLBFuncDesc;

/* internal Variable data */
typedef struct tagTLBVarDesc
{
    VARDESC vardesc;        /* lots of info on the variable and its attributes. */
    BSTR Name;             /* the name of this variable */
    int HelpContext;
    int HelpStringContext;  /* fixme: where? */
    BSTR HelpString;
    int ctCustData;
    TLBCustData * pCustData;/* linked list to cust data; */
    struct tagTLBVarDesc * next; 
} TLBVarDesc;

/* internal TypeInfo data */
typedef struct tagITypeInfoImpl
{
    ICOM_VFIELD(ITypeInfo2);
    UINT ref;
    TYPEATTR TypeAttr ;         /* _lots_ of type information. */
    ITypeLibImpl * pTypeLib;        /* back pointer to typelib */
    int index;                  /* index in this typelib; */
    /* type libs seem to store the doc strings in ascii 
     * so why should we do it in unicode?
     */
    BSTR Name;
    BSTR DocString;
    unsigned long  dwHelpContext;
    unsigned long  dwHelpStringContext;

    /* functions  */
    TLBFuncDesc * funclist;     /* linked list with function descriptions */

    /* variables  */
    TLBVarDesc * varlist;       /* linked list with variable descriptions */

    /* Implemented Interfaces  */
    TLBRefType * impltypelist;      
    int ctCustData;
    TLBCustData * pCustData;        /* linked list to cust data; */
    struct tagITypeInfoImpl * next;
} ITypeInfoImpl;

static struct ICOM_VTABLE(ITypeInfo2) tinfvt;

static ITypeInfo2 * WINAPI ITypeInfo_Constructor();

typedef struct tagTLBContext
{
	unsigned int oStart;  /* start of TLB in file */
	unsigned int pos;     /* current pos */
	unsigned int length;  /* total length */
	void *mapping;        /* memory mapping */
	TLBSegDir * pTblDir;
	ITypeLibImpl* pLibInfo;
} TLBContext;


static void TLB_DoRefType(TLBContext *pcx, int offset, TLBRefType ** pprtd);

/*
 debug
*/
static void dump_TLBFuncDesc(TLBFuncDesc * pfd)
{
	while (pfd)
	{
	  TRACE("%s(%u)\n", debugstr_w(pfd->Name), pfd->funcdesc.cParams);
	  pfd = pfd->next;
	};
}
static void dump_TLBVarDesc(TLBVarDesc * pvd)
{
	while (pvd)
	{
	  TRACE("%s\n", debugstr_w(pvd->Name));
	  pvd = pvd->next;
	};
}
static void dump_TLBRefType(TLBRefType * prt)
{
	while (prt)
	{
	  TRACE("%s\n", debugstr_guid(&(prt->guid)));
	  TRACE("  href:0x%08lx\n", prt->reference);
	  prt = prt->next;
	};
}

static void dump_Variant(VARIANT * pvar)
{
	TRACE("%p %x\n", pvar, pvar?pvar->vt:0 );
	if(!pvar) return;

	if (pvar->vt & VT_BYREF)
	  return dump_Variant(pvar->u.pvarVal);
}

static void dump_DispParms(DISPPARAMS * pdp)
{
	dump_Variant( pdp->rgvarg);
	TRACE("args=%u named args=%u\n", pdp->cArgs, pdp->cNamedArgs);
}

static char * typekind_desc[] =
{
	"TKIND_ENUM",
	"TKIND_RECORD",
	"TKIND_MODULE",
	"TKIND_INTERFACE",
	"TKIND_DISPATCH",
	"TKIND_COCLASS",
	"TKIND_ALIAS",
	"TKIND_UNION",
	"TKIND_MAX"
};

static void dump_TypeInfo(ITypeInfoImpl * pty)
{
    TRACE("%p ref=%u\n", pty, pty->ref);
    TRACE("attr:%s\n", debugstr_guid(&(pty->TypeAttr.guid)));
    TRACE("kind:%s\n", typekind_desc[pty->TypeAttr.typekind]);
    TRACE("fct:%u var:%u impl:%u\n",
      pty->TypeAttr.cFuncs, pty->TypeAttr.cVars, pty->TypeAttr.cImplTypes);
    TRACE("parent tlb:%p index in TLB:%u\n",pty->pTypeLib, pty->index);
    TRACE("%s %s\n", debugstr_w(pty->Name), debugstr_w(pty->DocString));
    dump_TLBFuncDesc(pty->funclist);
    dump_TLBVarDesc(pty->varlist);
    dump_TLBRefType(pty->impltypelist);
}

static TYPEDESC stndTypeDesc[VT_LPWSTR+1]=
{
    /* VT_LPWSTR is largest type that */
    /* may appear in type description*/
    {{0}, 0},{{0}, 1},{{0}, 2},{{0}, 3},{{0}, 4},
    {{0}, 5},{{0}, 6},{{0}, 7},{{0}, 8},{{0}, 9},
    {{0},10},{{0},11},{{0},12},{{0},13},{{0},14},
    {{0},15},{{0},16},{{0},17},{{0},18},{{0},19},
    {{0},20},{{0},21},{{0},22},{{0},23},{{0},24},
    {{0},25},{{0},26},{{0},27},{{0},28},{{0},29},
    {{0},30},{{0},31}
};

static void TLB_abort()
{
    DebugBreak();
}
static void * TLB_Alloc(unsigned size)
{
    void * ret;
    if((ret=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size))==NULL){
        /* FIXME */
        ERR("cannot allocate memory\n");
    }
    return ret;
}

static void TLB_Free(void * ptr)
{
    HeapFree(GetProcessHeap(), 0, ptr);
}
/* read function */
DWORD TLB_Read(void *buffer,  DWORD count, TLBContext *pcx, long where )
{
    TRACE("pos=0x%08x len=0x%08lx 0x%08x 0x%08x 0x%08lx\n",
       pcx->pos, count, pcx->oStart, pcx->length, where);

    if (where != DO_NOT_SEEK)
    {
        where += pcx->oStart;
        if (where > pcx->length)
        {
            /* FIXME */
            ERR("seek beyond end (%ld/%d)\n", where, pcx->length );
            TLB_abort();
        }
        pcx->pos = where;
    }
    if (pcx->pos + count > pcx->length) count = pcx->length - pcx->pos;
    memcpy( buffer, (char *)pcx->mapping + pcx->pos, count );
    pcx->pos += count;
    return count;
}

static void TLB_ReadGuid( GUID *pGuid, int offset, TLBContext *pcx)
{
    TRACE("%s\n", debugstr_guid(pGuid));

    if(offset<0 || pcx->pTblDir->pGuidTab.offset <0){
        memset(pGuid,0, sizeof(GUID));
        return;
    }
    TLB_Read(pGuid, sizeof(GUID), pcx, pcx->pTblDir->pGuidTab.offset+offset );
}

BSTR TLB_ReadName( TLBContext *pcx, int offset)
{
    char * name;
    TLBNameIntro niName;
    int lengthInChars;
    WCHAR* pwstring = NULL;
    BSTR bstrName = NULL;

    TLB_Read(&niName, sizeof(niName), pcx,
				pcx->pTblDir->pNametab.offset+offset);
    niName.namelen &= 0xFF; /* FIXME: correct ? */
    name=TLB_Alloc((niName.namelen & 0xff) +1);
    TLB_Read(name, (niName.namelen & 0xff), pcx, DO_NOT_SEEK);
    name[niName.namelen & 0xff]='\0';

    lengthInChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                                        name, -1, NULL, 0);

    /* no invalid characters in string */
    if (lengthInChars)
    {
        pwstring = HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR)*lengthInChars);

        /* don't check for invalid character since this has been done previously */
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, -1, pwstring, lengthInChars);

        bstrName = SysAllocStringLen(pwstring, lengthInChars);
        lengthInChars = SysStringLen(bstrName);
        HeapFree(GetProcessHeap(), 0, pwstring);
    }

    TRACE("%s %d\n", debugstr_w(bstrName), lengthInChars);
    return bstrName;
}

BSTR TLB_ReadString( TLBContext *pcx, int offset)
{
    char * string;
    INT16 length;
    int lengthInChars;
    BSTR bstr = NULL;

    if(offset<0) return NULL;
    TLB_Read(&length, sizeof(INT16), pcx, pcx->pTblDir->pStringtab.offset+offset);
    if(length <= 0) return 0;
    string=TLB_Alloc(length +1);
    TLB_Read(string, length, pcx, DO_NOT_SEEK);
    string[length]='\0';

    lengthInChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
                                        string, -1, NULL, 0);

    /* no invalid characters in string */
    if (lengthInChars)
    {
        WCHAR* pwstring = HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR)*lengthInChars);

        /* don't check for invalid character since this has been done previously */
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string, -1, pwstring, lengthInChars);

        bstr = SysAllocStringLen(pwstring, lengthInChars);
        lengthInChars = SysStringLen(bstr);
        HeapFree(GetProcessHeap(), 0, pwstring);
    }

    TRACE("%s %d\n", debugstr_w(bstr), lengthInChars);
    return bstr;
}
/*
 * read a value and fill a VARIANT structure 
 */
static void TLB_ReadValue( VARIANT * pVar, int offset, TLBContext *pcx )
{
    int size;

    TRACE("\n");

    if(offset <0) { /* data is packed in here */
        pVar->vt = (offset & 0x7c000000 )>> 26;
        V_UNION(pVar, iVal) = offset & 0xffff;
        return;
    }
    TLB_Read(&(pVar->vt), sizeof(VARTYPE), pcx, 
        pcx->pTblDir->pCustData.offset + offset );
    TRACE("Vartype = %x\n", pVar->vt);
    switch(pVar->vt){
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
            size=4; break;
        case VT_R8  :
        case VT_CY  :
        case VT_DATE    : 
        case VT_I8  : 
        case VT_UI8 : 
        case VT_DECIMAL :  /* FIXME: is this right? */
        case VT_FILETIME :
            size=8;break;
            /* pointer types with known behaviour */
        case VT_BSTR    :{
            char * ptr;
            TLB_Read(&size, sizeof(INT), pcx, DO_NOT_SEEK );
	    if(size <= 0) {
	        FIXME("BSTR length = %d?\n", size);
	    } else {
                ptr=TLB_Alloc(size);/* allocate temp buffer */
		TLB_Read(ptr, size, pcx, DO_NOT_SEEK); /* read string (ANSI) */
		V_UNION(pVar, bstrVal)=SysAllocStringLen(NULL,size);
		/* FIXME: do we need a AtoW conversion here? */
		V_UNION(pVar, bstrVal[size])=L'\0';
		while(size--) V_UNION(pVar, bstrVal[size])=ptr[size];
		TLB_Free(ptr);
	    }
	}
	size=-4; break;
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
            size=0; 
            FIXME("VARTYPE %d is not supported, setting pointer to NULL\n",
                pVar->vt);
    }

    if(size>0) /* (big|small) endian correct? */
        TLB_Read(&(V_UNION(pVar, iVal)), size, pcx, DO_NOT_SEEK );
    return;
}
/*
 * create a linked list with custom data
 */
static int TLB_CustData( TLBContext *pcx, int offset, TLBCustData** ppCustData )
{
    TLBCDGuid entry;
    TLBCustData* pNew;
    int count=0;

    TRACE("\n");

    while(offset >=0){
        count++;
        pNew=TLB_Alloc(sizeof(TLBCustData));
        TLB_Read(&entry, sizeof(entry), pcx, 
            pcx->pTblDir->pCDGuids.offset+offset);
        TLB_ReadGuid(&(pNew->guid), entry.GuidOffset , pcx);
        TLB_ReadValue(&(pNew->data), entry.DataOffset, pcx);
        /* add new custom data at head of the list */
        pNew->next=*ppCustData;
        *ppCustData=pNew;
        offset = entry.next;
    }
    return count;
}

static void TLB_GetTdesc(TLBContext *pcx, INT type,TYPEDESC * pTd )
{
    if(type <0)
        pTd->vt=type & VT_TYPEMASK;
    else
        *pTd=pcx->pLibInfo->pTypeDesc[type/(2*sizeof(INT))];

    TRACE("vt type = %X\n", pTd->vt);
}
static void TLB_DoFuncs(TLBContext *pcx, int cFuncs, int cVars,
                          int offset, TLBFuncDesc ** pptfd)
{
    /* 
     * member information is stored in a data structure at offset
     * indicated by the memoffset field of the typeinfo structure
     * There are several distinctive parts.
     * the first part starts with a field that holds the total length 
     * of this (first) part excluding this field. Then follow the records,
     * for each member there is one record.
     *
     * First entry is always the length of the record (excluding this
     * length word). 
     * Rest of the record depends on the type of the member. If there is 
     * a field indicating the member type (function variable intereface etc)
     * I have not found it yet. At this time we depend on the information
     * in the type info and the usual order how things are stored.
     *
     * Second follows an array sized nrMEM*sizeof(INT) with a memeber id
     * for each member;
     * 
     * Third is a equal sized array with file offsets to the name entry 
     * of each member.
     * 
     * Forth and last (?) part is an array with offsets to the records in the
     * first part of this file segment.
     */

    int infolen, nameoffset, reclength, nrattributes;
    char recbuf[512];
    TLBFuncRecord * pFuncRec=(TLBFuncRecord *) recbuf;
    int i, j;
    int recoffset=offset+sizeof(INT);

    TRACE("\n");

    TLB_Read(&infolen,sizeof(INT), pcx, offset);
    for(i=0;i<cFuncs;i++){
        *pptfd=TLB_Alloc(sizeof(TLBFuncDesc));
    /* name, eventually add to a hash table */
        TLB_Read(&nameoffset, sizeof(INT), pcx, 
            offset + infolen + (cFuncs + cVars + i + 1) * sizeof(INT));
        (*pptfd)->Name=TLB_ReadName(pcx, nameoffset);
    /* read the function information record */
        TLB_Read(&reclength, sizeof(INT), pcx, recoffset);
        reclength &=0x1ff;
        TLB_Read(pFuncRec, reclength - sizeof(INT), pcx, DO_NOT_SEEK) ; 
    /* do the attributes */
        nrattributes=(reclength-pFuncRec->nrargs*3*sizeof(int)-0x18)
            /sizeof(int);
        if(nrattributes>0){
            (*pptfd)->helpcontext = pFuncRec->OptAttr[0] ;
            if(nrattributes>1){
                (*pptfd)->HelpString = TLB_ReadString(pcx,
                                                      pFuncRec->OptAttr[1]) ;
                if(nrattributes>2){
                    if(pFuncRec->FKCCIC & 0x2000)
                        (*pptfd)->Entry = (WCHAR*) pFuncRec->OptAttr[2] ;
                    else
                        (*pptfd)->Entry = TLB_ReadString(pcx,
                                                         pFuncRec->OptAttr[2]);
                    if(nrattributes>5 )
                        (*pptfd)->HelpStringContext = pFuncRec->OptAttr[5] ;
                    if(nrattributes>6 && pFuncRec->FKCCIC & 0x80){
                        TLB_CustData(pcx, pFuncRec->OptAttr[6],
                                &(*pptfd)->pCustData);
                    }
                }
            }
        }
    /* fill the FuncDesc Structure */
        TLB_Read(&(*pptfd)->funcdesc.memid, sizeof(INT), pcx, 
            offset + infolen + ( i + 1) * sizeof(INT));
        (*pptfd)->funcdesc.funckind = (pFuncRec->FKCCIC) & 0x7;
        (*pptfd)->funcdesc.invkind = ((pFuncRec->FKCCIC) >>3) & 0xF;
        (*pptfd)->funcdesc.callconv = (pFuncRec->FKCCIC) >>8 & 0xF;
        (*pptfd)->funcdesc.cParams = pFuncRec->nrargs ;
        (*pptfd)->funcdesc.cParamsOpt = pFuncRec->nroargs ;
        (*pptfd)->funcdesc.oVft = pFuncRec->VtableOffset ;
        (*pptfd)->funcdesc.wFuncFlags = LOWORD(pFuncRec->Flags)  ;
        TLB_GetTdesc(pcx, pFuncRec->DataType,   
            &(*pptfd)->funcdesc.elemdescFunc.tdesc) ;
                                    
        /* do the parameters/arguments */
        if(pFuncRec->nrargs){
            TLBParameterInfo paraminfo;
            (*pptfd)->funcdesc.lprgelemdescParam=
                TLB_Alloc(pFuncRec->nrargs * sizeof(ELEMDESC));
            (*pptfd)->pParamDesc=TLB_Alloc(pFuncRec->nrargs *
                sizeof(TLBParDesc));

            TLB_Read(&paraminfo,sizeof(paraminfo), pcx, recoffset+reclength -
                pFuncRec->nrargs * sizeof(TLBParameterInfo));
            for(j=0;j<pFuncRec->nrargs;j++){
				TYPEDESC* lpArgTypeDesc = 0;
				
                TLB_GetTdesc(pcx, paraminfo.DataType,   
                    &(*pptfd)->funcdesc.lprgelemdescParam[j].tdesc) ;
                V_UNION(&((*pptfd)->funcdesc.lprgelemdescParam[j]),
                    paramdesc.wParamFlags) = paraminfo.Flags;
                (*pptfd)->pParamDesc[j].Name=(void *)paraminfo.oName;

				/* SEEK value = jump to offset, from there jump to the end of record,
				 * go back by (j-1) arguments
				 */
				TLB_Read(&paraminfo,sizeof(TLBParameterInfo), pcx,
                        recoffset+reclength - 
						((pFuncRec->nrargs - j - 1) * sizeof(TLBParameterInfo)));

                lpArgTypeDesc = & ((*pptfd)->funcdesc.lprgelemdescParam[j].tdesc) ;

                while (lpArgTypeDesc != NULL)
                {
                   switch (lpArgTypeDesc->vt)
                   {
                     case VT_PTR:
                       lpArgTypeDesc = lpArgTypeDesc->u.lptdesc;
                       break;
                     
                     case VT_CARRAY:
                       lpArgTypeDesc = & (lpArgTypeDesc->u.lpadesc->tdescElem);
                       break;
               
                     case VT_USERDEFINED:
                       (*pptfd)->pParamDesc[j].pRefType = TLB_Alloc(sizeof(TLBRefType));
                       TLB_DoRefType(pcx, lpArgTypeDesc->u.hreftype, &((*pptfd)->pParamDesc[j].pRefType));
                       lpArgTypeDesc = NULL;
                       break;
                     
                     default:
                       lpArgTypeDesc = NULL;
                   }
                }
            }
            /* second time around */
            for(j=0;j<pFuncRec->nrargs;j++){
                /* name */
                (*pptfd)->pParamDesc[j].Name=
                    TLB_ReadName(pcx, (int)(*pptfd)->pParamDesc[j].Name);
                /* default value */
                if((PARAMFLAG_FHASDEFAULT & V_UNION(&((*pptfd)->funcdesc.
                    lprgelemdescParam[j]),paramdesc.wParamFlags)) &&
                    ((pFuncRec->FKCCIC) & 0x1000)){
                    INT *pInt=(INT *)((char *)pFuncRec + reclength -
                        (pFuncRec->nrargs * 4 + 1) * sizeof(INT) );
                    PARAMDESC * pParamDesc= &V_UNION(&((*pptfd)->funcdesc.
                        lprgelemdescParam[j]),paramdesc);
                    pParamDesc->pparamdescex = TLB_Alloc(sizeof(PARAMDESCEX));
                    pParamDesc->pparamdescex->cBytes= sizeof(PARAMDESCEX);
                    TLB_ReadValue(&(pParamDesc->pparamdescex->varDefaultValue), 
                        pInt[j], pcx);
                }
                /* custom info */
                if(nrattributes>7+j && pFuncRec->FKCCIC & 0x80)
                    TLB_CustData(pcx, pFuncRec->OptAttr[7+j],
									&(*pptfd)->pParamDesc[j].pCustData);
            }
        }
    /* scode is not used: archaic win16 stuff FIXME: right? */
        (*pptfd)->funcdesc.cScodes = 0 ;
        (*pptfd)->funcdesc.lprgscode = NULL ;
        pptfd=&((*pptfd)->next);
        recoffset += reclength;
    }
}
static void TLB_DoVars(TLBContext *pcx, int cFuncs, int cVars,
                          int offset, TLBVarDesc ** pptvd)
{
    int infolen, nameoffset, reclength;
    char recbuf[256];
    TLBVarRecord * pVarRec=(TLBVarRecord *) recbuf;
    int i;
    int recoffset;

    TRACE("\n");

    TLB_Read(&infolen,sizeof(INT), pcx, offset);
    TLB_Read(&recoffset,sizeof(INT), pcx, offset + infolen + 
        ((cFuncs+cVars)*2+cFuncs + 1)*sizeof(INT));
    recoffset += offset+sizeof(INT);
    for(i=0;i<cVars;i++){
        *pptvd=TLB_Alloc(sizeof(TLBVarDesc));
    /* name, eventually add to a hash table */
        TLB_Read(&nameoffset, sizeof(INT), pcx, 
            offset + infolen + (cFuncs + cVars + i + 1) * sizeof(INT));
        (*pptvd)->Name=TLB_ReadName(pcx, nameoffset);
    /* read the variable information record */
        TLB_Read(&reclength, sizeof(INT), pcx, recoffset);
        reclength &=0xff;
        TLB_Read(pVarRec, reclength - sizeof(INT), pcx, DO_NOT_SEEK) ; 
    /* Optional data */
        if(reclength >(6*sizeof(INT)) )
            (*pptvd)->HelpContext=pVarRec->HelpContext;
        if(reclength >(7*sizeof(INT)) )
            (*pptvd)->HelpString = TLB_ReadString(pcx, pVarRec->oHelpString) ;
        if(reclength >(8*sizeof(INT)) )
        if(reclength >(9*sizeof(INT)) )
            (*pptvd)->HelpStringContext=pVarRec->HelpStringContext;
    /* fill the VarDesc Structure */
        TLB_Read(&(*pptvd)->vardesc.memid, sizeof(INT), pcx, 
            offset + infolen + ( i + 1) * sizeof(INT));
        (*pptvd)->vardesc.varkind = pVarRec->VarKind;
        (*pptvd)->vardesc.wVarFlags = pVarRec->Flags;
        TLB_GetTdesc(pcx, pVarRec->DataType,    
            &(*pptvd)->vardesc.elemdescVar.tdesc) ;
/*   (*pptvd)->vardesc.lpstrSchema; is reserved (SDK) fixme?? */
        if(pVarRec->VarKind == VAR_CONST ){
            V_UNION(&((*pptvd)->vardesc),lpvarValue)=TLB_Alloc(sizeof(VARIANT));
            TLB_ReadValue(V_UNION(&((*pptvd)->vardesc),lpvarValue), 
                pVarRec->OffsValue, pcx);
        }else
            V_UNION(&((*pptvd)->vardesc),oInst)=pVarRec->OffsValue;
        pptvd=&((*pptvd)->next);
        recoffset += reclength;
    }
}
/* fill in data for a hreftype (offset). When the refernced type is contained
 * in the typelib, its just an (file) offset in the type info base dir.
 * If comes from import, its an offset+1 in the ImpInfo table
 * */
static void TLB_DoRefType(TLBContext *pcx, 
                          int offset, TLBRefType ** pprtd)
{
    int j;

	TRACE("TLB context %p, TLB offset %x\n", pcx, offset);

    if(!HREFTYPE_INTHISFILE( offset)) {
        /* external typelib */
        TLBImpInfo impinfo;
        TLBImpLib *pImpLib=(pcx->pLibInfo->pImpLibs);

        TRACE("offset %x, masked offset %x\n", offset, offset + (offset & 0xfffffffc));
		
        TLB_Read(&impinfo, sizeof(impinfo), pcx, 
            pcx->pTblDir->pImpInfo.offset + (offset & 0xfffffffc));
        for(j=0;pImpLib;j++){   /* search the known offsets of all import libraries */
            if(pImpLib->offset==impinfo.oImpFile) break;
            pImpLib=pImpLib->next;
        }
        if(pImpLib){
            (*pprtd)->reference=offset;
            (*pprtd)->pImpTLInfo = pImpLib;
            TLB_ReadGuid(&(*pprtd)->guid, impinfo.oGuid, pcx);
        }else{
            ERR("Cannot find a reference\n");
            (*pprtd)->reference=-1;
            (*pprtd)->pImpTLInfo=(void *)-1;
        }
    }else{
        /* in this typelib */
        (*pprtd)->reference=offset;
        (*pprtd)->pImpTLInfo=(void *)-2;
    }
}

/* process Implemented Interfaces of a com class */
static void TLB_DoImplTypes(TLBContext *pcx, int count,
                          int offset, TLBRefType ** pprtd)
{
    int i;
    TLBRefRecord refrec;

    TRACE("\n");

    for(i=0;i<count;i++){
        if(offset<0) break; /* paranoia */
        *pprtd=TLB_Alloc(sizeof(TLBRefType));
        TLB_Read(&refrec,sizeof(refrec),pcx,offset+pcx->pTblDir->pRefTab.offset);
        TLB_DoRefType(pcx, refrec.reftype, pprtd);
        (*pprtd)->flags=refrec.flags;
        (*pprtd)->ctCustData=
            TLB_CustData(pcx, refrec.oCustData, &(*pprtd)->pCustData);
        offset=refrec.onext;
        pprtd=&((*pprtd)->next);
    }
}
/*
 * process a typeinfo record
 */
ITypeInfoImpl * TLB_DoTypeInfo(
	TLBContext *pcx,
	int count,
	ITypeLibImpl * pLibInfo)
{
    TLBTypeInfoBase tiBase;
    ITypeInfoImpl *ptiRet;

    TRACE("count=%u\n", count);

    ptiRet = (ITypeInfoImpl*) ITypeInfo_Constructor();
    TLB_Read(&tiBase, sizeof(tiBase) ,pcx ,
        pcx->pTblDir->pTypeInfoTab.offset+count*sizeof(tiBase));
/* this where we are coming from */
    ptiRet->pTypeLib = pLibInfo;
    ptiRet->index=count;
/* fill in the typeattr fields */
    TLB_ReadGuid(&ptiRet->TypeAttr.guid, tiBase.posguid, pcx);
    ptiRet->TypeAttr.lcid=pLibInfo->LibAttr.lcid;   /* FIXME: correct? */
    ptiRet->TypeAttr.memidConstructor=MEMBERID_NIL ;/* FIXME */
    ptiRet->TypeAttr.memidDestructor=MEMBERID_NIL ; /* FIXME */
    ptiRet->TypeAttr.lpstrSchema=NULL;              /* reserved */
    ptiRet->TypeAttr.cbSizeInstance=tiBase.size;
    ptiRet->TypeAttr.typekind=tiBase.typekind & 0xF;
    ptiRet->TypeAttr.cFuncs=LOWORD(tiBase.cElement);
    ptiRet->TypeAttr.cVars=HIWORD(tiBase.cElement);
    ptiRet->TypeAttr.cbAlignment=(tiBase.typekind >> 11 )& 0x1F; /* there are more flags there */
    ptiRet->TypeAttr.wTypeFlags=tiBase.flags;
    ptiRet->TypeAttr.wMajorVerNum=LOWORD(tiBase.version);
    ptiRet->TypeAttr.wMinorVerNum=HIWORD(tiBase.version);
    ptiRet->TypeAttr.cImplTypes=tiBase.cImplTypes;
    ptiRet->TypeAttr.cbSizeVft=tiBase.cbSizeVft; /* FIXME: this is only the non inherited part */
    if(ptiRet->TypeAttr.typekind == TKIND_ALIAS)
        TLB_GetTdesc(pcx, tiBase.datatype1, 
            &ptiRet->TypeAttr.tdescAlias) ;

/*  FIXME: */
/*    IDLDESC  idldescType; *//* never saw this one != zero  */

/* name, eventually add to a hash table */
    ptiRet->Name=TLB_ReadName(pcx, tiBase.NameOffset);
    TRACE("reading %s\n", debugstr_w(ptiRet->Name));
    /* help info */
    ptiRet->DocString=TLB_ReadString(pcx, tiBase.docstringoffs);
    ptiRet->dwHelpStringContext=tiBase.helpstringcontext;
    ptiRet->dwHelpContext=tiBase.helpcontext;
/* note: InfoType's Help file and HelpStringDll come from the containing
 * library. Further HelpString and Docstring appear to be the same thing :(
 */
    /* functions */
    if(ptiRet->TypeAttr.cFuncs >0 )
        TLB_DoFuncs(pcx, ptiRet->TypeAttr.cFuncs ,ptiRet->TypeAttr.cVars, 
        tiBase.memoffset, & ptiRet->funclist);
    /* variables */
    if(ptiRet->TypeAttr.cVars >0 )
        TLB_DoVars(pcx, ptiRet->TypeAttr.cFuncs ,ptiRet->TypeAttr.cVars, 
        tiBase.memoffset, & ptiRet->varlist);
    if(ptiRet->TypeAttr.cImplTypes >0 ){
        switch(ptiRet->TypeAttr.typekind)
		{
		case TKIND_COCLASS:
            TLB_DoImplTypes(pcx, ptiRet->TypeAttr.cImplTypes , 
                tiBase.datatype1, & ptiRet->impltypelist);
			break;
		case TKIND_DISPATCH:
            ptiRet->impltypelist=TLB_Alloc(sizeof(TLBRefType));
            
            if (tiBase.datatype1 != -1)
            {
              TLB_DoRefType(pcx, tiBase.datatype1, & ptiRet->impltypelist);
            }
            else
            {
              char* szStdOle     = "stdole2.tlb\0";
              int   nStdOleLen = strlen(szStdOle);
                            
              ptiRet->impltypelist->guid             = IID_IDispatch;
              ptiRet->impltypelist->reference        = -1;
              ptiRet->impltypelist->pImpTLInfo       = TLB_Alloc(sizeof(TLBImpLib));
              ptiRet->impltypelist->pImpTLInfo->guid = IID_StdOle;             
              ptiRet->impltypelist->pImpTLInfo->name = SysAllocStringLen(NULL, nStdOleLen  + 1);
              
              MultiByteToWideChar(CP_ACP,
                                  MB_PRECOMPOSED,
                                  szStdOle,
                                  -1,
                                  ptiRet->impltypelist->pImpTLInfo->name,
                                  SysStringLen(ptiRet->impltypelist->pImpTLInfo->name));
                                  
              ptiRet->impltypelist->pImpTLInfo->lcid          = 0;
              ptiRet->impltypelist->pImpTLInfo->wVersionMajor = 2;
              ptiRet->impltypelist->pImpTLInfo->wVersionMinor = 0;
            }
			break;
		default:
			ptiRet->impltypelist=TLB_Alloc(sizeof(TLBRefType));
            TLB_DoRefType(pcx, tiBase.datatype1, & ptiRet->impltypelist);
			break;
		}
    }
    ptiRet->ctCustData=
        TLB_CustData(pcx, tiBase.oCustData, &ptiRet->pCustData);

    TRACE("%s guid: %s kind:%s\n",
       debugstr_w(ptiRet->Name),
       debugstr_guid(&ptiRet->TypeAttr.guid),
       typekind_desc[ptiRet->TypeAttr.typekind]);

    return ptiRet;
}

/****************************************************************************
 *	TLB_ReadTypeLib
 *
 * find the type of the typelib file and map the typelib resource into
 * the memory
 */
#define MSFT_SIGNATURE 0x5446534D /* "MSFT" */
int TLB_ReadTypeLib(LPSTR pszFileName, ITypeLib2 **ppTypeLib)
{
    int ret = E_FAIL;
    DWORD dwSignature = 0;
    HFILE hFile;
	int nStrLen = strlen(pszFileName);
	int i;

	PCHAR pszTypeLibIndex = NULL;
    PCHAR pszDllName      = NULL;

    TRACE("%s\n", pszFileName);

    *ppTypeLib = NULL;

    /* is it a DLL? */
	for (i=0 ; i < nStrLen ; ++i)
	{
	    pszFileName[i] = tolower(pszFileName[i]);
	}
    pszTypeLibIndex = strstr(pszFileName, ".dll");

    /* find if there's a back-slash after .DLL (good sign of the presence of a typelib index) */
    if (pszTypeLibIndex)
    {
      pszTypeLibIndex = strstr(pszTypeLibIndex, "\\");
    }

    /* is there any thing after trailing back-slash  ? */
    if (pszTypeLibIndex && pszTypeLibIndex < pszFileName + nStrLen)
    {
      /* yes -> it's a index! store DLL name, without the trailing back-slash */
      size_t nMemToAlloc = pszTypeLibIndex - pszFileName;
      
      pszDllName = HeapAlloc(GetProcessHeap(),
                          HEAP_ZERO_MEMORY, 
                          nMemToAlloc + 1);
                          
      strncpy(pszDllName, pszFileName, nMemToAlloc);
      
      /* move index string pointer pass the backslash */
      while (*pszTypeLibIndex == '\\')
        ++pszTypeLibIndex;
    }
    else
    {
      /* No index, reset variable to 1 */
      pszDllName = HeapAlloc(GetProcessHeap(),
                          HEAP_ZERO_MEMORY, 
                          nStrLen + 1);
                          
      strncpy(pszDllName, pszFileName, nStrLen);
      
      pszTypeLibIndex = "1\0";
    }

    TRACE("File name without index %s\n", pszDllName);
    TRACE("Index of typelib %s\n",        pszTypeLibIndex);


    /* check the signature of the file */
    hFile = CreateFileA( pszDllName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, -1 );
    if (INVALID_HANDLE_VALUE != hFile)
    {
      HANDLE hMapping = CreateFileMappingA( hFile, NULL, PAGE_READONLY | SEC_COMMIT, 0, 0, NULL );
      if (hMapping)
      {
        LPVOID pBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	if(pBase)
	{
	  /* first try to load as *.tlb */
	  dwSignature = *((DWORD*) pBase);
	  if ( dwSignature == MSFT_SIGNATURE)
	  {
            /* retrieve file size */
            DWORD dwTLBLength = GetFileSize(hFile, NULL);

	    *ppTypeLib = ITypeLib2_Constructor(pBase, dwTLBLength);
	  }
	  UnmapViewOfFile(pBase);
	}
	CloseHandle(hMapping);
      }
      CloseHandle(hFile);
    }

    if( (WORD)dwSignature == IMAGE_DOS_SIGNATURE )
    {
      /* find the typelibrary resource*/
      HINSTANCE hinstDLL = LoadLibraryExA(pszDllName, 0, DONT_RESOLVE_DLL_REFERENCES|
                                          LOAD_LIBRARY_AS_DATAFILE|LOAD_WITH_ALTERED_SEARCH_PATH);
      if (hinstDLL)
      {
        HRSRC hrsrc = FindResourceA(hinstDLL, MAKEINTRESOURCEA(atoi(pszTypeLibIndex)), "TYPELIB");
        if (hrsrc)
        {
          HGLOBAL hGlobal = LoadResource(hinstDLL, hrsrc);
          if (hGlobal)
          {
            LPVOID pBase = LockResource(hGlobal);
            DWORD  dwTLBLength = SizeofResource(hinstDLL, hrsrc);
            
            if (pBase)
            {
	      /* try to load as incore resource */
	      dwSignature = *((DWORD*) pBase);
	      if ( dwSignature == MSFT_SIGNATURE)
		  *ppTypeLib = ITypeLib2_Constructor(pBase, dwTLBLength);
	      else
	        FIXME("Header type magic 0x%08lx not supported.\n",dwSignature);
            }
            FreeResource( hGlobal );
          }
        }
        FreeLibrary(hinstDLL);
      }
    }

	HeapFree(GetProcessHeap(), 0, pszDllName);

    if(*ppTypeLib)
      ret = S_OK;
    else
      ERR("Loading of typelib %s failed with error 0x%08lx\n", pszFileName, GetLastError());

    return ret;
}

/*================== ITypeLib(2) Methods ===================================*/

/****************************************************************************
 *	ITypeLib2_Constructor
 *
 * loading a typelib from a in-memory image
 */
static ITypeLib2* ITypeLib2_Constructor(LPVOID pLib, DWORD dwTLBLength)
{
    TLBContext cx;
    long lPSegDir;
    TLB2Header tlbHeader;
    TLBSegDir tlbSegDir;
    ITypeLibImpl * pTypeLibImpl;

    TRACE("%p, TLB length = %ld\n", pLib, dwTLBLength);

    pTypeLibImpl = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(ITypeLibImpl));
    if (!pTypeLibImpl) return NULL;

    ICOM_VTBL(pTypeLibImpl) = &tlbvt;
    pTypeLibImpl->ref = 1;

    /* get pointer to beginning of typelib data */
    cx.pos = 0;
    cx.oStart=0;
    cx.mapping = pLib;
    cx.pLibInfo = pTypeLibImpl;
    cx.length = dwTLBLength;
    
    /* read header */
    TLB_Read((void*)&tlbHeader, sizeof(tlbHeader), &cx, 0);
    TRACE("header:\n");
    TRACE("\tmagic1=0x%08x ,magic2=0x%08x\n",tlbHeader.magic1,tlbHeader.magic2 );
    if (memcmp(&tlbHeader.magic1,TLBMAGIC2,4)) {
	FIXME("Header type magic 0x%08x not supported.\n",tlbHeader.magic1);
	return NULL;
    }
    /* there is a small number of information here until the next important
     * part:
     * the segment directory . Try to calculate the amount of data */
    lPSegDir = sizeof(tlbHeader) + (tlbHeader.nrtypeinfos)*4 + ((tlbHeader.varflags & HELPDLLFLAG)? 4 :0);

    /* now read the segment directory */
    TRACE("read segment directory (at %ld)\n",lPSegDir);
    TLB_Read((void*)&tlbSegDir, sizeof(tlbSegDir), &cx, lPSegDir);  
    cx.pTblDir = &tlbSegDir;

    /* just check two entries */
    if ( tlbSegDir.pTypeInfoTab.res0c != 0x0F || tlbSegDir.pImpInfo.res0c != 0x0F)
    {
        ERR("cannot find the table directory, ptr=0x%lx\n",lPSegDir);
	HeapFree(GetProcessHeap(),0,pTypeLibImpl);
	return NULL;
    }

    /* now fill our internal data */
    /* TLIBATTR fields */
    TLB_ReadGuid(&pTypeLibImpl->LibAttr.guid, tlbHeader.posguid, &cx);
    pTypeLibImpl->LibAttr.lcid = tlbHeader.lcid;
    pTypeLibImpl->LibAttr.syskind = tlbHeader.varflags & 0x0f; /* check the mask */
    pTypeLibImpl->LibAttr.wMajorVerNum = LOWORD(tlbHeader.version);
    pTypeLibImpl->LibAttr.wMinorVerNum = HIWORD(tlbHeader.version);
    pTypeLibImpl->LibAttr.wLibFlags = (WORD) tlbHeader.flags & 0xffff;/* check mask */

    /* name, eventually add to a hash table */
    pTypeLibImpl->Name = TLB_ReadName(&cx, tlbHeader.NameOffset);

    /* help info */
    pTypeLibImpl->DocString = TLB_ReadString(&cx, tlbHeader.helpstring);
    pTypeLibImpl->HelpFile = TLB_ReadString(&cx, tlbHeader.helpfile);

    if( tlbHeader.varflags & HELPDLLFLAG)
    {
            int offset;
            TLB_Read(&offset, sizeof(offset), &cx, sizeof(tlbHeader));
            pTypeLibImpl->HelpStringDll = TLB_ReadString(&cx, offset);
    }

    pTypeLibImpl->dwHelpContext = tlbHeader.helpstringcontext;

    /* custom data */
    if(tlbHeader.CustomDataOffset >= 0)
    {
        pTypeLibImpl->ctCustData = TLB_CustData(&cx, tlbHeader.CustomDataOffset, &pTypeLibImpl->pCustData);
    }

    /* fill in typedescriptions */
    if(tlbSegDir.pTypdescTab.length > 0)
    {
        int i, j, cTD = tlbSegDir.pTypdescTab.length / (2*sizeof(INT));
        INT16 td[4];
        pTypeLibImpl->pTypeDesc = TLB_Alloc( cTD * sizeof(TYPEDESC));
        TLB_Read(td, sizeof(td), &cx, tlbSegDir.pTypdescTab.offset);
        for(i=0; i<cTD; )
	{
            /* FIXME: add several sanity checks here */
            pTypeLibImpl->pTypeDesc[i].vt = td[0] & VT_TYPEMASK;
            if(td[0] == VT_PTR || td[0] == VT_SAFEARRAY)
	    {
	        /* FIXME: check safearray */
                if(td[3] < 0)
                    V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lptdesc)= & stndTypeDesc[td[2]];
                else
                    V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lptdesc)= & pTypeLibImpl->pTypeDesc[td[2]/8];
            }
	    else if(td[0] == VT_CARRAY)
            {
	        /* array descr table here */
	        V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc) = (void *)((int) td[2]);  /* temp store offset in*/
            }                             
            else if(td[0] == VT_USERDEFINED)
	    {
                V_UNION(&(pTypeLibImpl->pTypeDesc[i]),hreftype) = MAKELONG(td[2],td[3]);
            }
	    if(++i<cTD) TLB_Read(td, sizeof(td), &cx, DO_NOT_SEEK);
        }

        /* second time around to fill the array subscript info */
        for(i=0;i<cTD;i++)
	{
            if(pTypeLibImpl->pTypeDesc[i].vt != VT_CARRAY) continue;
            if(tlbSegDir.pArrayDescriptions.offset>0)
	    {
                TLB_Read(td, sizeof(td), &cx, tlbSegDir.pArrayDescriptions.offset + (int) V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc));
                V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc) = TLB_Alloc(sizeof(ARRAYDESC)+sizeof(SAFEARRAYBOUND)*(td[3]-1));

                if(td[1]<0)
                    V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc)->tdescElem.vt = td[0] & VT_TYPEMASK;
                else
                    V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc)->tdescElem = stndTypeDesc[td[0]/8];

                V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc)->cDims = td[2];

                for(j = 0; j<td[2]; j++)
		{
                    TLB_Read(& V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc)->rgbounds[j].cElements, 
                        sizeof(INT), &cx, DO_NOT_SEEK);
                    TLB_Read(& V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc)->rgbounds[j].lLbound, 
                        sizeof(INT), &cx, DO_NOT_SEEK);
                }
            }
	    else
	    {
                V_UNION(&(pTypeLibImpl->pTypeDesc[i]),lpadesc) = NULL;
                ERR("didn't find array description data\n");
            }
        }
    }

    /* imported type libs */
    if(tlbSegDir.pImpFiles.offset>0)
    {
        TLBImpLib **ppImpLib = &(pTypeLibImpl->pImpLibs);
        int oGuid, offset = tlbSegDir.pImpFiles.offset;
        UINT16 size;

        while(offset < tlbSegDir.pImpFiles.offset +tlbSegDir.pImpFiles.length)
	{
            *ppImpLib = TLB_Alloc(sizeof(TLBImpLib));
            (*ppImpLib)->offset = offset - tlbSegDir.pImpFiles.offset;
            TLB_Read(&oGuid, sizeof(INT), &cx, offset);

			TLB_Read(&(*ppImpLib)->lcid,          sizeof(LCID),   &cx, DO_NOT_SEEK);
            TLB_Read(&(*ppImpLib)->wVersionMajor, sizeof(WORD),   &cx, DO_NOT_SEEK);
            TLB_Read(&(*ppImpLib)->wVersionMinor, sizeof(WORD),   &cx, DO_NOT_SEEK);
            TLB_Read(& size,                      sizeof(UINT16), &cx, DO_NOT_SEEK);

            size >>= 2;
            (*ppImpLib)->name = TLB_Alloc(size+1);
            TLB_Read((*ppImpLib)->name, size, &cx, DO_NOT_SEEK);
            TLB_ReadGuid(&(*ppImpLib)->guid, oGuid, &cx);
            offset = (offset + sizeof(INT) + sizeof(DWORD) + sizeof(LCID) + sizeof(UINT16) + size + 3) & 0xfffffffc;

            ppImpLib = &(*ppImpLib)->next;
        }
    }

    /* type info's */
    if(tlbHeader.nrtypeinfos >= 0 )
    {
        /*pTypeLibImpl->TypeInfoCount=tlbHeader.nrtypeinfos; */
        ITypeInfoImpl **ppTI = &(pTypeLibImpl->pTypeInfo);
        int i;

        for(i = 0; i<(int)tlbHeader.nrtypeinfos; i++)
		{
            *ppTI = TLB_DoTypeInfo(&cx, i, pTypeLibImpl);

            ppTI = &((*ppTI)->next);
            (pTypeLibImpl->TypeInfoCount)++;
        }
    }

    TRACE("(%p)\n", pTypeLibImpl);
    return (ITypeLib2*) pTypeLibImpl;
}

/* ITypeLib::QueryInterface
 */
static HRESULT WINAPI ITypeLib2_fnQueryInterface(
	ITypeLib2 * iface,
	REFIID riid,
	VOID **ppvObject)
{
    ICOM_THIS( ITypeLibImpl, iface);

    TRACE("(%p)->(IID: %s)\n",This,debugstr_guid(riid));

    *ppvObject=NULL;
    if(IsEqualIID(riid, &IID_IUnknown) || 
       IsEqualIID(riid,&IID_ITypeLib)||
       IsEqualIID(riid,&IID_ITypeLib2))
    {
        *ppvObject = This;
    }
	
    if(*ppvObject)
    {
        ITypeLib2_AddRef(iface);
        TRACE("-- Interface: (%p)->(%p)\n",ppvObject,*ppvObject);
        return S_OK;
    }
    TRACE("-- Interface: E_NOINTERFACE\n");
    return E_NOINTERFACE;
}

/* ITypeLib::AddRef
 */
static ULONG WINAPI ITypeLib2_fnAddRef( ITypeLib2 *iface)
{
    ICOM_THIS( ITypeLibImpl, iface);

    TRACE("(%p)->ref is %u\n",This, This->ref);

    return ++(This->ref);
}

/* ITypeLib::Release
 */
static ULONG WINAPI ITypeLib2_fnRelease( ITypeLib2 *iface)
{
    ICOM_THIS( ITypeLibImpl, iface);

    TRACE("(%p)->(%u)\n",This, This->ref);

    if (!--(This->ref))
    {
      /* fixme destroy child objects */

      TRACE(" destroying ITypeLib(%p)\n",This);

	  if (This->Name)
	  {
	 	  SysFreeString(This->Name);
		  This->Name = NULL;
	  }

	  if (This->DocString)
	  {
		  SysFreeString(This->DocString);
		  This->DocString = NULL;
	  }

	  if (This->HelpFile)
	  {
		  SysFreeString(This->HelpFile);
		  This->HelpFile = NULL;
	  }

	  if (This->HelpStringDll)
	  {
		  SysFreeString(This->HelpStringDll);
		  This->HelpStringDll = NULL;
	  }
	
  	  ITypeInfo_Release((ITypeInfo*) This->pTypeInfo);
      HeapFree(GetProcessHeap(),0,This);
      return 0;
    }
    return This->ref;
}

/* ITypeLib::GetTypeInfoCount
 * 
 * Returns the number of type descriptions in the type library
 */
static UINT WINAPI ITypeLib2_fnGetTypeInfoCount( ITypeLib2 *iface)
{
    ICOM_THIS( ITypeLibImpl, iface);
    TRACE("(%p)->count is %d\n",This, This->TypeInfoCount);
    return This->TypeInfoCount;
}

/* ITypeLib::GetTypeInfo
 *
 * retrieves the specified type description in the library.
 */
static HRESULT WINAPI ITypeLib2_fnGetTypeInfo(
	ITypeLib2 *iface,
	UINT index, 
	ITypeInfo **ppTInfo)
{
    int i;
    ICOM_THIS( ITypeLibImpl, iface);
    ITypeInfoImpl *pTLBTInfo = This->pTypeInfo;

    TRACE("(%p)->(index=%d) \n",This, index);

    if (!ppTInfo) return E_INVALIDARG;
    
    /* search element n in list */
    for(i=0; i < index; i++)
    {
      pTLBTInfo = pTLBTInfo->next;
      if (!pTLBTInfo)
      {
        TRACE("-- element not found\n");
        return TYPE_E_ELEMENTNOTFOUND;
      }
    }	

    *ppTInfo = (ITypeInfo *) pTLBTInfo;
    ITypeInfo_AddRef(*ppTInfo);
    TRACE("-- found (%p)\n",*ppTInfo);
    return S_OK;
}

/* ITypeLibs::GetTypeInfoType
 *
 * Retrieves the type of a type description.
 */
static HRESULT WINAPI ITypeLib2_fnGetTypeInfoType(
	ITypeLib2 *iface,
	UINT index,
	TYPEKIND *pTKind)
{
    ICOM_THIS( ITypeLibImpl, iface);
    int i;
    ITypeInfoImpl *pTInfo = This->pTypeInfo;

    TRACE("(%p) index %d \n",This, index);

    if(!pTKind) return E_INVALIDARG;
    
    /* search element n in list */
    for(i=0; i < index; i++)
    {
      if(!pTInfo)
      {
        TRACE("-- element not found\n");
        return TYPE_E_ELEMENTNOTFOUND;
      }
      pTInfo = pTInfo->next;
    }

    *pTKind = pTInfo->TypeAttr.typekind;
    TRACE("-- found Type (%d)\n", *pTKind);
    return S_OK;
}

/* ITypeLib::GetTypeInfoOfGuid
 *
 * Retrieves the type description that corresponds to the specified GUID.
 *
 */
static HRESULT WINAPI ITypeLib2_fnGetTypeInfoOfGuid(
	ITypeLib2 *iface,
	REFGUID guid,
	ITypeInfo **ppTInfo)
{
    ICOM_THIS( ITypeLibImpl, iface);
    ITypeInfoImpl *ppTLBTInfo = This->pTypeInfo; /* head of list */

    TRACE("(%p)\n\tguid:\t%s)\n",This,debugstr_guid(guid));

    /* serach linked list for guid */
    while( !IsEqualIID(guid,&ppTLBTInfo->TypeAttr.guid) )
    {
      ppTLBTInfo = ppTLBTInfo->next;
      if (!ppTLBTInfo)
      {
        /* end of list reached */
	TRACE("-- element not found\n");
	return TYPE_E_ELEMENTNOTFOUND;
      }
    }

    TRACE("-- found (%p, %s)\n", ppTLBTInfo, debugstr_w(ppTLBTInfo->Name));

    *ppTInfo = (ITypeInfo*)ppTLBTInfo;
    ITypeInfo_AddRef(*ppTInfo);
    return S_OK;
}

/* ITypeLib::GetLibAttr
 *
 * Retrieves the structure that contains the library's attributes.
 *
 */
static HRESULT WINAPI ITypeLib2_fnGetLibAttr(
	ITypeLib2 *iface, 
	LPTLIBATTR *ppTLibAttr)
{
    ICOM_THIS( ITypeLibImpl, iface);
    TRACE("(%p)\n",This);
    /* FIXME: must do a copy here */
    *ppTLibAttr=&This->LibAttr;
    return S_OK;
}

/* ITypeLib::GetTypeComp
 *
 * Enables a client compiler to bind to a library's types, variables,
 * constants, and global functions.
 *
 */
static HRESULT WINAPI ITypeLib2_fnGetTypeComp(
	ITypeLib2 *iface,
	ITypeComp **ppTComp)
{
    ICOM_THIS( ITypeLibImpl, iface);
    FIXME("(%p): stub!\n",This);
    return E_NOTIMPL;
}

/* ITypeLib::GetDocumentation
 *
 * Retrieves the library's documentation string, the complete Help file name
 * and path, and the context identifier for the library Help topic in the Help
 * file.
 *
 */
static HRESULT WINAPI ITypeLib2_fnGetDocumentation(
	ITypeLib2 *iface,
	INT index,
	BSTR *pBstrName,
	BSTR *pBstrDocString,
	DWORD *pdwHelpContext, 
	BSTR *pBstrHelpFile)
{
	ICOM_THIS( ITypeLibImpl, iface);
    HRESULT result;
    ITypeInfo *pTInfo;
    TRACE("(%p) index %d Name(%p) DocString(%p)"
           " HelpContext(%p) HelpFile(%p)\n",
        This, index, pBstrName, pBstrDocString, pdwHelpContext, pBstrHelpFile);
    if(index<0){ /* documentation for the typelib */
        if(pBstrName)
            *pBstrName=SysAllocString(This->Name);
        if(pBstrDocString)
            *pBstrDocString=SysAllocString(This->DocString);
        if(pdwHelpContext)
            *pdwHelpContext=This->dwHelpContext;
        if(pBstrHelpFile)
            *pBstrHelpFile=SysAllocString(This->HelpFile);
    }else {/* for a typeinfo */
        result=ITypeLib2_fnGetTypeInfo(iface, index, &pTInfo);
        if(SUCCEEDED(result)){
            result=ITypeInfo_GetDocumentation(pTInfo, MEMBERID_NIL,  pBstrName,
                pBstrDocString, pdwHelpContext, pBstrHelpFile);
            ITypeInfo_Release(pTInfo);
        }
        if(!SUCCEEDED(result))
            return result;
    }
    return S_OK;
}

/* ITypeLib::IsName
 *
 * Indicates whether a passed-in string contains the name of a type or member
 * described in the library.
 *
 */
static HRESULT WINAPI ITypeLib2_fnIsName(
	ITypeLib2 *iface,
	LPOLESTR szNameBuf,
	ULONG lHashVal,
	BOOL *pfName)
{
    ICOM_THIS( ITypeLibImpl, iface);
    ITypeInfoImpl *pTInfo;
    TLBFuncDesc *pFInfo;
    TLBVarDesc *pVInfo;
    int i;
    UINT nNameBufLen = SysStringLen(szNameBuf);

    TRACE("(%p)->(%s,%08lx,%p)\n", This, debugstr_w(szNameBuf), lHashVal,
	  pfName);

    *pfName=TRUE;
    for(pTInfo=This->pTypeInfo;pTInfo;pTInfo=pTInfo->next){
        if(!memcmp(szNameBuf,pTInfo->Name, nNameBufLen)) goto ITypeLib2_fnIsName_exit;
        for(pFInfo=pTInfo->funclist;pFInfo;pFInfo=pFInfo->next) {
            if(!memcmp(szNameBuf,pFInfo->Name, nNameBufLen)) goto ITypeLib2_fnIsName_exit;
            for(i=0;i<pFInfo->funcdesc.cParams;i++)
                if(!memcmp(szNameBuf,pFInfo->pParamDesc[i].Name, nNameBufLen))
                    goto ITypeLib2_fnIsName_exit;
        }
        for(pVInfo=pTInfo->varlist;pVInfo;pVInfo=pVInfo->next)
            if(!memcmp(szNameBuf,pVInfo->Name, nNameBufLen)) goto ITypeLib2_fnIsName_exit;
       
    }
    *pfName=FALSE;

ITypeLib2_fnIsName_exit:
    TRACE("(%p)slow! search for %s: %s found!\n", This,
          debugstr_w(szNameBuf), *pfName?"NOT":"");
    
    return S_OK;
}

/* ITypeLib::FindName
 *
 * Finds occurrences of a type description in a type library. This may be used
 * to quickly verify that a name exists in a type library.
 *
 */
static HRESULT WINAPI ITypeLib2_fnFindName(
	ITypeLib2 *iface,
	LPOLESTR szNameBuf,
	ULONG lHashVal,
	ITypeInfo **ppTInfo,
	MEMBERID *rgMemId,
	UINT16 *pcFound)
{
    ICOM_THIS( ITypeLibImpl, iface);
    ITypeInfoImpl *pTInfo;
    TLBFuncDesc *pFInfo;
    TLBVarDesc *pVInfo;
    int i,j = 0;
   
    UINT nNameBufLen = SysStringLen(szNameBuf);

    for(pTInfo=This->pTypeInfo;pTInfo && j<*pcFound; pTInfo=pTInfo->next){
        if(!memcmp(szNameBuf,pTInfo->Name, nNameBufLen)) goto ITypeLib2_fnFindName_exit;
        for(pFInfo=pTInfo->funclist;pFInfo;pFInfo=pFInfo->next) {
            if(!memcmp(szNameBuf,pFInfo->Name,nNameBufLen)) goto ITypeLib2_fnFindName_exit;
            for(i=0;i<pFInfo->funcdesc.cParams;i++)
                if(!memcmp(szNameBuf,pFInfo->pParamDesc[i].Name,nNameBufLen))
                    goto ITypeLib2_fnFindName_exit;
        }
        for(pVInfo=pTInfo->varlist;pVInfo;pVInfo=pVInfo->next) ;
            if(!memcmp(szNameBuf,pVInfo->Name, nNameBufLen)) goto ITypeLib2_fnFindName_exit;
        continue;
ITypeLib2_fnFindName_exit:
        ITypeInfo_AddRef((ITypeInfo*)pTInfo);
        ppTInfo[j]=(LPTYPEINFO)pTInfo;
        j++;
    }
    TRACE("(%p)slow! search for %d with %s: found %d TypeInfo's!\n",
          This, *pcFound, debugstr_w(szNameBuf), j);

    *pcFound=j;
    
    return S_OK;
}

/* ITypeLib::ReleaseTLibAttr
 *
 * Releases the TLIBATTR originally obtained from ITypeLib::GetLibAttr.
 *
 */
static VOID WINAPI ITypeLib2_fnReleaseTLibAttr(
	ITypeLib2 *iface,
	TLIBATTR *pTLibAttr)
{
    ICOM_THIS( ITypeLibImpl, iface);
    TRACE("freeing (%p)\n",This);
    /* nothing to do */
}

/* ITypeLib2::GetCustData
 *
 * gets the custom data
 */
static HRESULT WINAPI ITypeLib2_fnGetCustData(
	ITypeLib2 * iface,
	REFGUID guid, 
        VARIANT *pVarVal)
{
    ICOM_THIS( ITypeLibImpl, iface);
    TLBCustData *pCData;

    for(pCData=This->pCustData; pCData; pCData = pCData->next)
    {
      if( IsEqualIID(guid, &pCData->guid)) break;
    }
    
    TRACE("(%p) guid %s %s found!x)\n", This, debugstr_guid(guid), pCData? "" : "NOT");

    if(pCData)
    {
        VariantInit( pVarVal);
        VariantCopy( pVarVal, &pCData->data);
        return S_OK;
    }
    return E_INVALIDARG;  /* FIXME: correct? */
}

/* ITypeLib2::GetLibStatistics
 *
 * Returns statistics about a type library that are required for efficient
 * sizing of hash tables.
 *
 */
static HRESULT WINAPI ITypeLib2_fnGetLibStatistics(
	ITypeLib2 * iface, 
        ULONG *pcUniqueNames,
	ULONG *pcchUniqueNames)
{
    ICOM_THIS( ITypeLibImpl, iface);

    FIXME("(%p): stub!\n", This);

    if(pcUniqueNames) *pcUniqueNames=1;
    if(pcchUniqueNames) *pcchUniqueNames=1;
    return S_OK;
}

/* ITypeLib2::GetDocumentation2
 *
 * Retrieves the library's documentation string, the complete Help file name
 * and path, the localization context to use, and the context ID for the
 * library Help topic in the Help file.
 *
 */
static HRESULT WINAPI ITypeLib2_fnGetDocumentation2(
	ITypeLib2 * iface, 
        INT index,
	LCID lcid,
	BSTR *pbstrHelpString,
        DWORD *pdwHelpStringContext,
	BSTR *pbstrHelpStringDll)
{
    ICOM_THIS( ITypeLibImpl, iface);
    HRESULT result;
    ITypeInfo *pTInfo;

    FIXME("(%p) index %d lcid %ld half implemented stub!\n", This, index, lcid);

    /* the help string should be obtained from the helpstringdll,
     * using the _DLLGetDocumentation function, based on the supplied
     * lcid. Nice to do sometime...
     */
    if(index<0)
    {
      /* documentation for the typelib */
      if(pbstrHelpString)
        *pbstrHelpString=SysAllocString(This->DocString);
      if(pdwHelpStringContext)
        *pdwHelpStringContext=This->dwHelpContext;
      if(pbstrHelpStringDll)
        *pbstrHelpStringDll=SysAllocString(This->HelpStringDll);
    }
    else
    {
      /* for a typeinfo */
      result=ITypeLib2_GetTypeInfo(iface, index, &pTInfo);
      if(SUCCEEDED(result))
      {	
	ITypeInfo2 * pTInfo2;
	result = ITypeInfo_QueryInterface(pTInfo, &IID_ITypeInfo2, (LPVOID*) &pTInfo2);
	if(SUCCEEDED(result))
	{
          result = ITypeInfo2_GetDocumentation2(pTInfo2, MEMBERID_NIL, lcid,
                pbstrHelpString, pdwHelpStringContext, pbstrHelpStringDll);
          ITypeInfo2_Release(pTInfo2);
	}
        ITypeInfo_Release(pTInfo);
      }
      if(!SUCCEEDED(result))
        return result;
    }
    return S_OK;
}

/* ITypeLib2::GetAllCustData
 *
 * Gets all custom data items for the library. 
 *
 */
static HRESULT WINAPI ITypeLib2_fnGetAllCustData(
	ITypeLib2 * iface,
        CUSTDATA *pCustData)
{
    ICOM_THIS( ITypeLibImpl, iface);
    TLBCustData *pCData;
    int i;
    TRACE("(%p) returning %d items\n", This, This->ctCustData); 
    pCustData->prgCustData = TLB_Alloc(This->ctCustData * sizeof(CUSTDATAITEM));
    if(pCustData->prgCustData ){
        pCustData->cCustData=This->ctCustData;
        for(i=0, pCData=This->pCustData; pCData; i++, pCData = pCData->next){
            pCustData->prgCustData[i].guid=pCData->guid;
            VariantCopy(& pCustData->prgCustData[i].varValue, & pCData->data);
        }
    }else{
        ERR(" OUT OF MEMORY! \n");
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

static ICOM_VTABLE(ITypeLib2) tlbvt = {
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    ITypeLib2_fnQueryInterface,
    ITypeLib2_fnAddRef,
    ITypeLib2_fnRelease,
    ITypeLib2_fnGetTypeInfoCount,
    ITypeLib2_fnGetTypeInfo,
    ITypeLib2_fnGetTypeInfoType,
    ITypeLib2_fnGetTypeInfoOfGuid,
    ITypeLib2_fnGetLibAttr,
    ITypeLib2_fnGetTypeComp,
    ITypeLib2_fnGetDocumentation,
    ITypeLib2_fnIsName,
    ITypeLib2_fnFindName,
    ITypeLib2_fnReleaseTLibAttr,

    ITypeLib2_fnGetCustData,
    ITypeLib2_fnGetLibStatistics,
    ITypeLib2_fnGetDocumentation2,
    ITypeLib2_fnGetAllCustData
 };

/*================== ITypeInfo(2) Methods ===================================*/
static ITypeInfo2 * WINAPI ITypeInfo_Constructor(void)
{
    ITypeInfoImpl * pTypeInfoImpl;

    pTypeInfoImpl = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(ITypeInfoImpl));
    if (pTypeInfoImpl)
    {
      ICOM_VTBL(pTypeInfoImpl) = &tinfvt;
      pTypeInfoImpl->ref=1;
    }
    TRACE("(%p)\n", pTypeInfoImpl);
    return (ITypeInfo2*) pTypeInfoImpl;
}

/* ITypeInfo::QueryInterface
 */
static HRESULT WINAPI ITypeInfo_fnQueryInterface(
	ITypeInfo2 *iface,
	REFIID riid,
	VOID **ppvObject)
{
    ICOM_THIS( ITypeLibImpl, iface);

    TRACE("(%p)->(IID: %s)\n",This,debugstr_guid(riid));

    *ppvObject=NULL;
    if(IsEqualIID(riid, &IID_IUnknown) || 
            IsEqualIID(riid,&IID_ITypeInfo)||
            IsEqualIID(riid,&IID_ITypeInfo2))
        *ppvObject = This;

    if(*ppvObject){
        ITypeInfo_AddRef(iface);
        TRACE("-- Interface: (%p)->(%p)\n",ppvObject,*ppvObject);
        return S_OK;
    }
    TRACE("-- Interface: E_NOINTERFACE\n");
    return E_NOINTERFACE;
}

/* ITypeInfo::AddRef
 */
static ULONG WINAPI ITypeInfo_fnAddRef( ITypeInfo2 *iface)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TRACE("(%p)->ref is %u\n",This, This->ref);
    return ++(This->ref);
}

/* ITypeInfo::Release
 */
static ULONG WINAPI ITypeInfo_fnRelease( ITypeInfo2 *iface)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TRACE("(%p)->(%u)\n",This, This->ref);

	
    if (!--(This->ref))
    {
      FIXME("destroy child objects\n");

      TRACE(" destroying ITypeInfo(%p)\n",This); 
	  if (This->Name)
	  {
		  SysFreeString(This->Name);
		  This->Name = 0;
	  }
	  
	  if (This->DocString)
	  {
	      SysFreeString(This->DocString);
		  This->DocString = 0;
	  }

	  if (This->next)
	  {
		ITypeInfo_Release((ITypeInfo*)This->next);
	  }

      HeapFree(GetProcessHeap(),0,This);
      return 0;
    }
    return This->ref;
}

/* ITypeInfo::GetTypeAttr
 *
 * Retrieves a TYPEATTR structure that contains the attributes of the type
 * description.
 *
 */
static HRESULT WINAPI ITypeInfo_fnGetTypeAttr( ITypeInfo2 *iface,
        LPTYPEATTR  *ppTypeAttr)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TRACE("(%p)\n",This);
    /* FIXME: must do a copy here */
    *ppTypeAttr=&This->TypeAttr;
    return S_OK;
}

/* ITypeInfo::GetTypeComp
 *
 * Retrieves the ITypeComp interface for the type description, which enables a
 * client compiler to bind to the type description's members.
 *
 */
static HRESULT WINAPI ITypeInfo_fnGetTypeComp( ITypeInfo2 *iface,
        ITypeComp  * *ppTComp)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    FIXME("(%p) stub!\n", This);
    return S_OK;
}

/* ITypeInfo::GetFuncDesc
 *
 * Retrieves the FUNCDESC structure that contains information about a
 * specified function.
 *
 */
static HRESULT WINAPI ITypeInfo_fnGetFuncDesc( ITypeInfo2 *iface, UINT index,
        LPFUNCDESC  *ppFuncDesc)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    int i;
    TLBFuncDesc * pFDesc; 
    TRACE("(%p) index %d\n", This, index);
    for(i=0, pFDesc=This->funclist; i!=index && pFDesc; i++, pFDesc=pFDesc->next)
        ;
    if(pFDesc){
        /* FIXME: must do a copy here */
        *ppFuncDesc=&pFDesc->funcdesc;
        return S_OK;
    }
    return E_INVALIDARG;
}

/* ITypeInfo::GetVarDesc
 *
 * Retrieves a VARDESC structure that describes the specified variable. 
 *
 */
static HRESULT WINAPI ITypeInfo_fnGetVarDesc( ITypeInfo2 *iface, UINT index,
        LPVARDESC  *ppVarDesc)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    int i;
    TLBVarDesc * pVDesc; 
    TRACE("(%p) index %d\n", This, index);
    for(i=0, pVDesc=This->varlist; i!=index && pVDesc; i++, pVDesc=pVDesc->next)
        ;
    if(pVDesc){
        /* FIXME: must do a copy here */
        *ppVarDesc=&pVDesc->vardesc;
        return S_OK;
    }
    return E_INVALIDARG;
}

/* ITypeInfo_GetNames
 *
 * Retrieves the variable with the specified member ID (or the name of the
 * property or method and its parameters) that correspond to the specified
 * function ID.
 */
static HRESULT WINAPI ITypeInfo_fnGetNames( ITypeInfo2 *iface, MEMBERID memid,
        BSTR  *rgBstrNames, UINT cMaxNames, UINT  *pcNames)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBFuncDesc * pFDesc; 
    TLBVarDesc * pVDesc; 
    int i;
    TRACE("(%p) memid=0x%08lx Maxname=%d\n", This, memid, cMaxNames);
    for(pFDesc=This->funclist; pFDesc && pFDesc->funcdesc.memid != memid; pFDesc=pFDesc->next);
    if(pFDesc)
    {
      /* function found, now return function and parameter names */
      for(i=0; i<cMaxNames && i <= pFDesc->funcdesc.cParams; i++)
      {
        if(!i)
	  	  *rgBstrNames=SysAllocString(pFDesc->Name);
        else
          rgBstrNames[i]=SysAllocString(pFDesc->pParamDesc[i-1].Name);
      }
      *pcNames=i;
    }
    else
    {
      for(pVDesc=This->varlist; pVDesc && pVDesc->vardesc.memid != memid; pVDesc=pVDesc->next);
      if(pVDesc)
      {
        *rgBstrNames=SysAllocString(pVDesc->Name);
        *pcNames=1;
      }
      else
      {
        if(This->TypeAttr.typekind==TKIND_INTERFACE && This->TypeAttr.cImplTypes )
        {
          /* recursive search */
          ITypeInfo *pTInfo;
          HRESULT result;
          result=ITypeInfo_GetRefTypeInfo(iface, This->impltypelist->reference, &pTInfo);
          if(SUCCEEDED(result))
	  {
            result=ITypeInfo_GetNames(pTInfo, memid, rgBstrNames, cMaxNames, pcNames);
            ITypeInfo_Release(pTInfo);
            return result;
          }
          WARN("Could not search inherited interface!\n");
        }
        else
	{
          WARN("no names found\n");
	}
        *pcNames=0;
        return TYPE_E_ELEMENTNOTFOUND;
      }
    }
    return S_OK;
}


/* ITypeInfo::GetRefTypeOfImplType
 *
 * If a type description describes a COM class, it retrieves the type
 * description of the implemented interface types. For an interface,
 * GetRefTypeOfImplType returns the type information for inherited interfaces,
 * if any exist.
 *
 */
static HRESULT WINAPI ITypeInfo_fnGetRefTypeOfImplType(
	ITypeInfo2 *iface,
        UINT index,
	HREFTYPE  *pRefType)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    int(i);
    TLBRefType *pIref = This->impltypelist;

    TRACE("(%p) index %d\n", This, index);
    dump_TypeInfo(This);


    if(index==(UINT)-1)
    {
      /* only valid on dual interfaces;
         retrieve the associated TKIND_INTERFACE handle for the current TKIND_DISPATCH
      */
      if( This->TypeAttr.typekind != TKIND_DISPATCH) return E_INVALIDARG;
      
      if (This->TypeAttr.wTypeFlags & TYPEFLAG_FDISPATCHABLE &&
          This->TypeAttr.wTypeFlags & TYPEFLAG_FDUAL )
      {
        *pRefType = -1;
      }
      else
      {
        *pRefType = pIref->reference;
      }
    }
    else
    {
      /* get element n from linked list */
      for(i=0; i<index; i++)
      {
        if (!pIref) return TYPE_E_ELEMENTNOTFOUND;
        pIref = pIref->next;
      }
      
      *pRefType = pIref->reference;
    }
    
    TRACE("-- 0x%08lx %s\n",pIref->reference, debugstr_guid(&pIref->guid) );
    return S_OK;
   
}

/* ITypeInfo::GetImplTypeFlags
 * 
 * Retrieves the IMPLTYPEFLAGS enumeration for one implemented interface 
 * or base interface in a type description.
 */
static HRESULT WINAPI ITypeInfo_fnGetImplTypeFlags( ITypeInfo2 *iface,
        UINT index, INT  *pImplTypeFlags)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    int(i);
    TLBRefType *pIref;
    TRACE("(%p) index %d\n", This, index);
    for(i=0, pIref=This->impltypelist; i<index && pIref; i++, pIref=pIref->next)
        ;
    if(i==index && pIref){
        *pImplTypeFlags=pIref->flags;
        return S_OK;
    }
    *pImplTypeFlags=0;
    return TYPE_E_ELEMENTNOTFOUND;
}

/* GetIDsOfNames
 * Maps between member names and member IDs, and parameter names and
 * parameter IDs.
 */
static HRESULT WINAPI ITypeInfo_fnGetIDsOfNames( ITypeInfo2 *iface,
        LPOLESTR  *rgszNames, UINT cNames, MEMBERID  *pMemId)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBFuncDesc * pFDesc; 
    TLBVarDesc * pVDesc; 
    HRESULT ret=S_OK;
	UINT nNameLen = SysStringLen(*rgszNames);
	
    TRACE("(%p) Name %s cNames %d\n", This, debugstr_w(*rgszNames),
            cNames);
    for(pFDesc=This->funclist; pFDesc; pFDesc=pFDesc->next) {
        int i, j;
        if( !memcmp(*rgszNames, pFDesc->Name, nNameLen)) {
            if(cNames) *pMemId=pFDesc->funcdesc.memid;
            for(i=1; i < cNames; i++){
				UINT nParamLen = SysStringLen(rgszNames[i]);
                for(j=0; j<pFDesc->funcdesc.cParams; j++)
                    if(memcmp(rgszNames[i],pFDesc->pParamDesc[j].Name, nParamLen))
                            break;
                if( j<pFDesc->funcdesc.cParams)
                    pMemId[i]=j;
                else
                   ret=DISP_E_UNKNOWNNAME;
            };
            return ret;
        }
    }   
    for(pVDesc=This->varlist; pVDesc; pVDesc=pVDesc->next) {
        if( !memcmp(*rgszNames, pVDesc->Name, nNameLen)) {
            if(cNames) *pMemId=pVDesc->vardesc.memid;
            return ret;
        }
    }
    /* not found, see if this is and interface with an inheritance */       
    if(This->TypeAttr.typekind==TKIND_INTERFACE && 
            This->TypeAttr.cImplTypes ){
        /* recursive search */
        ITypeInfo *pTInfo;
        ret=ITypeInfo_GetRefTypeInfo(iface, 
                This->impltypelist->reference, &pTInfo);
        if(SUCCEEDED(ret)){
            ret=ITypeInfo_GetIDsOfNames(pTInfo, rgszNames, cNames, pMemId );
            ITypeInfo_Release(pTInfo);
            return ret;
        }
        WARN("Could not search inherited interface!\n");
    } else
        WARN("no names found\n");
    return DISP_E_UNKNOWNNAME;
}

/* ITypeInfo::Invoke
 * 
 * Invokes a method, or accesses a property of an object, that implements the
 * interface described by the type description.
 */
static HRESULT WINAPI ITypeInfo_fnInvoke(
	ITypeInfo2 *iface,
	VOID  *pIUnk,
        MEMBERID memid,
	UINT16 dwFlags,
	DISPPARAMS  *pDispParams,
        VARIANT  *pVarResult,
	EXCEPINFO  *pExcepInfo,
	UINT  *pArgErr)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    FIXME("(%p)(%p,id=0x%08lx,0x%08x,%p,%p,%p,%p) stub!\n",
      This, pIUnk, memid, dwFlags, pDispParams, pVarResult, pExcepInfo, pArgErr );
    dump_DispParms(pDispParams);
    return S_OK;
}

/* ITypeInfo::GetDocumentation
 * 
 * Retrieves the documentation string, the complete Help file name and path,
 * and the context ID for the Help topic for a specified type description.
 */
static HRESULT WINAPI ITypeInfo_fnGetDocumentation( ITypeInfo2 *iface,
        MEMBERID memid, BSTR  *pBstrName, BSTR  *pBstrDocString,
        DWORD  *pdwHelpContext, BSTR  *pBstrHelpFile)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBFuncDesc * pFDesc; 
    TLBVarDesc * pVDesc; 
    TRACE("(%p) memid %ld Name(%p) DocString(%p)"
          " HelpContext(%p) HelpFile(%p)\n",
        This, memid, pBstrName, pBstrDocString, pdwHelpContext, pBstrHelpFile);
    if(memid==MEMBERID_NIL){ /* documentation for the typeinfo */
        if(pBstrName)
            *pBstrName=SysAllocString(This->Name);
        if(pBstrDocString)
            *pBstrDocString=SysAllocString(This->DocString);
        if(pdwHelpContext)
            *pdwHelpContext=This->dwHelpContext;
        if(pBstrHelpFile)
            *pBstrHelpFile=SysAllocString(This->DocString);/* FIXME */
        return S_OK;
    }else {/* for a member */
    for(pFDesc=This->funclist; pFDesc; pFDesc=pFDesc->next)
        if(pFDesc->funcdesc.memid==memid){
            return S_OK;
        }
    for(pVDesc=This->varlist; pVDesc; pVDesc=pVDesc->next)
        if(pVDesc->vardesc.memid==memid){
            return S_OK;
        }
    }
    return TYPE_E_ELEMENTNOTFOUND;
}

/*  ITypeInfo::GetDllEntry
 * 
 * Retrieves a description or specification of an entry point for a function
 * in a DLL.
 */
static HRESULT WINAPI ITypeInfo_fnGetDllEntry( ITypeInfo2 *iface, MEMBERID memid,
        INVOKEKIND invKind, BSTR  *pBstrDllName, BSTR  *pBstrName,
        WORD  *pwOrdinal)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    FIXME("(%p) stub!\n", This);
    return E_FAIL;
}

/* ITypeInfo::GetRefTypeInfo
 * 
 * If a type description references other type descriptions, it retrieves
 * the referenced type descriptions.
 */
static HRESULT WINAPI ITypeInfo_fnGetRefTypeInfo(
	ITypeInfo2 *iface,
        HREFTYPE hRefType,
	ITypeInfo  **ppTInfo)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    HRESULT result = E_FAIL;

    if(HREFTYPE_INTHISFILE(hRefType))
    {
      ITypeLib *pTLib;
      int Index;
      result = ITypeInfo_GetContainingTypeLib(iface, &pTLib, &Index);
      if(SUCCEEDED( result ))
      {
        result=ITypeLib2_GetTypeInfo(pTLib, HREFTYPE_INDEX(hRefType), ppTInfo);
        ITypeLib2_Release(pTLib );
      }
    }
	else if (hRefType == -1 && 
             (((ITypeInfoImpl*) This)->TypeAttr.typekind   == TKIND_DISPATCH) &&
             (((ITypeInfoImpl*) This)->TypeAttr.wTypeFlags &  TYPEFLAG_FDUAL))
    {
	  /* when we meet a DUAL dispinterface, we must create the interface 
	  * version of it.
	  */
	  ITypeInfoImpl* pTypeInfoImpl = (ITypeInfoImpl*) ITypeInfo_Constructor();

		
	  /* the interface version contains the same information as the dispinterface
	   * copy the contents of the structs.
	   */
	  *pTypeInfoImpl = *This;
	  pTypeInfoImpl->ref = 1;
		
	  /* change the type to interface */
	  pTypeInfoImpl->TypeAttr.typekind = TKIND_INTERFACE;
		
      *ppTInfo = (ITypeInfo*) pTypeInfoImpl;

	  ITypeInfo_AddRef((ITypeInfo*) pTypeInfoImpl);

      result = S_OK;
	}
    else
    {
      /* imported type lib */
      TLBRefType   *pRefType     = NULL;
      
      /* search in implemented types */
      for( pRefType = This->impltypelist; 
           pRefType && (pRefType->reference != hRefType);
           pRefType = pRefType->next);
      
      if(!pRefType)
      {                       
        TYPEATTR*       pMyTypeAttr  = &This->TypeAttr;
        unsigned short  cFuncs       = pMyTypeAttr->cFuncs;
        unsigned short  cVars        = pMyTypeAttr->cVars;
        
        /* search in arguments */      
        if (cFuncs > 0)
        {
          unsigned short  cFuncIndex   = 0;
  
          TLBFuncDesc*    pCurrFunc    = This->funclist;
          
          for (cFuncIndex = 0; !pRefType && cFuncIndex < cFuncs ; ++cFuncIndex)
          {
            FUNCDESC*  pCurrFuncDesc = &pCurrFunc->funcdesc;
            
            short      cParams       = pCurrFuncDesc->cParams;
            short      cParamIndex   = 0;
              
            for (cParamIndex = 0 ; 
                 !pRefType && cParamIndex < cParams ; 
                 ++cParamIndex)
            {
              TLBParDesc* pCurrParamDesc = &(pCurrFunc->pParamDesc[cParamIndex]);
             
              if ( pCurrParamDesc->pRefType && pCurrParamDesc->pRefType->reference == hRefType)
              {
                pRefType = pCurrParamDesc->pRefType;
                break; /* also break from outer loop since pRefType != 0 */
              }
            }
            
            pCurrFunc = pCurrFunc->next;
          }
        }
        /* search in variables */
        else if (cVars > 0)
        {
          FIXME("search hreftype in variables, if any\n");
          result = E_INVALIDARG; // FIXME : correct?
        }
      }
      
      /* href-referenced typeinfo found! */
      if (pRefType || hRefType == -1)
      {
        ITypeLibImpl *pTypeLib = pRefType->pImpTLInfo->pImpTypeLib;
        
        if(pTypeLib) 
        {
          TRACE("typeinfo in imported typelib that is already loaded\n");
          
          result = ITypeLib2_GetTypeInfoOfGuid((LPTYPELIB)pTypeLib, 
                                           &pRefType->guid, 
                                           ppTInfo);
        }
        else
        {
          result = LoadRegTypeLib( &pRefType->pImpTLInfo->guid,
                      pRefType->pImpTLInfo->wVersionMajor,
                      pRefType->pImpTLInfo->wVersionMinor,
                      pRefType->pImpTLInfo->lcid,
                      (LPTYPELIB *)&pTypeLib);
          
          if(!SUCCEEDED(result))
	  	  {
            BSTR libnam=SysAllocString(pRefType->pImpTLInfo->name);
            TRACE("typeinfo in imported typelib that isn't already loaded\n");
            result=LoadTypeLib(libnam, (LPTYPELIB *)&pTypeLib);
            SysFreeString(libnam);
          } 
          if(SUCCEEDED(result))
          {
            result=ITypeLib2_GetTypeInfoOfGuid((LPTYPELIB)pTypeLib, &pRefType->guid, ppTInfo);
            pRefType->pImpTLInfo->pImpTypeLib = pTypeLib;
			ITypeLib2_AddRef((ITypeLib*) pTypeLib);
          }        
        }
      }
    }

    TRACE("(%p) hreftype 0x%04lx loaded %s (%p)\n", This, hRefType,
          SUCCEEDED(result)? "SUCCESS":"FAILURE", *ppTInfo);
    return result;
}

/* ITypeInfo::AddressOfMember
 * 
 * Retrieves the addresses of static functions or variables, such as those
 * defined in a DLL.
 */
static HRESULT WINAPI ITypeInfo_fnAddressOfMember( ITypeInfo2 *iface,
        MEMBERID memid, INVOKEKIND invKind, PVOID *ppv)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    FIXME("(%p) stub!\n", This);
    return S_OK;
}

/* ITypeInfo::CreateInstance
 * 
 * Creates a new instance of a type that describes a component object class 
 * (coclass).
 */
static HRESULT WINAPI ITypeInfo_fnCreateInstance( ITypeInfo2 *iface, 
        IUnknown *pUnk, REFIID riid, VOID  **ppvObj) 
{
    ICOM_THIS( ITypeInfoImpl, iface);
    FIXME("(%p) stub!\n", This);
    return S_OK;
}

/* ITypeInfo::GetMops
 *
 * Retrieves marshaling information.
 */
static HRESULT WINAPI ITypeInfo_fnGetMops( ITypeInfo2 *iface, MEMBERID memid,
				BSTR  *pBstrMops)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    FIXME("(%p) stub!\n", This);
    return S_OK;
}

/* ITypeInfo::GetContainingTypeLib
 * 
 * Retrieves the containing type library and the index of the type description
 * within that type library.
 */
static HRESULT WINAPI ITypeInfo_fnGetContainingTypeLib( ITypeInfo2 *iface,
        ITypeLib  * *ppTLib, UINT  *pIndex)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    if (!pIndex)
        return E_INVALIDARG;
    *ppTLib=(LPTYPELIB )(This->pTypeLib);
    *pIndex=This->index;
    ITypeLib2_AddRef(*ppTLib);
    TRACE("(%p) returns (%p) index %d!\n", This, *ppTLib, *pIndex);
    return S_OK;
}

/* ITypeInfo::ReleaseTypeAttr
 *
 * Releases a TYPEATTR previously returned by GetTypeAttr.
 *
 */
static HRESULT WINAPI ITypeInfo_fnReleaseTypeAttr( ITypeInfo2 *iface,
        TYPEATTR* pTypeAttr)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TRACE("(%p)->(%p)\n", This, pTypeAttr);
    return S_OK;
}

/* ITypeInfo::ReleaseFuncDesc
 *
 * Releases a FUNCDESC previously returned by GetFuncDesc. *
 */
static HRESULT WINAPI ITypeInfo_fnReleaseFuncDesc(
	ITypeInfo2 *iface,
        FUNCDESC *pFuncDesc)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TRACE("(%p)->(%p)\n", This, pFuncDesc);
    return S_OK;
}

/* ITypeInfo::ReleaseVarDesc
 *
 * Releases a VARDESC previously returned by GetVarDesc.
 */
static HRESULT WINAPI ITypeInfo_fnReleaseVarDesc( ITypeInfo2 *iface,
        VARDESC *pVarDesc)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TRACE("(%p)->(%p)\n", This, pVarDesc);
    return S_OK;
}

/* ITypeInfo2::GetTypeKind
 *
 * Returns the TYPEKIND enumeration quickly, without doing any allocations.
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetTypeKind( ITypeInfo2 * iface,
    TYPEKIND *pTypeKind)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    *pTypeKind=This->TypeAttr.typekind;
    TRACE("(%p) type 0x%0x\n", This,*pTypeKind);
    return S_OK;
}

/* ITypeInfo2::GetTypeFlags
 *
 * Returns the type flags without any allocations. This returns a DWORD type
 * flag, which expands the type flags without growing the TYPEATTR (type
 * attribute). 
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetTypeFlags( ITypeInfo2 * iface,
    UINT *pTypeFlags)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    *pTypeFlags=This->TypeAttr.wTypeFlags;
    TRACE("(%p) flags 0x%04x\n", This,*pTypeFlags);
     return S_OK;
}

/* ITypeInfo2::GetFuncIndexOfMemId
 * Binds to a specific member based on a known DISPID, where the member name
 * is not known (for example, when binding to a default member).
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetFuncIndexOfMemId( ITypeInfo2 * iface,
    MEMBERID memid, INVOKEKIND invKind, UINT *pFuncIndex)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBFuncDesc *pFuncInfo;
    int i;
    HRESULT result;
    /* FIXME: should check for invKind??? */
    for(i=0, pFuncInfo=This->funclist;pFuncInfo && 
            memid != pFuncInfo->funcdesc.memid; i++, pFuncInfo=pFuncInfo->next);
    if(pFuncInfo){
        *pFuncIndex=i;
        result= S_OK;
    }else{
        *pFuncIndex=0;
        result=E_INVALIDARG;
    }
    TRACE("(%p) memid 0x%08lx invKind 0x%04x -> %s\n", This,
          memid, invKind, SUCCEEDED(result)? "SUCCES":"FAILED");
    return result;
}

/* TypeInfo2::GetVarIndexOfMemId
 *
 * Binds to a specific member based on a known DISPID, where the member name
 * is not known (for example, when binding to a default member). 
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetVarIndexOfMemId( ITypeInfo2 * iface,
    MEMBERID memid, UINT *pVarIndex)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBVarDesc *pVarInfo;
    int i;
    HRESULT result;
    for(i=0, pVarInfo=This->varlist; pVarInfo && 
            memid != pVarInfo->vardesc.memid; i++, pVarInfo=pVarInfo->next)
        ;
    if(pVarInfo){
        *pVarIndex=i;
        result= S_OK;
    }else{
        *pVarIndex=0;
        result=E_INVALIDARG;
    }
    TRACE("(%p) memid 0x%08lx -> %s\n", This,
          memid, SUCCEEDED(result)? "SUCCES":"FAILED");
    return result;
}

/* ITypeInfo2::GetCustData
 *
 * Gets the custom data
 */
static HRESULT WINAPI ITypeInfo2_fnGetCustData(
	ITypeInfo2 * iface,
	REFGUID guid,
	VARIANT *pVarVal)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData;

    for(pCData=This->pCustData; pCData; pCData = pCData->next)
        if( IsEqualIID(guid, &pCData->guid)) break;

    TRACE("(%p) guid %s %s found!x)\n", This, debugstr_guid(guid), pCData? "" : "NOT");

    if(pCData)
    {
        VariantInit( pVarVal);
        VariantCopy( pVarVal, &pCData->data);
        return S_OK;
    }
    return E_INVALIDARG;  /* FIXME: correct? */
}

/* ITypeInfo2::GetFuncCustData
 *
 * Gets the custom data
 */
static HRESULT WINAPI ITypeInfo2_fnGetFuncCustData(
	ITypeInfo2 * iface,
	UINT index,
	REFGUID guid,
	VARIANT *pVarVal)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData=NULL;
    TLBFuncDesc * pFDesc; 
    int i;
    for(i=0, pFDesc=This->funclist; i!=index && pFDesc; i++,
            pFDesc=pFDesc->next);

    if(pFDesc)
        for(pCData=pFDesc->pCustData; pCData; pCData = pCData->next)
            if( IsEqualIID(guid, &pCData->guid)) break;

    TRACE("(%p) guid %s %s found!x)\n", This, debugstr_guid(guid), pCData? "" : "NOT");

    if(pCData){
        VariantInit( pVarVal);
        VariantCopy( pVarVal, &pCData->data);
        return S_OK;
    }
    return E_INVALIDARG;  /* FIXME: correct? */
}

/* ITypeInfo2::GetParamCustData
 *
 * Gets the custom data
 */
static HRESULT WINAPI ITypeInfo2_fnGetParamCustData(
	ITypeInfo2 * iface,
	UINT indexFunc,
	UINT indexParam,
	REFGUID guid,
	VARIANT *pVarVal)
{   
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData=NULL;
    TLBFuncDesc * pFDesc; 
    int i;

    for(i=0, pFDesc=This->funclist; i!=indexFunc && pFDesc; i++,pFDesc=pFDesc->next);

    if(pFDesc && indexParam >=0 && indexParam<pFDesc->funcdesc.cParams)
        for(pCData=pFDesc->pParamDesc[indexParam].pCustData; pCData; 
                pCData = pCData->next)
            if( IsEqualIID(guid, &pCData->guid)) break;

    TRACE("(%p) guid %s %s found!x)\n", This, debugstr_guid(guid), pCData? "" : "NOT");

    if(pCData)
    {
        VariantInit( pVarVal);
        VariantCopy( pVarVal, &pCData->data);
        return S_OK;
    }
    return E_INVALIDARG;  /* FIXME: correct? */
}

/* ITypeInfo2::GetVarcCustData
 *
 * Gets the custom data
 */
static HRESULT WINAPI ITypeInfo2_fnGetVarCustData(
	ITypeInfo2 * iface,
	UINT index,
	REFGUID guid,
	VARIANT *pVarVal)
{   
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData=NULL;
    TLBVarDesc * pVDesc; 
    int i;

    for(i=0, pVDesc=This->varlist; i!=index && pVDesc; i++, pVDesc=pVDesc->next);

    if(pVDesc)
    {
      for(pCData=pVDesc->pCustData; pCData; pCData = pCData->next)
      {
        if( IsEqualIID(guid, &pCData->guid)) break;
      }
    }

    TRACE("(%p) guid %s %s found!x)\n", This, debugstr_guid(guid), pCData? "" : "NOT");

    if(pCData)
    {
        VariantInit( pVarVal);
        VariantCopy( pVarVal, &pCData->data);
        return S_OK;
    }
    return E_INVALIDARG;  /* FIXME: correct? */
}

/* ITypeInfo2::GetImplcCustData
 *
 * Gets the custom data
 */
static HRESULT WINAPI ITypeInfo2_fnGetImplTypeCustData(
	ITypeInfo2 * iface,
	UINT index,
	REFGUID guid,
	VARIANT *pVarVal)
{   
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData=NULL;
    TLBRefType * pRDesc; 
    int i;

    for(i=0, pRDesc=This->impltypelist; i!=index && pRDesc; i++, pRDesc=pRDesc->next);

    if(pRDesc)
    {
      for(pCData=pRDesc->pCustData; pCData; pCData = pCData->next)
      {
        if( IsEqualIID(guid, &pCData->guid)) break;
      }
    }
    
    TRACE("(%p) guid %s %s found!x)\n", This, debugstr_guid(guid), pCData? "" : "NOT");

    if(pCData)
    {
        VariantInit( pVarVal);
        VariantCopy( pVarVal, &pCData->data);
        return S_OK;
    }
    return E_INVALIDARG;  /* FIXME: correct? */
}

/* ITypeInfo2::GetDocumentation2
 * 
 * Retrieves the documentation string, the complete Help file name and path,
 * the localization context to use, and the context ID for the library Help
 * topic in the Help file.
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetDocumentation2(
	ITypeInfo2 * iface,
	MEMBERID memid,
	LCID lcid,
	BSTR *pbstrHelpString,
	DWORD *pdwHelpStringContext,
	BSTR *pbstrHelpStringDll)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBFuncDesc * pFDesc; 
    TLBVarDesc * pVDesc; 
    TRACE("(%p) memid %ld lcid(0x%lx)  HelpString(%p) "
          "HelpStringContext(%p) HelpStringDll(%p)\n",
          This, memid, lcid, pbstrHelpString, pdwHelpStringContext,
          pbstrHelpStringDll );
    /* the help string should be obtained from the helpstringdll,
     * using the _DLLGetDocumentation function, based on the supplied
     * lcid. Nice to do sometime...
     */
    if(memid==MEMBERID_NIL){ /* documentation for the typeinfo */
        if(pbstrHelpString)
            *pbstrHelpString=SysAllocString(This->Name);
        if(pdwHelpStringContext)
            *pdwHelpStringContext=This->dwHelpStringContext;
        if(pbstrHelpStringDll)
            *pbstrHelpStringDll=
                SysAllocString(This->pTypeLib->HelpStringDll);/* FIXME */
        return S_OK;
    }else {/* for a member */
    for(pFDesc=This->funclist; pFDesc; pFDesc=pFDesc->next)
        if(pFDesc->funcdesc.memid==memid){
             if(pbstrHelpString)
                *pbstrHelpString=SysAllocString(pFDesc->HelpString);
            if(pdwHelpStringContext)
                *pdwHelpStringContext=pFDesc->HelpStringContext;
            if(pbstrHelpStringDll)
                *pbstrHelpStringDll=
                    SysAllocString(This->pTypeLib->HelpStringDll);/* FIXME */
        return S_OK;
    }
    for(pVDesc=This->varlist; pVDesc; pVDesc=pVDesc->next)
        if(pVDesc->vardesc.memid==memid){
             if(pbstrHelpString)
                *pbstrHelpString=SysAllocString(pVDesc->HelpString);
            if(pdwHelpStringContext)
                *pdwHelpStringContext=pVDesc->HelpStringContext;
            if(pbstrHelpStringDll)
                *pbstrHelpStringDll=
                    SysAllocString(This->pTypeLib->HelpStringDll);/* FIXME */
            return S_OK;
        }
    }
    return TYPE_E_ELEMENTNOTFOUND;
}

/* ITypeInfo2::GetAllCustData
 *
 * Gets all custom data items for the Type info. 
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetAllCustData(
	ITypeInfo2 * iface,
	CUSTDATA *pCustData)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData;
    int i;

    TRACE("(%p) returning %d items\n", This, This->ctCustData); 

    pCustData->prgCustData = TLB_Alloc(This->ctCustData * sizeof(CUSTDATAITEM));
    if(pCustData->prgCustData ){
        pCustData->cCustData=This->ctCustData;
        for(i=0, pCData=This->pCustData; pCData; i++, pCData = pCData->next){
            pCustData->prgCustData[i].guid=pCData->guid;
            VariantCopy(& pCustData->prgCustData[i].varValue, & pCData->data);
        }
    }else{
        ERR(" OUT OF MEMORY! \n");
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

/* ITypeInfo2::GetAllFuncCustData
 *
 * Gets all custom data items for the specified Function
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetAllFuncCustData(
	ITypeInfo2 * iface,
	UINT index,
	CUSTDATA *pCustData)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData;
    TLBFuncDesc * pFDesc; 
    int i;
    TRACE("(%p) index %d\n", This, index); 
    for(i=0, pFDesc=This->funclist; i!=index && pFDesc; i++,
            pFDesc=pFDesc->next)
        ;
    if(pFDesc){
        pCustData->prgCustData =
            TLB_Alloc(pFDesc->ctCustData * sizeof(CUSTDATAITEM));
        if(pCustData->prgCustData ){
            pCustData->cCustData=pFDesc->ctCustData;
            for(i=0, pCData=pFDesc->pCustData; pCData; i++,
                    pCData = pCData->next){
                pCustData->prgCustData[i].guid=pCData->guid;
                VariantCopy(& pCustData->prgCustData[i].varValue,
                        & pCData->data);
            }
        }else{
            ERR(" OUT OF MEMORY! \n");
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }
    return TYPE_E_ELEMENTNOTFOUND;
}

/* ITypeInfo2::GetAllParamCustData
 *
 * Gets all custom data items for the Functions
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetAllParamCustData( ITypeInfo2 * iface,
    UINT indexFunc, UINT indexParam, CUSTDATA *pCustData)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData=NULL;
    TLBFuncDesc * pFDesc; 
    int i;
    TRACE("(%p) index %d\n", This, indexFunc); 
    for(i=0, pFDesc=This->funclist; i!=indexFunc && pFDesc; i++,
            pFDesc=pFDesc->next)
        ;
    if(pFDesc && indexParam >=0 && indexParam<pFDesc->funcdesc.cParams){
        pCustData->prgCustData = 
            TLB_Alloc(pFDesc->pParamDesc[indexParam].ctCustData *
                    sizeof(CUSTDATAITEM));
        if(pCustData->prgCustData ){
            pCustData->cCustData=pFDesc->pParamDesc[indexParam].ctCustData;
            for(i=0, pCData=pFDesc->pParamDesc[indexParam].pCustData;
                    pCData; i++, pCData = pCData->next){
                pCustData->prgCustData[i].guid=pCData->guid;
                VariantCopy(& pCustData->prgCustData[i].varValue,
                        & pCData->data);
            }
        }else{
            ERR(" OUT OF MEMORY! \n");
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }
    return TYPE_E_ELEMENTNOTFOUND;
}

/* ITypeInfo2::GetAllVarCustData
 *
 * Gets all custom data items for the specified Variable
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetAllVarCustData( ITypeInfo2 * iface,
    UINT index, CUSTDATA *pCustData)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData;
    TLBVarDesc * pVDesc; 
    int i;
    TRACE("(%p) index %d\n", This, index); 
    for(i=0, pVDesc=This->varlist; i!=index && pVDesc; i++,
            pVDesc=pVDesc->next)
        ;
    if(pVDesc){
        pCustData->prgCustData =
            TLB_Alloc(pVDesc->ctCustData * sizeof(CUSTDATAITEM));
        if(pCustData->prgCustData ){
            pCustData->cCustData=pVDesc->ctCustData;
            for(i=0, pCData=pVDesc->pCustData; pCData; i++,
                    pCData = pCData->next){
                pCustData->prgCustData[i].guid=pCData->guid;
                VariantCopy(& pCustData->prgCustData[i].varValue,
                        & pCData->data);
            }
        }else{
            ERR(" OUT OF MEMORY! \n");
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }
    return TYPE_E_ELEMENTNOTFOUND;
}

/* ITypeInfo2::GetAllImplCustData
 *
 * Gets all custom data items for the specified implementation type
 *
 */
static HRESULT WINAPI ITypeInfo2_fnGetAllImplTypeCustData(
	ITypeInfo2 * iface,
	UINT index,
	CUSTDATA *pCustData)
{
    ICOM_THIS( ITypeInfoImpl, iface);
    TLBCustData *pCData;
    TLBRefType * pRDesc; 
    int i;
    TRACE("(%p) index %d\n", This, index); 
    for(i=0, pRDesc=This->impltypelist; i!=index && pRDesc; i++,
            pRDesc=pRDesc->next)
        ;
    if(pRDesc){
        pCustData->prgCustData =
            TLB_Alloc(pRDesc->ctCustData * sizeof(CUSTDATAITEM));
        if(pCustData->prgCustData ){
            pCustData->cCustData=pRDesc->ctCustData;
            for(i=0, pCData=pRDesc->pCustData; pCData; i++,
                    pCData = pCData->next){
                pCustData->prgCustData[i].guid=pCData->guid;
                VariantCopy(& pCustData->prgCustData[i].varValue,
                        & pCData->data);
            }
        }else{
            ERR(" OUT OF MEMORY! \n");
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }
    return TYPE_E_ELEMENTNOTFOUND;
}

static ICOM_VTABLE(ITypeInfo2) tinfvt = 
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE

    ITypeInfo_fnQueryInterface,
    ITypeInfo_fnAddRef,
    ITypeInfo_fnRelease,

    ITypeInfo_fnGetTypeAttr,
    ITypeInfo_fnGetTypeComp,
    ITypeInfo_fnGetFuncDesc,
    ITypeInfo_fnGetVarDesc,
    ITypeInfo_fnGetNames,
    ITypeInfo_fnGetRefTypeOfImplType,
    ITypeInfo_fnGetImplTypeFlags,
    ITypeInfo_fnGetIDsOfNames,
    ITypeInfo_fnInvoke,
    ITypeInfo_fnGetDocumentation,
    ITypeInfo_fnGetDllEntry,
    ITypeInfo_fnGetRefTypeInfo,
    ITypeInfo_fnAddressOfMember,
    ITypeInfo_fnCreateInstance,
    ITypeInfo_fnGetMops,
    ITypeInfo_fnGetContainingTypeLib,
    ITypeInfo_fnReleaseTypeAttr,
    ITypeInfo_fnReleaseFuncDesc,
    ITypeInfo_fnReleaseVarDesc,

    ITypeInfo2_fnGetTypeKind,
    ITypeInfo2_fnGetTypeFlags,
    ITypeInfo2_fnGetFuncIndexOfMemId,
    ITypeInfo2_fnGetVarIndexOfMemId,
    ITypeInfo2_fnGetCustData,
    ITypeInfo2_fnGetFuncCustData,
    ITypeInfo2_fnGetParamCustData,
    ITypeInfo2_fnGetVarCustData,
    ITypeInfo2_fnGetImplTypeCustData,
    ITypeInfo2_fnGetDocumentation2,
    ITypeInfo2_fnGetAllCustData,
    ITypeInfo2_fnGetAllFuncCustData,
    ITypeInfo2_fnGetAllParamCustData,
    ITypeInfo2_fnGetAllVarCustData,
    ITypeInfo2_fnGetAllImplTypeCustData,
};
