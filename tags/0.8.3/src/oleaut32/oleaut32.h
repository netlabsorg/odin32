/* $Id: oleaut32.h,v 1.12 2002-05-17 10:16:59 sandervl Exp $ */
/*
 * Win32 OLE stubs for OS/2
 *
 * 1998/06/12
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Copyright 1998 Sander van Leeuwen
 */
#ifndef __OLEAUT32_H__
#define __OLEAUT32_H__

// ><DJR 17.05.99 Force to use C-interfaces for now to prevent CALLBACK definition compiler error
#define CINTERFACE

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

#include <ole2ver.h>
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
#include <misc.h>
#include <unicode.h>

#define snprintf wsnprintfA
#endif
