/* $Id: w32skrnl.h,v 1.3 1999-06-10 16:21:56 achimha Exp $ */
#ifndef _W32SKRNL_H
#define _W32SKRNL_H
#include "windef.h"
LPSTR WINAPI GetWin32sDirectory(void);
DWORD WINAPI RtlNtStatusToDosError(DWORD error);
#endif /* _W32SKRNL_H */
