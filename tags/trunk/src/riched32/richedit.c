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

HANDLE RICHED32_hHeap = (HANDLE)NULL;
/* LPSTR  RICHED32_aSubclass = (LPSTR)NULL; */

#define DPRINTF_EDIT_MSG32(str) \
        TRACE(\
                     "32 bit : " str ": hwnd=%p, wParam=%08x, lParam=%08x\n"\
                     , \
                     hwnd, (UINT)wParam, (UINT)lParam)

#ifdef __WIN32OS2__
#define RICHEDIT_WND_PROP	"RICHEDIT_PROP"

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

#ifdef __WIN32OS2__
    HWND hwndEdit;
    HWND hwndParent;
    char* rtfBuffer;
    HANDLE hProp = 0;
    int rtfBufferSize;

    CHARRANGE *cr;
#else
    static HWND hwndEdit;
    static HWND hwndParent;
    static char* rtfBuffer;
    int rtfBufferSize;

    CHARRANGE *cr;
    TRACE("previous hwndEdit: %p hwndParent %p\n",hwndEdit,hwndParent);
#endif
    hwndEdit = GetWindow(hwnd,GW_CHILD);
    TRACE("uMsg: 0x%x hwnd: %p hwndEdit: %p\n",uMsg,hwnd,hwndEdit);

    switch (uMsg)
    {

    case WM_CREATE :
	    DPRINTF_EDIT_MSG32("WM_CREATE");

	    /* remove SCROLLBARS from the current window style */
	    hwndParent = ((LPCREATESTRUCTA) lParam)->hwndParent;

	    newstyle = style = ((LPCREATESTRUCTA) lParam)->style;
            newstyle &= ~WS_HSCROLL;
            newstyle &= ~WS_VSCROLL;
            newstyle &= ~ES_AUTOHSCROLL;
            newstyle &= ~ES_AUTOVSCROLL;

    TRACE("previous hwndEdit: %p\n",hwndEdit);
            hwndEdit = CreateWindowA ("edit", ((LPCREATESTRUCTA) lParam)->lpszName,
                                   style, 0, 0, 0, 0,
                                   hwnd, (HMENU) ID_EDIT,
                                   ((LPCREATESTRUCTA) lParam)->hInstance, NULL) ;
    TRACE("hwndEdit: %p hwnd: %p\n",hwndEdit,hwnd);

	    SetWindowLongA(hwnd,GWL_STYLE, newstyle);
#ifdef __WIN32OS2__
          {
            CHARFORMAT2A *pcf;

            hProp = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, sizeof(CHARFORMAT2A));
            SetPropA(hwnd, RICHEDIT_WND_PROP, hProp);
            pcf = (CHARFORMAT2A *)GlobalLock(hProp);
            if(pcf) {
                pcf->cbSize = sizeof(CHARFORMAT2A);
                GlobalUnlock(hProp);
            }
          }
#endif
            return 0 ;

    case WM_SETFOCUS :
	    DPRINTF_EDIT_MSG32("WM_SETFOCUS");
            SetFocus (hwndEdit) ;
            return 0 ;

    case WM_SIZE :
            DPRINTF_EDIT_MSG32("WM_SIZE");
            MoveWindow (hwndEdit, 0, 0, LOWORD (lParam), HIWORD (lParam), TRUE) ;
            return 0 ;

    case WM_COMMAND :
        DPRINTF_EDIT_MSG32("WM_COMMAND");
	switch(HIWORD(wParam)) {
		case EN_CHANGE:
		case EN_HSCROLL:
		case EN_KILLFOCUS:
		case EN_SETFOCUS:
		case EN_UPDATE:
		case EN_VSCROLL:
			return SendMessageA(hwndParent, WM_COMMAND,
				wParam, (LPARAM)(hwnd));

		case EN_ERRSPACE:
		case EN_MAXTEXT:
			MessageBoxA (hwnd, "RichEdit control out of space.",
                                  "ERROR", MB_OK | MB_ICONSTOP) ;
			return 0 ;
		}

    case EM_STREAMIN:
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
            	SetWindowTextA(hwndEdit,rtfBuffer);
	    	HeapFree(RICHED32_hHeap, 0,rtfBuffer);
	    }
	    else
		WARN("Not enough memory for a allocating rtfBuffer\n");

            return 0;

