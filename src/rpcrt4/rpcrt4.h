/* $Id: rpcrt4.h,v 1.2 2001-04-04 09:02:15 sandervl Exp $ */
/* Definitions for the OLE RPCRT4
 *
 * Copyright 2000 D.J.Raison
 */

#ifndef __ODIN_RPCRT4_H
#define __ODIN_RPCRT4_H

#define CINTERFACE 1
#define	RPC_UNICODE_SUPPORTED	1

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <wchar.h>

#include <win/windef.h>
#include <win/wtypes.h>
#include <winbase.h>
#undef CALLCONV

#include <os2win.h>
#include <ole2ver.h>
#include <ole.h>
#include <ole2.h>
#include <winreg.h>

#include "objbase.h"

#include "wine/obj_inplace.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_surrogate.h"
#include "wine/obj_oleview.h"
#include "wine/obj_cache.h"

#include <rpcdce.h>

#include <unicode.h>

#endif /* __ODIN_RPCRT4_H */
