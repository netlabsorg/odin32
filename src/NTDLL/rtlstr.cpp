/* $Id: rtlstr.cpp,v 1.8 1999-08-22 22:45:53 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 * Rtl string functions
 *
 * Copyright 1996-1998 Marcus Meissner
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <os2win.h>
#include <unicode.h>

#include "ntdll.h"
#include "windef.h"

#define HAVE_WCTYPE_H
#ifdef HAVE_WCTYPE_H
# include <wctype.h>
#endif
#include "wine/winestring.h"
#include "winnls.h"
//#include <heapstring.h>


/*
 * STRING FUNCTIONS
 */

/**************************************************************************
 *                 RtlAnsiStringToUnicodeString      [NTDLL.269]
 */
DWORD WINAPI RtlAnsiStringToUnicodeString(PUNICODE_STRING uni,
                                             PANSI_STRING    ansi,
                                             BOOLEAN         doalloc)
{
  DWORD unilen = (ansi->Length+1)*sizeof(WCHAR);

  dprintf(("NTDLL: RtlAnsiStringToUnicodeString(%08xh,%08xh,%08xh)\n",
           uni,
           ansi,
           doalloc));

  if (unilen>0xFFFF)
    return STATUS_INVALID_PARAMETER_2;

  uni->Length = unilen;
  if (doalloc)
  {
    uni->MaximumLength = unilen;
    uni->Buffer = (WCHAR *)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,unilen);
    if (!uni->Buffer)
      return STATUS_NO_MEMORY;
  }

  if (unilen>uni->MaximumLength)
    return STATUS_BUFFER_OVERFLOW;

  AsciiToUnicodeN(ansi->Buffer,
                  uni->Buffer,
                  unilen/2);

  return STATUS_SUCCESS;
}


/**************************************************************************
 *                 RtlOemStringToUnicodeString    [NTDLL.447]
 */
DWORD WINAPI RtlOemStringToUnicodeString(PUNICODE_STRING uni,
                                            PSTRING         ansi,
                                            BOOLEAN         doalloc)
{
  DWORD unilen = (ansi->Length+1)*sizeof(WCHAR);

  dprintf(("NTDLL: RtlOemStringToUnicodeString(%08xh,%08xh,%08xh)\n",
           uni,
           ansi,
           doalloc));

  if (unilen>0xFFFF)
    return STATUS_INVALID_PARAMETER_2;

  uni->Length = unilen;
  if (doalloc)
  {
    uni->MaximumLength = unilen;
    uni->Buffer = (WCHAR *)HeapAlloc(GetProcessHeap(),
                            HEAP_ZERO_MEMORY,
                            unilen);

    if (!uni->Buffer)
      return STATUS_NO_MEMORY;
  }

  if (unilen>uni->MaximumLength)
    return STATUS_BUFFER_OVERFLOW;

  AsciiToUnicodeN(ansi->Buffer,
               uni->Buffer,
               unilen/2);
  return STATUS_SUCCESS;
}


/**************************************************************************
 *                 RtlMultiByteToUnicodeN      [NTDLL.436]
 * FIXME: multibyte support
 */
DWORD WINAPI RtlMultiByteToUnicodeN(LPWSTR  unistr,
                                       DWORD   unilen,
                                       LPDWORD reslen,
                                       LPSTR   oemstr,
                                       DWORD   oemlen)
{
  DWORD len;
  LPWSTR x;

  dprintf(("NTDLL: RtlMultiByteToUnicodeN(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           unistr,
           unilen,
           reslen,
           oemstr,
           oemlen));

  len = oemlen;

  if (unilen/2 < len)
     len = unilen/2;

  x=(LPWSTR)HeapAlloc(GetProcessHeap(),
                      HEAP_ZERO_MEMORY,
                      (len+1)*sizeof(WCHAR));

  AsciiToUnicodeN(oemstr,
                  x,
                  len+1);

  memcpy(unistr,
         x,
         len*2);

  if (reslen)
    *reslen = len*2;

  return 0;
}


/**************************************************************************
 *                 RtlOemToUnicodeN            [NTDLL.448]
 */
