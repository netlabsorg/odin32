/* $Id: guid.c,v 1.4 2001-08-04 17:19:20 sandervl Exp $ */
#define ICOM_CINTERFACE 1
#include <odin.h>

#define CINTERFACE 1
#define INITGUID
#include <wine\obj_oleview.h>
#include "initguid.h"

#include "shlwapi.h"
#include "shlguid.h"
#include "shlobj.h"
#include "docobj.h"
#include "..\shell32\shellfolder.h"

#include "wine/obj_inplace.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_surrogate.h"
#include "wine/obj_errorinfo.h"
#include "wine/obj_oleview.h"
#include "wine/obj_clientserver.h"
#include "wine/obj_cache.h"
#include "wine\obj_oleaut.h"
#include "wine\obj_olefont.h"

#include <wine/obj_oleview.h>
#include <wine/obj_dragdrop.h>
#include <wine/obj_inplace.h>
#include <wine/obj_control.h>
#include <wine/obj_shellfolder.h>
#include <wine/obj_shelllink.h>
#include <wine/obj_contextmenu.h>
#include <wine/obj_commdlgbrowser.h>
#include <wine/obj_extracticon.h>
#include <wine/obj_shellextinit.h>
#include <wine/obj_shellbrowser.h>
#include <wine/obj_serviceprovider.h>
#include <wine/unicode.h>

#include <misc.h>

/*********************************************************************
 *           CRTDLL__wcsnicmp	 (CRTDLL.321)
 */
int CDECL CRTDLL__wcsnicmp(LPCWSTR str1, LPCWSTR str2, int n)
{
    if (!n) return 0;
    while ((--n > 0) && *str1 && (towupper(*str1) == towupper(*str2)))
    {
        str1++;
        str2++;
    }
    return toupperW(*str1) - toupperW(*str2);
}

/*********************************************************************
 *           wcstombs    (NTDLL.@)
 */
INT __cdecl NTDLL_wcstombs( LPSTR dst, LPCWSTR src, INT n )
{
    INT ret;
    if (n <= 0) return 0;
    ret = WideCharToMultiByte( CP_ACP, 0, src, -1, dst, dst ? n : 0, NULL, NULL );
    if (!ret) return n;  /* overflow */
    return ret - 1;  /* do not count terminating NULL */
}

/*********************************************************************
 *           _wtol    (NTDLL.@)
 * Like atol, but for wide character strings.
 */
LONG __cdecl _wtol(LPWSTR string)
{
    char buffer[30];
    NTDLL_wcstombs( buffer, string, sizeof(buffer) );
    return atol( buffer );
}

/*********************************************************************
 *           _wtoi    (NTDLL.@)
 */
INT __cdecl _wtoi(LPWSTR string)
{
    return _wtol(string);
}

/******************************************************************************
 *  RtlAllocateAndInitializeSid             [NTDLL.265]
 *
 */
BOOLEAN WINAPI RtlAllocateAndInitializeSid ( PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
					     BYTE nSubAuthorityCount,
					     DWORD nSubAuthority0,
					     DWORD nSubAuthority1,
					     DWORD nSubAuthority2,
					     DWORD nSubAuthority3,
					     DWORD nSubAuthority4,
					     DWORD nSubAuthority5,
					     DWORD nSubAuthority6,
					     DWORD nSubAuthority7,
					     PSID *pSid)
{
  dprintf(("NTDLL: RtlAllocateAndInitializeSid(%08xh,%08xh,%08xh,"
           "%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)",
           pIdentifierAuthority,
           nSubAuthorityCount,
           nSubAuthority0,
           nSubAuthority1,
           nSubAuthority2,
           nSubAuthority3,
           nSubAuthority4,
           nSubAuthority5,
           nSubAuthority6,
           nSubAuthority7,
           pSid));

  *pSid = (PSID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SID)+nSubAuthorityCount*sizeof(DWORD));
  if(*pSid == NULL) {
	SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	return FALSE;
  }
  (*pSid)->Revision          = SID_REVISION;
  (*pSid)->SubAuthorityCount = nSubAuthorityCount;

  if (nSubAuthorityCount > 0)
        (*pSid)->SubAuthority[0] = nSubAuthority0;
  if (nSubAuthorityCount > 1)
        (*pSid)->SubAuthority[1] = nSubAuthority1;
  if (nSubAuthorityCount > 2)
        (*pSid)->SubAuthority[2] = nSubAuthority2;
  if (nSubAuthorityCount > 3)
        (*pSid)->SubAuthority[3] = nSubAuthority3;
  if (nSubAuthorityCount > 4)
        (*pSid)->SubAuthority[4] = nSubAuthority4;
  if (nSubAuthorityCount > 5)
        (*pSid)->SubAuthority[5] = nSubAuthority5;
  if (nSubAuthorityCount > 6)
        (*pSid)->SubAuthority[6] = nSubAuthority6;
  if (nSubAuthorityCount > 7)
        (*pSid)->SubAuthority[7] = nSubAuthority7;

  if(pIdentifierAuthority) 
  	memcpy((PVOID)&(*pSid)->IdentifierAuthority, (PVOID)pIdentifierAuthority, sizeof(SID_IDENTIFIER_AUTHORITY));
  return TRUE;
}
