/* $Id: windows.h,v 1.6 2000-04-09 11:09:11 sandervl Exp $ */

#ifndef __WINE_WINDOWS_H
#define __WINE_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __WIN32OS2__
#include <odin.h>
#if defined (__IBMC__) || (__IBMCPP__ < 400)
#define CINTERFACE 1
#endif
#endif

#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "windef.h"
#include "shell.h"
#include "winreg.h"
#include "ddeml.h"
#include "winnetwk.h"
#include "winver.h"
#include "lzexpand.h"
#include "shellapi.h"
#include "winnls.h"
#ifdef __cplusplus
#include "ole2.h"
#include "objbase.h"
#endif
#include "winver.h"


#ifdef __cplusplus
}
#endif

#endif  /* __WINE_WINDOWS_H */
