/* $Id: crt.cpp,v 1.1 1999-06-22 16:18:14 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 * Copyright 1999 Patrick Haller (phaller@gmx.net)
 */

#include <odin.h>
#include <stdlib.h>
#include <string.h>
#include "ntdll.h"

/*
NTDLL.sprintf
NTDLL.wcslen
NTDLL._itoa
NTDLL._wcsicmp
*/


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

