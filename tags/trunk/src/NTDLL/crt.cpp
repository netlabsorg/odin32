/* $Id: crt.cpp,v 1.10 1999-10-25 19:24:13 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 * Copyright 1999 Patrick Haller (phaller@gmx.net)
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

#include "ntdll.h"
#include <heapstring.h>

/*
NTDLL.sprintf
NTDLL._wcsicmp
*/


/****************************************************************************
 * Local Prototypes                                                         *
 ****************************************************************************/


LPWSTR CDECL OS2_wcsupr(LPWSTR str);
int    CDECL OS2_wcsnicmp(LPWSTR str1, LPWSTR str2, long l);



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

int CDECL OS2_wcsicmp(LPWSTR str1, LPWSTR str2)
{
  dprintf(("NTDLL: _wcsicmp(%08xh,%08xh)\n",
           str1,
           str2));

  return (OS2_wcsnicmp(str1,
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

LPWSTR CDECL OS2_wcslwr(LPWSTR str)
{
  DWORD dwIndex;

  dprintf(("NTDLL: _wcslwr(%08xh)\n",
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

int CDECL OS2_wcsnicmp(LPWSTR str1, LPWSTR str2, long l)
{
  LPWSTR w1;
  LPWSTR w2;

  dprintf(("NTDLL: _wcsnicmp(%08xh,%08xh,%08xh)\n",
           str1,
           str2,
           l));

  w1 = HEAP_strdupW(GetProcessHeap(),0,str1);
  w2 = HEAP_strdupW(GetProcessHeap(),0,str2);
  OS2_wcsupr(w1);
  OS2_wcsupr(w2);

  return (wcsncmp((wchar_t*)w1,
                  (wchar_t*)w2,
                  l));
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

LPWSTR CDECL OS2_wcsupr(LPWSTR str)
{
  DWORD dwIndex;

  dprintf(("NTDLL: _wcsupr(%08xh)\n",
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

double CDECL OS2abs(double d)
{
  dprintf(("NTDLL: abs(%f)\n",
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

double CDECL OS2atan(double d)
{
  dprintf(("NTDLL: atan(%f)\n",
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

int CDECL OS2atoi(LPSTR str)
{
  dprintf(("NTDLL: atoi(%s)\n",
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

long CDECL OS2atol(LPSTR str)
{
  dprintf(("NTDLL: atol(%s)\n",
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

double CDECL OS2ceil(double d)
{
  dprintf(("NTDLL: ceil(%f)\n",
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

double CDECL OS2cos(double d)
{
  dprintf(("NTDLL: cos(%f)\n",
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

double CDECL OS2fabs(double d)
{
  dprintf(("NTDLL: fabs(%f)\n",
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

double CDECL OS2floor(double d)
{
  dprintf(("NTDLL: floor(%f)\n",
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

int CDECL OS2isalpha(int i)
{
  dprintf(("NTDLL: isalpha(%08xh)\n",
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

int CDECL OS2isdigit(int i)
{
  dprintf(("NTDLL: isdigit(%08xh)\n",
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

int CDECL OS2islower(int i)
{
  dprintf(("NTDLL: islower(%08xh)\n",
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

int CDECL OS2isprint(int i)
{
  dprintf(("NTDLL: isprint(%08xh)\n",
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

int CDECL OS2isspace(int i)
{
  dprintf(("NTDLL: isspace(%08xh)\n",
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

int CDECL OS2isupper(int i)
{
  dprintf(("NTDLL: isupper(%08xh)\n",
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

LPSTR CDECL OS2sprintf(LPSTR lpstrBuffer,
                       LPSTR lpstrFormat,
                       ...)
{
  va_list argptr;                          /* -> variable argument list */

  dprintf(("NTDLL: sprintf(%08xh,%s)\n",
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

LPSTR CDECL OS2strcat(      LPSTR str1,
                      const LPSTR str2)
{
  dprintf(("NTDLL: strcat(%s,%s)\n",
           str1,
           str2));

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

LPSTR CDECL OS2strchr(const LPSTR str,
                       int         i)
{
  dprintf(("NTDLL: strchr(%s,%08xh)\n",
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

int CDECL OS2strcmp(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf(("NTDLL: strcmp(%s,%s)\n",
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

int CDECL OS2_stricmp(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf(("NTDLL: _stricmp(%s,%s)\n",
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

LPSTR CDECL OS2strcpy(      LPSTR str1,
                       const LPSTR str2)
{
  dprintf(("NTDLL: strcpy(%s,%s)\n",
           str1,
           str2));

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

size_t CDECL OS2strcspn(const LPSTR str1,
                                LPSTR str2)
{
  dprintf(("NTDLL: strcspn(%s,%s)\n",
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

size_t CDECL OS2strlen(const LPSTR str)
{
  dprintf(("NTDLL: strlen(%s)\n",
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

LPSTR CDECL OS2strncat(      LPSTR str1,
                        const LPSTR str2,
                        size_t      i)
{
  dprintf(("NTDLL: strncat(%s,%s,%08xh)\n",
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

int CDECL OS2strncmp(const LPSTR str1,
                       const LPSTR str2,
                       size_t      i)
{
  dprintf(("NTDLL: strncmp(%s,%s,%08xh)\n",
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

LPSTR CDECL OS2strncpy(const LPSTR str1,
                        const LPSTR str2,
                        size_t      i)
{
  dprintf(("NTDLL: strncpy(%s,%s,%08xh)\n",
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

LPSTR CDECL OS2strpbrk(const LPSTR str1,
                      const LPSTR str2)
{
  dprintf(("NTDLL: strpbrk(%s,%s)\n",
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

LPSTR CDECL OS2strrchr(const LPSTR str,
                        size_t      i)
{
  dprintf(("NTDLL: strrchr(%s,%08xh)\n",
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

size_t CDECL OS2strspn(const LPSTR str1,
                       const LPSTR str2)
{
  dprintf(("NTDLL: strspn(%s,%s)\n",
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

LPSTR CDECL OS2strstr(const LPSTR str1,
                       const LPSTR str2)
{
  dprintf(("NTDLL: strstr(%s,%s)\n",
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

int CDECL OS2swprintf(const LPWSTR str,
                      int   i,
                      const LPWSTR format,
                      ...)
{
  va_list valist;
  int     rc;

  dprintf(("NTDLL: swprintf(%s,%d,%s)\n",
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

double CDECL OS2tan(double d)
{
  dprintf(("NTDLL: tan(%f)\n",
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

int CDECL OS2toupper(int c)
{
  dprintf(("NTDLL: toupper(%c)\n",
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

int CDECL OS2tolower(int c)
{
  dprintf(("NTDLL: tolower(%c)\n",
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

int CDECL OS2towupper(int c)
{
  dprintf(("NTDLL: towupper(%c)\n",
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

int CDECL OS2towlower(int c)
{
  dprintf(("NTDLL: towlower(%c)\n",
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

wchar_t* CDECL OS2wcscat(      wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf(("NTDLL: wcscat(%08xh,%08xh)\n",
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

wchar_t* CDECL OS2wcschr(const wchar_t* str,
                       int          i)
{
  dprintf(("NTDLL: wcschr(%08xh,%08xh)\n",
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

int CDECL OS2wcscmp(const wchar_t* str1,
                    const wchar_t* str2)
{
  dprintf(("NTDLL: wcscmp(%08xh,%08xh)\n",
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

wchar_t* CDECL OS2wcscpy(      wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf(("NTDLL: wcscpy(%08xh,%08xh)\n",
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

size_t CDECL OS2wcscspn(const wchar_t* str1,
                              wchar_t* str2)
{
  dprintf(("NTDLL: wcscspn(%08xh,%08xh)\n",
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

size_t CDECL OS2wcslen(const wchar_t* str)
{
  dprintf(("NTDLL: wcslen(%08xh)\n",
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

wchar_t* CDECL OS2wcsncat(      wchar_t* str1,
                        const wchar_t* str2,
                        size_t      i)
{
  dprintf(("NTDLL: wcsncat(%08xh,%08xh,%08xh)\n",
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

int CDECL OS2wcsncmp(const wchar_t* str1,
                     const wchar_t* str2,
                     size_t      i)
{
  dprintf(("NTDLL: wcsncmp(%08xh,%08xh,%08xh)\n",
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

wchar_t* CDECL OS2wcsncpy(      wchar_t* str1,
                          const wchar_t* str2,
                          size_t       i)
{
  dprintf(("NTDLL: wcsncpy(%s,%s,%08xh)\n",
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

wchar_t* CDECL OS2wcspbrk(const wchar_t* str1,
                        const wchar_t* str2)
{
  dprintf(("NTDLL: wcspbrk(%08xh,%08xh)\n",
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

wchar_t* CDECL OS2wcsrchr(const wchar_t* str,
                        size_t       i)
{
  dprintf(("NTDLL: wcsrchr(%08xh,%08xh)\n",
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

size_t CDECL OS2wcsspn(const wchar_t* str1,
                       const wchar_t* str2)
{
  dprintf(("NTDLL: wcsspn(%08xh,%08xh)\n",
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

wchar_t* CDECL OS2wcsstr(const wchar_t* str1,
                         const wchar_t* str2)
{
  dprintf(("NTDLL: wcsstr(%s,%s)\n",
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

char * CDECL OS2_itoa(int i, char *s, int r)
{
  dprintf(("NTDLL: _itoa(%08xh, %08xh, %08xh)\n",
           i,
           s,
           r));

  return (_itoa(i,s,r));
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

char * CDECL OS2_itow(int i, char *s, int r)
{
  dprintf(("NTDLL: _itow(%08xh, %08xh, %08xh) no unicode support !\n",
           i,
           s,
           r));

  return (_itoa(i,s,r));
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

LONG CDECL NTDLL__CIpow(double x, double y)
{
  dprintf(("NTDLL: _CIpow\n"));
  dprintf(("should be register function\n"));
  return pow(x, y);
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

LONG CDECL NTDLL__ftol(void)
{
  dprintf(("NTDLL: _ftol not implemented.\n"));

  return 0;
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

LPSTR  CDECL NTDLL__ltoa(long x,LPSTR buf,INT radix)
{
  dprintf(("NTDLL: _ltoa(%08xh, %08xh, %08xh) not implemented\n",
           x,
           buf,
           radix));

  return 0;
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

INT CDECL NTDLL__memicmp(
	LPCSTR s1,	/* [in] first string */
	LPCSTR s2,	/* [in] second string */
	DWORD len	/* [in] length to compare */ )
{
	dprintf(("NTDLL: memicmp(%08xh, %08xh, %08xh)\n",s1,s2,len));
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

int  CDECL NTDLL__snprintf( char *buf, size_t bufsize, const char *fmt, ... )
{
  dprintf(("NTDLL: _snprintf(%08xh, %08xh, %08xh) not implemented\n",
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

int  CDECL NTDLL__snwprintf( wchar_t *buf, size_t bufsize, const wchar_t *fmt, ... )
{
  dprintf(("NTDLL: _snwprintf(%08xh, %08xh, %08xh) not implemented\n",
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

void CDECL NTDLL__splitpath( const char *path, char *drive, char *dir, char *fname, char *ext )
{
	dprintf(("NTDLL: _splitpath"));

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

void CDECL NTDLL__strcmpi( LPCSTR s1, LPCSTR s2 )
{
  dprintf(("NTDLL: _strcmpi(%08xh, %08xh)\n",
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

CHAR * CDECL NTDLL__strlwr(char *x)
{
  char *y =x;

  dprintf(("NTDLL: _strlwr got %s\n", x));
  while (*y) {
    if ((*y > 0x40) && (*y< 0x5b))
      *y = *y + 0x20;
    y++;
  }
  dprintf(("   returned %s\n", x));
		
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

int  CDECL NTDLL__strnicmp( LPCSTR s1, LPCSTR s2, INT n )
{
  dprintf(("NTDLL: _strnicmp (%s,%s,%d)\n",
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

LPSTR CDECL NTDLL__strupr(LPSTR x)
{
  dprintf(("NTDLL: _strupr(%s)\n",
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

LPSTR  CDECL NTDLL__ultoa(long x,LPSTR buf,INT radix)
{
  dprintf(("NTDLL: _ultoa(%08xh, %08xh, %08xh) not implemented\n",
           x,
           buf,
           radix));

  return 0;
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

int CDECL NTDLL__vsnprintf( char *s, size_t bufsize, const char *format, va_list arg )
{
  dprintf(("NTDLL: _ultoa(%08xh, %08xh, %08xh) not implemented\n",
           s,
           bufsize,
           format));

  return 0;
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

int CDECL NTDLL_iswalpha(wint_t i)
{
  dprintf(("NTDLL: iswalpha(%08xh)\n", i));

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

int CDECL NTDLL_iswctype(wint_t i, wctype_t wct)
{
  dprintf(("NTDLL: iswctype(%08xh, %08xh)\n", i, wct));

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

int CDECL NTDLL_isxdigit(int i)
{
  dprintf(("NTDLL: isxdigit(%08xh)\n", i));

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

long int CDECL NTDLL_labs( long int j )
{
  dprintf(("NTDLL: labs(%08xh)\n", j));

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

double CDECL NTDLL_log( double x )
{
  dprintf(("NTDLL: log(%08xh)\n", x));
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

size_t CDECL NTDLL_mbstowcs( wchar_t *pwcs, const char *s, size_t n )
{
      dprintf(("NTDLL: mbstowcs(%08xh, %08xh, %08xh)\n", pwcs, s, n));
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

void * CDECL NTDLL_memchr( const void *s, int c, size_t n )
{
    dprintf(("NTDLL: memchr(%08xh, %08xh, %08xh)\n", s, c, n));
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

int CDECL NTDLL_memcmp( const void * c1, const void * c2, size_t n )
{
    dprintf(("NTDLL: memcmp(%08xh, %08xh, %08xh)\n", c1, c2, n));
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

void * CDECL NTDLL_memcpy( void *s1, const void *s2, size_t n )
{
    dprintf(("NTDLL: memcpy(%08xh, %08xh, %08xh)\n", s1, s2, n));
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

void * CDECL NTDLL_memset( void *s, int i, size_t n )
{
    dprintf(("NTDLL: memset(%08xh, %08xh, %08xh)\n", s, i, n));
    return memset( s, i, n );
}


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

double CDECL NTDLL_pow( double x, double y )	
{
    dprintf(("NTDLL: pow(%08xh, %08xh)\n",x, y));
    return pow( x, y );
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : NTDLL.909
 * Status    :
 *
 * Author    : Jens Wiessner
 *****************************************************************************/

void CDECL NTDLL_qsort( void *base, size_t nmemb, size_t size,
			int (*compar)( const void *s1, const void *s2 ))
{
    dprintf(("NTDLL: qsort(%08xh, %08xh, %08xh, %08xh) not implemented.\n",
		base, nmemb, size, compar));
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

double CDECL NTDLL_sin( double x )
{
  dprintf(("NTDLL: sin(%08xh)\n", x));
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

double CDECL NTDLL_sqrt( double x )
{
  dprintf(("NTDLL: sqrt(%08xh)\n", x));
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

int CDECL NTDLL_sscanf( const char *s, const char *format, ... )
{
  dprintf(("NTDLL: sscanf(%08xh, %08xh) not implemented.\n"));
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

int CDECL NTDLL_vsprintf( char *s, const char *format, va_list arg )
{
  dprintf(("NTDLL: vsprintf(%08xh, %08xh)\n", s, format));
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

wchar_t * CDECL NTDLL_wcstok( wchar_t *s1, const wchar_t *s2, wchar_t **ptr )
{
  dprintf(("NTDLL: wcstok(%08xh, %08xh, %08xh)\n",s1,s2,ptr));
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

long int CDECL NTDLL_wcstol( const wchar_t *s1, wchar_t **s2, int i )
{
  dprintf(("NTDLL: wcstol(%08xh, %08xh, %08xh)\n",s1,s2,i));
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

size_t CDECL NTDLL_wcstombs( char *s, const wchar_t *pwcs, size_t n )
{
  dprintf(("NTDLL: wcstombs(%08xh, %08xh, %08xh)\n",s,pwcs,n));
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

unsigned long int CDECL NTDLL_wcstoul( const wchar_t *s1, wchar_t **s2, int i )
{
  dprintf(("NTDLL: wcstoul(%08xh, %08xh, %08xh)\n",s1,s2,i));
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

int CDECL NTDLL__wtoi( const wchar_t *s )
{
  dprintf(("NTDLL: _wtoi(%08xh) not implemented.\n"));
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

long int CDECL NTDLL__wtol( const wchar_t *s )
{
  dprintf(("NTDLL: _wtol(%08xh) not implemented.\n"));
  return 0;
}
