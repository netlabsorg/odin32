/* $Id: mapidefs.h,v 1.1 1999-05-24 20:19:14 ktk Exp $ */

#ifndef __WINE_MAPIDEFS_H
#define __WINE_MAPIDEFS_H

#include "windef.h"

#ifndef _tagCY_DEFINED
#define _tagCY_DEFINED
typedef union tagCY
{
   struct {
#ifdef BIG_ENDIAN
        long Hi;
        long Lo;
#else
      unsigned long Lo;
      long Hi;
#endif
   } u;
   LONGLONG int64;
} CY;
#endif /* _tagCY_DEFINED */

#endif /*__WINE_MAPIDEFS_H*/