/* Messages specific to Richedit controls */

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
            if (hwndEdit) SendMessageA( hwndEdit, EM_GETSEL, (INT)&cr->cpMin, (INT)&cr->cpMax);
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
           return SendMessageA(hwndEdit,EM_SETLIMITTEXT,limit,0);
        }

    case EM_EXLINEFROMCHAR:
            DPRINTF_EDIT_MSG32("EM_EXLINEFROMCHAR -> LINEFROMCHAR");
            if (hwndEdit) return SendMessageA( hwndEdit, EM_LINEFROMCHAR, lParam, wParam);
            return 0;

    case EM_EXSETSEL:
            DPRINTF_EDIT_MSG32("EM_EXSETSEL -> EM_SETSEL");
            cr = (VOID *) lParam;
            if (hwndEdit) SendMessageA( hwndEdit, EM_SETSEL, cr->cpMin, cr->cpMax);
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
            DPRINTF_EDIT_MSG32("EM_FINDWORDBREAK Ignored");
            return 0;

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
            return RICHEDIT_GetSelText(hwndEdit,(void *)lParam);

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
            return RICHEDIT_GetTextRange(hwndEdit,(TEXTRANGEA *)lParam);

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
#ifdef __WIN32OS2__
{
            CHARFORMAT2A *pcf;

            hProp = GetPropA(hwnd, RICHEDIT_WND_PROP);
            pcf = (CHARFORMAT2A *)GlobalLock(hProp);
            if(pcf) 
            {
                pcf->dwMask     |= CFM_BACKCOLOR;
                pcf->crBackColor = (wParam) ? GetSysColor(COLOR_BACKGROUND) : (COLORREF)lParam;

                //Destroy old brush if present
                if(pcf->dwReserved) DeleteObject(pcf->dwReserved);

                //Create a brush that we return in WM_CTLCOLORSTATIC
                pcf->dwReserved  = (DWORD)CreateSolidBrush(pcf->crBackColor);
 
                dprintf(("Set background color to %x brush %x", pcf->crBackColor, pcf->dwReserved));

                GlobalUnlock(hProp);
            }
}
#endif
            DPRINTF_EDIT_MSG32("EM_SETBKGNDCOLOR Ignored");
            return 0;

    case EM_SETCHARFORMAT:
#ifdef __WIN32OS2__
    {
            CHARFORMAT2A *pnewcf = (CHARFORMAT2A *)lParam;
            CHARFORMAT2A *pcf;

            hProp = GetPropA(hwnd, RICHEDIT_WND_PROP);
            pcf = (CHARFORMAT2A *)GlobalLock(hProp);
            if(pcf && pnewcf && pnewcf->cbSize >= sizeof(CHARFORMATA)) 
            {
                if((pnewcf->dwMask & CFM_COLOR) && !(pnewcf->dwEffects & CFE_AUTOCOLOR)) {
                    pcf->dwMask     |= CFM_COLOR;
                    pcf->crTextColor = pnewcf->crTextColor;
                    dprintf(("Set text color to %x", pcf->crTextColor));
                }
                if(pnewcf->cbSize == sizeof(CHARFORMAT2A)) 
                {
                    if((pnewcf->dwMask & CFM_BACKCOLOR) && !(pnewcf->dwEffects & CFE_AUTOBACKCOLOR)) 
                    {
                        pcf->dwMask     |= CFM_BACKCOLOR;
                        pcf->crBackColor = pnewcf->crBackColor;
 
                        //Destroy old brush if present
                        if(pcf->dwReserved) DeleteObject(pcf->dwReserved);

                        //Create a brush that we return in WM_CTLCOLORSTATIC
                        pcf->dwReserved  = (DWORD)CreateSolidBrush(pcf->crBackColor);
 
                        dprintf(("Set background color to %x brush %x", pcf->crBackColor, pcf->dwReserved));
                    }
                }
            }

            if(pcf) GlobalUnlock(hProp);
    }
#else
            DPRINTF_EDIT_MSG32("EM_SETCHARFORMAT Ignored");
#endif
            return 0;

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

