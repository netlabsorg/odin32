/* $Id: shlwapiguid.c,v 1.2 2001-09-05 13:48:38 bird Exp $ */
/*
 *  SHRegOpenStream
 */
#ifdef __WIN32OS2__
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#define HAVE_WCTYPE_H

#define ICOM_CINTERFACE 1
#define CINTERFACE 1
#define INITGUID

#endif

#include <string.h>

#include "winerror.h"
#include "winreg.h"
#include "wine/obj_storage.h"

#include "heap.h"
#include "debugtools.h"

