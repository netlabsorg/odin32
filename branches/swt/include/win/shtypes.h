#ifndef __WINE_SHTYPES_H
#define __WINE_SHTYPES_H

/****************************************************************************
*  STRRET
*/
#define STRRET_WSTR     0x0000
#define STRRET_OFFSET   0x0001
#define STRRET_CSTR     0x0002

#include "wtypes.h"

typedef struct _STRRET
{ UINT uType;           /* STRRET_xxx */
  union
  { LPWSTR      pOleStr;        /* OLESTR that will be freed */
    LPSTR       pStr;
    UINT        uOffset;        /* OffsetINT32o SHITEMID (ANSI) */
    char        cStr[MAX_PATH]; /* Buffer to fill in */
    WCHAR       cStrW[MAX_PATH];
  } DUMMYUNIONNAME;
} STRRET,*LPSTRRET;

#endif /* __WINE_SHTYPES_H */