/* Messages dispatched to the edit control */
     case EM_CANUNDO:
            DPRINTF_EDIT_MSG32("EM_CANUNDO Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_CHARFROMPOS:
            DPRINTF_EDIT_MSG32("EM_CHARFROMPOS Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_EMPTYUNDOBUFFER:
            DPRINTF_EDIT_MSG32("EM_EMPTYUNDOBUFFER Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_FMTLINES:
            DPRINTF_EDIT_MSG32("EM_FMTLINES Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETFIRSTVISIBLELINE:
            DPRINTF_EDIT_MSG32("EM_GETFIRSTVISIBLELINE Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETHANDLE:
            DPRINTF_EDIT_MSG32("EM_GETHANDLE Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
 /*    case EM_GETIMESTATUS:*/
     case EM_GETLIMITTEXT:
            DPRINTF_EDIT_MSG32("EM_GETLIMITTEXT Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETLINE:
            DPRINTF_EDIT_MSG32("EM_GETLINE Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETLINECOUNT:
            DPRINTF_EDIT_MSG32("EM_GETLINECOUNT Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETMARGINS:
            DPRINTF_EDIT_MSG32("EM_GETMARGINS Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETMODIFY:
            DPRINTF_EDIT_MSG32("EM_GETMODIFY Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETPASSWORDCHAR:
            DPRINTF_EDIT_MSG32("EM_GETPASSWORDCHAR Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETRECT:
            DPRINTF_EDIT_MSG32("EM_GETRECT Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETSEL:
            DPRINTF_EDIT_MSG32("EM_GETSEL Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETTHUMB:
            DPRINTF_EDIT_MSG32("EM_GETTHUMB Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_GETWORDBREAKPROC:
            DPRINTF_EDIT_MSG32("EM_GETWORDBREAKPROC Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_LINEFROMCHAR:
            DPRINTF_EDIT_MSG32("EM_LINEFROMCHAR Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_LINEINDEX:
            DPRINTF_EDIT_MSG32("EM_LINEINDEX Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_LINELENGTH:
            DPRINTF_EDIT_MSG32("EM_LINELENGTH Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_LINESCROLL:
            DPRINTF_EDIT_MSG32("EM_LINESCROLL Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_POSFROMCHAR:
            DPRINTF_EDIT_MSG32("EM_POSFROMCHAR Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_REPLACESEL:
            DPRINTF_EDIT_MSG32("case EM_REPLACESEL Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SCROLL:
            DPRINTF_EDIT_MSG32("case EM_SCROLL Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SCROLLCARET:
            DPRINTF_EDIT_MSG32("EM_SCROLLCARET Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETHANDLE:
            DPRINTF_EDIT_MSG32("EM_SETHANDLE Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
 /*    case EM_SETIMESTATUS:*/
     case EM_SETLIMITTEXT:
            DPRINTF_EDIT_MSG32("EM_SETLIMITTEXT Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETMARGINS:
            DPRINTF_EDIT_MSG32("case EM_SETMARGINS Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETMODIFY:
            DPRINTF_EDIT_MSG32("EM_SETMODIFY Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETPASSWORDCHAR:
            DPRINTF_EDIT_MSG32("EM_SETPASSWORDCHAR Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETREADONLY:
            DPRINTF_EDIT_MSG32("EM_SETREADONLY Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETRECT:
            DPRINTF_EDIT_MSG32("EM_SETRECT Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETRECTNP:
            DPRINTF_EDIT_MSG32("EM_SETRECTNP Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETSEL:
            DPRINTF_EDIT_MSG32("EM_SETSEL Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETTABSTOPS:
            DPRINTF_EDIT_MSG32("EM_SETTABSTOPS Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_SETWORDBREAKPROC:
            DPRINTF_EDIT_MSG32("EM_SETWORDBREAKPROC Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case EM_UNDO:
            DPRINTF_EDIT_MSG32("EM_UNDO Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);

     case WM_STYLECHANGING:
            DPRINTF_EDIT_MSG32("WM_STYLECHANGING Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case WM_STYLECHANGED:
            DPRINTF_EDIT_MSG32("WM_STYLECHANGED Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case WM_NCCALCSIZE:
#ifdef __WIN32OS2__
            break; //this is completely wrong, we resize the control in the WM_SIZE handler
#else
            DPRINTF_EDIT_MSG32("WM_NCCALCSIZE Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
#endif
     case WM_GETTEXT:
            DPRINTF_EDIT_MSG32("WM_GETTEXT Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case WM_GETTEXTLENGTH:
            DPRINTF_EDIT_MSG32("WM_GETTEXTLENGTH Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case WM_SETTEXT:
            DPRINTF_EDIT_MSG32("WM_SETTEXT Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case WM_CUT:
            DPRINTF_EDIT_MSG32("WM_CUT Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
     case WM_COPY:
            DPRINTF_EDIT_MSG32("WM_COPY Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);
    case WM_PASTE:
            DPRINTF_EDIT_MSG32("WM_PASTE Passed to edit control");
	    return SendMessageA( hwndEdit, uMsg, wParam, lParam);

    /* Messages passed to default handler. */
    case WM_NCPAINT:
        DPRINTF_EDIT_MSG32("WM_NCPAINT Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_PAINT:
        DPRINTF_EDIT_MSG32("WM_PAINT Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_ERASEBKGND:
        DPRINTF_EDIT_MSG32("WM_ERASEBKGND Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_KILLFOCUS:
        DPRINTF_EDIT_MSG32("WM_KILLFOCUS Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_DESTROY:
#ifdef __WIN32OS2__
    {
        CHARFORMAT2A *pcf;

        hProp = GetPropA(hwnd, RICHEDIT_WND_PROP);
        pcf = (CHARFORMAT2A *)GlobalLock(hProp);
        if(pcf) {
            //Destroy old brush if present
            if(pcf->dwReserved) DeleteObject(pcf->dwReserved);
            GlobalUnlock(hProp);
        }

        if(hProp) GlobalFree(hProp);
        RemovePropA(hwnd, RICHEDIT_WND_PROP);
    }
#endif
        DPRINTF_EDIT_MSG32("WM_DESTROY Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_CHILDACTIVATE:
	DPRINTF_EDIT_MSG32("WM_CHILDACTIVATE Passed to default");
	return DefWindowProcA( hwnd,uMsg,wParam,lParam);

    case WM_WINDOWPOSCHANGING:
        DPRINTF_EDIT_MSG32("WM_WINDOWPOSCHANGING Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_WINDOWPOSCHANGED:
        DPRINTF_EDIT_MSG32("WM_WINDOWPOSCHANGED Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
/*    case WM_INITIALUPDATE:
        DPRINTF_EDIT_MSG32("WM_INITIALUPDATE Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam); */
#ifndef __WIN32OS2__
    case WM_CTLCOLOREDIT:
        DPRINTF_EDIT_MSG32("WM_CTLCOLOREDIT Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
#endif
    case WM_SETCURSOR:
        DPRINTF_EDIT_MSG32("WM_SETCURSOR Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_MOVE:
        DPRINTF_EDIT_MSG32("WM_MOVE Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_SHOWWINDOW:
        DPRINTF_EDIT_MSG32("WM_SHOWWINDOW Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_NCCREATE:
        DPRINTF_EDIT_MSG32("WM_NCCREATE Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_PARENTNOTIFY:
        DPRINTF_EDIT_MSG32("WM_PARENTNOTIFY Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_SETREDRAW:
        DPRINTF_EDIT_MSG32("WM_SETREDRAW Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_NCDESTROY:
        DPRINTF_EDIT_MSG32("WM_NCDESTROY Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_NCHITTEST:
        DPRINTF_EDIT_MSG32("WM_NCHITTEST Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_CTLCOLORSTATIC:
#ifdef __WIN32OS2__
    case WM_CTLCOLOREDIT:
{
            CHARFORMAT2A *pcf;
            HBRUSH hBrush = 0;
            HDC hdc = (HDC)wParam;

            hProp = GetPropA(hwnd, RICHEDIT_WND_PROP);
            pcf = (CHARFORMAT2A *)GlobalLock(hProp);
            if(pcf)
            {
                if(pcf->dwMask & CFM_BACKCOLOR) {
                    SetBkColor(hdc, pcf->crBackColor);
                    hBrush = pcf->dwReserved;
                }
                if(pcf->dwMask & CFM_COLOR) {
                    SetTextColor(hdc, pcf->crTextColor);
                }
            }
            if(pcf) GlobalUnlock(hProp);

            if(hBrush) return hBrush;          
}
#endif
        DPRINTF_EDIT_MSG32("WM_CTLCOLORSTATIC Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_NCMOUSEMOVE:
        DPRINTF_EDIT_MSG32("WM_NCMOUSEMOVE Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_CLEAR:
        DPRINTF_EDIT_MSG32("WM_CLEAR Passed to default");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
   /*
    * used by IE in the EULA box
    */
    case WM_ALTTABACTIVE:
        DPRINTF_EDIT_MSG32("WM_ALTTABACTIVE");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_GETDLGCODE:
        DPRINTF_EDIT_MSG32("WM_GETDLGCODE");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);
    case WM_SETFONT:
        DPRINTF_EDIT_MSG32("WM_SETFONT");
        return DefWindowProcA( hwnd,uMsg,wParam,lParam);

    }

    FIXME("Unknown message 0x%x Passed to default hwnd=%p, wParam=%08x, lParam=%08x\n",
           uMsg, hwnd, (UINT)wParam, (UINT)lParam);

   return DefWindowProcA( hwnd,uMsg,wParam,lParam);
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

    TRACE("\n");

    ZeroMemory(&wndClass, sizeof(WNDCLASSA));
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
    wndClass.lpfnWndProc = (WNDPROC)RICHED32_WindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0; /*(sizeof(RICHED32_INFO *);*/
    wndClass.hCursor = LoadCursorA(0, IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszClassName = RICHEDIT_CLASS10A; /* WC_RICHED32A; */

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
