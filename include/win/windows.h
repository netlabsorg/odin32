/* $Id: windows.h,v 1.9 2003-04-02 12:57:38 sandervl Exp $ */

#ifndef __WINE_WINDOWS_H
#define __WINE_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __WIN32OS2__
#include <odin.h>
#if defined (__IBMC__) || (defined(__IBMCPP__) && (__IBMCPP__ < 400))
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
#include "dlgs.h"
#include "winnetwk.h"
#include "winver.h"
#include "lzexpand.h"
#include "shellapi.h"
#include "winnls.h"
#ifdef __cplusplus
#include "ole2.h"
#include "objbase.h"
#endif
#include <winver.h>
#include <winerror.h>

#include <commdlg.h>
#include <winspool.h>
#include <imm.h>

#ifdef __cplusplus
}
#endif

#endif  /* __WINE_WINDOWS_H */
