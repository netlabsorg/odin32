/*
 * RichEdit32  functions
 *
 * This module is a simple wrapper for the edit controls.
 * At the point, it is good only for application who use the RICHEDIT
 * control to display RTF text.
 *
 * Copyright 2000 by Jean-Claude Batista
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
#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winreg.h"
#include "winerror.h"
#include "riched32.h"
#include "richedit.h"
#include "charlist.h"
#define NO_SHLWAPI_STREAM
#include "shlwapi.h"

#include "rtf.h"
#include "rtf2text.h"
#include "wine/debug.h"

#define ID_EDIT      1

WINE_DEFAULT_DEBUG_CHANNEL(richedit);

#ifdef __WIN32OS2__
/** custom build hack. */
BOOL (* WINAPI pfnCustomRichedHack_EM_STREAMIN)(HWND, LPSTR) = NULL;
#endif

HANDLE RICHED32_hHeap = (HANDLE)NULL;
/* LPSTR  RICHED32_aSubclass = (LPSTR)NULL; */

#define DPRINTF_EDIT_MSG32(str) \
        TRACE(\
                     "32 bit : " str ": hwnd=%p, wParam=%08x, lParam=%08x\n"\
                     , \
                     hwnd, (UINT)wParam, (UINT)lParam)


static INT CALLBACK EDIT_WordBreakProc(LPWSTR s, INT index, INT count, INT action);

#ifdef __WIN32OS2__
#include <dbglog.h>

#define RICHEDIT_WND_PROP	"RICHEDIT_PROP"

FARPROC  pfnEditProcA = (FARPROC)DefWindowProcA;
FARPROC  pfnEditProcW = (FARPROC)DefWindowProcW;

typedef struct {
  CHARFORMAT2A cf;
  HBRUSH       hbrBackground;
} RICHEDIT_INFO;
#endif

/***********************************************************************
 * DllMain [Internal] Initializes the internal 'RICHED32.DLL'.
 *
 * PARAMS
 *     hinstDLL    [I] handle to the DLL's instance
 *     fdwReason   [I]
 *     lpvReserved [I] reserved, must be NULL
 *
 * RETURNS
 *     Success: TRUE
 *     Failure: FALSE
 */

#ifdef __WIN32OS2__
BOOL WINAPI RICHED32_LibMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
#else
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
#endif
{
    TRACE("\n");
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        /* create private heap */
        RICHED32_hHeap = HeapCreate (0, 0x10000, 0);
        /* register the Rich Edit class */
        RICHED32_Register ();
        break;

    case DLL_PROCESS_DETACH:
        /* unregister all common control classes */
        RICHED32_Unregister ();
        HeapDestroy (RICHED32_hHeap);
        RICHED32_hHeap = (HANDLE)NULL;
        break;
    }
    return TRUE;
}

/* Support routines for window procedure */
   INT RICHEDIT_GetTextRange(HWND hwnd,TEXTRANGEA *tr);
   INT RICHEDIT_GetSelText(HWND hwnd,LPSTR lpstrBuffer);


/*
 *
 * DESCRIPTION:
 * Window procedure of the RichEdit control.
 *
 */
