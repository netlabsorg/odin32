/* $Id: windows.h,v 1.2 1999-08-14 09:05:06 sandervl Exp $ */

#ifndef __WINE_WINDOWS_H
#define __WINE_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __WIN32OS2__
#include <odin.h>
#endif

#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "windef.h"
#include "shell.h"
#include "winreg.h"
#include "winnetwk.h"
#include "winver.h"
#include "lzexpand.h"
#include "shellapi.h"
#include "ole2.h"
#include "winnls.h"
#include "objbase.h"

#if 0
  Where does this belong? Nobody uses this stuff anyway.
typedef struct {
	BYTE i;  /* much more .... */
} KANJISTRUCT;
typedef KANJISTRUCT *LPKANJISTRUCT;
typedef KANJISTRUCT *NPKANJISTRUCT;
typedef KANJISTRUCT *PKANJISTRUCT;

BOOL16      WINAPI CheckMenuRadioButton16(HMENU16,UINT16,UINT16,UINT16,BOOL16);
BOOL      WINAPI CheckMenuRadioButton(HMENU,UINT,UINT,UINT,BOOL);
WORD        WINAPI WOWHandle16(HANDLE,WOW_HANDLE_TYPE);
BOOL16      WINAPI GetPrivateProfileStruct16(LPCSTR,LPCSTR,LPVOID,UINT16,LPCSTR);
INT16       WINAPI GetPrivateProfileSection16(LPCSTR,LPSTR,UINT16,LPCSTR);

#endif /* 0 */

#ifdef __cplusplus
}
#endif

#endif  /* __WINE_WINDOWS_H */
