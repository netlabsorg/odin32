/* $Id: olepro32.h,v 1.1 2000-02-21 10:33:00 sandervl Exp $ */
/*
 * Win32 OLE stubs for OS/2
 *
 * 1998/06/12
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OLEPRO32_H__
#define __OLEPRO32_H__

// ><DJR 17.05.99 Force to use C-interfaces for now to prevent CALLBACK definition compiler error
#define CINTERFACE 1

#include <odin.h>

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
#include "oleauto.h"
#undef CALLCONV

#include <os2win.h>
#include <ole2ver.h>
#include <ole.h>
#include <winreg.h>

#include "objbase.h"		// Common obj include
#include "wine/obj_inplace.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_surrogate.h"
#include "wine/obj_oleview.h"
#include "wine/obj_cache.h"

#include "wine/obj_property.h"	// needed for oleundo
#include "wine/obj_olefont.h"	// needed for oleundo
#include "wine/obj_oleundo.h"	// for OLE_COLOR of all things :-)

#include "wine/obj_picture.h"

#include <heapstring.h>

#endif