DWORD WINAPI RtlOemToUnicodeN(LPWSTR  unistr,
                                 DWORD   unilen,
                                 LPDWORD reslen,
                                 LPSTR   oemstr,
                                 DWORD   oemlen)
{
  DWORD len;
  LPWSTR x;

  dprintf(("NTDLL: RtlOemToUnicodeN(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           unistr,
           unilen,
           reslen,
           oemstr,
           oemlen));

  len = oemlen;

  if (unilen/2 < len)
    len = unilen/2;

  x=(LPWSTR)HeapAlloc(GetProcessHeap(),
                      HEAP_ZERO_MEMORY,
                      (len+1)*sizeof(WCHAR));

  AsciiToUnicodeN(oemstr,
                  x,
                  len+1);

  memcpy(unistr,
         x,
         len*2);

  if (reslen)
    *reslen = len*2;

  return 0;
}


/**************************************************************************
 *                 RtlInitAnsiString           [NTDLL.399]
 */
VOID WINAPI RtlInitAnsiString(PANSI_STRING target,
                              LPCSTR       source)
{
  dprintf(("NTDLL: RtlInitAnsiString(%08xh, %08xh)\n",
           target,
           source));

  target->Length        = target->MaximumLength = 0;
  target->Buffer        = (LPSTR)source;
  if (!source)
     return;

  target->MaximumLength = lstrlenA(target->Buffer);
  target->Length        = target->MaximumLength+1;
}


/**************************************************************************
 *                 RtlInitOemString
 */
VOID WINAPI RtlInitOemString(POEM_STRING target,
                              LPCSTR       source)
{
  dprintf(("NTDLL: RtlInitOemString(%08xh, %08xh)\n",
           target,
           source));

  target->Length        = target->MaximumLength = 0;
  target->Buffer        = (LPSTR)source;
  if (!source)
     return;

  target->MaximumLength = lstrlenA(target->Buffer);
  target->Length        = target->MaximumLength+1;
}



/**************************************************************************
 *                 RtlInitString               [NTDLL.402]
 */
VOID WINAPI RtlInitString(PSTRING target,
                          LPCSTR  source)
{
  dprintf (("NTDLL: RtlInitString(%08xh, %08xh)\n",
            target,
            source));

  target->Length        = target->MaximumLength = 0;
  target->Buffer        = (LPSTR)source;
  if (!source)
     return;

  target->MaximumLength = lstrlenA(target->Buffer);
  target->Length        = target->MaximumLength+1;
}


/**************************************************************************
 *                 RtlInitUnicodeString        [NTDLL.403]
 */
VOID WINAPI RtlInitUnicodeString(PUNICODE_STRING target,
                                 LPCWSTR         source)
{
  dprintf(("NTDLL: RtlInitUnicodeString(%08xh,%08xh)\n",
           target,
           source));

   target->Length = target->MaximumLength = 0;
   target->Buffer = (LPWSTR)source;
   if (!source)
      return;

   target->MaximumLength = lstrlenW(target->Buffer)*2;
   target->Length        = target->MaximumLength+2;
}


/**************************************************************************
 *                 RtlFreeUnicodeString        [NTDLL.377]
 */
VOID WINAPI RtlFreeUnicodeString(PUNICODE_STRING str)
{
  dprintf(("NTDLL: RtlFreeUnicodeString(%08xh)\n",
           str));

  if (str->Buffer)
    HeapFree(GetProcessHeap(),
             0,
             str->Buffer);
}


/**************************************************************************
 * RtlFreeAnsiString [NTDLL.373]
 */
VOID WINAPI RtlFreeAnsiString(PANSI_STRING AnsiString)
{
  dprintf(("NTDLL: RtlFreeAnsiString(%08xh)\n",
           AnsiString));

  if( AnsiString->Buffer )
      HeapFree(GetProcessHeap(),
               0,
               AnsiString->Buffer);
}


/**************************************************************************
 * RtlFreeOemString
 */
VOID WINAPI RtlFreeOemString(POEM_STRING OemString)
{
  dprintf(("NTDLL: RtlFreeOemString(%08xh)\n",
           OemString));

  if( OemString->Buffer )
      HeapFree(GetProcessHeap(),
               0,
               OemString->Buffer);
}



/**************************************************************************
 *                 RtlUnicodeToOemN            [NTDLL.515]
 */
