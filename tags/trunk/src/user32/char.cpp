/* $Id: char.cpp,v 1.1 1999-05-24 20:20:01 ktk Exp $ */

/*
 * Win32 character API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include "user32.h"


//******************************************************************************
//******************************************************************************
LPSTR WIN32API CharLowerA( LPSTR arg1)
{
    dprintf(("USER32:  OS2CharLowerA\n"));
    return O32_CharLower(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API CharLowerBuffA( LPSTR arg1, DWORD  arg2)
{
    dprintf(("USER32:  OS2CharLowerBuffA\n"));
    return O32_CharLowerBuff(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API CharLowerBuffW( LPWSTR arg1, DWORD  arg2)
{
    dprintf(("USER32:  OS2CharLowerBuffW DOESN'T WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_CharLowerBuff(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LPWSTR WIN32API CharLowerW( LPWSTR arg1)
{
    dprintf(("USER32:  OS2CharLowerW DOESN'T WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return NULL;
//    return O32_CharLower(arg1);
}
//******************************************************************************
//******************************************************************************
LPSTR WIN32API CharNextA( LPCSTR arg1)
{
    dprintf(("USER32:  OS2CharNextA\n"));
    return O32_CharNext(arg1);
}
//******************************************************************************
//******************************************************************************
LPWSTR WIN32API CharNextW( LPCWSTR arg1)
{
    dprintf(("USER32:  OS2CharNextW DOESN'T WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_CharNext(arg1);
}
//******************************************************************************
//******************************************************************************
LPSTR WIN32API CharPrevA( LPCSTR arg1, LPCSTR  arg2)
{
    dprintf(("USER32:  OS2CharPrevA\n"));
    return O32_CharPrev(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LPWSTR WIN32API CharPrevW( LPCWSTR arg1, LPCWSTR  arg2)
{
    dprintf(("USER32:  OS2CharPrevW DOESN'T WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
 //   return O32_CharPrev(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CharToOemA( LPCSTR arg1, LPSTR  arg2)
{
    dprintf(("USER32:  OS2CharToOemA\n"));
    return O32_CharToOem(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CharToOemBuffA( LPCSTR arg1, LPSTR arg2, DWORD  arg3)
{
    dprintf(("USER32:  OS2CharToOemBuffA\n"));
    return O32_CharToOemBuff(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CharToOemBuffW( LPCWSTR arg1, LPSTR arg2, DWORD  arg3)
{
    dprintf(("USER32:  OS2CharToOemBuffW DOESN'T WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_CharToOemBuff(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CharToOemW( LPCWSTR arg1, LPSTR  arg2)
{
    dprintf(("USER32:  OS2CharToOemW DOESN'T WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_CharToOem(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LPSTR WIN32API CharUpperA( LPSTR arg1)
{
 LPSTR rc;

    if((int)arg1 >> 16 != 0) {
     	 dprintf(("USER32:  OS2CharUpperA %s\n", arg1));
    }
    else {
	dprintf(("USER32:  OS2CharUpperA %X\n", arg1));
    }

    rc = O32_CharUpper(arg1);

    if((int)rc >> 16 != 0) {
     	 dprintf(("USER32:  OS2CharUpperA %s\n", rc));
    }
    else {
	 dprintf(("USER32:  OS2CharUpperA %X\n", rc));
    }

    return(rc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API CharUpperBuffA( LPSTR arg1, DWORD  arg2)
{
    dprintf(("USER32:  OS2CharUpperBuffA\n"));
    return O32_CharUpperBuff(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API CharUpperBuffW( LPWSTR arg1, DWORD  arg2)
{
    dprintf(("USER32:  OS2CharUpperBuffW DOESN'T WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_CharUpperBuff(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
LPWSTR WIN32API CharUpperW( LPWSTR arg1)
{
    dprintf(("USER32:  OS2CharUpperW  DOESN'T WORK\n"));
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_CharUpper(arg1);
}
//******************************************************************************
//******************************************************************************
LPSTR WIN32API CharNextExA( WORD codepage, LPCSTR ptr, DWORD flags )
{
    if (!*ptr) return (LPSTR)ptr;
//    if (O32_IsDBCSLeadByteEx( codepage, *ptr )) return (LPSTR)(ptr + 2);
    return (LPSTR)(ptr + 1);
}
//******************************************************************************
//******************************************************************************
LPSTR WIN32API CharPrevExA(WORD codepage, LPCSTR start, LPCSTR ptr, DWORD flags)
{
    while (*start && (start < ptr))
    {
        LPCSTR next = CharNextExA(codepage, start, flags);
        if (next > ptr) break;
        start = next;
    }
    return (LPSTR)start;
}
/*****************************************************************************
 * Name      : LPSTR WIN32API CharNextExW
 * Purpose   : The CharNextExA function retrieves the address of the next
 *             character in a string. This function can handle strings
 *             consisting of either single- or multi-byte characters.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is a pointer to the
 *             next character in the string, or to the terminating null character
 *             if at the end of the string.
 *             If lpCurrentChar points to the terminating null character, the
 *             return value is equal to lpCurrentChar.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

LPWSTR WIN32API CharNextExW(WORD   CodePage,
                            LPCWSTR lpCurrentChar,
                            DWORD  dwFlags)
{
  dprintf(("USER32:CharNextExW(%u,%08xh,%08x) not implemented.\n",
          CodePage,
          lpCurrentChar,
          dwFlags));

  return (NULL);
}


/*****************************************************************************
 * Name      : LPSTR WIN32API CharPrevExW
 * Purpose   : The CharPrevExA function retrieves the address of the preceding
 *             character in a string. This function can handle strings
 *             consisting of either single- or multi-byte characters.
 * Parameters:
 * Variables :
 * Result    : If the function succeeds, the return value is a pointer to the
 *             preceding character in the string, or to the first character in
 *             the string if the lpCurrentChar parameter equals the lpStart parameter.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1998/02/26 11:55]
 *****************************************************************************/

