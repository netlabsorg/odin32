/* $Id: ole32.c,v 1.1 2002-11-12 17:06:30 sandervl Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * COM/OLE misc. functions.
 * 
 * 1/7/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Some portions from Wine Implementation
 *   Copyright 1995  Martin von Loewis
 *   Copyright 1998  Justin Bradford
 *   Copyright 1999  Francis Beaudet
 *   Copyright 1999  Sylvain St-Germain
 */

#include "ole32.h"
#include "compobj_private.h"

// ----------------------------------------------------------------------
// CoDosDateTimeToFileTime
// ----------------------------------------------------------------------
BOOL WIN32API CoDosDateTimeToFileTime(WORD nDosDate, WORD nDosTime,
                                         FILETIME *lpFileTime)
{
    dprintf(("OLE32: CoDosDateTimeToFileTime"));

    return DosDateTimeToFileTime(nDosDate, nDosTime, lpFileTime);
}

// ----------------------------------------------------------------------
// CoDosDateTimeToFileTime
// ----------------------------------------------------------------------
BOOL WIN32API CoFileTimeToDosDateTime(FILETIME *lpFileTime, LPWORD lpDosDate,
                                         LPWORD lpDosTime)
{
    dprintf(("OLE32: CoFileTimeToDosDateTime"));

    return FileTimeToDosDateTime(lpFileTime, lpDosDate, lpDosTime);
}

// ----------------------------------------------------------------------
// CLSIDFromStringA()
// @@@PH: this is not a WINE API, but a replacement for CLSIDFromString16
//        which used to accept ASCII strings instead of OLE strings
// ----------------------------------------------------------------------

HRESULT WIN32API CLSIDFromStringA(
    LPCSTR		lpsz,		// [in] - ASCII string CLSID
    LPCLSID		pclsid)		// [out] - Binary CLSID
{
    return CLSIDFromString16(lpsz, pclsid);
}

/******************************************************************************
 *		CLSIDFromProgID	[COMPOBJ.61]
 * Converts a program id into the respective GUID. (By using a registry lookup)
 * RETURNS
 *	riid associated with the progid
 */
HRESULT WINAPI CLSIDFromProgID16(
	LPCOLESTR16 progid,	/* [in] program id as found in registry */
	LPCLSID riid		/* [out] associated CLSID */
) {
	char	*buf,buf2[80];
	DWORD	buf2len;
	HRESULT	err;
	HKEY	xhkey;

	buf = HeapAlloc(GetProcessHeap(),0,strlen(progid)+8);
	sprintf(buf,"%s\\CLSID",progid);
	if ((err=RegOpenKeyA(HKEY_CLASSES_ROOT,buf,&xhkey))) {
		HeapFree(GetProcessHeap(),0,buf);
                return CO_E_CLASSSTRING;
	}
	HeapFree(GetProcessHeap(),0,buf);
	buf2len = sizeof(buf2);
	if ((err=RegQueryValueA(xhkey,NULL,buf2,&buf2len))) {
		RegCloseKey(xhkey);
                return CO_E_CLASSSTRING;
	}
	RegCloseKey(xhkey);
	return __CLSIDFromStringA(buf2,riid);
}

/******************************************************************************
 *		CLSIDFromString	[COMPOBJ.20]
 * Converts a unique identifier from its string representation into
 * the GUID struct.
 *
 * Class id: DWORD-WORD-WORD-BYTES[2]-BYTES[6]
 *
 * RETURNS
 *	the converted GUID
 */
HRESULT WINAPI CLSIDFromString16(
	LPCOLESTR16 idstr,	/* [in] string representation of guid */
	CLSID *id)		/* [out] GUID converted from string */
{

  return __CLSIDFromStringA(idstr,id);
}