DWORD WINAPI RtlUnicodeToOemN(LPSTR   oemstr,
                                 DWORD   oemlen,
                                 LPDWORD reslen,
                                 LPWSTR  unistr,
                                 DWORD   unilen)
{
  DWORD len;
  LPSTR x;

  dprintf(("NTDLL: RtlUnicodeToOemN(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           oemstr,
           oemlen,
           reslen,
           unistr,
           unilen));

  len = oemlen;

  if (unilen/2 < len)
    len = unilen/2;

  x=(LPSTR)HeapAlloc(GetProcessHeap(),
                     HEAP_ZERO_MEMORY,
                     len+1);

  UnicodeToAsciiN(unistr,
                  x,
                  len+1);

  memcpy(oemstr,
         x,
         len);

  if (reslen)
    *reslen = len;

  return 0;
}


/**************************************************************************
 *                 RtlUnicodeToMultiByteN            [NTDLL.513]
 */
DWORD WINAPI RtlUnicodeToMultiByteN(LPSTR   oemstr,
                                    DWORD   oemlen,
                                    LPDWORD reslen,
                                    LPWSTR  unistr,
                                    DWORD   unilen)
{
  DWORD len;
  LPSTR x;

  dprintf(("NTDLL: RtlUnicodeToMultiByteN(%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           oemstr,
           oemlen,
           reslen,
           unistr,
           unilen));

  len = oemlen;

  if (unilen/2 < len)
    len = unilen/2;

  x=(LPSTR)HeapAlloc(GetProcessHeap(),
                     HEAP_ZERO_MEMORY,
                     len+1);

  UnicodeToAsciiN(unistr,
                  x,
                  len+1);

  memcpy(oemstr,
         x,
         len);

  if (reslen)
    *reslen = len;

  return 0;
}


/**************************************************************************
 *                 RtlUnicodeStringToOemString    [NTDLL.511]
 */
DWORD WINAPI RtlUnicodeStringToOemString(PANSI_STRING    oem,
                                            PUNICODE_STRING uni,
                                            BOOLEAN         alloc)
{
  dprintf(("NTDLL: RtlUnicodeStringToOemString(%08xh,%08xh,%08xh)\n",
           oem,
           uni,
           alloc));

  if (alloc)
  {
    oem->Buffer = (LPSTR)HeapAlloc(GetProcessHeap(),
                                   HEAP_ZERO_MEMORY,
                                   uni->Length/2)+1;
    oem->MaximumLength = uni->Length/2+1;
  }

  oem->Length = uni->Length/2;
  UnicodeToAsciiN(uni->Buffer,
                  oem->Buffer,
                  oem->MaximumLength);
  return 0;
}

/**************************************************************************
 *                 RtlUnicodeStringToAnsiString      [NTDLL.507]
 */
DWORD WINAPI RtlUnicodeStringToAnsiString(PANSI_STRING    oem,
                                             PUNICODE_STRING uni,
                                             BOOLEAN         alloc)
{
  dprintf(("NTDLL: RtlUnicodeStringToAnsiString(%08xh,%08xh,%08xh)\n",
           oem,
           uni,
           alloc));

  if (alloc)
  {
     oem->Buffer = (LPSTR)HeapAlloc(GetProcessHeap(),
                                    HEAP_ZERO_MEMORY,
                                    uni->Length/2)+1;
     oem->MaximumLength = uni->Length/2+1;
  }

  oem->Length = uni->Length/2;
  UnicodeToAsciiN(uni->Buffer,
                  oem->Buffer,
                  oem->MaximumLength);

  return 0;
}

/**************************************************************************
 *                 RtlEqualUnicodeString    [NTDLL]
 */
DWORD WINAPI RtlEqualUnicodeString(PUNICODE_STRING s1,
                                   PUNICODE_STRING s2,
                                   DWORD           x)
{
  dprintf(("NTDLL: RtlEqualUnicodeString(%08xh,%08xh,%08xh)\n",
           s1,
           s2,
           x));

  if (s1->Length != s2->Length)
    return 1;

  return !lstrncmpW(s1->Buffer,
                    s2->Buffer,
                    s1->Length/2);
}


/**************************************************************************
 *                 RtlUpcaseUnicodeString      [NTDLL.520]
 */
