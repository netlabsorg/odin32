/*
 *                              Shell Library definitions
 */
#ifndef __WINE_SHELL_H
#define __WINE_SHELL_H

#include "windef.h"

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

/****************************************************************************
* shell 16
*/
extern void SHELL_LoadRegistry(void);
extern void SHELL_SaveRegistry(void);
extern void SHELL_Init(void);

/* global functions used from shell32 */
extern HINSTANCE SHELL_FindExecutable(LPCSTR,LPCSTR ,LPSTR);
extern HGLOBAL WINAPI InternalExtractIcon(HINSTANCE,LPCSTR,UINT,WORD);

/****************************************************************************
* shell 32
*/
/****************************************************************************
* common return codes
*/
#define SHELL_ERROR_SUCCESS           0L
#define SHELL_ERROR_BADDB             1L
#define SHELL_ERROR_BADKEY            2L
#define SHELL_ERROR_CANTOPEN          3L
#define SHELL_ERROR_CANTREAD          4L
#define SHELL_ERROR_CANTWRITE         5L
#define SHELL_ERROR_OUTOFMEMORY       6L
#define SHELL_ERROR_INVALID_PARAMETER 7L
#define SHELL_ERROR_ACCESS_DENIED     8L

/****************************************************************************
* common shell file structures
*/
/******************************
* DRAG&DROP API
*/
typedef struct {           /* structure for dropped files */
        WORD            wSize;
        POINT16         ptMousePos;
        BOOL16          fInNonClientArea;
        /* memory block with filenames follows */
} DROPFILESTRUCT16, *LPDROPFILESTRUCT16;

typedef struct {           /* structure for dropped files */
        DWORD           lSize;
        POINT           ptMousePos;
        BOOL            fInNonClientArea;
        BOOL          fWideChar;
        /* memory block with filenames follows */
} DROPFILESTRUCT, *LPDROPFILESTRUCT;


/****************************************************************************
* SHITEMID, ITEMIDLIST, PIDL API
*/
#include "pshpack1.h"
typedef struct
{ WORD  cb;     /* nr of bytes in this item */
  BYTE  abID[1];/* first byte in this item */
} SHITEMID,*LPSHITEMID;
typedef LPSHITEMID const LPCSHITEMID;

typedef struct
{ SHITEMID mkid; /* first itemid in list */
} ITEMIDLIST,*LPITEMIDLIST,*LPCITEMIDLIST;
#include "poppack.h"

DWORD WINAPI SHGetPathFromIDListA (LPCITEMIDLIST pidl,LPSTR pszPath);
DWORD WINAPI SHGetPathFromIDListW (LPCITEMIDLIST pidl,LPWSTR pszPath);
#define  SHGetPathFromIDList WINELIB_NAME_AW(SHGetPathFromIDList)


/****************************************************************************
*  other functions
*/

#ifdef __WIN32OS2__
/****************************************************************************
*  STRRET
*/
#define STRRET_WSTR     0x0000
#define STRRET_OFFSETA  0x0001
#define STRRET_CSTRA    0x0002
#define STRRET_ASTR     0X0003
#define STRRET_OFFSETW  0X0004
#define STRRET_CSTRW    0X0005


typedef struct _STRRET
{ UINT uType;           /* STRRET_xxx */
  union
  { LPWSTR      pOleStr;        /* OLESTR that will be freed */
    LPSTR       pStr;
    UINT        uOffset;        /* OffsetINT32o SHITEMID (ANSI) */
    char        cStr[MAX_PATH]; /* Buffer to fill in */
    WCHAR       cStrW[MAX_PATH];
  }u;
} STRRET,*LPSTRRET;
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif  /* __WINE_SHELL_H */
