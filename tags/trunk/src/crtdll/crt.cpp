/* $Id: crt.cpp,v 1.4 2000-01-06 20:08:06 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 *
 * Copyright 1999 Patrick Haller (phaller@gmx.net)
 * Copyright 1999 Alexandre Julliard (CRTDLL__wcsnicmp)
 *
 */

/****************************************************************************
 * Include                                                                  *
 ****************************************************************************/

#include <odin.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <wchar.h>
#include <wcstr.h>
#include <wctype.h>

#include <os2win.h>
#include <misc.h>

#include <ntdef.h>
#include <winnt.h>
#include "winbase.h"                     /* fixme: should be taken out sometimes */
#include <heapstring.h>
#include "asmhlp.h"


/****************************************************************************
 * Local Prototypes                                                         *
 ****************************************************************************/


LPWSTR CDECL CRTDLL__wcsupr(LPWSTR str);
int    CDECL CRTDLL__wcsnicmp(LPWSTR str1, LPWSTR str2, long l);



/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.879
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL__wcsicmp(LPWSTR str1, LPWSTR str2)
{
  dprintf2(("CRTDLL: _wcsicmp(%08xh,%08xh)\n",
           str1,
           str2));

  return (CRTDLL__wcsnicmp(str1,
                       str2,
                       wcslen((wchar_t*) str1)));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.880
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPWSTR CDECL CRTDLL__wcslwr(LPWSTR str)
{
  DWORD dwIndex;

  dprintf2(("CRTDLL: _wcslwr(%08xh)\n",
           str));

  for (dwIndex = wcslen((const wchar_t*)str);
       dwIndex;
       dwIndex--)
  {
    towlower(str[dwIndex]);
  }

  return (str);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.881
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL__wcsnicmp(LPWSTR str1, LPWSTR str2, long n)
{
  dprintf2(("CRTDLL: _wcsnicmp(%08xh,%08xh,%08xh)\n",
           str1,
           str2,
           l));

    if (!n) return 0;
    while ((--n > 0) && *str1 && (towupper(*str1) == towupper(*str2)))
    {
        str1++;
        str2++;
    }
    return towupper(*str1) - towupper(*str2);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.882
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPWSTR CDECL CRTDLL__wcsupr(LPWSTR str)
{
  DWORD dwIndex;

  dprintf2(("CRTDLL: _wcsupr(%08xh)\n",
           str));

  for (dwIndex = wcslen((const wchar_t*)str);
       dwIndex;
       dwIndex--)
  {
    towupper(str[dwIndex]);
  }

  return (str);
}



/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.883
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

double CDECL CRTDLL_abs(double d)
{
  dprintf2(("CRTDLL: abs(%f)\n",
           d));

  return (abs(d));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.884
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

double CDECL CRTDLL_atan(double d)
{
  dprintf2(("CRTDLL: atan(%f)\n",
           d));

  return (atan(d));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.885
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_atoi(LPSTR str)
{
  dprintf2(("CRTDLL: atoi(%s)\n",
           str));

  return (atoi(str));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.886
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

long CDECL CRTDLL_atol(LPSTR str)
{
  dprintf2(("CRTDLL: atol(%s)\n",
           str));

  return (atol(str));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.887
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

double CDECL CRTDLL_ceil(double d)
{
  dprintf2(("CRTDLL: ceil(%f)\n",
           d));

  return (ceil(d));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.888
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

double CDECL CRTDLL_cos(double d)
{
  dprintf2(("CRTDLL: cos(%f)\n",
           d));

  return (cos(d));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.889
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

double CDECL CRTDLL_fabs(double d)
{
  dprintf2(("CRTDLL: fabs(%f)\n",
           d));

  return (fabs(d));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.890
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

double CDECL CRTDLL_floor(double d)
{
  dprintf2(("CRTDLL: floor(%f)\n",
           d));

  return (floor(d));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.891
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_isalpha(int i)
{
  dprintf2(("CRTDLL: isalpha(%08xh)\n",
           i));

  return (isalpha(i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.892
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_isdigit(int i)
{
  dprintf2(("CRTDLL: isdigit(%08xh)\n",
           i));

  return (isdigit(i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.893
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_islower(int i)
{
  dprintf2(("CRTDLL: islower(%08xh)\n",
           i));

  return (islower(i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.894
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_isprint(int i)
{
  dprintf2(("CRTDLL: isprint(%08xh)\n",
           i));

  return (isprint(i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.895
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_isspace(int i)
{
  dprintf2(("CRTDLL: isspace(%08xh)\n",
           i));

  return (isspace(i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.896
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_isupper(int i)
{
  dprintf2(("CRTDLL: isupper(%08xh)\n",
           i));

  return (isupper(i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.911
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_sprintf(LPSTR lpstrBuffer,
                       LPSTR lpstrFormat,
                       ...)
{
  va_list argptr;                          /* -> variable argument list */

  dprintf2(("CRTDLL: sprintf(%08xh,%s)\n",
           lpstrBuffer,
           lpstrFormat));

  va_start(argptr,
           lpstrFormat);                   /* get pointer to argument list */
  vsprintf(lpstrBuffer,
           lpstrFormat,
           argptr);
  va_end(argptr);                          /* done with variable arguments */

  return (lpstrBuffer);
}



/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.914
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_strcat(      LPSTR str1,
                      const LPSTR str2)
{
  dprintf2(("CRTDLL: strcat\n"));

  return (strcat(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.915
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_strchr(const LPSTR str,
                       int         i)
{
  dprintf2(("CRTDLL: strchr(%s,%08xh)\n",
           str,
           i));

  return (strchr(str, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.916
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_strcmp(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf2(("CRTDLL: strcmp(%s,%s)\n",
           str1,
           str2));

  return (strcmp(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL__stricmp(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf2(("CRTDLL: _stricmp(%s,%s)\n",
           str1,
           str2));

  return (stricmp(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.917
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_strcpy(      LPSTR str1,
                       const LPSTR str2)
{
  dprintf2(("CRTDLL: strcpy\n"));

  return (strcpy(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.918
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

size_t CDECL CRTDLL_strcspn(const LPSTR str1,
                                LPSTR str2)
{
  dprintf2(("CRTDLL: strcspn(%s,%s)\n",
           str1,
           str2));

  return (strcspn(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.919
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

size_t CDECL CRTDLL_strlen(const LPSTR str)
{
  dprintf2(("CRTDLL: strlen(%s)\n",
           str));

  return (strlen(str));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.920
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_strncat(      LPSTR str1,
                        const LPSTR str2,
                        size_t      i)
{
  dprintf2(("CRTDLL: strncat(%s,%s,%08xh)\n",
           str1,
           str2,
           i));

  return (strncat(str1, str2, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.921
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_strncmp(const LPSTR str1,
                       const LPSTR str2,
                       size_t      i)
{
  dprintf2(("CRTDLL: strncmp(%s,%s,%08xh)\n",
           str1,
           str2,
           i));

  return (strncmp(str1, str2, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.922
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_strncpy(const LPSTR str1,
                        const LPSTR str2,
                        size_t      i)
{
  dprintf2(("CRTDLL: strncpy(%s,%s,%08xh)\n",
           str1,
           str2,
           i));

  return (strncpy(str1, str2, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.923
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_strpbrk(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf2(("CRTDLL: strpbrk(%s,%s)\n",
           str1,
           str2));

  return (strpbrk(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.924
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_strrchr(const LPSTR str,
                        size_t      i)
{
  dprintf2(("CRTDLL: strrchr(%s,%08xh)\n",
           str,
           i));

  return (strrchr(str, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.925
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

size_t CDECL CRTDLL_strspn(const LPSTR str1,
                       const LPSTR str2)
{
  dprintf2(("CRTDLL: strspn(%s,%s)\n",
           str1,
           str2));

  return (strspn(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.926
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

LPSTR CDECL CRTDLL_strstr(const LPSTR str1,
                       const LPSTR str2)
{
  dprintf2(("CRTDLL: strstr(%s,%s)\n",
           str1,
           str2));

  return (strstr(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.927
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_swprintf(const LPWSTR str,
                      int   i,
                      const LPWSTR format,
                      ...)
{
  va_list valist;
  int     rc;

  dprintf2(("CRTDLL: swprintf(%s,%d,%s)\n",
           str,
           i,
           format));

  va_start( valist, format );
  rc = vswprintf( (wchar_t*)str,
                  i,
                  (wchar_t*)format,
                  valist );
  va_end( valist );
  return rc;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.928
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

double CDECL CRTDLL_tan(double d)
{
  dprintf2(("CRTDLL: tan(%f)\n",
           d));

  return (tan(d));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.929
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_toupper(int c)
{
  dprintf2(("CRTDLL: toupper(%c)\n",
           c));

  return (toupper(c));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.930
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_tolower(int c)
{
  dprintf2(("CRTDLL: tolower(%c)\n",
           c));

  return (tolower(c));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.931
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_towupper(int c)
{
  dprintf2(("CRTDLL: towupper(%c)\n",
           c));

  return (towupper(c));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.932
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_towlower(int c)
{
  dprintf2(("CRTDLL: towlower(%c)\n",
           c));

  return (towlower(c));
}



/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.934
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

wchar_t* CDECL CRTDLL_wcscat(      wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcscat(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcscat(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.935
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

wchar_t* CDECL CRTDLL_wcschr(const wchar_t* str,
                       int          i)
{
  dprintf2(("CRTDLL: wcschr(%08xh,%08xh)\n",
           str,
           i));

  return (wcschr(str, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.936
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_wcscmp(const wchar_t* str1,
                    const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcscmp(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcscmp(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.937
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

wchar_t* CDECL CRTDLL_wcscpy(      wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcscpy(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcscpy(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.938
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

size_t CDECL CRTDLL_wcscspn(const wchar_t* str1,
                              wchar_t* str2)
{
  dprintf2(("CRTDLL: wcscspn(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcscspn(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.939
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

size_t CDECL CRTDLL_wcslen(const wchar_t* str)
{
  dprintf2(("CRTDLL: wcslen(%08xh)\n",
           str));

  return (wcslen(str));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.940
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

wchar_t* CDECL CRTDLL_wcsncat(      wchar_t* str1,
                        const wchar_t* str2,
                        size_t      i)
{
  dprintf2(("CRTDLL: wcsncat(%08xh,%08xh,%08xh)\n",
           str1,
           str2,
           i));

  return (wcsncat(str1, str2, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.941
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

int CDECL CRTDLL_wcsncmp(const wchar_t* str1,
                     const wchar_t* str2,
                     size_t      i)
{
  dprintf2(("CRTDLL: wcsncmp(%08xh,%08xh,%08xh)\n",
           str1,
           str2,
           i));

  return (wcsncmp(str1, str2, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.942
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

wchar_t* CDECL CRTDLL_wcsncpy(      wchar_t* str1,
                          const wchar_t* str2,
                          size_t       i)
{
  dprintf2(("CRTDLL: wcsncpy(%s,%s,%08xh)\n",
           str1,
           str2,
           i));

  return (wcsncpy(str1, str2, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.943
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

wchar_t* CDECL CRTDLL_wcspbrk(const wchar_t* str1,
                        const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcspbrk(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcspbrk(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.944
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

wchar_t* CDECL CRTDLL_wcsrchr(const wchar_t* str,
                        size_t       i)
{
  dprintf2(("CRTDLL: wcsrchr(%08xh,%08xh)\n",
           str,
           i));

  return (wcsrchr(str, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.945
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

size_t CDECL CRTDLL_wcsspn(const wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcsspn(%08xh,%08xh)\n",
           str1,
           str2));

  return (wcsspn(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.946
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

wchar_t* CDECL CRTDLL_wcsstr(const wchar_t* str1,
                         const wchar_t* str2)
{
  dprintf2(("CRTDLL: wcsstr(%s,%s)\n",
           str1,
           str2));

  return (wcsstr(str1, str2));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

char * CDECL CRTDLL__itoa(int i, char *s, int r)
{
  dprintf2(("CRTDLL: _itoa(%08xh, %08xh, %08xh)\n",
           i,
           s,
           r));

  return (itoa(i,s,r));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.?
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 1999/06/22 20:44]
 *****************************************************************************/

char * CDECL CRTDLL__itow(int i, char *s, int r)
{
  dprintf(("CRTDLL: _itow(%08xh, %08xh, %08xh) no unicode support !\n",
           i,
           s,
           r));

  return (itoa(i,s,r));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.749
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

LONG CDECL CRTDLL__CIpow()
{
	double x,y;
	POP_FPU(y);
	POP_FPU(x);
	return pow(x,y);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.864
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

LONG CDECL CRTDLL__ftol(void)
{
	/* don't just do DO_FPU("fistp",retval), because the rounding
	 * mode must also be set to "round towards zero"... */
	double fl;
	POP_FPU(fl);
	return (LONG)fl;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.866
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

LPSTR  CDECL CRTDLL__ltoa(long x,LPSTR buf,INT radix)
{
    return ltoa(x,buf,radix);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.868
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

INT CDECL CRTDLL__memicmp(
	LPCSTR s1,	/* [in] first string */
	LPCSTR s2,	/* [in] second string */
	DWORD len	/* [in] length to compare */ )
{
	dprintf2(("CRTDLL: memicmp(%08xh, %08xh, %08xh)\n",s1,s2,len));
	int	i;

	for (i=0;i<len;i++) {
		if (tolower(s1[i])<tolower(s2[i]))
			return -1;
		if (tolower(s1[i])>tolower(s2[i]))
			return  1;
	}
	return 0;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.869
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int  CDECL CRTDLL__snprintf( char *buf, size_t bufsize, const char *fmt, ... )
{
  dprintf(("CRTDLL: _snprintf(%08xh, %08xh, %08xh) not implemented\n",
           buf,
           bufsize,
           fmt));

  return 0;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.870
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int  CDECL CRTDLL__snwprintf( wchar_t *buf, size_t bufsize, const wchar_t *fmt, ... )
{
  dprintf(("CRTDLL: _snwprintf(%08xh, %08xh, %08xh) not implemented\n",
           buf,
           bufsize,
           fmt));

  return 0;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.871
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

void CDECL CRTDLL__splitpath( const char *path, char *drive, char *dir, char *fname, char *ext )
{
	dprintf2(("CRTDLL: _splitpath"));

	char *tmp_drive;
	char *tmp_dir;
	char *tmp_ext;

	tmp_drive = (char *)strchr(path,':');
	if ( tmp_drive != (char *)NULL ) {
		strncpy(drive,tmp_drive-1,1);
		*(drive+1) = 0;
	}
	else {
		*drive = 0;
		tmp_drive = (char *)path;
	}

	tmp_dir = (char *)strrchr(path,'\\');
	if( tmp_dir != NULL && tmp_dir != tmp_drive + 1 ) {
		strncpy(dir,tmp_drive+1,tmp_dir - tmp_drive);
		*(dir + (tmp_dir - tmp_drive)) = 0;
	}
	else 	
		*dir =0;

	tmp_ext = ( char *)strrchr(path,'.');
	if ( tmp_ext != NULL ) {
		strcpy(ext,tmp_ext);
	}
	else
		*ext = 0;
    if ( tmp_dir != NULL ) {
		strncpy(fname,tmp_dir+1,tmp_ext - tmp_dir - 1);
		*(fname + (tmp_ext - tmp_dir -1)) = 0;
	}
	else
		strncpy(fname,path,tmp_ext - path);

}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.872
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

void CDECL CRTDLL__strcmpi( LPCSTR s1, LPCSTR s2 )
{
  dprintf2(("CRTDLL: _strcmpi(%08xh, %08xh)\n",
           s1,
           s2));

  lstrcmpiA( s1, s2 );
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.874
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

CHAR * CDECL CRTDLL__strlwr(char *x)
{
  char *y =x;

  dprintf2(("CRTDLL: _strlwr got %s\n", x));
  while (*y) {
    if ((*y > 0x40) && (*y< 0x5b))
      *y = *y + 0x20;
    y++;
  }
  dprintf2(("   returned %s\n", x));
		
  return x;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.875
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int  CDECL CRTDLL__strnicmp( LPCSTR s1, LPCSTR s2, INT n )
{
  dprintf2(("CRTDLL: _strnicmp (%s,%s,%d)\n",
           s1,
           s2,
           n));

  // @@@PH: sure it's not a UNICODE API?
  return (lstrncmpiA(s1,s2,n));

/*
  if (n == 0)
    return 0;
  do {
    if (toupper(*s1) != toupper(*s2++))
      return toupper(*(unsigned const char *)s1) - toupper(*(unsigned const char *)--s2);
    if (*s1++ == 0)
      break;
  } while (--n != 0);
  return 0;
*/
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.876
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

LPSTR CDECL CRTDLL__strupr(LPSTR x)
{
  dprintf2(("CRTDLL: _strupr(%s)\n",
           x));

  LPSTR y=x;

  while (*y)
  {
    *y=toupper(*y);
    y++;
  }
  return x;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.877
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

LPSTR  CDECL CRTDLL__ultoa(long x,LPSTR buf,INT radix)
{
    return ultoa(x,buf,radix);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.878
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int CDECL CRTDLL__vsnprintf( char *s, size_t bufsize, const char *format, va_list arg )
{
  dprintf2(("CRTDLL: _vsnprintf(%08xh, %08xh, %08xh)\n",
           s,
           bufsize,
           format));

  return wvsnprintfA(s, bufsize, format, arg);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.897
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int CDECL CRTDLL_iswalpha(wint_t i)
{
  dprintf2(("CRTDLL: iswalpha(%08xh)\n", i));

  return (iswalpha(i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.898
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int CDECL CRTDLL_iswctype(wint_t i, wctype_t wct)
{
  dprintf2(("CRTDLL: iswctype(%08xh, %08xh)\n", i, wct));

  return (iswctype(i, wct));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.899
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int CDECL CRTDLL_isxdigit(int i)
{
  dprintf2(("CRTDLL: isxdigit(%08xh)\n", i));

  return (isxdigit(i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.900
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

long int CDECL CRTDLL_labs( long int j )
{
  dprintf2(("CRTDLL: labs(%08xh)\n", j));

  return (labs(j));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.901
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

double CDECL CRTDLL_log( double x )
{
  dprintf2(("CRTDLL: log(%08xh)\n", x));
  return (log(x));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.902
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

size_t CDECL CRTDLL_mbstowcs( wchar_t *pwcs, const char *s, size_t n )
{
      dprintf2(("CRTDLL: mbstowcs(%08xh, %08xh, %08xh)\n", pwcs, s, n));
      return (mbstowcs(pwcs, s, n));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.903
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

void * CDECL CRTDLL_memchr( const void *s, int c, size_t n )
{
    dprintf2(("CRTDLL: memchr(%08xh, %08xh, %08xh)\n", s, c, n));
    return memchr( s, c, n );
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.904
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int CDECL CRTDLL_memcmp( const void * c1, const void * c2, size_t n )
{
    dprintf2(("CRTDLL: memcmp(%08xh, %08xh, %08xh)\n", c1, c2, n));
    return memcmp( c1, c2, n );
}

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.905
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

void * CDECL CRTDLL_memcpy( void *s1, const void *s2, size_t n )
{
    dprintf2(("CRTDLL: memcpy(%08xh, %08xh, %08xh)\n", s1, s2, n));
    return memcpy( s1, s2, n );
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.907
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

void * CDECL CRTDLL_memset( void *s, int i, size_t n )
{
    dprintf2(("CRTDLL: memset(%08xh, %08xh, %08xh)\n", s, i, n));
    return memset( s, i, n );
}
//******************************************************************************
VOID CDECL CRTDLL_memmove(VOID UNALIGNED *Destination, CONST VOID UNALIGNED *Source, DWORD Length)
{
  memmove(Destination, Source, Length);
}
//******************************************************************************

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.908
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

double CDECL CRTDLL_pow( double x, double y )	
{
    dprintf2(("CRTDLL: pow(%08xh, %08xh)\n",x, y));
    return pow( x, y );
}



/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.910
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

double CDECL CRTDLL_sin( double x )
{
  dprintf2(("CRTDLL: sin(%08xh)\n", x));
  return (sin(x));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.912
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

double CDECL CRTDLL_sqrt( double x )
{
  dprintf2(("CRTDLL: sqrt(%08xh)\n", x));
  return (sqrt(x));
}



/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.913
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int CDECL CRTDLL_sscanf( const char *s, const char *format, ... )
{
  dprintf(("CRTDLL: sscanf(%08xh, %08xh) not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.933
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int CDECL CRTDLL_vsprintf( char *s, const char *format, va_list arg )
{
  dprintf2(("CRTDLL: vsprintf(%08xh, %08xh)\n", s, format));
  return (vsprintf(s, format, arg));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.947
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

wchar_t * CDECL CRTDLL_wcstok( wchar_t *s1, const wchar_t *s2, wchar_t **ptr )
{
  dprintf2(("CRTDLL: wcstok(%08xh, %08xh, %08xh)\n",s1,s2,ptr));
  return (wcstok(s1, s2, ptr));
}

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.948
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

long int CDECL CRTDLL_wcstol( const wchar_t *s1, wchar_t **s2, int i )
{
  dprintf2(("CRTDLL: wcstol(%08xh, %08xh, %08xh)\n",s1,s2,i));
  return (wcstol(s1, s2, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.949
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

size_t CDECL CRTDLL_wcstombs( char *s, const wchar_t *pwcs, size_t n )
{
  dprintf2(("CRTDLL: wcstombs(%08xh, %08xh, %08xh)\n",s,pwcs,n));
  return (wcstombs(s, pwcs, n));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.950
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

unsigned long int CDECL CRTDLL_wcstoul( const wchar_t *s1, wchar_t **s2, int i )
{
  dprintf2(("CRTDLL: wcstoul(%08xh, %08xh, %08xh)\n",s1,s2,i));
  return (wcstoul(s1, s2, i));
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.983
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

int CDECL CRTDLL__wtoi( const wchar_t *s )
{
  dprintf(("CRTDLL: _wtoi(%08xh) not implemented.\n"));
  return 0;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.984
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

long int CDECL CRTDLL__wtol( const wchar_t *s )
{
  dprintf(("CRTDLL: _wtol(%08xh) not implemented.\n"));
  return 0;
}
