/* $Id: atom.cpp,v 1.10 2001-07-08 11:02:10 sandervl Exp $ */

/*
 * Win32 ATOM api functions
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * TODO: DeleteAtom doesn't appear to work properly. FindAtom still works
 *       after deleting it.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_WIN
#include <os2wrap.h>
#include <win32api.h>
#include <winconst.h>
#include <unicode.h>
#include <heapstring.h>
#include <misc.h>

#define DBG_LOCALLOG	DBG_atom
#include "dbglocal.h"

// action codes for LookupAtom
#define LOOKUP_FIND     0
#define LOOKUP_ADD      1
#define LOOKUP_DELETE   2
#define LOOKUP_NOCASE   0x80000000

ATOM APIENTRY LookupAtom(HATOMTBL hAtomTbl, PSZ psz, ULONG actionMask);

inline ATOM _LookupAtom(HATOMTBL hAtomTbl, PSZ psz, ULONG actionMask)
{
 ATOM yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = LookupAtom(hAtomTbl, psz, actionMask);
    SetFS(sel);

    return yyrc;
}

#undef  LookupAtom
#define LookupAtom _LookupAtom


HATOMTBL privateAtomTable = NULL;
HATOMTBL systemAtomTable  = NULL;
//******************************************************************************
//******************************************************************************
HATOMTBL inline getPrivateAtomTable()
{
    if(privateAtomTable == NULL) {
        privateAtomTable = WinCreateAtomTable(0, 37); 
    }
    return privateAtomTable;
}
//******************************************************************************
//******************************************************************************
HATOMTBL inline getSystemAtomTable()
{
    if(systemAtomTable == NULL) {
        systemAtomTable = WinQuerySystemAtomTable(); 
    }
    return systemAtomTable;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InitAtomTable(DWORD numEntries)
{
    dprintf(("KERNEL32: InitAtomTable %d", numEntries));

    if(privateAtomTable == NULL) {
        privateAtomTable = WinCreateAtomTable(0, numEntries);
    }
    return (privateAtomTable != NULL);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API FindAtomA( LPCSTR atomName)
{ 
    HATOMTBL atomTable = getPrivateAtomTable();
    ATOM     atom = 0;
 
    if(HIWORD(atomName)) {
         dprintf(("FindAtomA %s", atomName));
    }
    else dprintf(("FindAtomA %x", atomName));

    if(atomTable != NULL) {
        atom = LookupAtom(atomTable, HIWORD(atomName) ? 
                          (PSZ) atomName : (PSZ) LOWORD(atomName),
                          LOOKUP_ADD | LOOKUP_NOCASE);
    }
    dprintf(("FindAtomA returned %x", atom));

    if(!atom) {
         SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: find real error
    }
    else SetLastError(ERROR_SUCCESS_W);
    return atom;
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API FindAtomW(LPCWSTR atomName)
{
    ATOM  rc;
    char *astring;

    dprintf(("KERNEL32: FindAtomW"));
    if(HIWORD(atomName))
    {
         astring = UnicodeToAsciiString((LPWSTR)atomName);
         rc = FindAtomA(astring);
         FreeAsciiString(astring);
    }
    else rc = FindAtomA((char*)atomName);

    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API AddAtomA(LPCSTR atomName)
{
    ATOM atom = 0;
    HATOMTBL atomTable = getPrivateAtomTable();

    if(atomTable != NULL)
    {
        atom = LookupAtom(atomTable, HIWORD(atomName) ? 
                          (PSZ) atomName : (PSZ) LOWORD(atomName),
                          LOOKUP_ADD | LOOKUP_NOCASE);
    }

    if(HIWORD(atomName)) {
    	 dprintf(("KERNEL32: AddAtomA %s returned %x", atomName, atom));
    }
    else dprintf(("KERNEL32: AddAtomA %x returned %x", atomName, atom));

    if(!atom) {
         SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: find real error
    }
    else SetLastError(ERROR_SUCCESS_W);
    return atom;
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API AddAtomW(LPCWSTR atomName)
{
 ATOM  rc;
 char *astring;

    if(HIWORD(atomName) == 0) {
        rc = AddAtomA((char*)atomName);
    }
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)atomName);
        rc = AddAtomA(astring);
        FreeAsciiString(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetAtomNameA( ATOM atom, LPSTR atomName, int nameLen)
{
    UINT     result = 0;
    HATOMTBL atomTable = getPrivateAtomTable();

    dprintf(("KERNEL32: GetAtomNameA %x %x %d", LOWORD(atom), atomName, nameLen));
    if(atomTable != NULL)
        result = (UINT)WinQueryAtomName( atomTable, LOWORD(atom), atomName, nameLen);

    dprintf(("KERNEL32: GetAtomNameA returned %s", (result) ? atomName : NULL));

    if(!result) {
         SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: find real error
    }
    else SetLastError(ERROR_SUCCESS_W);
    return (result);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetAtomNameW(ATOM atom, LPWSTR lpszBuffer, int cchBuffer)
{
 char *astring;
 UINT rc;

    dprintf(("KERNEL32: GetAtomNameW %x %x %d", atom, lpszBuffer, cchBuffer));
    astring = (char *)alloca(cchBuffer);
    if(astring == NULL) {
	dprintf(("GlobalGetAtomNameW: alloca failed!!"));
	DebugInt3();
	return 0;
    }
    rc = GetAtomNameA(atom, astring, cchBuffer);
    if(rc) {
    	 lstrcpyAtoW(lpszBuffer, astring);
    }
    else lpszBuffer[0] = 0; //necessary?
    return rc;
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API DeleteAtom(ATOM atom)
{
   HATOMTBL atomTable = getPrivateAtomTable();

   dprintf(("DeleteAtom %x", atom));
   if (atomTable != NULL) {
       return (ATOM) LookupAtom(atomTable, (PSZ) MAKEULONG(atom, 0xFFFF),
                                LOOKUP_DELETE | LOOKUP_NOCASE);
   }
   return 0;
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalDeleteAtom(ATOM atom)
{
   HATOMTBL atomTable = getSystemAtomTable();

   dprintf(("KERNEL32: GlobalDeleteAtom %x", atom));
   return (ATOM) LookupAtom(atomTable, (PSZ) MAKEULONG(atom, 0xFFFF),
                            LOOKUP_DELETE | LOOKUP_NOCASE);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalAddAtomA(LPCSTR atomName)
{
    ATOM atom = 0;
    HATOMTBL atomTable = getSystemAtomTable();

    if(atomTable != NULL)
    {
        atom = LookupAtom(atomTable, HIWORD(atomName) ? 
                          (PSZ) atomName : (PSZ) LOWORD(atomName),
                          LOOKUP_ADD | LOOKUP_NOCASE);
    }

    if(HIWORD(atomName)) {
    	 dprintf(("KERNEL32: GlobalAddAtomA %s returned %x", atomName, atom));
    }
    else dprintf(("KERNEL32: GlobalAddAtomA %x returned %x", atomName, atom));

    if(!atom) {
         SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: find real error
    }
    else SetLastError(ERROR_SUCCESS_W);
    return atom;
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalAddAtomW(LPCWSTR atomName)
{
 char *astring;
 ATOM  rc;

    if(HIWORD(atomName) == 0)
    {
        rc = GlobalAddAtomA((char*)atomName);
    }
    else
    {
        astring = UnicodeToAsciiString((LPWSTR)atomName);
        rc = GlobalAddAtomA(astring);
        FreeAsciiString(astring);
    }
    return(rc);
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalFindAtomA( LPCSTR atomName)
{
    HATOMTBL atomTable = getSystemAtomTable();
    ATOM     atom = 0;

    if(HIWORD(atomName)) {
         dprintf(("GlobalFindAtomA %s", atomName));
    }
    else dprintf(("GlobalFindAtomA %x", atomName));

    atom = LookupAtom(atomTable, HIWORD(atomName) ? 
                      (PSZ) atomName : (PSZ) LOWORD(atomName),
                      LOOKUP_ADD | LOOKUP_NOCASE);
    dprintf(("GlobalFindAtomA returned %x", atom));

    if(!atom) {
         SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: find real error
    }
    else SetLastError(ERROR_SUCCESS_W);

    return atom;
}
//******************************************************************************
//******************************************************************************
ATOM WIN32API GlobalFindAtomW(LPCWSTR atomName)
{
 ATOM  rc;
 char *astring;

    dprintf(("KERNEL32: GlobalFindAtomW"));
    if(HIWORD(atomName))
    {
         astring = UnicodeToAsciiString((LPWSTR)atomName);
         rc = GlobalFindAtomA(astring);
         FreeAsciiString(astring);
    }
    else rc = GlobalFindAtomA((char*)atomName);

    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GlobalGetAtomNameA(ATOM atom, LPSTR lpszBuffer, int cchBuffer)
{
    UINT     result = 0;
    HATOMTBL atomTable = getSystemAtomTable();

    dprintf(("KERNEL32: GlobalGetAtomNameA %x %x %d", LOWORD(atom), lpszBuffer, cchBuffer));
    if(atomTable != NULL)
        result = (UINT)WinQueryAtomName( atomTable, LOWORD(atom), lpszBuffer, cchBuffer);

    if(!result) {
         SetLastError(ERROR_INVALID_PARAMETER_W); //TODO: find real error
    }
    else SetLastError(ERROR_SUCCESS_W);

    dprintf(("KERNEL32: GlobalGetAtomNameA returned %s", (result) ? lpszBuffer : NULL));
    return (result);
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