static LRESULT WINAPI RICHED32_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam)
{
    int RTFToBuffer(char* pBuffer, int nBufferSize);
    LONG newstyle = 0;
    LONG style = 0;

    HWND hwndParent = GetParent(hwnd);
    char* rtfBuffer;
    HANDLE hProp = 0;
    int rtfBufferSize;

    CHARRANGE *cr;
    FARPROC pfnEditProc;

    TRACE("uMsg: 0x%x hwnd: %p",uMsg,hwnd);

    if(IsWindowUnicode(hwnd)) {
         pfnEditProc = pfnEditProcW;
    } 
    else pfnEditProc = pfnEditProcA;

    switch (uMsg)
    {

    case WM_CREATE :
    {
        RICHEDIT_INFO *prinfo;
        LRESULT ret;

        DPRINTF_EDIT_MSG32("WM_CREATE");

        ret = pfnEditProc(hwnd, WM_CREATE, wParam, lParam);
        if(ret) return ret; /* window creation cancelled */

        hProp = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, sizeof(RICHEDIT_INFO));
        SetPropA(hwnd, RICHEDIT_WND_PROP, hProp);          
        return 0 ;
    }

    case WM_DESTROY:
    {
        RICHEDIT_INFO *prinfo;

        hProp = GetPropA(hwnd, RICHEDIT_WND_PROP);
        prinfo = (RICHEDIT_INFO *)GlobalLock(hProp);
        if(prinfo) {
            //Destroy old brush if present
            if(prinfo->hbrBackground) DeleteObject(prinfo->hbrBackground);
            GlobalUnlock(hProp);
        }

        if(hProp) GlobalFree(hProp);
        RemovePropA(hwnd, RICHEDIT_WND_PROP);

        return pfnEditProc(hwnd, uMsg, wParam, lParam);
    }

    /* Messages specific to Richedit controls */

    case EM_STREAMIN:
    {
            DPRINTF_EDIT_MSG32("EM_STREAMIN");

	    /* setup the RTF parser */
	    RTFSetEditStream(( EDITSTREAM*)lParam);
	    rtfFormat = wParam&(SF_TEXT|SF_RTF);
	    WriterInit();
	    RTFInit ();
	    BeginFile();

	    /* do the parsing */
	    RTFRead ();

	    rtfBufferSize = RTFToBuffer(NULL, 0);
	    rtfBuffer = HeapAlloc(RICHED32_hHeap, 0,rtfBufferSize*sizeof(char));
	    if(rtfBuffer)
	    {
	    	RTFToBuffer(rtfBuffer, rtfBufferSize);
#ifdef __WIN32OS2__
                /* This is a very very ugly hack to work around some major parsing errors. */
                if (pfnCustomRichedHack_EM_STREAMIN)
            	    pfnCustomRichedHack_EM_STREAMIN(hwnd, rtfBuffer);
                else
#endif
            	SetWindowTextA(hwnd,rtfBuffer);
	    	HeapFree(RICHED32_hHeap, 0,rtfBuffer);
	    }
	    else
		WARN("Not enough memory for a allocating rtfBuffer\n");

            return 0;
    }
 
    case EM_AUTOURLDETECT:
            DPRINTF_EDIT_MSG32("EM_AUTOURLDETECT Ignored");
	    return 0;

    case EM_CANPASTE:
            DPRINTF_EDIT_MSG32("EM_CANPASTE Ignored");
	    return 0;

    case EM_CANREDO:
            DPRINTF_EDIT_MSG32("EM_CANREDO Ignored");
	    return 0;

    case EM_DISPLAYBAND:
            DPRINTF_EDIT_MSG32("EM_DISPLAYBAND Ignored");
	    return 0;

    case EM_EXGETSEL:
            DPRINTF_EDIT_MSG32("EM_EXGETSEL -> EM_GETSEL");
            cr = (VOID *) lParam;
            pfnEditProc( hwnd, EM_GETSEL, (INT)&cr->cpMin, (INT)&cr->cpMax);
            TRACE("cpMin: 0x%x cpMax: 0x%x\n",(INT)cr->cpMin,(INT)cr->cpMax);
            return 0;

    case EM_EXLIMITTEXT:
        {
           DWORD limit = lParam;
           DPRINTF_EDIT_MSG32("EM_EXLIMITTEXT");
           if (limit > 65534)
           {
                limit = 0xFFFFFFFF;
           }
           return pfnEditProc(hwnd,EM_SETLIMITTEXT,limit,0);
        }

    case EM_EXLINEFROMCHAR:
            DPRINTF_EDIT_MSG32("EM_EXLINEFROMCHAR -> LINEFROMCHAR");
            return pfnEditProc( hwnd, EM_LINEFROMCHAR, lParam, wParam);

    case EM_EXSETSEL:
            DPRINTF_EDIT_MSG32("EM_EXSETSEL -> EM_SETSEL");
            cr = (VOID *) lParam;
            pfnEditProc( hwnd, EM_SETSEL, cr->cpMin, cr->cpMax);
            return 0;

    case EM_FINDTEXT:
            DPRINTF_EDIT_MSG32("EM_FINDTEXT Ignored");
            return 0;

    case EM_FINDTEXTEX:
            DPRINTF_EDIT_MSG32("EM_FINDTEXTEX Ignored");
            return 0;

    case EM_FINDTEXTEXW:
            DPRINTF_EDIT_MSG32("EM_FINDTEXTEXW Ignored");
            return 0;

    case EM_FINDTEXTW:
            DPRINTF_EDIT_MSG32("EM_FINDTEXTW Ignored");
            return 0;

    case EM_FINDWORDBREAK:
    {
            DWORD ret = 0;
            DWORD len = GetWindowTextLengthA(hwnd);
            LPWSTR lpszText = (LPWSTR)HeapAlloc(RICHED32_hHeap, 0, (len+1)*sizeof(WCHAR));

            if(lpszText == NULL) {
                DebugInt3();
                return 0;
            }
            lpszText[0] = 0;
            GetWindowTextW(hwnd, lpszText, len);

            DPRINTF_EDIT_MSG32("EM_FINDWORDBREAK: partly implemented");
            switch(wParam) {
            case WB_ISDELIMITER:
            case WB_LEFT:
            case WB_RIGHT:
                ret = EDIT_WordBreakProc(lpszText, lParam, len, wParam);
                break;
            }
            HeapFree(RICHED32_hHeap, 0, lpszText);
            return ret;
    }

    case EM_FORMATRANGE:
            DPRINTF_EDIT_MSG32("EM_FORMATRANGE Ignored");
            return 0;

    case EM_GETAUTOURLDETECT:
            DPRINTF_EDIT_MSG32("EM_GETAUTOURLDETECT Ignored");
            return 0;

    case EM_GETBIDIOPTIONS:
            DPRINTF_EDIT_MSG32("EM_GETBIDIOPTIONS Ignored");
            return 0;

    case EM_GETCHARFORMAT:
            DPRINTF_EDIT_MSG32("EM_GETCHARFORMAT Ignored");
            return 0;

    case EM_GETEDITSTYLE:
            DPRINTF_EDIT_MSG32("EM_GETEDITSTYLE Ignored");
            return 0;

    case EM_GETEVENTMASK:
            DPRINTF_EDIT_MSG32("EM_GETEVENTMASK Ignored");
            return 0;

    case EM_GETIMECOLOR:
            DPRINTF_EDIT_MSG32("EM_GETIMECOLOR Ignored");
            return 0;

    case EM_GETIMECOMPMODE:
            DPRINTF_EDIT_MSG32("EM_GETIMECOMPMODE Ignored");
            return 0;

    case EM_GETIMEOPTIONS:
            DPRINTF_EDIT_MSG32("EM_GETIMEOPTIONS Ignored");
            return 0;

    case EM_GETLANGOPTIONS:
            DPRINTF_EDIT_MSG32("STUB: EM_GETLANGOPTIONS");
            return 0;

    case EM_GETOLEINTERFACE:
            DPRINTF_EDIT_MSG32("EM_GETOLEINTERFACE Ignored");
            return 0;

    case EM_GETOPTIONS:
            DPRINTF_EDIT_MSG32("EM_GETOPTIONS Ignored");
            return 0;

    case EM_GETPARAFORMAT:
            DPRINTF_EDIT_MSG32("EM_GETPARAFORMAT Ignored");
            return 0;

    case EM_GETPUNCTUATION:
            DPRINTF_EDIT_MSG32("EM_GETPUNCTUATION Ignored");
            return 0;

    case EM_GETREDONAME:
            DPRINTF_EDIT_MSG32("EM_GETREDONAME Ignored");
            return 0;

    case EM_GETSCROLLPOS:
            DPRINTF_EDIT_MSG32("EM_GETSCROLLPOS Ignored");
            return 0;

    case EM_GETSELTEXT:
            DPRINTF_EDIT_MSG32("EM_GETSELTEXT");
            return RICHEDIT_GetSelText(hwnd,(void *)lParam);

    case EM_GETTEXTEX:
            DPRINTF_EDIT_MSG32("EM_GETTEXTEX Ignored");
            return 0;

    case EM_GETTEXTLENGTHEX:
            DPRINTF_EDIT_MSG32("EM_GETTEXTLENGTHEX Ignored");
            return 0;

    case EM_GETTEXTMODE:
            DPRINTF_EDIT_MSG32("EM_GETTEXTMODE Ignored");
            return 0;

    case EM_GETTEXTRANGE:
            DPRINTF_EDIT_MSG32("EM_GETTEXTRANGE");
            return RICHEDIT_GetTextRange(hwnd,(TEXTRANGEA *)lParam);

    case EM_GETTYPOGRAPHYOPTIONS:
            DPRINTF_EDIT_MSG32("EM_GETTYPOGRAPHYOPTIONS Ignored");
            return 0;

    case EM_GETUNDONAME:
            DPRINTF_EDIT_MSG32("EM_GETUNDONAME Ignored");
            return 0;

    case EM_GETWORDBREAKPROCEX:
            DPRINTF_EDIT_MSG32("EM_GETWORDBREAKPROCEX Ignored");
            return 0;

    case EM_GETWORDWRAPMODE:
            DPRINTF_EDIT_MSG32("EM_GETWORDWRAPMODE Ignored");
            return 0;

    case EM_GETZOOM:
            DPRINTF_EDIT_MSG32("EM_GETZOOM Ignored");
            return 0;

    case EM_HIDESELECTION:
            DPRINTF_EDIT_MSG32("EM_HIDESELECTION Ignored");
            return 0;

    case EM_PASTESPECIAL:
            DPRINTF_EDIT_MSG32("EM_PASTESPECIAL Ignored");
            return 0;

    case EM_RECONVERSION:
            DPRINTF_EDIT_MSG32("EM_RECONVERSION Ignored");
            return 0;

    case EM_REDO:
            DPRINTF_EDIT_MSG32("EM_REDO Ignored");
            return 0;

    case EM_REQUESTRESIZE:
            DPRINTF_EDIT_MSG32("EM_REQUESTRESIZE Ignored");
            return 0;

    case EM_SELECTIONTYPE:
            DPRINTF_EDIT_MSG32("EM_SELECTIONTYPE Ignored");
            return 0;

    case EM_SETBIDIOPTIONS:
            DPRINTF_EDIT_MSG32("EM_SETBIDIOPTIONS Ignored");
            return 0;

    case EM_SETBKGNDCOLOR:
    {
            RICHEDIT_INFO *prinfo;

            DPRINTF_EDIT_MSG32("EM_SETBKGNDCOLOR");

            hProp = GetPropA(hwnd, RICHEDIT_WND_PROP);
            prinfo = (RICHEDIT_INFO *)GlobalLock(hProp);
            if(prinfo) 
            {
                prinfo->cf.dwMask     |= CFM_BACKCOLOR;
                prinfo->cf.crBackColor = (wParam) ? GetSysColor(COLOR_BACKGROUND) : (COLORREF)lParam;

                //Destroy old brush if present
                if(prinfo->hbrBackground) DeleteObject(prinfo->hbrBackground);

                //Create a brush that we return in WM_CTLCOLORSTATIC
                prinfo->hbrBackground  = (DWORD)CreateSolidBrush(prinfo->cf.crBackColor);
 
                dprintf(("Set background color to %x brush %x", prinfo->cf.crBackColor, prinfo->hbrBackground));

                GlobalUnlock(hProp);
            }
            return 0;
    }

    case EM_SETCHARFORMAT:
    {
            CHARFORMAT2A *pnewcf = (CHARFORMAT2A *)lParam;
            RICHEDIT_INFO *prinfo;

            DPRINTF_EDIT_MSG32("EM_SETCHARFORMAT: not completely implemented!!");

            hProp = GetPropA(hwnd, RICHEDIT_WND_PROP);
            prinfo = (RICHEDIT_INFO *)GlobalLock(hProp);
            if(prinfo && pnewcf && pnewcf->cbSize >= sizeof(CHARFORMATA)) 
            {
                if((pnewcf->dwMask & CFM_COLOR) && !(pnewcf->dwEffects & CFE_AUTOCOLOR)) {
                    prinfo->cf.dwMask     |= CFM_COLOR;
                    prinfo->cf.crTextColor = pnewcf->crTextColor;
                    dprintf(("Set text color to %x", prinfo->cf.crTextColor));
                }
                if(pnewcf->cbSize == sizeof(CHARFORMAT2A)) 
                {
                    if((pnewcf->dwMask & CFM_BACKCOLOR) && !(pnewcf->dwEffects & CFE_AUTOBACKCOLOR)) 
                    {
                        prinfo->cf.dwMask     |= CFM_BACKCOLOR;
                        prinfo->cf.crBackColor = pnewcf->crBackColor;
 
                        //Destroy old brush if present
                        if(prinfo->hbrBackground) DeleteObject(prinfo->hbrBackground);

                        //Create a brush that we return in WM_CTLCOLORSTATIC
                        prinfo->hbrBackground = (DWORD)CreateSolidBrush(prinfo->cf.crBackColor);
 
                        dprintf(("Set background color to %x brush %x", prinfo->cf.crBackColor, prinfo->hbrBackground));
                    }
                }
            }

            if(prinfo) GlobalUnlock(hProp);
            return 0;
    }

    case EM_SETEDITSTYLE:
            DPRINTF_EDIT_MSG32("EM_SETEDITSTYLE Ignored");
            return 0;

    case EM_SETEVENTMASK:
            DPRINTF_EDIT_MSG32("EM_SETEVENTMASK Ignored");
            return 0;

    case EM_SETFONTSIZE:
            DPRINTF_EDIT_MSG32("EM_SETFONTSIZE Ignored");
            return 0;

    case EM_SETIMECOLOR:
            DPRINTF_EDIT_MSG32("EM_SETIMECOLO Ignored");
            return 0;

    case EM_SETIMEOPTIONS:
            DPRINTF_EDIT_MSG32("EM_SETIMEOPTIONS Ignored");
            return 0;

    case EM_SETLANGOPTIONS:
            DPRINTF_EDIT_MSG32("EM_SETLANGOPTIONS Ignored");
            return 0;

    case EM_SETOLECALLBACK:
            DPRINTF_EDIT_MSG32("EM_SETOLECALLBACK Ignored");
            return 0;

    case EM_SETOPTIONS:
            DPRINTF_EDIT_MSG32("EM_SETOPTIONS Ignored");
            return 0;

    case EM_SETPALETTE:
            DPRINTF_EDIT_MSG32("EM_SETPALETTE Ignored");
            return 0;

    case EM_SETPARAFORMAT:
            DPRINTF_EDIT_MSG32("EM_SETPARAFORMAT Ignored");
            return 0;

    case EM_SETPUNCTUATION:
            DPRINTF_EDIT_MSG32("EM_SETPUNCTUATION Ignored");
            return 0;

    case EM_SETSCROLLPOS:
            DPRINTF_EDIT_MSG32("EM_SETSCROLLPOS Ignored");
            return 0;

    case EM_SETTARGETDEVICE:
            DPRINTF_EDIT_MSG32("EM_SETTARGETDEVICE Ignored");
            return 0;

    case EM_SETTEXTEX:
            DPRINTF_EDIT_MSG32("EM_SETTEXTEX Ignored");
            return 0;

    case EM_SETTEXTMODE:
            DPRINTF_EDIT_MSG32("EM_SETTEXTMODE Ignored");
            return 0;

    case EM_SETTYPOGRAPHYOPTIONS:
            DPRINTF_EDIT_MSG32("EM_SETTYPOGRAPHYOPTIONS Ignored");
            return 0;

    case EM_SETUNDOLIMIT:
            DPRINTF_EDIT_MSG32("EM_SETUNDOLIMIT Ignored");
            return 0;

    case EM_SETWORDBREAKPROCEX:
            DPRINTF_EDIT_MSG32("EM_SETWORDBREAKPROCEX Ignored");
            return 0;

    case EM_SETWORDWRAPMODE:
            DPRINTF_EDIT_MSG32("EM_SETWORDWRAPMODE Ignored");
            return 0;

    case EM_SETZOOM:
            DPRINTF_EDIT_MSG32("EM_SETZOOM Ignored");
            return 0;

    case EM_SHOWSCROLLBAR:
            DPRINTF_EDIT_MSG32("EM_SHOWSCROLLBAR Ignored");
            return 0;

    case EM_STOPGROUPTYPING:
            DPRINTF_EDIT_MSG32("EM_STOPGROUPTYPING Ignored");
            return 0;

    case EM_STREAMOUT:
            DPRINTF_EDIT_MSG32("EM_STREAMOUT Ignored");
            return 0;

#if 0
    case WM_CTLCOLORSTATIC:
#ifdef __WIN32OS2__
    case WM_CTLCOLOREDIT:
    {
            RICHEDIT_INFO *prinfo;
            HBRUSH hBrush = 0;
            HDC hdc = (HDC)wParam;

            hProp = GetPropA(hwnd, RICHEDIT_WND_PROP);
            prinfo = (RICHEDIT_INFO *)GlobalLock(hProp);
            if(prinfo)
            {
                if(prinfo->cf.dwMask & CFM_BACKCOLOR) {
                    SetBkColor(hdc, prinfo->cf.crBackColor);
                    hBrush = prinfo->hbrBackground;
                }
                if(prinfo->cf.dwMask & CFM_COLOR) {
                    SetTextColor(hdc, prinfo->cf.crTextColor);
                }
            }
            if(prinfo) GlobalUnlock(hProp);

            if(hBrush) return hBrush;          
    }
#endif
        DPRINTF_EDIT_MSG32("WM_CTLCOLORSTATIC Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
#endif

   /* Edit control messages that are different for RichEdit controls */
   case EM_CHARFROMPOS:
   {
        POINTL *lpPoint = (POINTL *)lParam;
        DWORD   curpos;

        curpos = pfnEditProc(hwnd, EM_CHARFROMPOS, wParam, MAKELPARAM(lpPoint->x, lpPoint->y));
        TRACE("curpos: 0x%x richedit pos: 0x%x\n", curpos, LOWORD(curpos));
        return LOWORD(curpos);
   }

   /*
    * used by IE in the EULA box
    */
    case WM_ALTTABACTIVE:
        DPRINTF_EDIT_MSG32("WM_ALTTABACTIVE");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    }

    /* pass the rest to the edit window procedure */
    TRACE("Message 0x%x Passed to pfnEditProc hwnd=%p, wParam=%08x, lParam=%08x\n",
           uMsg, hwnd, (UINT)wParam, (UINT)lParam);

    return pfnEditProc(hwnd,uMsg,wParam,lParam);
}

