/* $Id: dialogs.c,v 1.2 2001-04-28 13:33:43 sandervl Exp $ */
/*
 *	common shell dialogs
 */
#ifdef __WIN32OS2__
#define ICOM_CINTERFACE 1
#include <odin.h>
#endif
#include <string.h>
#include <stdio.h>
#include "winerror.h"
#include "debugtools.h"

#include "shellapi.h"
#include "shlobj.h"
#include "shell32_main.h"
#include "wine/undocshell.h"

DEFAULT_DEBUG_CHANNEL(shell);


/*************************************************************************
 * PickIconDlg					[SHELL32.62]
 * 
 */
BOOL WINAPI PickIconDlg(
	HWND hwndOwner,
	LPSTR lpstrFile,
	DWORD nMaxFile,
	LPDWORD lpdwIconIndex)
{
	FIXME("(%08x,%s,%08lx,%p):stub.\n",
	  hwndOwner, lpstrFile, nMaxFile,lpdwIconIndex);
	return 0xffffffff;
}

/*************************************************************************
 * RunFileDlg					[SHELL32.61]
 *
 * NOTES
 *     Original name: RunFileDlg (exported by ordinal)
 */
void WINAPI RunFileDlg(
	HWND hwndOwner,
	HICON hIcon,
	LPCSTR lpstrDirectory,
	LPCSTR lpstrTitle,
	LPCSTR lpstrDescription,
	UINT uFlags)
{
	FIXME("(0x%04x 0x%04x %s %s %s 0x%08x):stub.\n",
	   hwndOwner, hIcon, lpstrDirectory, lpstrTitle, lpstrDescription, uFlags);
}

/*************************************************************************
 * ExitWindowsDialog				[SHELL32.60]
 *
 * NOTES
 *     exported by ordinal
 */
void WINAPI ExitWindowsDialog (HWND hWndOwner)
{
	TRACE("(0x%08x)\n", hWndOwner);
#ifdef __WIN32OS2__
	if (MessageBoxA( hWndOwner, "Do you want to exit Odin?", "Shutdown", MB_YESNO|MB_ICONQUESTION) == IDOK)
#else
	if (MessageBoxA( hWndOwner, "Do you want to exit WINE?", "Shutdown", MB_YESNO|MB_ICONQUESTION) == IDOK)
#endif
	{
	  SendMessageA ( hWndOwner, WM_QUIT, 0, 0);
	}
}
