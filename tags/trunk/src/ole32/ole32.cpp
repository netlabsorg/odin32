/* $Id: ole32.cpp,v 1.15 2001-04-26 19:32:49 sandervl Exp $ */
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
