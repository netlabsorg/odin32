/* $Id: w32skrnl.h,v 1.1 1999-05-24 20:19:29 ktk Exp $ */

#ifndef _W32SKRNL_H
#define _W32SKRNL_H
#include "windef.h"
LPSTR WINAPI GetWin32sDirectory(void);
DWORD WINAPI RtlNtStatusToDosError(DWORD error);
#endif /* _W32SKRNL_H */
