/*
 *	common shell dialogs
 *
 * Copyright 2000 Juergen Schmied
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string.h>
#include <stdio.h>
#include "winerror.h"
#include "winreg.h"
#include "commdlg.h"
#include "wine/debug.h"

#include "shellapi.h"
#include "shlobj.h"
#include "shell32_main.h"
#include "undocshell.h"

typedef struct 
    {
	HWND hwndOwner ;
	HICON hIcon ;
	LPCSTR lpstrDirectory ;
	LPCSTR lpstrTitle ;
	LPCSTR lpstrDescription ;
	UINT uFlags ;
    } RUNFILEDLGPARAMS ;
    
typedef BOOL (*LPFNOFN) (OPENFILENAMEA *) ;

WINE_DEFAULT_DEBUG_CHANNEL(shell);
BOOL CALLBACK RunDlgProc (HWND, UINT, WPARAM, LPARAM) ;
void FillList (HWND, char *) ;


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

    RUNFILEDLGPARAMS rfdp = {hwndOwner, hIcon, lpstrDirectory, lpstrTitle, lpstrDescription, uFlags} ;
    HRSRC hRes;
    LPVOID template;
    TRACE("\n");

    if(!(hRes = FindResourceA(shell32_hInstance, "SHELL_RUN_DLG", RT_DIALOGA)))
        {
        MessageBoxA (hwndOwner, "Couldn't find dialog.", "Nix", MB_OK) ;
        return;
        }
    if(!(template = (LPVOID)LoadResource(shell32_hInstance, hRes)))
        {
        MessageBoxA (hwndOwner, "Couldn't load dialog.", "Nix", MB_OK) ;
        return;
        }

    DialogBoxIndirectParamA(GetWindowLongA( hwndOwner, GWL_HINSTANCE ),
                                      template, hwndOwner, RunDlgProc, (LPARAM)&rfdp);

}

/* Dialog procedure for RunFileDlg */
BOOL CALLBACK RunDlgProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    int ic ;
    char *psz, szMsg[256] ;
    static RUNFILEDLGPARAMS *prfdp = NULL ;
    
    switch (message)
        {
        case WM_INITDIALOG :
            prfdp = (RUNFILEDLGPARAMS *)lParam ;
            SetWindowTextA (hwnd, prfdp->lpstrTitle) ;
            SetClassLongA (hwnd, GCL_HICON, prfdp->hIcon) ;
            SendMessageA (GetDlgItem (hwnd, 12297), STM_SETICON, (WPARAM)LoadIconA ((HINSTANCE)NULL, IDI_WINLOGOA), 0) ;
            FillList (GetDlgItem (hwnd, 12298), NULL) ;
            SetFocus (GetDlgItem (hwnd, 12298)) ;
            return TRUE ;
            
        case WM_COMMAND :
            {
            STARTUPINFOA si ;
            PROCESS_INFORMATION pi ;
            
            si.cb = sizeof (STARTUPINFOA) ;
            si.lpReserved = NULL ;
            si.lpDesktop = NULL ;
            si.lpTitle = NULL ;
            si.dwX = 0 ;
            si.dwY = 0 ;
            si.dwXSize = 0 ;
            si.dwYSize = 0 ;
            si.dwXCountChars = 0 ;
            si.dwYCountChars = 0 ;
            si.dwFillAttribute = 0 ;
            si.dwFlags = 0 ;
            si.cbReserved2 = 0 ;
            si.lpReserved2 = NULL ;
            
            switch (LOWORD (wParam))
                {
                case IDOK :
                    {
                    HWND htxt = (HWND)NULL ;
                    if ((ic = GetWindowTextLengthA (htxt = GetDlgItem (hwnd, 12298))))
                        {
                        psz = malloc (ic + 2) ;
                        GetWindowTextA (htxt, psz, ic + 1) ;
                        
                        if (!CreateProcessA (NULL, psz, NULL, NULL, TRUE,
                            NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
                            {
                            char *pszSysMsg = NULL ;
                            FormatMessageA (
                                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                FORMAT_MESSAGE_FROM_SYSTEM |
                                FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL, GetLastError (),
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPSTR)&pszSysMsg, 0, NULL
                                ) ;
                            sprintf (szMsg, "Error: %s", pszSysMsg) ;
                            LocalFree ((HLOCAL)pszSysMsg) ;
                            MessageBoxA (hwnd, szMsg, "Nix", MB_OK | MB_ICONEXCLAMATION) ;
                            
                            free (psz) ;
                            SendMessageA (htxt, CB_SETEDITSEL, 0, MAKELPARAM (0, -1)) ;
                            return TRUE ;
                            }
                        FillList (htxt, psz) ;
                        free (psz) ;
                        EndDialog (hwnd, 0) ;
                        }
                    }
                    
                case IDCANCEL :
                    EndDialog (hwnd, 0) ;
                    return TRUE ;
                
                case 12288 :
                    {
                    HMODULE hComdlg = (HMODULE)NULL ;
                    LPFNOFN ofnProc = NULL ;
                    static char szFName[1024] = "", szFileTitle[256] = "", szInitDir[768] = "" ;
                    static OPENFILENAMEA ofn =
                        {
                        sizeof (OPENFILENAMEA),
                        (HWND)NULL,
                        (HINSTANCE)NULL,
                        "Executable Files\0*.exe\0All Files\0*.*\0\0\0\0",
                        (LPSTR)NULL,
                        0,
                        0,
                        szFName,
                        1023,
                        szFileTitle,
                        255,
                        (LPCSTR)szInitDir,
                        "Browse",
                        OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
                        0,
                        0, 
                        (LPCSTR)NULL,
                        0,
                        (LPOFNHOOKPROC)NULL, 
                        (LPCSTR)NULL
                        } ;
                    
                    ofn.hwndOwner = hwnd ;
                    
                    if ((HMODULE)NULL == (hComdlg = LoadLibraryExA ("comdlg32", (HANDLE)NULL, 0)))
                        {
                        MessageBoxA (hwnd, "Unable to display dialog box (LoadLibraryEx) !", "Nix", MB_OK | MB_ICONEXCLAMATION) ;
                        return TRUE ;
                        }
                    
                    if ((LPFNOFN)NULL == (ofnProc = (LPFNOFN)GetProcAddress (hComdlg, "GetOpenFileNameA")))
                        {
                        MessageBoxA (hwnd, "Unable to display dialog box (GetProcAddress) !", "Nix", MB_OK | MB_ICONEXCLAMATION) ;
                        return TRUE ;
                        }
                    
                    ofnProc (&ofn) ;
                    
                    SetFocus (GetDlgItem (hwnd, IDOK)) ;
                    SetWindowTextA (GetDlgItem (hwnd, 12298), szFName) ;
                    SendMessageA (GetDlgItem (hwnd, 12298), CB_SETEDITSEL, 0, MAKELPARAM (0, -1)) ;
                    SetFocus (GetDlgItem (hwnd, IDOK)) ;
                    
                    FreeLibrary (hComdlg) ;
                    
                    return TRUE ;
                    }
                }
            return TRUE ;
            }
        }
        
    /* This should be DefDlgProcA, but that doesn't work */
    return FALSE ;
    }