LPWSTR WIN32API CharPrevExW(WORD CodePage,
                            LPCWSTR lpStart,
                            LPCWSTR lpCurrentChar,
                            DWORD  dwFlags)
{
  dprintf(("USER32:CharPrevExW(%u,%08xh,%08xh,%08x) not implemented.\n",
          CodePage,
          lpStart,
          lpCurrentChar,
          dwFlags));

  return (NULL);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharAlphaA( CHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2IsCharAlphaA\n");
#endif
    return O32_IsCharAlpha(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharAlphaNumericA( CHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2IsCharAlphaNumericA\n");
#endif
    return O32_IsCharAlphaNumeric(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharAlphaNumericW( WCHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2IsCharAlphaNumericW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_IsCharAlphaNumeric((CHAR)arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharAlphaW( WCHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2IsCharAlphaW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_IsCharAlpha((CHAR)arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharLowerA( CHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2IsCharLowerA\n");
#endif
    return O32_IsCharLower(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharLowerW( WCHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2IsCharLowerW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_IsCharLower((CHAR)arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharUpperA( CHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2IsCharUpperA\n");
#endif
    return O32_IsCharUpper(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsCharUpperW( WCHAR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2IsCharUpperW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_IsCharUpper((CHAR)arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OemToCharA( LPCSTR arg1, LPSTR  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2OemToCharA\n");
#endif
    return O32_OemToChar(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OemToCharBuffA( LPCSTR arg1, LPSTR arg2, DWORD  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2OemToCharBuffA\n");
#endif
    return O32_OemToCharBuff(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OemToCharBuffW(LPCSTR arg1, LPWSTR arg2, DWORD  arg3)
{
#ifdef DEBUG
    WriteLog("USER32: OemToCharBuffW DOESN'T WORK \n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_OemToCharBuff(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OemToCharW( LPCSTR arg1, LPWSTR  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  OS2OemToCharW DOESN'T WORK\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return 0;
//    return O32_OemToChar(arg1, arg2);
}
