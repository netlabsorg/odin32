/* $Id: clsid.cpp,v 1.14 2001-04-26 19:32:42 sandervl Exp $ */
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
// IIDFromString
// ----------------------------------------------------------------------
HRESULT WIN32API IIDFromString(LPSTR lpsz, LPIID lpiid)
{
//    dprintf(("OLE32: IIDFromString"));
    return CLSIDFromString((LPCOLESTR)lpsz, (LPCLSID)lpiid);
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
// CONCRETE_IsEqualGUID
// ----------------------------------------------------------------------
int WIN32API CONCRETE_IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
    return IsEqualGUID(rguid1, rguid2);
}

