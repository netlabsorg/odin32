/* $Id: ole32.h,v 1.20 2001-06-08 06:50:52 sandervl Exp $ */
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
#ifndef __OLE32_H__
#define __OLE32_H__

#define NONAMELESSUNION
// ><DJR 17.05.99 Force to use C-interfaces for now to prevent CALLBACK definition compiler error
#define CINTERFACE 1

// ><DJR 17.05.99 Move standard includes to before os2win.h [memcmp]
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <wchar.h>

// ><DJR 17.05.99 Prevent CALLCONV becoming _System
#include <win/wtypes.h>
#include <winbase.h>
//#include <uconv.h>
#undef CALLCONV

#ifdef __cplusplus
#include <os2win.h>
#endif
#include <ole2ver.h>
#include <ole.h>
#include <ole2.h>
#include <winreg.h>

#include "objbase.h"
#include "docobj.h"    /* for the IID_StdOleLink */

#include "wine/obj_inplace.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_surrogate.h"
#include "wine/obj_errorinfo.h"
#include "wine/obj_oleview.h"
#include "wine/obj_cache.h"

#include "debugtools.h"
#include <unicode.h>

#endif
