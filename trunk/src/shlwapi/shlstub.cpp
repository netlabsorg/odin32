/* $Id: shlstub.cpp,v 1.1 2000-05-19 12:10:04 sandervl Exp $ */

/*
 * Win32 stub shlwapi APIs for OS/2
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#define HAVE_WCTYPE_H
#include <odin.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"
#include "crtdll.h"

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>


ODINDEBUGCHANNEL(SHLWAPI-SHLSTUB)

#include "shlwapi.h"


