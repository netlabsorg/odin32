/* $Id: guid.c,v 1.2 2001-07-29 19:02:35 sandervl Exp $ */
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
    return towupper(*str1) - towupper(*str2);
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
