/* $Id: shellguid.c,v 1.1 2000-08-30 13:52:55 sandervl Exp $ */
/*
 *  Shell UID
 *
 *  1998 Juergen Schmied (jsch)  *  <juergen.schmied@metronet.de>
 *
 *  this in in a single file due to interfering definitions
 *
 */
#ifdef __WIN32OS2__
#define ICOM_CINTERFACE 1
#include <odin.h>
#define ICOM_CINTERFACE 1
#define CINTERFACE 1
#define INITGUID
#include <wine\obj_oleview.h>
#endif
#include "initguid.h"

#include "shlwapi.h"
#include "shlguid.h"
#include "shlobj.h"
#include "docobj.h"
