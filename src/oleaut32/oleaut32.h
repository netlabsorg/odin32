/* $Id: oleaut32.h,v 1.3 1999-09-02 16:23:18 sandervl Exp $ */
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
//#include <uconv.h>
#undef CALLCONV

#include <os2win.h>
#include <ole2ver.h>
#include <ole.h>
#include <winreg.h>

#include "wine/obj_base.h"
#include "wine/obj_misc.h"
#include "wine/obj_inplace.h"
#include "wine/obj_dataobject.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_marshal.h"
#include "wine/obj_moniker.h"
#include "wine/obj_clientserver.h"
#include "wine/obj_dragdrop.h"
#include <heapstring.h>

#endif
