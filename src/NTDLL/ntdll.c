/* $Id: ntdll.c,v 1.1 1999-05-24 20:19:31 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 NT Runtime for OS/2
 *
 * 1998/05/20 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) ntdll.cpp  1.0.1   1999/05/08 SvL: Changes for compilation with Wine headers     
 *                 1.0.0   1998/05/20 PH Start from WINE/NTDLL.C
 *
 * NT basis DLL
 *
 * Copyright 1996 Marcus Meissner
 * Copyright 1998 Patrick Haller (adapted for win32os2)
 */

 /* Changes to the original NTDLL.C from the WINE project

  - includes replaced by the win32os2 standard includes
  - replaced WINAPI by WIN32API
  - moved in some string functions
  - replaced HANDLE32 by HANDLE
  - lstrlen32A -> OS2lstrlenA
  - lstrlen32W -> OS2lstrlenW
*/

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <winnt.h>
#include <ntdef.h>

#include <builtin.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "misc.h"
#include "unicode.h"


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/


/***********************************************************************
*           lstrcpynAtoW   (Not a Windows API)
* Note: this function differs from the UNIX strncpy, it _always_ writes
* a terminating \0
*/
LPWSTR WIN32API lstrcpynAtoW(LPWSTR dst,
                             LPCSTR src,
                             INT  n)
{
  LPWSTR p = dst;
  while ((n-- > 1) && *src) *p++ = (WCHAR)(unsigned char)*src++;
  if (n >= 0) *p = 0;
  return dst;
}


/***********************************************************************
*           lstrcpynWtoA   (Not a Windows API)
* Note: this function differs from the UNIX strncpy, it _always_ writes
* a terminating \0
*/
LPSTR WIN32API lstrcpynWtoA(LPSTR   dst,
                            LPCWSTR src,
                            INT   n)
{
  LPSTR p = dst;
  while ((n-- > 1) && *src) *p++ = (CHAR)*src++;
  if (n >= 0) *p = 0;
  return dst;
}


/***********************************************************************
*           HEAP_strdupA
*/
LPSTR HEAP_strdupA(HANDLE heap,
                   DWORD  flags,
                   LPCSTR str)
{
  LPSTR p = HeapAlloc(heap,
                      flags,
                      lstrlenA(str) + 1 );

  lstrcpyA(p, str);
  return p;
}


/***********************************************************************
*           HEAP_strdupW
*/
LPWSTR HEAP_strdupW(HANDLE  heap,
                    DWORD   flags,
                    LPCWSTR str)
{
  INT len = lstrlenW(str) + 1;
  LPWSTR p = HeapAlloc(heap,
                       flags,
                       len * sizeof(WCHAR) );
  lstrcpyW(p,
              str);
  return p;
}


/***********************************************************************
*           HEAP_strdupWtoA
*/
LPSTR HEAP_strdupWtoA(HANDLE  heap,
                      DWORD   flags,
                      LPCWSTR str )
{
  LPSTR ret;

  if (!str)
    return NULL;

  ret = HeapAlloc(heap,
                  flags,
                  lstrlenW(str) + 1 );
  UnicodeToAscii((LPWSTR)str,
                 ret);
/*  lstrcpyWtoA(ret,
              str);*/
  return ret;
}

/***********************************************************************
*           HEAP_strdupAtoW
*/
LPWSTR HEAP_strdupAtoW(HANDLE heap,
                       DWORD  flags,
                       LPCSTR str)
{
  LPWSTR ret;

  if (!str)
    return NULL;

  ret = HeapAlloc(heap,
                     flags,
                     (lstrlenA(str)+1) * sizeof(WCHAR) );
  AsciiToUnicode((char *)str,
                 ret);
  /* lstrcpyAtoW(ret,
              str );*/

  return ret;
}


/**************************************************************************
 *                 RtlLengthRequiredSid          [NTDLL]
 */
DWORD WIN32API RtlLengthRequiredSid(DWORD nrofsubauths)
{
  dprintf(("NTDLL: RtlLengthRequireSid(%08x)\n",
           nrofsubauths));

  return sizeof(DWORD)*nrofsubauths+sizeof(SID);
}

/**************************************************************************
 *                 RtlLengthSid              [NTDLL]
 */
DWORD WIN32API RtlLengthSid(PSID sid)
{
  dprintf(("NTDLL: RtlLengthSid(%08x)\n",
           sid));

  return sizeof(DWORD)*sid->SubAuthorityCount+sizeof(SID);
}

