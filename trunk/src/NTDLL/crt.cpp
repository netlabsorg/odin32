/* $Id: crt.cpp,v 1.3 1999-06-25 13:59:01 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 * Copyright 1999 Patrick Haller (phaller@gmx.net)
 */

#include <odin.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <wchar.h>
#include <wcstr.h>
#include <wctype.h>

#include "ntdll.h"

/*
NTDLL.sprintf
NTDLL._itoa
NTDLL._wcsicmp
*/


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

