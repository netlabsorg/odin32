/* $Id: clsid.cpp,v 1.1 2001-04-26 19:26:08 sandervl Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * ClassID Manipulation.
 *
 * 1/7/99
 *
 * Copyright 2000 David J. Raison 
 *
 * Some portions from Wine Implementation
 *
 *   Copyright 1995  Martin von Loewis
 *             1998  Justin Bradford
 *             1999  Francis Beaudet
 *             1999  Sylvain St-Germain
 */

#include "ole32.h"
#include "rpcdce.h"

#include "oString.h"


// ----------------------------------------------------------------------
// CoCreateGuid						[OLE32.6]
//
// Creates a 128bit GUID.
// ----------------------------------------------------------------------
HRESULT WINAPI CoCreateGuid(
    GUID *	pguid)		/* [out] points to the GUID to initialize */
{
    HRESULT		hr;

    dprintf(("OLE32: CoCreateGuid"));
    hr = UuidCreate(pguid);

    return hr;
}

// ----------------------------------------------------------------------
// CLSIDFromProgID16()
// ----------------------------------------------------------------------
HRESULT WIN32API CLSIDFromProgID16(
    LPCOLESTR16		lpszProgID,	// [in] - UNICODE program id as found in registry
    LPCLSID		pclsid)		// [out] - CLSID
{
//    dprintf(("OLE32: CLSIDFromProgID16"));

    LONG		lDataLen = 80;
    oStringA		szKey(lpszProgID);
    oStringA		szCLSID(lDataLen, 1);
    HKEY		hKey;
    HRESULT		rc;

    // Create the registry lookup string...
    szKey += "\\CLSID";

    // Try to open the key in the registry...
    rc = RegOpenKeyA(HKEY_CLASSES_ROOT, szKey, &hKey);
    if (rc != 0)
    	return OLE_ERROR_GENERIC;

    // Now get the data from the _default_ entry on this key...
    rc = RegQueryValueA(hKey, NULL, szCLSID, &lDataLen);
    RegCloseKey(hKey);
    if (rc != 0)
    	return OLE_ERROR_GENERIC;

    // Now convert from a string to a UUID
    return CLSIDFromString16(szCLSID, pclsid);
}

// ----------------------------------------------------------------------
// CLSIDFromProgID()
// ----------------------------------------------------------------------
HRESULT WIN32API CLSIDFromProgID(
    LPCOLESTR		lpszProgID,	// [in] - UNICODE program id as found in registry
    LPCLSID		pclsid)		// [out] - CLSID
{
//    dprintf(("OLE32: CLSIDFromProgID"));

    LONG		lDataLen = 80;
    oStringW		szKey(lpszProgID);
    oStringW		szCLSID(lDataLen, 1);
    HKEY		hKey;
    HRESULT		rc;

    // Create the registry lookup string...
    szKey += L"\\CLSID";

    // Try to open the key in the registry...
    rc = RegOpenKeyW(HKEY_CLASSES_ROOT, szKey, &hKey);
    if (rc != 0)
    	return OLE_ERROR_GENERIC;

    // Now get the data from the _default_ entry on this key...
    rc = RegQueryValueW(hKey, NULL, szCLSID, &lDataLen);
    RegCloseKey(hKey);
    if (rc != 0)
    	return OLE_ERROR_GENERIC;

    // Now convert from a string to a UUID
    return CLSIDFromString(szCLSID, pclsid);
}