/***********************************************************************
 * DllGetVersion [RICHED32.2]
 *
 * Retrieves version information of the 'RICHED32.DLL'
 *
 * PARAMS
 *     pdvi [O] pointer to version information structure.
 *
 * RETURNS
 *     Success: S_OK
 *     Failure: E_INVALIDARG
 *
 * NOTES
 *     Returns version of a comctl32.dll from IE4.01 SP1.
 */

HRESULT WINAPI
RICHED32_DllGetVersion (DLLVERSIONINFO *pdvi)
{
    TRACE("\n");

    if (pdvi->cbSize != sizeof(DLLVERSIONINFO)) {

	return E_INVALIDARG;
    }

    pdvi->dwMajorVersion = 4;
    pdvi->dwMinorVersion = 0;
    pdvi->dwBuildNumber = 0;
    pdvi->dwPlatformID = 0;

    return S_OK;
}

/***
 * DESCRIPTION:
 * Registers the window class.
 *
 * PARAMETER(S):
 * None
 *
 * RETURN:
 * None
 */
VOID RICHED32_Register(void)
{
    WNDCLASSA wndClass;

#ifdef __WIN32OS2__
    WNDCLASSA classinfoA;
    WNDCLASSW classinfoW;

    if(GetClassInfoA(NULL, "EDIT", &classinfoA)) 
    {
        pfnEditProcA = classinfoA.lpfnWndProc;
    }
    else DebugInt3();
    if(GetClassInfoW(NULL, L"EDIT", &classinfoW)) 
    {
        pfnEditProcW = classinfoW.lpfnWndProc;
    }
    else DebugInt3();

    ZeroMemory(&wndClass, sizeof(WNDCLASSA));
    wndClass.style = classinfoA.style;
    wndClass.lpfnWndProc = (WNDPROC)RICHED32_WindowProc;
    wndClass.cbClsExtra = classinfoA.cbClsExtra;
    wndClass.cbWndExtra = classinfoA.cbWndExtra;
    wndClass.hCursor = classinfoA.hCursor;
    wndClass.hbrBackground = classinfoA.hbrBackground;
    wndClass.lpszClassName = RICHEDIT_CLASS10A; /* WC_RICHED32A; */
#else
    TRACE("\n");

    ZeroMemory(&wndClass, sizeof(WNDCLASSA));
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
    wndClass.lpfnWndProc = (WNDPROC)RICHED32_WindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0; /*(sizeof(RICHED32_INFO *);*/
    wndClass.hCursor = LoadCursorA(0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszClassName = RICHEDIT_CLASS10A; /* WC_RICHED32A; */
#endif

    RegisterClassA (&wndClass);
}

/***
 * DESCRIPTION:
 * Unregisters the window class.
 *
 * PARAMETER(S):
 * None
 *
 * RETURN:
 * None
 */
VOID RICHED32_Unregister(void)
{
    TRACE("\n");

    UnregisterClassA(RICHEDIT_CLASS10A, (HINSTANCE)NULL);

#ifdef __WIN32OS2__
    pfnEditProcA = (FARPROC)DefWindowProcA;
    pfnEditProcW = (FARPROC)DefWindowProcW;
#endif
}

INT RICHEDIT_GetTextRange(HWND hwnd,TEXTRANGEA *tr)
{
    UINT alloc_size, text_size, range_size;
    char *text;

    TRACE("start: 0x%x stop: 0x%x\n",(INT)tr->chrg.cpMin,(INT)tr->chrg.cpMax);

    if (!(alloc_size = SendMessageA(hwnd,WM_GETTEXTLENGTH,0,0))) return FALSE;
    if (!(text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (alloc_size+1))))
		return FALSE;
    text_size = SendMessageA(hwnd,WM_GETTEXT,alloc_size,(INT)text);

    if (text_size > tr->chrg.cpMin)
    {
       range_size = (text_size> tr->chrg.cpMax) ? (tr->chrg.cpMax - tr->chrg.cpMin) : (text_size - tr->chrg.cpMin);
       TRACE("EditText: %.30s ...\n",text+tr->chrg.cpMin);
       memcpy(tr->lpstrText,text+tr->chrg.cpMin,range_size);
    }
    else range_size = 0;
    HeapFree(GetProcessHeap(), 0, text);

    return range_size;
}

