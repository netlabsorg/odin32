/* $Id: shellguid.cpp,v 1.2 2000-08-18 02:01:21 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*
 *  Shell UID
 *
 *  1998 Juergen Schmied (jsch)  *  <juergen.schmied@metronet.de>
 *
 *  this in in a single file due to interfering definitions
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1
#define INITGUID

#include <os2win.h>
#include <shlguid.h>
#include <wine/obj_oleview.h>
#include <wine/obj_dragdrop.h>
#include <wine/obj_inplace.h>
#include <wine/obj_control.h>
#include <wine/obj_shellfolder.h>
#include <wine/obj_shelllink.h>
#include <wine/obj_contextmenu.h>
#include <wine/obj_commdlgbrowser.h>
#include <wine/obj_extracticon.h>
#include <wine/obj_shellextinit.h>
#include "shellfolder.h"
#include <docobj.h>

/*
 * Francis Beaudet <francis@macadamian.com>
 *
 * I moved the contents of this file to the ole/guid.c file.
 *
 * I know that the purpose of this file being here is that it would
 * separate the definitions reserved to the shell DLL into one place. *
 However, until the shell DLL is a real DLL, as long as it is *
 statically linked with the rest of wine, the initializer of it's * GUIDs
 will have to be in the same place as everybody else. This is * the same
 problem as with "real" Windows programs. */

ODINDEBUGCHANNEL(SHELL32-SHELLGUID)