// ----------------------------------------------------------------------
// IIDFromString
// ----------------------------------------------------------------------
HRESULT WIN32API IIDFromString(LPSTR lpsz, LPIID lpiid)
{
//    dprintf(("OLE32: IIDFromString"));
    return CLSIDFromString((LPCOLESTR)lpsz, (LPCLSID)lpiid);
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


// ----------------------------------------------------------------------
// CLSIDFromString16()
// ----------------------------------------------------------------------
HRESULT WIN32API CLSIDFromString16(
    LPCOLESTR16		lpsz,		// [in] - Unicode string CLSID
    LPCLSID		pclsid)		// [out] - Binary CLSID
{
//    dprintf(("OLE32: CLSIDFromString16"));

    // Convert to binary CLSID
    char *s = (char *) lpsz;
    char *p;
    int   i;
    char table[256];

    /* quick lookup table */
    memset(table, 0, 256);

    for (i = 0; i < 10; i++)
    {
	table['0' + i] = i;
    }
    for (i = 0; i < 6; i++)
    {
	table['A' + i] = i+10;
	table['a' + i] = i+10;
    }

    /* in form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} */

    if (lstrlenA(lpsz) != 38)
	return OLE_ERROR_OBJECT;

    p = (char *) pclsid;

    s++;  /* skip leading brace  */
    for (i = 0; i < 4; i++)
    {
	p[3 - i] = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }
    p += 4;
    s++;  /* skip - */

    for (i = 0; i < 2; i++)
    {
	p[1-i] = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }
    p += 2;
    s++;  /* skip - */

    for (i = 0; i < 2; i++)
    {
	p[1-i] = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }
    p += 2;
    s++;  /* skip - */

    /* these are just sequential bytes */
    for (i = 0; i < 2; i++)
    {
	*p++ = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }
    s++;  /* skip - */

    for (i = 0; i < 6; i++)
    {
	*p++ = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }

    return S_OK;
}

// ----------------------------------------------------------------------
// CLSIDFromString()
// ----------------------------------------------------------------------
HRESULT WIN32API CLSIDFromString(
    LPCOLESTR		lpsz,		// [in] - Unicode string CLSID
    LPCLSID		pclsid)		// [out] - Binary CLSID
{
//    dprintf(("OLE32: CLSIDFromString"));

    oStringA		tClsId(lpsz);

    return CLSIDFromString16(tClsId, pclsid);
}

// ----------------------------------------------------------------------
// WINE_StringFromCLSID
// ----------------------------------------------------------------------
HRESULT WIN32API WINE_StringFromCLSID(const CLSID *rclsid, LPSTR idstr)
{
//    dprintf(("OLE32: WINE_StringFromCLSID"));

    if (rclsid == NULL)
    {
	dprintf(("       clsid: (NULL)"));
	*idstr = 0;
	return E_FAIL;
    }

    // Setup new string...
    sprintf(idstr, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
	    rclsid->Data1,
	    rclsid->Data2,
	    rclsid->Data3,
	    rclsid->Data4[0],
	    rclsid->Data4[1],
	    rclsid->Data4[2],
	    rclsid->Data4[3],
	    rclsid->Data4[4],
	    rclsid->Data4[5],
	    rclsid->Data4[6],
	    rclsid->Data4[7]);

//    dprintf(("       clsid: %s", idstr));

    return OLE_OK;
}

// ----------------------------------------------------------------------
// StringFromCLSID
// Memory allocated here on behalf of application should be freed using CoTaskMemFree()
// ----------------------------------------------------------------------
HRESULT WIN32API StringFromCLSID(REFCLSID rclsid, LPOLESTR *ppsz)
{
    char	tmp[50];
    LPWSTR	szclsid;
    size_t	strLen;

//    dprintf(("OLE32: StringFromCLSID"));

    // Setup new string...
    WINE_StringFromCLSID(rclsid, tmp);

    strLen = strlen(tmp);

    // Grab buffer for string...
    szclsid = (LPWSTR)CoTaskMemAlloc((strLen + 1) * sizeof(WCHAR));

    AsciiToUnicode(tmp, szclsid);

    *ppsz = (LPOLESTR)szclsid;

    return S_OK;
}

// ----------------------------------------------------------------------
// StringFromIID
// Memory allocated here on behalf of application should be freed using CoTaskMemFree()
// ----------------------------------------------------------------------
HRESULT WIN32API StringFromIID(REFIID riid, LPOLESTR *ppsz)
{
    char	tmp[50];
    LPWSTR	sziid;
    size_t	strLen;

//    dprintf(("OLE32: StringFromIID"));

    // Setup new string...
    WINE_StringFromCLSID(riid, tmp);

    strLen = strlen(tmp);

    // Grab buffer for string...
    sziid = (LPWSTR)CoTaskMemAlloc((strLen + 1) * sizeof(WCHAR));

    AsciiToUnicode(tmp, sziid);

    *ppsz = (LPOLESTR)sziid;

    return S_OK;
}

// ----------------------------------------------------------------------
// StringFromGUID2
// ----------------------------------------------------------------------
int WIN32API StringFromGUID2(REFGUID rguid, LPOLESTR lpsz, int cbMax)
{
// NB cbMax is a CHARACTER count not a BYTE count... :-)
    char 	tmp[50];
    size_t	strLen;

    // Setup new string...
    WINE_StringFromCLSID(rguid, tmp);

    strLen = (strlen(tmp) + 1);
    if (strLen > cbMax)
	strLen = cbMax;

    AsciiToUnicodeN(tmp, lpsz, strLen);

    return strLen;  // Num CHARACTERS including 0 terminator
}

// ----------------------------------------------------------------------
// CONCRETE_IsEqualGUID
// ----------------------------------------------------------------------
int WIN32API CONCRETE_IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
    return IsEqualGUID(rguid1, rguid2);
}

