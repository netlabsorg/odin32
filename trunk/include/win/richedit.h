/* $Id: richedit.h,v 1.1 2000-05-10 13:11:50 sandervl Exp $ */
#ifndef __WINE_RICHEDIT_H
#define __WINE_RICHEDIT_H

#include "windef.h"
#include "pshpack4.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _RICHEDIT_VER
#define _RICHEDIT_VER   0x0210
#endif /* _RICHEDIT_VER */

#define cchTextLimitDefault 0x7fff

#define RICHEDIT_CLASS20A	"RichEdit20A"
#define RICHEDIT_CLASS20W	L"RichEdit20W"
#define RICHEDIT_CLASS10A	"RICHEDIT"

#if (_RICHEDIT_VER >= 0x0200 )
#define RICHEDIT_CLASS		WINELIB_NAME_AW(RICHEDIT_CLASS20)
#else
#define RICHEDIT_CLASS		RICHEDIT_CLASS10A
#endif

#define EM_CANPASTE		(WM_USER + 50)
#define EM_DISPLAYBAND		(WM_USER + 51)
#define EM_EXGETSEL		(WM_USER + 52)
#define EM_EXLIMITTEXT		(WM_USER + 53)
#define EM_EXLINEFROMCHAR	(WM_USER + 54)
#define EM_EXSETSEL		(WM_USER + 55)
#define EM_FINDTEXT		(WM_USER + 56)
#define EM_FORMATRANGE		(WM_USER + 57)
#define EM_GETCHARFORMAT	(WM_USER + 58)
#define EM_GETEVENTMASK		(WM_USER + 59)
#define EM_GETOLEINTERFACE	(WM_USER + 60)
#define EM_GETPARAFORMAT	(WM_USER + 61)
#define EM_GETSELTEXT		(WM_USER + 62)
#define EM_HIDESELECTION	(WM_USER + 63)
#define EM_PASTESPECIAL		(WM_USER + 64)
#define EM_REQUESTRESIZE	(WM_USER + 65)
#define EM_SELECTIONTYPE	(WM_USER + 66)
#define EM_SETBKGNDCOLOR	(WM_USER + 67)
#define EM_SETCHARFORMAT	(WM_USER + 68)
#define EM_SETEVENTMASK		(WM_USER + 69)
#define EM_SETOLECALLBACK	(WM_USER + 70)
#define EM_SETPARAFORMAT	(WM_USER + 71)
#define EM_SETTARGETDEVICE	(WM_USER + 72)
#define EM_STREAMIN		(WM_USER + 73)
#define EM_STREAMOUT		(WM_USER + 74)
#define EM_GETTEXTRANGE		(WM_USER + 75)
#define EM_FINDWORDBREAK	(WM_USER + 76)
#define EM_SETOPTIONS		(WM_USER + 77)
#define EM_GETOPTIONS		(WM_USER + 78)
#define EM_FINDTEXTEX		(WM_USER + 79)
#define EM_GETWORDBREAKPROCEX	(WM_USER + 80)
#define EM_SETWORDBREAKPROCEX	(WM_USER + 81)

typedef DWORD (* CALLBACK EDITSTREAMCALLBACK)( DWORD, LPBYTE, LONG, LONG );

typedef struct
{
    DWORD		dwCookie;
    DWORD		dwError;
    EDITSTREAMCALLBACK	pfnCallback;
} EDITSTREAM;

#define SF_TEXT		0x0001
#define SF_RTF		0x0002
#define SF_RTFNOOBJS	0x0003
#define SF_TEXTIZED	0x0004



#ifdef __cplusplus
}
#endif

#include "poppack.h"

#endif /* __WINE_RICHEDIT_H */
