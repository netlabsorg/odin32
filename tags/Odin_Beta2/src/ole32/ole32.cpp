/* $Id: ole32.cpp,v 1.16 2002-02-15 17:18:50 sandervl Exp $ */
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
