/* $Id: gdiplus.cpp,v 1.94 2016-02-09 10:00:00 rousseau Exp $ */

/*
 * GDIPLUS apis
 *
 * Copyright (C) 2007 Google (Evan Stade)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 * Portions Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Portions Copyright 1998 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include <debugtools.h>
#include <misc.h>
#include "unicode.h"
#include <codepage.h>
#include <dcdata.h>
#include <winuser32.h>
#include <stats.h>
#include <objhandle.h>
#include <winspool.h>

#include <shlwapi.h>

// rousseau.201602140232
// In 'initguid.h' we made the GUID in macro 'DEFINE_GUID' static to work
// around duplicate defined GUIDs when building 'swt.dll'. (Rev:22089)
// Since 'DEFINE_GUID' is also defined in 'guiddef.h', we will use that
// (non-static) version here. See 'comtype.h' for some more info.
// TODO: Investigate duplicate ID's in 'swt.dll'.
//       Have one definition of 'DEFINE_GUID' ?
///#include <initguid.h>
#include <guiddef.h>

#include <gdiplus.h>

#define DBG_LOCALLOG    DBG_gdiplus
#include "dbglocal.h"

ODINDEBUGCHANNEL(GDIPLUS-GDIPLUS)

void* WINGDIPAPI GdipAlloc(SIZE_T size)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

void WINGDIPAPI GdipFree(void* ptr)
{
    HeapFree(GetProcessHeap(), 0, ptr);
}

void WINAPI GdiplusShutdown(ULONG_PTR token)
{
    /* FIXME: no object tracking */
}

Status WINAPI GdiplusStartup(ULONG_PTR *token, const struct GdiplusStartupInput *input,
                             struct GdiplusStartupOutput *output)
{
    if(!token)
        return InvalidParameter;

    if(input->GdiplusVersion != 1) {
        return UnsupportedGdiplusVersion;
    } else if ((input->DebugEventCallback) ||
        (input->SuppressBackgroundThread) || (input->SuppressExternalCodecs)){
        FIXME("Unimplemented for non-default GdiplusStartupInput");
        return NotImplemented;
    } else if(output) {
        FIXME("Unimplemented for non-null GdiplusStartupOutput");
        return NotImplemented;
    }

    return Ok;
}