/**************************************************************************
 *                 RtlCreateAcl              [NTDLL]
 *
 * NOTES
 *    This should return NTSTATUS
 */
DWORD WIN32API RtlCreateAcl(PACL acl,
                            DWORD size,
                            DWORD rev)
{
  dprintf(("NTDLL: RtlCreateAcl(%08x,%08x,%08x)\n",
           acl,
           size,
           rev));

   if (rev!=ACL_REVISION)
     return STATUS_INVALID_PARAMETER;

   if (size<sizeof(ACL))
     return STATUS_BUFFER_TOO_SMALL;

   if (size>0xFFFF)
     return STATUS_INVALID_PARAMETER;


  memset(acl,
         '\0',
         sizeof(ACL));

  acl->AclRevision  = rev;
  acl->AclSize      = size;
  acl->AceCount     = 0;
  return 0;
}

/**************************************************************************
 *                 RtlFirstFreeAce           [NTDLL]
 * looks for the AceCount+1 ACE, and if it is still within the alloced
 * ACL, return a pointer to it
 */
BOOL WIN32API RtlFirstFreeAce(PACL acl,
                              PACE_HEADER *x)
{
  PACE_HEADER ace;
  int          i;

  dprintf(("NTDLL: RtlFirstFreeAce(%08x,%08x)\n",
           acl,
           x));

   *x = 0;
   ace = (PACE_HEADER)(acl+1);
   for (i=0;i<acl->AceCount;i++) {
           if ((DWORD)ace>=(((DWORD)acl)+acl->AclSize))
                   return 0;
           ace = (PACE_HEADER)(((BYTE*)ace)+ace->AceSize);
   }
   if ((DWORD)ace>=(((DWORD)acl)+acl->AclSize))
           return 0;
   *x = ace;
   return 1;
}

/**************************************************************************
 *                 RtlAddAce             [NTDLL]
 */
DWORD  WIN32API RtlAddAce(PACL        acl,
                             DWORD        rev,
                             DWORD        xnrofaces,
                             PACE_HEADER acestart,
                             DWORD        acelen)
{
  PACE_HEADER  ace,targetace;
  int       nrofaces;

  dprintf(("NTDLL: RtlAddAce(%08x,%08x,%08x,%08x,%08x)\n",
           acl,
           rev,
           xnrofaces,
           acestart,
           acelen));

  if (acl->AclRevision != ACL_REVISION)
          return STATUS_INVALID_PARAMETER;
  if (!RtlFirstFreeAce(acl,&targetace))
          return STATUS_INVALID_PARAMETER;
  nrofaces=0;ace=acestart;
  while (((DWORD)ace-(DWORD)acestart)<acelen) {
          nrofaces++;
          ace = (PACE_HEADER)(((BYTE*)ace)+ace->AceSize);
  }
  if ((DWORD)targetace+acelen>(DWORD)acl+acl->AclSize) /* too much aces */
          return STATUS_INVALID_PARAMETER;
  memcpy((LPBYTE)targetace,acestart,acelen);
  acl->AceCount+=nrofaces;
  return 0;
}

/**************************************************************************
 *                 RtlCreateSecurityDescriptor       [NTDLL]
 */