DWORD WINAPI RtlUpcaseUnicodeString(PUNICODE_STRING dest,
                                    PUNICODE_STRING src,
                                    BOOLEAN         doalloc)
{
  LPWSTR s,t;
  DWORD  i,len;

  dprintf(("NTDLL: RtlUpcaseUnicodeString(%08xh,%08xh,%08xh)\n",
           dest,
           src,
           doalloc));

  len = src->Length;
  if (doalloc)
  {
    dest->MaximumLength = len;
    dest->Buffer = (LPWSTR)HeapAlloc(GetProcessHeap(),
                                     HEAP_ZERO_MEMORY,
                                     len);
    if (!dest->Buffer)
      return STATUS_NO_MEMORY;
   }

  if (dest->MaximumLength < len)
    return STATUS_BUFFER_OVERFLOW;

  s=dest->Buffer;
  t=src->Buffer;

  /* len is in bytes */
  for (i = 0;
       i < len/2;
       i++)
    s[i] = towupper(t[i]);

  return STATUS_SUCCESS;
}


/**************************************************************************
 *                 RtlxOemStringToUnicodeSize     [NTDLL.549]
 */
UINT WINAPI RtlxOemStringToUnicodeSize(PSTRING str)
{
  dprintf(("NTDLL: RtlxOemStringToUnicodeSize(%08xh)\n",
           str));

  return str->Length*2+2;
}


/**************************************************************************
 *                 RtlxAnsiStringToUnicodeSize    [NTDLL.548]
 */
UINT WINAPI RtlxAnsiStringToUnicodeSize(PANSI_STRING str)
{
  dprintf(("NTDLL: RtlxAnsiStringToUnicodeSize(%08xh)\n",
           str));

  return str->Length*2+2;
}


/**************************************************************************
 *                 RtlIsTextUnicode            [NTDLL.417]
 *
 * Apply various feeble heuristics to guess whether
 * the text buffer contains Unicode.
 * FIXME: should implement more tests.
 */
DWORD WINAPI RtlIsTextUnicode(LPVOID buf,
                              DWORD  len,
                              DWORD  *pf)
{
  LPWSTR s = (LPWSTR)buf;
  DWORD  flags = -1,
         out_flags = 0;

  dprintf(("NTDLL: RtlIsTextUnicode(%08xh,%08xh,%08xh)\n",
           buf,
           len,
           pf));

  if (!len)
    goto out;

  if (pf)
    flags = *pf;

  /*
   * Apply various tests to the text string. According to the
   * docs, each test "passed" sets the corresponding flag in
   * the output flags. But some of the tests are mutually
   * exclusive, so I don't see how you could pass all tests ...
   */

  /* Check for an odd length ... pass if even. */
  if (!(len & 1))
    out_flags |= IS_TEXT_UNICODE_ODD_LENGTH;

  /* Check for the special unicode marker byte. */
  if (*s == 0xFEFF)
    out_flags |= IS_TEXT_UNICODE_SIGNATURE;

  /*
   * Check whether the string passed all of the tests.
   */
  flags &= ITU_IMPLEMENTED_TESTS;
  if ((out_flags & flags) != flags)
    len = 0;

out:
  if (pf)
    *pf = out_flags;

  return len;
}


/******************************************************************************
 * RtlCompareUnicodeString               [NTDLL]
 */
DWORD WINAPI RtlCompareUnicodeString(PUNICODE_STRING String1,
                                        PUNICODE_STRING String2,
                                        BOOLEAN         CaseInSensitive)
{
  dprintf(("NTDLL: RtlCompareUnicodeString(%08xh,%08xh,%08xh) not implemented.\n",
           String1,
           String2,
           CaseInSensitive));

  return 0;
}



/**************************************************************************
 *                 RtlUpcaseUnicodeStringToOemString    [NTDLL.?]
 * @@@PH: parameters are pure speculation!
 */
DWORD WINAPI RtlUpcaseUnicodeStringToOemString(PANSI_STRING    oem,
                                               PUNICODE_STRING uni,
                                               BOOLEAN         alloc)
{
  DWORD rc;

  dprintf(("NTDLL: RtlUpcaseUnicodeStringToOemString(%08xh,%08xh,%08xh)\n",
           oem,
           uni,
           alloc));

  rc = RtlUnicodeStringToOemString(oem,
                                   uni,
                                   alloc);
  if (rc == 0)
     strupr(oem->Buffer);

  return rc;
}


