/* $Id: atom.cpp,v 1.7 2000-02-16 14:25:30 sandervl Exp $ */

/*
 * Win32 ATOM api functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include "unicode.h"

#define DBG_LOCALLOG	DBG_atom
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
ATOM WIN32API FindAtomA( LPCSTR arg1)
{
    dprintf(("KERNEL32:  OS2FindAtomA\n"));
    return O32_FindAtom(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API FindAtomW(LPCWSTR arg1)
{
 ATOM  rc;
 char *astring;

    dprintf(("KERNEL32:  OS2FindAtomW"));
    if (arg1 < (LPCWSTR)0x10000)
        rc = O32_FindAtom((char*)arg1);
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)arg1);
        rc = O32_FindAtom(astring);
        FreeAsciiString(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API AddAtomA(LPCSTR arg1)
{
    dprintf(("KERNEL32:  OS2AddAtomA\n"));
    return O32_AddAtom(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API AddAtomW(LPCWSTR arg1)
{
 ATOM  rc;
 char *astring;

    dprintf(("KERNEL32:  OS2AddAtomW\n"));
    if (arg1 < (LPCWSTR)0x10000)
        rc = O32_AddAtom((char*)arg1);
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)arg1);
        rc = O32_AddAtom(astring);
        FreeAsciiString(astring);
    }
    return(rc);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
UINT WIN32API GetAtomNameA( ATOM arg1, LPSTR arg2, int  arg3)
{
    dprintf(("KERNEL32:  OS2GetAtomNameA\n"));
    return O32_GetAtomName(arg1, arg2, arg3);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
UINT WIN32API GetAtomNameW(ATOM arg1, LPWSTR arg2, int arg3)
{
 UINT  rc;
 char *astring;

    dprintf(("KERNEL32:  OS2GetAtomNameW\n"));
    astring = UnicodeToAsciiString(arg2); /* FIXME! */
    rc = O32_GetAtomName(arg1, astring, arg3);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API DeleteAtom( ATOM arg1)
{
    dprintf(("KERNEL32:  OS2DeleteAtom\n"));
    return O32_DeleteAtom(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalDeleteAtom( ATOM arg1)
{
    dprintf(("KERNEL32:  OS2GlobalDeleteAtom\n"));
    return O32_GlobalDeleteAtom(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InitAtomTable( DWORD arg1)
{
    dprintf(("KERNEL32:  OS2InitAtomTable\n"));
    return O32_InitAtomTable(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalAddAtomA(LPCSTR arg1)
{
    dprintf(("KERNEL32:  OS2GlobalAddAtomA %s\n", arg1));
    return O32_GlobalAddAtom(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalAddAtomW(LPCWSTR arg1)
{
 char *astring;
 ATOM  rc;

    if (arg1 < (LPCWSTR)0x10000)
    {
        dprintf(("KERNEL32:  OS2GlobalAddAtomW %#4x\n", astring));
        rc = O32_GlobalAddAtom((char*)arg1);
    }
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)arg1);
        dprintf(("KERNEL32:  OS2GlobalAddAtomW %s\n", astring));
        rc = O32_GlobalAddAtom(astring);
        FreeAsciiString(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalFindAtomA( LPCSTR arg1)
{
    if (arg1 < (LPCSTR)0x10000)
      dprintf(("KERNEL32:  GlobalFindAtomA %#4x\n", arg1));
    else
      dprintf(("KERNEL32:  GlobalFindAtomA %s\n", arg1));
    return O32_GlobalFindAtom(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalFindAtomW( LPCWSTR arg1)
{
 char *astring;
 ATOM  rc;

    dprintf(("KERNEL32:  OS2GlobalFindAtomW\n"));
    if (arg1 < (LPCWSTR)0x10000)
        rc = O32_GlobalFindAtom((char*)arg1);
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)arg1);
        rc = O32_GlobalFindAtom(astring);
        FreeAsciiString(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GlobalGetAtomNameA( ATOM arg1, LPSTR arg2, int  arg3)
{
    dprintf(("KERNEL32:  OS2GlobalGetAtomNameA\n"));
    return O32_GlobalGetAtomName(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GlobalGetAtomNameW( ATOM arg1, LPWSTR arg2, int  arg3)
{
 char *astring;
 ATOM  rc;

    dprintf(("KERNEL32:  OS2GlobalGetAtomNameW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg1); /* FIXME! */
    rc = O32_GlobalGetAtomName(arg1, astring, arg3);
    FreeAsciiString(astring);
    return(rc);
}