/* This grabs the MRU list from the registry and fills the combo for the "Run" dialog above */
void FillList (HWND hCb, char *pszLatest)
    {
    HKEY hkey ;
/*    char szDbgMsg[256] = "" ; */
    char *pszList = NULL, *pszCmd = NULL, cMatch = 0, cMax = 0x60, szIndex[2] = "-" ;
    DWORD icList = 0, icCmd = 0 ;
    int Nix ;
    
    SendMessageA (hCb, CB_RESETCONTENT, 0, 0) ;
    
    if (ERROR_SUCCESS != RegCreateKeyExA (
        HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
        0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL))
        MessageBoxA (hCb, "Unable to open registry key !", "Nix", MB_OK) ;
        
    if (ERROR_SUCCESS != RegQueryValueExA (hkey, "MRUList", NULL, NULL, NULL, &icList))
        MessageBoxA (hCb, "Unable to grab size for MRUList !", "Nix", MB_OK) ;
    if (icList > 0)
        {
        pszList = malloc (icList) ;
        if (ERROR_SUCCESS != RegQueryValueExA (hkey, "MRUList", NULL, NULL, pszList, &icList))
            MessageBoxA (hCb, "Unable to grab MRUList !", "Nix", MB_OK) ;
        }
    else
        {
        pszList = malloc (icList = 1) ;
        pszList[0] = 0 ;
        }
    
    for (Nix = 0 ; Nix < icList - 1 ; Nix++)
        {
        if (pszList[Nix] > cMax)
            cMax = pszList[Nix] ;
            
        szIndex[0] = pszList[Nix] ;
        
        if (ERROR_SUCCESS != RegQueryValueExA (hkey, szIndex, NULL, NULL, NULL, &icCmd))
            MessageBoxA (hCb, "Unable to grab size of index", "Nix", MB_OK) ;
        pszCmd = realloc (pszCmd, icCmd) ;
        if (ERROR_SUCCESS != RegQueryValueExA (hkey, szIndex, NULL, NULL, pszCmd, &icCmd))
            MessageBoxA (hCb, "Unable to grab index", "Nix", MB_OK) ;
        
        if (NULL != pszLatest)
            {
            if (!strcasecmp (pszCmd, pszLatest))
                {
                /*
                sprintf (szDbgMsg, "Found existing (%d).\n", Nix) ;
                MessageBoxA (hCb, szDbgMsg, "Nix", MB_OK) ;
                */
                SendMessageA (hCb, CB_INSERTSTRING, 0, (LPARAM)pszCmd) ;
                SetWindowTextA (hCb, pszCmd) ;
                SendMessageA (hCb, CB_SETEDITSEL, 0, MAKELPARAM (0, -1)) ;
                    
                cMatch = pszList[Nix] ;
                memmove (&pszList[1], pszList, Nix) ;
                pszList[0] = cMatch ;
                continue ;
                }
            }
            
        if (26 != icList - 1 || icList - 2 != Nix || cMatch || NULL == pszLatest)
            {
            /*
            sprintf (szDbgMsg, "Happily appending (%d).\n", Nix) ;
            MessageBoxA (hCb, szDbgMsg, "Nix", MB_OK) ;
            */
            SendMessageA (hCb, CB_ADDSTRING, 0, (LPARAM)pszCmd) ;
            if (!Nix)
                {
                SetWindowTextA (hCb, pszCmd) ;
                SendMessageA (hCb, CB_SETEDITSEL, 0, MAKELPARAM (0, -1)) ;
                }
                
            }
        else
            {
            /*
            sprintf (szDbgMsg, "Doing loop thing.\n") ;
            MessageBoxA (hCb, szDbgMsg, "Nix", MB_OK) ;
            */
            SendMessageA (hCb, CB_INSERTSTRING, 0, (LPARAM)pszLatest) ;
            SetWindowTextA (hCb, pszLatest) ;
            SendMessageA (hCb, CB_SETEDITSEL, 0, MAKELPARAM (0, -1)) ;
        
            cMatch = pszList[Nix] ;
            memmove (&pszList[1], pszList, Nix) ;
            pszList[0] = cMatch ;
            szIndex[0] = cMatch ;
            RegSetValueExA (hkey, szIndex, 0, REG_SZ, pszLatest, strlen (pszLatest) + 1) ;
            }
        }

    if (!cMatch && NULL != pszLatest)
        {
        /*
        sprintf (szDbgMsg, "Simply inserting (increasing list).\n") ;
        MessageBoxA (hCb, szDbgMsg, "Nix", MB_OK) ;
        */
        SendMessageA (hCb, CB_INSERTSTRING, 0, (LPARAM)pszLatest) ;
        SetWindowTextA (hCb, pszLatest) ;
        SendMessageA (hCb, CB_SETEDITSEL, 0, MAKELPARAM (0, -1)) ;
        
        cMatch = ++cMax ;
        pszList = realloc (pszList, ++icList) ;
        memmove (&pszList[1], pszList, icList - 1) ;
        pszList[0] = cMatch ;
        szIndex[0] = cMatch ;
        RegSetValueExA (hkey, szIndex, 0, REG_SZ, pszLatest, strlen (pszLatest) + 1) ;
        }

    RegSetValueExA (hkey, "MRUList", 0, REG_SZ, pszList, strlen (pszList) + 1) ;
    
    free (pszCmd) ;    
    free (pszList) ;
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
	if (MessageBoxA( hWndOwner, "Do you want to exit Odin?", "Shutdown", MB_YESNO|MB_ICONQUESTION) == IDYES)
#else
	if (MessageBoxA( hWndOwner, "Do you want to exit WINE?", "Shutdown", MB_YESNO|MB_ICONQUESTION) == IDYES)
#endif
	{
	  SendMessageA ( hWndOwner, WM_QUIT, 0, 0);
	}
}
