/* $Id: clsid.cpp,v 1.6 1999-09-08 11:29:27 davidr Exp $ */
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
 * Copyright 1999 David J. Raison
 *
 * Some portions from Wine Implementation
 *   Copyright 1995  Martin von Loewis
 *   Copyright 1998  Justin Bradford
 *   Copyright 1999  Francis Beaudet
 *   Copyright 1999  Sylvain St-Germain
 */

#include "ole32.h"

#include "oString.h"

// ----------------------------------------------------------------------
// CLSIDFromProgID()
// ----------------------------------------------------------------------
HRESULT WIN32API CLSIDFromProgID(
    LPCOLESTR		lpszProgID,	// [in] - UNICODE program id as found in registry
    LPCLSID		pclsid)		// [out] - CLSID
{
    dprintf(("OLE32: CLSIDFromProgID"));

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
    dprintf(("OLE32: IIDFromString"));
    return CLSIDFromString((LPCOLESTR)lpsz, (LPCLSID)lpiid);
}


// ----------------------------------------------------------------------
// CLSIDFromStringA()
// @@@PH: this is not a WINE API, but a replacement for CLSIDFromString16
//        which used to accept ASCII strings instead of OLE strings
// ----------------------------------------------------------------------

// missing prototype
LPWSTR WIN32API HEAP_strdupAtoW( HANDLE heap, DWORD flags, LPCSTR str );

HRESULT WIN32API CLSIDFromStringA(
    LPSTR		lpsz,		// [in] - ASCII string CLSID
    LPCLSID		pclsid)		// [out] - Binary CLSID
{
  LPWSTR  lpszOle = HEAP_strdupAtoW(GetProcessHeap(),
                                    0,
                                    lpsz);
  HRESULT hRes;

  dprintf(("OLE32: CLSIDFromStringA"));

  hRes = CLSIDFromString(lpszOle, pclsid);
  HeapFree(GetProcessHeap(), 0, lpszOle);
  return hRes;
}


// ----------------------------------------------------------------------
// CLSIDFromString()
// ----------------------------------------------------------------------
HRESULT WIN32API CLSIDFromString(
    LPCOLESTR		lpsz,		// [in] - Unicode string CLSID
    LPCLSID		pclsid)		// [out] - Binary CLSID
{
    dprintf(("OLE32: CLSIDFromString"));

    oStringA		tClsId(lpsz);

    HRESULT		ret = OLE_ERROR_GENERIC;

    // Convert to binary CLSID
    char *s = (char *) tClsId;
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

    if (lstrlenW(lpsz) != 38)
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
// CoCreateGuid()
// ----------------------------------------------------------------------
HRESULT WIN32API CoCreateGuid(GUID *pguid)
{
   dprintf(("OLE32: CoCreateGuid"));
   memset(pguid, 0, sizeof(GUID));      //TODO: should be random GUID
   return S_OK;
}

// ----------------------------------------------------------------------
// WINE_StringFromCLSID
// ----------------------------------------------------------------------
HRESULT WIN32API WINE_StringFromCLSID(const CLSID *rclsid, LPSTR idstr)
{
    dprintf(("OLE32: WINE_StringFromCLSID"));

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

    dprintf(("       clsid: %s", idstr));

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

    dprintf(("OLE32: StringFromCLSID"));

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

    dprintf(("OLE32: StringFromIID"));

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
    char 	tmp[50];
    size_t	strLen;

    dprintf(("OLE32: StringFromGUID2"));

    // Setup new string...
    WINE_StringFromCLSID(rguid, tmp);

    strLen = strlen(tmp);

    if(cbMax < (strLen * 2 + 1))
	return 0;

    AsciiToUnicode(tmp, lpsz);

    return(strLen * 2 + 1);  // including 0 terminator
}

