/* $Id: atom.cpp,v 1.8 2000-05-10 13:13:31 sandervl Exp $ */

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
#include <heapstring.h>

#define DBG_LOCALLOG	DBG_atom
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
ATOM WIN32API FindAtomA( LPCSTR arg1)
{
    dprintf(("KERNEL32:  FindAtomA\n"));
    return O32_FindAtom(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API FindAtomW(LPCWSTR arg1)
{
 ATOM  rc;
 char *astring;

    dprintf(("KERNEL32:  FindAtomW"));
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
 ATOM atom;

    atom = O32_AddAtom(arg1);
    if(HIWORD(arg1)) {
    	 dprintf(("KERNEL32: AddAtomA %s returned %x", arg1, atom));
    }
    else dprintf(("KERNEL32: AddAtomA %x returned %x", arg1, atom));
    return atom;
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API AddAtomW(LPCWSTR arg1)
{
 ATOM  rc;
 char *astring;

    if(HIWORD(arg1) == 0) {
        rc = AddAtomA((char*)arg1);
    }
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)arg1);
        rc = AddAtomA(astring);
        FreeAsciiString(astring);
    }
    return(rc);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
UINT WIN32API GetAtomNameA( ATOM arg1, LPSTR arg2, int  arg3)
{
    dprintf(("KERNEL32:  GetAtomNameA\n"));
    return O32_GetAtomName(arg1, arg2, arg3);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
//******************************************************************************
UINT WIN32API GetAtomNameW(ATOM arg1, LPWSTR arg2, int arg3)
{
 UINT  rc;
 char *astring;

    dprintf(("KERNEL32:  GetAtomNameW\n"));
    astring = UnicodeToAsciiString(arg2); /* FIXME! */
    rc = O32_GetAtomName(arg1, astring, arg3);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API DeleteAtom( ATOM arg1)
{
    dprintf(("KERNEL32:  DeleteAtom\n"));
    return O32_DeleteAtom(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalDeleteAtom( ATOM arg1)
{
    dprintf(("KERNEL32:  GlobalDeleteAtom\n"));
    return O32_GlobalDeleteAtom(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InitAtomTable( DWORD arg1)
{
    dprintf(("KERNEL32:  InitAtomTable\n"));
    return O32_InitAtomTable(arg1);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalAddAtomA(LPCSTR arg1)
{
 ATOM atom;

    atom = O32_GlobalAddAtom(arg1);
    if(HIWORD(arg1)) {
    	 dprintf(("KERNEL32: GlobalAddAtomA %s returned %x", arg1, atom));
    }
    else dprintf(("KERNEL32: GlobalAddAtomA %x returned %x", arg1, atom));
    return atom;
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalAddAtomW(LPCWSTR arg1)
{
 char *astring;
 ATOM  rc;

    if(HIWORD(arg1) == 0)
    {
        rc = GlobalAddAtomA((char*)arg1);
    }
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)arg1);
        rc = GlobalAddAtomA(astring);
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

    if(HIWORD(arg1) == 0)
        rc = GlobalFindAtomA((char*)arg1);
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)arg1);
        rc = GlobalFindAtomA(astring);
        FreeAsciiString(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GlobalGetAtomNameA(ATOM atom, LPSTR lpszBuffer, int cchBuffer)
{
    dprintf(("KERNEL32: GlobalGetAtomNameA %x %x %d", atom, lpszBuffer, cchBuffer));
    return O32_GlobalGetAtomName(atom, lpszBuffer, cchBuffer);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GlobalGetAtomNameW(ATOM atom, LPWSTR lpszBuffer, int cchBuffer)
{
 char *astring;
 UINT rc;

    dprintf(("KERNEL32: GlobalGetAtomNameW %x %x %d", atom, lpszBuffer, cchBuffer));
    astring = (char *)alloca(cchBuffer);
    if(astring == NULL) {
	dprintf(("GlobalGetAtomNameW: alloca failed!!"));
	DebugInt3();
	return 0;
    }
    rc = GlobalGetAtomNameA(atom, astring, cchBuffer);
    if(rc) {
    	 lstrcpyAtoW(lpszBuffer, astring);
    }
    else lpszBuffer[0] = 0; //necessary?
    return rc;
}
//******************************************************************************
//******************************************************************************
