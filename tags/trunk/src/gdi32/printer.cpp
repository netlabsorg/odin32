/* $Id: printer.cpp,v 1.1 2001-07-14 15:31:45 sandervl Exp $ */

/*
 * GDI32 printer apis
 *
 * Copyright 2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
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
#include <misc.h>
#include <heapstring.h>

#define DBG_LOCALLOG	DBG_printer
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
INT WIN32API StartDocA(HDC hdc, const DOCINFOA *lpDocInfo)
{
    dprintf(("GDI32: StartDocA %x %x", hdc, lpDocInfo));
    return O32_StartDoc(hdc, (LPDOCINFOA)lpDocInfo);
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
