/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 OLE stubs for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 */

#include "ole32.h"

#include "oString.h"

// ----------------------------------------------------------------------
// CLSIDFromStringA() [Internal]
// ----------------------------------------------------------------------
static HRESULT CLSIDFromStringA(
    const char *	lpsz,		// [in] - ASCII string to convert
    LPCLSID		pclisid)	// [out] - CLSID
{
    dprintf(("OLE32.CLSIDFromStringA [Internal]\n"));

    BYTE *s = (BYTE *) lpsz;
    BYTE *p;
    int   i;
    BYTE table[256];

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

    if (strlen(lpsz) != 38)
	return OLE_ERROR_OBJECT;

    p = (BYTE *) pclisid;

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
// CLSIDFromProgID()
// ----------------------------------------------------------------------
HRESULT WIN32API CLSIDFromProgID(
    LPCOLESTR		lpszProgID,	// [in] - UNICODE program id as found in registry
    LPCLSID		pclsid)		// [out] - CLSID
{
    dprintf(("OLE32.CLSIDFromProgID\n"));

    LONG		lDataLen = 80;
    oStringA		szKey(lpszProgID);
    oStringA		szCLSID(lDataLen);
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
    return CLSIDFromStringA(szCLSID, pclsid);
}

// ----------------------------------------------------------------------
// CLSIDFromString()
// ----------------------------------------------------------------------
HRESULT WIN32API CLSIDFromString(
    LPCOLESTR		lpsz,		// [in] - Unicode string CLSID
    LPCLSID		pclsid)		// [out] - Binary CLSID
{
    dprintf(("OLE32.CLSIDFromString\n"));

    HRESULT		ret = OLE_ERROR_GENERIC;
    oStringA		szCLSID(lpsz);

    // Convert to binary CLSID
    return CLSIDFromStringA(szCLSID, pclsid);
}