DWORD WIN32API RtlCreateSecurityDescriptor(PSECURITY_DESCRIPTOR lpsd,
                                           DWORD rev)
{
  dprintf(("NTDLL: RtlCreateSecurityDescriptor(%08x,%08x)\n",
           lpsd,
           rev));

  if (rev!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;

  memset(lpsd,
         '\0',
         sizeof(*lpsd));
  lpsd->Revision = SECURITY_DESCRIPTOR_REVISION;
  return 0;
}

/**************************************************************************
 *                 RtlSetDaclSecurityDescriptor      [NTDLL]
 */
DWORD WIN32API RtlSetDaclSecurityDescriptor (PSECURITY_DESCRIPTOR lpsd,
                                                BOOL daclpresent,
                                                PACL dacl,
                                                BOOL dacldefaulted )
{
  dprintf(("NTDLL: RtlSetDaclSecurityDescriptor(%08x,%08x,%08x,%08x)\n",
           lpsd,
           daclpresent,
           dacl,
           dacldefaulted));

  if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;

  if (lpsd->Control & SE_SELF_RELATIVE)
    return STATUS_INVALID_SECURITY_DESCR;

  if (!daclpresent)
  {
    lpsd->Control &= ~SE_DACL_PRESENT;
    return 0;
  }

  lpsd->Control |= SE_DACL_PRESENT;
  lpsd->Dacl = dacl;

  if (dacldefaulted)
    lpsd->Control |= SE_DACL_DEFAULTED;
  else
    lpsd->Control &= ~SE_DACL_DEFAULTED;

  return 0;
}

/**************************************************************************
 *                 RtlSetSaclSecurityDescriptor      [NTDLL]
 */
DWORD WIN32API RtlSetSaclSecurityDescriptor (PSECURITY_DESCRIPTOR lpsd,
                                                BOOL saclpresent,
                                                PACL sacl,
                                                BOOL sacldefaulted)
{
  dprintf(("NTDLL: RtlSetSaclSecurityDescriptor(%08x,%08x,%08x,%08x)\n",
           lpsd,
           saclpresent,
           sacl,
           sacldefaulted));

  if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;

  if (lpsd->Control & SE_SELF_RELATIVE)
    return STATUS_INVALID_SECURITY_DESCR;

  if (!saclpresent)
  {
    lpsd->Control &= ~SE_SACL_PRESENT;
    return 0;
  }

  lpsd->Control |= SE_SACL_PRESENT;
  lpsd->Sacl = sacl;

  if (sacldefaulted)
    lpsd->Control |= SE_SACL_DEFAULTED;
  else
    lpsd->Control &= ~SE_SACL_DEFAULTED;

  return 0;
}

/**************************************************************************
 *                 RtlSetOwnerSecurityDescriptor     [NTDLL]
 */
DWORD WIN32API RtlSetOwnerSecurityDescriptor (PSECURITY_DESCRIPTOR lpsd,
                                                 PSID owner,
                                                 BOOL ownerdefaulted)
{
  dprintf(("NTDLL: RtlSetOwnerSecurityDescriptor(%08x,%08x,%08x)\n",
           lpsd,
           owner,
           ownerdefaulted));

  if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;

  if (lpsd->Control & SE_SELF_RELATIVE)
    return STATUS_INVALID_SECURITY_DESCR;

  lpsd->Owner = owner;

  if (ownerdefaulted)
    lpsd->Control |= SE_OWNER_DEFAULTED;
  else
    lpsd->Control &= ~SE_OWNER_DEFAULTED;

  return 0;
}

/**************************************************************************
 *                 RtlSetOwnerSecurityDescriptor     [NTDLL]
 */
DWORD WIN32API RtlSetGroupSecurityDescriptor (PSECURITY_DESCRIPTOR lpsd,
                                                 PSID group,
                                                 BOOL groupdefaulted)
{
  dprintf(("NTDLL: SetGroupSecurityDescriptor(%08x,%08x,%08x)\n",
           lpsd,
           group,
           groupdefaulted));

  if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
    return STATUS_UNKNOWN_REVISION;

  if (lpsd->Control & SE_SELF_RELATIVE)
    return STATUS_INVALID_SECURITY_DESCR;

  lpsd->Group = group;

  if (groupdefaulted)
    lpsd->Control |= SE_GROUP_DEFAULTED;
  else
    lpsd->Control &= ~SE_GROUP_DEFAULTED;

  return 0;
}


/**************************************************************************
 *                 RtlNormalizeProcessParams     [NTDLL]
 */
LPVOID WIN32API RtlNormalizeProcessParams(LPVOID x)
{
  dprintf(("NTDLL: RtlNormalizeProcessParams(%08xh)\n",
           x));

  return x;
}

/**************************************************************************
 *                 RtlInitializeSid          [NTDLL]
 */
DWORD WIN32API RtlInitializeSid(PSID PSID,
                                   PSID_IDENTIFIER_AUTHORITY PSIDauth,
                                   DWORD c)
{
  BYTE  a = c&0xff;

  dprintf(("NTDLL: RtlInitializeSid(%08x,%08x,%08x)\n",
           PSID,
           PSIDauth,
           c));

  if (a>=SID_MAX_SUB_AUTHORITIES)
    return a;

  PSID->SubAuthorityCount = a;
  PSID->Revision      = SID_REVISION;

  memcpy(&(PSID->IdentifierAuthority),
         PSIDauth,
         sizeof(SID_IDENTIFIER_AUTHORITY));

  return 0;
}

/**************************************************************************
 *                 RtlSubAuthoritySid            [NTDLL]
 */
LPDWORD WIN32API RtlSubAuthoritySid(PSID PSID,
                                       DWORD nr)
{
  dprintf(("NTDLL: RtlSubAuthoritySid(%08x,%08d)\n",
           PSID,
           nr));

  return &(PSID->SubAuthority[nr]);
}

/**************************************************************************
 *                 RtlSubAuthorityCountSid       [NTDLL]
 */
LPBYTE WIN32API RtlSubAuthorityCountSid(PSID PSID)
{
  dprintf(("NTDLL: RtlSubAuthorityCountSid(%08x)\n",
           PSID));

  return ((LPBYTE)PSID)+1;
}

/**************************************************************************
 *                 RtlCopySid                [NTDLL]
 */
DWORD WIN32API RtlCopySid(DWORD len,
                             PSID to,
                             PSID from)
{
  dprintf(("NTDLL: RtlCopySid(%08x,%08x,%08x)\n",
           len,
           to,
           from));

  if (len<(from->SubAuthorityCount*4+8))
    return STATUS_BUFFER_TOO_SMALL;

  memmove(to,
          from,
          from->SubAuthorityCount*4+8);

  return STATUS_SUCCESS;
}

/**************************************************************************
 *                 RtlAnsiStringToUnicodeString      [NTDLL]
 */
DWORD WIN32API RtlAnsiStringToUnicodeString(PUNICODE_STRING uni,
                                            PANSI_STRING    ansi,
                                            BOOL            doalloc)
{
  DWORD unilen = (ansi->Length+1)*sizeof(WCHAR);

  dprintf(("NTDLL: RtlAnsiStringToUnicodeString(%08x,%s,%08x)\n",
           uni,
           ansi,
           doalloc));

  if (unilen>0xFFFF)
    return STATUS_INVALID_PARAMETER_2;

  uni->Length = unilen;

  if (doalloc)
  {
    uni->MaximumLength = unilen;
    uni->Buffer = HeapAlloc(GetProcessHeap(),
                               HEAP_ZERO_MEMORY,
                               unilen);
    if (!uni->Buffer)
      return STATUS_NO_MEMORY;
  }

  if (unilen>uni->MaximumLength)
    return STATUS_BUFFER_OVERFLOW;

  lstrcpynAtoW(uni->Buffer,
               ansi->Buffer,
               unilen/2);

  return STATUS_SUCCESS;
}

/**************************************************************************
 *                 RtlOemStringToUnicodeString       [NTDLL]
 */
DWORD WIN32API RtlOemStringToUnicodeString(PUNICODE_STRING uni,
                                           PSTRING ansi,
                                           BOOL doalloc)
{
  DWORD unilen = (ansi->Length+1)*sizeof(WCHAR);

  dprintf(("NTDLL: RtlOemStringToUnicodeString (%08x,%s,%08x)\n",
           uni,
           ansi,
           doalloc));

  if (unilen>0xFFFF)
    return STATUS_INVALID_PARAMETER_2;

  uni->Length = unilen;

  if (doalloc)
  {
    uni->MaximumLength = unilen;
    uni->Buffer = HeapAlloc(GetProcessHeap(),
                               HEAP_ZERO_MEMORY,
                               unilen);

    if (!uni->Buffer)
      return STATUS_NO_MEMORY;
  }

  if (unilen>uni->MaximumLength)
    return STATUS_BUFFER_OVERFLOW;

  lstrcpynAtoW(uni->Buffer,
               ansi->Buffer,
               unilen/2);

  return STATUS_SUCCESS;
}


/**************************************************************************
 *                 RtlMultiByteToUnicodeN        [NTDLL]
 * FIXME: multibyte support
 */
DWORD WIN32API RtlMultiByteToUnicodeN(LPWSTR  unistr,
                                         DWORD   unilen,
                                         LPDWORD reslen,
                                         LPSTR   oemstr,
                                         DWORD   oemlen)
{
  DWORD  len;
  LPWSTR x;

  dprintf(("NTDLL: RtlMultiByteToUnicodeN(%08x,%08x,%08x,%s,%08x)\n",
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
  lstrcpynAtoW(x,
               oemstr,
               len+1);
  memcpy(unistr,
         x,
         len*2);

  if (reslen)
    *reslen = len*2;

  return 0;
}

/**************************************************************************
 *                 RtlOemToUnicodeN          [NTDLL]
 */
DWORD WIN32API RtlOemToUnicodeN(LPWSTR unistr,
                                   DWORD unilen,
                                   LPDWORD reslen,
                                   LPSTR oemstr,
                                   DWORD oemlen)
{
  DWORD  len;
  LPWSTR x;

  dprintf(("NTDLL: RtlOemToUnicodeN(%08x,%08x,%08x,%s,%08x)\n",
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

  lstrcpynAtoW(x,
               oemstr,
               len+1);

  memcpy(unistr,
         x,
         len*2);

  if (reslen)
    *reslen = len*2;

  return 0;
}

/**************************************************************************
 *                 RtlInitString         [NTDLL]
 */
VOID WIN32API RtlInitAnsiString(PANSI_STRING target,
                                   LPCSTR source)
{
  dprintf(("NTDLL: RtlInitAnsiString(%08x,%08x)\n",
           target,
           source));

  target->Length = target->MaximumLength = 0;
  target->Buffer = (LPSTR)source;
  if (!source)
    return;

  target->Length = lstrlenA(target->Buffer);
  target->MaximumLength = target->Length+1;
}

/**************************************************************************
 *                 RtlInitString         [NTDLL]
 */
VOID WIN32API RtlInitString(PSTRING target,
                               LPCSTR source)
{
  dprintf(("NTDLL: RtlInitString(%08x,%08x)\n",
           target,
           source));

  target->Length = target->MaximumLength = 0;
  target->Buffer = (LPSTR)source;
  if (!source)
    return;

  target->Length = lstrlenA(target->Buffer);
  target->MaximumLength = target->Length+1;
}

/**************************************************************************
 *                 RtlInitUnicodeString          [NTDLL]
 */
VOID WIN32API RtlInitUnicodeString(PUNICODE_STRING target,
                                      LPCWSTR source)
{
  dprintf(("NTDLL: RtlInitUnicodeString(%08x,%08x)\n",
           target,
           source));

  target->Length = target->MaximumLength = 0;
  target->Buffer = (LPWSTR)source;
  if (!source)
    return;

  target->Length = lstrlenW(target->Buffer)*2;
  target->MaximumLength = target->Length+2;
}

/**************************************************************************
 *                 RtlFreeUnicodeString          [NTDLL]
 */
VOID WIN32API RtlFreeUnicodeString(PUNICODE_STRING str)
{
  dprintf(("NTDLL: RtlFreeUnicodeString(%08x)\n",
           str));

  if (str->Buffer)
    HeapFree(GetProcessHeap(),
                0,
                str->Buffer);
}

/**************************************************************************
 *                 RtlUnicodeToOemN          [NTDLL]
 */
DWORD WIN32API RtlUnicodeToOemN(LPSTR   oemstr,
                                   DWORD   oemlen,
                                   LPDWORD reslen,
                                   LPWSTR  unistr,
                                   DWORD   unilen)
{
  DWORD len;
  LPSTR x;

  dprintf(("NTDLL: RtlUnicodeToOemN (%08x,%08x,%08x,%08x,%08x)\n",
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

  lstrcpynWtoA(x,
               unistr,
               len+1);

  memcpy(oemstr,
         x,
         len);

  if (reslen)
    *reslen = len;

  return 0;
}

/**************************************************************************
 *                 RtlUnicodeStringToOemString       [NTDLL]
 */
DWORD WIN32API RtlUnicodeStringToOemString(PANSI_STRING oem,
                                              PUNICODE_STRING uni,
                                              BOOL alloc)
{
  dprintf(("NTDLL: RtlUnicodeStringToOemString (%08x,%08x,%08x)\n",
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
  lstrcpynWtoA(oem->Buffer,
               uni->Buffer,
               uni->Length/2+1);

  return 0;
}

/**************************************************************************
 *                 RtlUnicodeStringToAnsiString      [NTDLL]
 */
DWORD WIN32API RtlUnicodeStringToAnsiString(PUNICODE_STRING uni,
                                               PANSI_STRING oem,
                                               BOOL alloc)
{
  dprintf(("NTDLL: RtlUnicodeStringToAnsiString(%08x,%08x,%08x)\n",
           uni,
           oem,
           alloc));

  if (alloc)
  {
    oem->Buffer = (LPSTR)HeapAlloc(GetProcessHeap(),
                                      HEAP_ZERO_MEMORY,
                                      uni->Length/2)+1;
    oem->MaximumLength = uni->Length/2+1;
  }

  oem->Length = uni->Length/2;
  lstrcpynWtoA(oem->Buffer,
               uni->Buffer,
               uni->Length/2+1);

  return 0;
}

/**************************************************************************
 *                 RtlNtStatusToDosErro          [NTDLL]
 */
DWORD WIN32API RtlNtStatusToDosError(DWORD error)
{
  dprintf(("NTDLL: RtlNtStatusToDosErro(%08x) not implemented properly.\n",
           error));

  /* @@@PH: map STATUS_ to ERROR_ */
  return error;
}

/**************************************************************************
 *                 RtlGetNtProductType           [NTDLL]
 */
DWORD WIN32API RtlGetNtProductType(LPVOID x)
{
  dprintf(("NTDLL: RtlGetNtProductType(%08x) not implemented.\n",
           x));

  /* @@@PH : find documentation for this one */
  return 0;
}

/**************************************************************************
 *                 RtlUpcaseUnicodeString        [NTDLL]
 */
DWORD WIN32API RtlUpcaseUnicodeString(PUNICODE_STRING dest,
                                         PUNICODE_STRING src,
                                         BOOL           doalloc)
{
  LPWSTR s;
  LPWSTR t;
  DWORD  i,len;

  dprintf(("NTDLL: RtlUpcaseUnicodeString(%08x,%08x,%08x)\n",
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
  for (i=0;
       i<len/2;
       i++)
    s[i]=toupper(t[i]);

  return STATUS_SUCCESS;
}

/**************************************************************************
 *                 RtlxOemStringToUnicodeSize        [NTDLL]
 */
UINT WIN32API RtlxOemStringToUnicodeSize(PSTRING str)
{
  dprintf(("NTDLL: RtlxOemStringToUnicodeSize(%08x)\n",
           str));

  return str->Length*2+2;
}

/**************************************************************************
 *                 RtlxAnsiStringToUnicodeSize       [NTDLL]
 */
UINT WIN32API RtlxAnsiStringToUnicodeSize(PANSI_STRING str)
{
  dprintf(("NTDLL: RtlxAnsiStringToUnicodeSize(%08x)\n",
           str));

  return str->Length*2+2;
}

/**************************************************************************
 *                 RtlDosPathNameToNtPathName_U      [NTDLL]
 *
 * FIXME: convert to UNC or whatever is expected here
 */
BOOL  WIN32API RtlDosPathNameToNtPathName_U(
    LPWSTR from,PUNICODE_STRING us,DWORD x2,DWORD x3)
{
  LPSTR fromA = HEAP_strdupWtoA(GetProcessHeap(),
                                0,
                                from);

  dprintf(("NTDLL: RtlDosPathNameToNtPathName_U(%s,%p,%08lx,%08lx)\n",
           fromA,
           us,
           x2,
           x3));

  if (us)
    RtlInitUnicodeString(us,
                         HEAP_strdupW(GetProcessHeap(),
                                      0,
                                      from));

  return TRUE;
}

/**************************************************************************
 *                 NtOpenFile               [NTDLL]
 */
DWORD WIN32API NtOpenFile(DWORD x1,
                             DWORD flags,
                             DWORD x3,
                             DWORD x4,
                             DWORD alignment,
                             DWORD x6)
{
  dprintf(("NTDLL: NtOpenFile (%08lx,%08lx,%08lx,%08lx,%08lx,%08lx) not implemented\n",
           x1,
           flags,
           x3,
           x4,
           alignment,
           x6));

  /* returns file io completion status */
  return 0;
}


//******************************************************************************
//NtClose has to call CloseHandle since the Handlemanager has to be
//called.
//******************************************************************************
DWORD WIN32API NtClose(HANDLE hHandle)
{
  dprintf(("KERNEL32: NtClose(%08x) not properly implemented.\n",
           hHandle));

  /* @@@PH 98/05/05 function from NTDLL */
  return (CloseHandle(hHandle));
}


//******************************************************************************
//TODO: What's this? (not found in SDK)
//******************************************************************************
HANDLE WIN32API RtlOpenCurrentUser(ULONG ulDummy1,
                                      ULONG ulDummy2)
{
  dprintf(("KERNEL32: RtlOpenCurrentUser(%08x, %08x) not implemented.\n",
           ulDummy1,
           ulDummy2));

  /* @@@PH required by NT's MSACM32 */
  return (HANDLE)(NULL);
}