// ----------------------------------------------------------------------
// ReadClassStm
// ----------------------------------------------------------------------
HRESULT WIN32API ReadClassStm(IStream *pStm, CLSID *rclsid)
{
    ULONG nbByte;
    HRESULT res;

    dprintf(("OLE32: ReadClassStm"));

    if (rclsid == NULL)
        return E_INVALIDARG;

    res = IStream_Read(pStm,(void*)rclsid,sizeof(CLSID),&nbByte);

    if (FAILED(res))
        return res;

    if (nbByte != sizeof(CLSID))
        return S_FALSE;

    return S_OK;
}

// ----------------------------------------------------------------------
// WriteClassStm
// ----------------------------------------------------------------------
HRESULT WIN32API WriteClassStm(IStream *pStm, REFCLSID rclsid)
{
    dprintf(("OLE32: WriteClassStm"));

    if (rclsid == NULL)
        return E_INVALIDARG;

    return IStream_Write(pStm, rclsid, sizeof(CLSID), NULL);
}

// ----------------------------------------------------------------------
// ProgIDFromCLSID
// ----------------------------------------------------------------------
HRESULT WIN32API ProgIDFromCLSID(REFCLSID clsid, LPOLESTR *lplpszProgID)
{
    oStringA		tClsId(clsid);
    oStringA		szKey("CLSID\\");
    LONG		lDataLen = 255;
    oStringA		szProgID(lDataLen, 1);
    HKEY		hKey;
    HRESULT		rc;
    LPOLESTR		tmp;
    LPMALLOC		pMllc;

//    dprintf(("OLE32: ProgIDFromCLSID"));
//    dprintf(("       clsid = %s", (char *)tClsId));

    szKey += tClsId + "\\ProgID";

    // Open key...
    if (RegOpenKeyA(HKEY_CLASSES_ROOT, szKey, &hKey))
    	return REGDB_E_CLASSNOTREG;

    // Get default string from the key...
    rc = RegQueryValueA(hKey, NULL, szProgID, &lDataLen);
    RegCloseKey(hKey);
    if (rc != 0)
	return REGDB_E_CLASSNOTREG;

    if (CoGetMalloc(0, &pMllc))		// Singleton instance, no need to release
	return E_OUTOFMEMORY;

    tmp = (LPOLESTR)IMalloc_Alloc(pMllc, (strlen(szProgID) + 1) * 2); 
    if (tmp == NULL)
	return E_OUTOFMEMORY;

    AsciiToUnicode(szProgID, tmp);
    *lplpszProgID = tmp;

    return S_OK;
}
