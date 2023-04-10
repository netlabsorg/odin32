/* $Id: printer.cpp,v 1.14 2004/05/04 13:49:24 sandervl Exp $ */

/*
 * GDI32 printer apis
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2002-2003 InnoTek Systemberatung GmbH (sandervl@innotek.de)
 *
 * Parts based on Wine code (StartDocW)
 * 
 * Copyright 1996 John Harvey
 * Copyright 1998 Huw Davies
 * Copyright 1998 Andreas Mohr
 * Copyright 1999 Klaas van Gend
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <dcdata.h>
#include <dbglog.h>
#include <heapstring.h>
#include "oslibgpi.h"

#define DBG_LOCALLOG	DBG_printer
#include "dbglocal.h"

// Private WGSS escape extensions
#define PRIVATE_QUERYORIENTATION	0x99990001
#define PRIVATE_QUERYCOPIES 		0x99990002
#define PRIVATE_QUERYCOLLATE		0x99990003

static char *lpszPassThrough = NULL;
static int   cbPassThrough   = 0;

//NOTE: We need to insert this postscript statement into the stream or else
//      the output will be completely inverted (x & y)
static char  szSetupStringPortrait[]  = "%%BeginSetup\n[{\n%%BeginColorModelSetup\n<< /ProcessColorModel /DeviceCMYK >> setpagedevice\n%%EndColorModelSetup\n} stopped cleartomark\n%%EndSetup\n";
static char  szSetupStringLandscape[] = "%%BeginSetup\n[{\n%%BeginColorModelSetup\n<< /ProcessColorModel /DeviceCMYK >> setpagedevice\n%%EndColorModelSetup\n} stopped cleartomark\n-90 rotate\n%%EndSetup\n";


static BOOL  fPostscriptPassthrough = TRUE;

//******************************************************************************
//******************************************************************************
BOOL WIN32API ODIN_QueryPostscriptPassthrough()
{
    return fPostscriptPassthrough;
}
//******************************************************************************
//******************************************************************************
void WIN32API ODIN_SetPostscriptPassthrough(BOOL fEnabled)
{
    fPostscriptPassthrough = fEnabled;
}
//******************************************************************************
//******************************************************************************
int WIN32API Escape( HDC hdc, int nEscape, int cbInput, LPCSTR lpvInData, PVOID lpvOutData)
{
    int rc;
    const char *lpszEscape = NULL;

    switch(nEscape) 
    {
    case SET_BOUNDS:
    {
        RECT *r = (RECT *)lpvInData;
        if(cbInput != sizeof(RECT)) {
            dprintf(("WARNING: cbInput != sizeof(RECT) (=%d) for SET_BOUNDS", cbInput));
            return 0;
        }
        dprintf(("SET_BOUNDS (%d,%d) (%d,%d)", r->left, r->top, r->right, r->bottom));
        return 0;
    }
    case POSTSCRIPT_PASSTHROUGH:
        if(!lpszEscape) lpszEscape = "POSTSCRIPT_PASSTHROUGH";
        nEscape = PASSTHROUGH;
    case POSTSCRIPT_DATA:
        if(!lpszEscape) lpszEscape = "POSTSCRIPT_DATA";
    case PASSTHROUGH:
    {
        if(!lpszEscape) lpszEscape = "PASSTHROUGH";
        dprintf(("Postscript %s data of size %d", lpszEscape, *(WORD *)lpvInData));

        if(fPostscriptPassthrough == FALSE) 
        {
            return 0;
        }
        if(lpszPassThrough) {
            O32_Escape(hdc, nEscape, cbPassThrough, lpszPassThrough, NULL);
            free(lpszPassThrough);
            lpszPassThrough = NULL;
        }
        pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
        if(pHps && (pHps->Reserved & DC_FLAG_SEND_POSTSCRIPT_SETUP_STRING))
        { 
            DWORD dmOrientation = 0, hdrsize, dmCollate, dmCopies;
            char *pvSetupData = NULL;

            O32_Escape(hdc, PRIVATE_QUERYORIENTATION, 0, NULL, &dmOrientation);
            if(dmOrientation == DMORIENT_LANDSCAPE) {
                hdrsize = sizeof(szSetupStringLandscape)-1;
                pvSetupData = (char *)alloca(hdrsize+sizeof(WORD));
            }
            else {//portrait
                hdrsize = sizeof(szSetupStringPortrait)-1;
                pvSetupData = (char *)alloca(hdrsize+sizeof(WORD));
            }
            if(pvSetupData) {
                *(WORD *)pvSetupData = hdrsize;
                memcpy(pvSetupData+sizeof(WORD), (hdrsize == sizeof(szSetupStringPortrait)-1) ? szSetupStringPortrait : szSetupStringLandscape, hdrsize);

                O32_Escape(hdc, nEscape, *(WORD *)pvSetupData, pvSetupData, NULL);

                dprintf(("Send Postscript setup string %d (%x):\n%s", dmOrientation, pHps->Reserved, pvSetupData+sizeof(WORD)));
            }
            pHps->Reserved &= ~DC_FLAG_SEND_POSTSCRIPT_SETUP_STRING;
        }

        rc = O32_Escape(hdc, nEscape, cbInput, lpvInData, lpvOutData);
        if(rc == 1) rc = *(WORD *)lpvInData;
        else        rc = 0;
        return rc;
    }

    case PRIVATE_QUERYORIENTATION:
        DebugInt3();
        return 0;
    
    case SPCLPASSTHROUGH2:
    {
        int rawsize = *(WORD *)(lpvInData+4);

        dprintf(("SPCLPASSTHROUGH2: pretend success"));
        dprintf(("SPCLPASSTHROUGH2: virt mem %x size %x", *(DWORD *)lpvInData, rawsize));
        if(lpszPassThrough == NULL) {
            lpszPassThrough = (char *)malloc(rawsize+sizeof(WORD));
            if(lpszPassThrough == NULL) {
                DebugInt3();
                return 0;
            }
            memcpy(lpszPassThrough+sizeof(WORD), (char *)lpvInData+6, rawsize);
            cbPassThrough = rawsize;
            *(WORD *)lpszPassThrough = (WORD)cbPassThrough;
        }
        else {
            char *tmp = lpszPassThrough;

            lpszPassThrough = (char *)malloc(cbPassThrough+rawsize+sizeof(WORD));
            if(lpszPassThrough == NULL) {
                DebugInt3();
                return 0;
            }
            memcpy(lpszPassThrough+sizeof(WORD), tmp+sizeof(WORD), cbPassThrough);
            free(tmp);
            memcpy(lpszPassThrough+sizeof(WORD)+cbPassThrough, (char *)lpvInData+6, rawsize);
            cbPassThrough += rawsize;
            *(WORD *)lpszPassThrough = (WORD)cbPassThrough;
        }
        return 1;
    }

    case DOWNLOADFACE:
        dprintf(("DOWNLOADFACE: pretend success"));
        return 1;

    case POSTSCRIPT_IGNORE:
    {
	BOOL ret = FALSE;
        dprintf(("POSTSCRIPT_IGNORE %d", *(WORD *)lpvInData));
	return ret;
    }
    case EPSPRINTING:
    {
        UINT epsprint = *(UINT*)lpvInData;
        dprintf(("EPSPRINTING support %sable.\n",epsprint?"en":"dis"));
        return 1;
    }

    case QUERYESCSUPPORT:
    {
        DWORD nEscapeSup = *(DWORD *)lpvInData;
        BOOL fdprintf = FALSE;

        switch(nEscapeSup) {
        case SET_BOUNDS:
            if(!fdprintf) dprintf(("QUERYESCSUPPORT: SET_BOUNDS"));
            fdprintf = TRUE;
/*        case SPCLPASSTHROUGH2:
            if(!fdprintf) dprintf(("QUERYESCSUPPORT: SPCLPASSTHROUGH2"));
            fdprintf = TRUE;
        case DOWNLOADHEADER:
            if(!fdprintf) dprintf(("QUERYESCSUPPORT: DOWNLOADHEADER"));
            fdprintf = TRUE;
        case DOWNLOADFACE:
            if(!fdprintf) dprintf(("QUERYESCSUPPORT: DOWNLOADFACE"));
            fdprintf = TRUE;
*/
        case POSTSCRIPT_IGNORE:
            if(!fdprintf) dprintf(("QUERYESCSUPPORT: POSTSCRIPT_IGNORE"));
            fdprintf = TRUE;
        case EPSPRINTING:
        {
            if(!fdprintf) dprintf(("QUERYESCSUPPORT: EPSPRINTING"));
            fdprintf = TRUE;

            nEscapeSup = POSTSCRIPT_PASSTHROUGH;
            return Escape(hdc, QUERYESCSUPPORT, sizeof(nEscapeSup), (LPCSTR)&nEscapeSup, NULL);
        }
        case POSTSCRIPT_PASSTHROUGH:
        case POSTSCRIPT_DATA:
        case PASSTHROUGH:
            if(fPostscriptPassthrough == FALSE) 
            {
                return 0;
            }
            break;

        default:
            break;
        }
    }
    default:
        if(cbInput && lpvInData) {
            ULONG *tmp = (ULONG *)lpvInData;
            for(int i=0;i<std::min(16,cbInput/4);i++) {
                    dprintf(("GDI32: Escape par %d: %x", i, *tmp++));
            }
        }
        break;
    }

    rc = O32_Escape(hdc, nEscape, cbInput, lpvInData, lpvOutData);
    if(rc == 0) {
         dprintf(("GDI32: Escape %x %d %d %x %x returned %d (WARNING: might not be implemented!!) ", hdc, nEscape, cbInput, lpvInData, lpvOutData, rc));
    }
    else dprintf(("GDI32: Escape %x %d %d %x %x returned %d ", hdc, nEscape, cbInput, lpvInData, lpvOutData, rc));

    return rc;
}
//******************************************************************************
//******************************************************************************
INT WIN32API ExtEscape(HDC hdc, INT nEscape, INT cbInput, LPCSTR lpszInData,
                       INT cbOutput, LPSTR lpszOutData)
{
    dprintf(("GDI32: ExtEscape, %x %x %d %x %d %x partly implemented", hdc, nEscape, cbInput, lpszInData, cbOutput, lpszOutData));

    switch(nEscape) {
    case DOWNLOADHEADER:
    {
        dprintf(("DOWNLOADHEADER: hardcoded result"));
        if(lpszOutData && cbOutput >= 8) {
            strcpy(lpszOutData, "PM_1.2");
            return 1;
        }
        dprintf(("Not enough room for proc name"));
        return 0;
    }
    }
    return Escape(hdc, nEscape, cbInput, lpszInData, lpszOutData);
}
//******************************************************************************
//******************************************************************************
INT WIN32API StartDocA(HDC hdc, const DOCINFOA *lpDocInfo)
{
    INT ret;

    if(lpDocInfo) {
         dprintf(("GDI32: StartDocA %x %x (%s %s %s %x)", hdc, lpDocInfo, lpDocInfo->lpszDocName, lpDocInfo->lpszOutput, lpDocInfo->lpszDatatype, lpDocInfo->fwType));
    }
    else dprintf(("GDI32: StartDocA %x %x", hdc, lpDocInfo));

    dprintf(("lpDocInfo->cbSize %x", lpDocInfo->cbSize)); 
    dprintf(("lpDocInfo->lpszDocName %s", lpDocInfo->lpszDocName)); 
    dprintf(("lpDocInfo->lpszOutput %s", lpDocInfo->lpszOutput)); 
    dprintf(("lpDocInfo->lpszDatatype %s", lpDocInfo->lpszDatatype)); 
    dprintf(("lpDocInfo->fwType %x", lpDocInfo->fwType)); 

    ret = O32_StartDoc(hdc, (LPDOCINFOA)lpDocInfo);

    if(ret != 0) {
        pDCData pHps = (pDCData)OSLibGpiQueryDCData((HPS)hdc);
        if(pHps)
        { 
            pHps->Reserved |= DC_FLAG_SEND_POSTSCRIPT_SETUP_STRING;
        }
        else DebugInt3();
    }

    return ret;
}
//******************************************************************************
//******************************************************************************
INT WIN32API StartDocW(HDC hdc, const DOCINFOW* doc)
{
    DOCINFOA docA;
    INT ret;

    docA.cbSize = doc->cbSize;
    docA.lpszDocName = doc->lpszDocName ? 
      HEAP_strdupWtoA( GetProcessHeap(), 0, doc->lpszDocName ) : NULL;
    docA.lpszOutput = doc->lpszOutput ?
      HEAP_strdupWtoA( GetProcessHeap(), 0, doc->lpszOutput ) : NULL;
    docA.lpszDatatype = doc->lpszDatatype ?
      HEAP_strdupWtoA( GetProcessHeap(), 0, doc->lpszDatatype ) : NULL;
    docA.fwType = doc->fwType;

    ret = StartDocA(hdc, &docA);

    if(docA.lpszDocName)
        HeapFree( GetProcessHeap(), 0, (LPSTR)docA.lpszDocName );
    if(docA.lpszOutput)
        HeapFree( GetProcessHeap(), 0, (LPSTR)docA.lpszOutput );
    if(docA.lpszDatatype)
        HeapFree( GetProcessHeap(), 0, (LPSTR)docA.lpszDatatype );

    return ret;
}
//******************************************************************************
//******************************************************************************
int WIN32API EndDoc( HDC hdc)
{
    dprintf(("GDI32: EndDoc %x", hdc));
    return O32_EndDoc(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API AbortDoc(HDC hdc)
{
    dprintf(("GDI32: AbortDoc %x", hdc));
    return O32_AbortDoc(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API StartPage( HDC hdc)
{
    dprintf(("GDI32: StartPage %x", hdc));
    return O32_StartPage(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API EndPage( HDC hdc)
{
    dprintf(("GDI32: EndPage %x", hdc));
    return O32_EndPage(hdc);
}
//******************************************************************************
//******************************************************************************
int WIN32API SetAbortProc(HDC hdc, ABORTPROC lpAbortProc)
{
    dprintf(("GDI32: SetAbortProc %x %x - stub (1)", hdc, lpAbortProc));
    return(1);
}
//******************************************************************************
//******************************************************************************