INT RICHEDIT_GetSelText(HWND hwnd,LPSTR lpstrBuffer)
{
    TEXTRANGEA textrange;

    textrange.lpstrText = lpstrBuffer;
    SendMessageA(hwnd,EM_GETSEL,(INT)&textrange.chrg.cpMin,(INT)&textrange.chrg.cpMax);
    return RICHEDIT_GetTextRange(hwnd,&textrange);
}

/*********************************************************************
 *
 *	EDIT_WordBreakProc
 *
 *	Find the beginning of words.
 *	Note:	unlike the specs for a WordBreakProc, this function only
 *		allows to be called without linebreaks between s[0] upto
 *		s[count - 1].  Remember it is only called
 *		internally, so we can decide this for ourselves.
 *
 */
static INT CALLBACK EDIT_WordBreakProc(LPWSTR s, INT index, INT count, INT action)
{
	INT ret = 0;

	TRACE("s=%p, index=%d, count=%d, action=%d\n", s, index, count, action);

	if(!s) return 0;

	switch (action) {
	case WB_LEFT:
		if (!count)
			break;
		if (index)
			index--;
		if (s[index] == ' ') {
			while (index && (s[index] == ' '))
				index--;
			if (index) {
				while (index && (s[index] != ' '))
					index--;
				if (s[index] == ' ')
					index++;
			}
		} else {
			while (index && (s[index] != ' '))
				index--;
			if (s[index] == ' ')
				index++;
		}
		ret = index;
		break;
	case WB_RIGHT:
		if (!count)
			break;
		if (index)
			index--;
		if (s[index] == ' ')
			while ((index < count) && (s[index] == ' ')) index++;
		else {
			while (s[index] && (s[index] != ' ') && (index < count))
				index++;
			while ((s[index] == ' ') && (index < count)) index++;
		}
		ret = index;
		break;
	case WB_ISDELIMITER:
		ret = (s[index] == ' ');
		break;
	default:
		ERR("unknown action code, please report !\n");
		break;
	}
	return ret;
}


#ifdef __WIN32OS2__
/** Enables a callback replacement for the SetWindowTextA call finalizing
 * the handling of an EM_STREAMIN message.
 * @remark don't use this hack.
 */
BOOL WINAPI ODIN_RichedHack_EM_STREAMIN(BOOL (* WINAPI pfn)(HWND, LPSTR))
{
    pfnCustomRichedHack_EM_STREAMIN = pfn;
    return TRUE;
}
#endif

