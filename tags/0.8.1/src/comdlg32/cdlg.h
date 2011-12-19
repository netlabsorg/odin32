/* $Id: cdlg.h,v 1.8 2002-06-08 07:23:52 sandervl Exp $ */
/*
 *  Common Dialog Boxes interface (32 bit)
 *
 * Copyright 1998 Bertho A. Stultiens
 */

#ifndef _WINE_DLL_CDLG_H
#define _WINE_DLL_CDLG_H

#include "dlgs.h"

extern HINSTANCE	COMDLG32_hInstance;
#define COMMDLG_hInstance32 COMDLG32_hInstance

/*---------------- 32-bit ----------------*/

/* Common dialogs implementation globals */
#define COMDLG32_Atom	((ATOM)0xa000)	/* MS uses this one to identify props */

extern HINSTANCE	COMDLG32_hInstance;

#ifdef __cplusplus
extern "C" {
#endif

void	COMDLG32_SetCommDlgExtendedError(DWORD err);
LPVOID	COMDLG32_AllocMem(int size);

#ifdef __cplusplus
}
#endif



/* Find/Replace local definitions */

#define FR_WINE_UNICODE		0x80000000
#define FR_WINE_REPLACE		0x40000000

typedef struct {
	FINDREPLACEA	fr;	/* Internally used structure */
        union {
		FINDREPLACEA	*fra;	/* Reference to the user supplied structure */
		FINDREPLACEW	*frw;
        } user_fr;
} COMDLG32_FR_Data;

#define PD32_PRINT_TITLE        7000

#define PD32_VALUE_UREADABLE                  1104
#define PD32_INVALID_PAGE_RANGE               1105
#define PD32_FROM_NOT_ABOVE_TO                1106
#define PD32_MARGINS_OVERLAP                  1107
#define PD32_NR_OF_COPIES_EMPTY               1108
#define PD32_TOO_LARGE_COPIES                 1109
#define PD32_PRINT_ERROR                      1110
#define PD32_NO_DEFAULT_PRINTER               1111
#define PD32_CANT_FIND_PRINTER                1112
#define PD32_OUT_OF_MEMORY                    1113
#define PD32_GENERIC_ERROR                    1114
#define PD32_DRIVER_UNKNOWN                   1115

#define PD32_PRINTER_STATUS_READY             1536
#define PD32_PRINTER_STATUS_PAUSED            1537
#define PD32_PRINTER_STATUS_ERROR             1538
#define PD32_PRINTER_STATUS_PENDING_DELETION  1539
#define PD32_PRINTER_STATUS_PAPER_JAM         1540
#define PD32_PRINTER_STATUS_PAPER_OUT         1541
#define PD32_PRINTER_STATUS_MANUAL_FEED       1542
#define PD32_PRINTER_STATUS_PAPER_PROBLEM     1543
#define PD32_PRINTER_STATUS_OFFLINE           1544
#define PD32_PRINTER_STATUS_IO_ACTIVE         1545
#define PD32_PRINTER_STATUS_BUSY              1546
#define PD32_PRINTER_STATUS_PRINTING          1547
#define PD32_PRINTER_STATUS_OUTPUT_BIN_FULL   1548
#define PD32_PRINTER_STATUS_NOT_AVAILABLE     1549
#define PD32_PRINTER_STATUS_WAITING           1550
#define PD32_PRINTER_STATUS_PROCESSING        1551
#define PD32_PRINTER_STATUS_INITIALIZING      1552
#define PD32_PRINTER_STATUS_WARMING_UP        1553
#define PD32_PRINTER_STATUS_TONER_LOW         1554
#define PD32_PRINTER_STATUS_NO_TONER          1555
#define PD32_PRINTER_STATUS_PAGE_PUNT         1556
#define PD32_PRINTER_STATUS_USER_INTERVENTION 1557
#define PD32_PRINTER_STATUS_OUT_OF_MEMORY     1558
#define PD32_PRINTER_STATUS_DOOR_OPEN         1559
#define PD32_PRINTER_STATUS_SERVER_UNKNOWN    1560
#define PD32_PRINTER_STATUS_POWER_SAVE        1561

#define PD32_DEFAULT_PRINTER                  1582
#define PD32_NR_OF_DOCUMENTS_IN_QUEUE         1583
#define PD32_PRINT_ALL_X_PAGES                1584
#define PD32_MARGINS_IN_INCHES                1585
#define PD32_MARGINS_IN_MILIMETERS            1586
#define PD32_MILIMETERS                       1587

#include "commctrl.h"
//#include "wine/undocshell.h"
#include "shellapi.h"

/* DPA */
#ifdef __WIN32OS2__
#include <win/shell.h>

LPVOID WINAPI SHAlloc(DWORD);
DWORD WINAPI SHFree(LPVOID);
BOOL WINAPI SHGetFolderPathA(HWND,int,HANDLE,DWORD,LPSTR);

#define COMDLG32_DPA_Create             DPA_Create
#define COMDLG32_DPA_GetPtr             DPA_GetPtr
#define COMDLG32_DPA_DeleteAllPtrs      DPA_DeleteAllPtrs
#define COMDLG32_DPA_DeletePtr          DPA_DeletePtr
#define COMDLG32_DPA_InsertPtr          DPA_InsertPtr
#define COMDLG32_DPA_Destroy		DPA_Destroy

#define COMDLG32_ImageList_GetIcon     	ImageList_GetIcon
#define COMDLG32_ImageList_LoadImageA   ImageList_LoadImageA
#define COMDLG32_ImageList_Draw         ImageList_Draw
#define COMDLG32_ImageList_Destroy      ImageList_Destroy

/* ITEMIDLIST */

LPITEMIDLIST WINAPI ILClone(LPCITEMIDLIST pidl);
LPITEMIDLIST WINAPI ILCombine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
LPITEMIDLIST WINAPI ILGetNext(LPITEMIDLIST pidl);
BOOL         WINAPI ILRemoveLastID(LPCITEMIDLIST pidl);
BOOL         WINAPI ILIsEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);

#define COMDLG32_PIDL_ILClone		ILClone
#define COMDLG32_PIDL_ILCombine		ILCombine
#define COMDLG32_PIDL_ILGetNext		ILGetNext
#define COMDLG32_PIDL_ILRemoveLastID	ILRemoveLastID
#define COMDLG32_PIDL_ILIsEqual		ILIsEqual

/* SHELL */
#define COMDLG32_SHGetPathFromIDListA	SHGetPathFromIDListA
#define COMDLG32_SHGetSpecialFolderLocation	SHGetSpecialFolderLocation
#define COMDLG32_SHGetDesktopFolder	SHGetDesktopFolder
#define COMDLG32_SHGetFileInfoA		SHGetFileInfoA
#define COMDLG32_SHFree			SHFree
#define COMDLG32_SHAlloc                SHAlloc
#define COMDLG32_SHGetDataFromIDListA   SHGetDataFromIDListA
#define COMDLG32_SHGetFolderPathA       SHGetFolderPathA

DWORD WINAPI PathRemoveFileSpecA(LPSTR fn);
BOOL  WINAPI PathMatchSpecW(LPCWSTR name, LPCWSTR mask);

/* PATH */
#define COMDLG32_PathIsRootA		PathIsRootA
#define COMDLG32_PathFindFileNameA	PathFindFileNameA
#define COMDLG32_PathRemoveFileSpecA	PathRemoveFileSpecA
#define COMDLG32_PathMatchSpecW		PathMatchSpecW
#define COMDLG32_PathAddBackslashA	PathAddBackslashA
#define COMDLG32_PathCanonicalizeA      PathCanonicalizeA
#define COMDLG32_PathGetDriveNumberA    PathGetDriveNumberA
#define COMDLG32_PathIsRelativeA        PathIsRelativeA
#define COMDLG32_PathFindNextComponentA PathFindNextComponentA
#define COMDLG32_PathAddBackslashW      PathAddBackslashW
#define COMDLG32_PathFindExtensionA     PathFindExtensionA
#define COMDLG32_PathAddExtensionA      PathAddExtensionA

#else
/* IMAGELIST */
extern BOOL	(WINAPI* COMDLG32_ImageList_Draw) (HIMAGELIST himl, int i, HDC hdcDest, int x, int y, UINT fStyle);

/* ITEMIDLIST */

extern LPITEMIDLIST (WINAPI *COMDLG32_PIDL_ILClone) (LPCITEMIDLIST);
extern LPITEMIDLIST (WINAPI *COMDLG32_PIDL_ILCombine)(LPCITEMIDLIST,LPCITEMIDLIST);
extern LPITEMIDLIST (WINAPI *COMDLG32_PIDL_ILGetNext)(LPITEMIDLIST);
extern BOOL (WINAPI *COMDLG32_PIDL_ILRemoveLastID)(LPCITEMIDLIST);
extern BOOL (WINAPI *COMDLG32_PIDL_ILIsEqual)(LPCITEMIDLIST, LPCITEMIDLIST);

/* SHELL */
extern BOOL (WINAPI *COMDLG32_SHGetPathFromIDListA) (LPCITEMIDLIST,LPSTR);
extern HRESULT (WINAPI *COMDLG32_SHGetSpecialFolderLocation)(HWND,INT,LPITEMIDLIST *);
extern DWORD (WINAPI *COMDLG32_SHGetDesktopFolder)(IShellFolder **);
extern DWORD (WINAPI *COMDLG32_SHGetFileInfoA)(LPCSTR,DWORD,SHFILEINFOA*,UINT,UINT);
extern LPVOID (WINAPI *COMDLG32_SHAlloc)(DWORD);
extern DWORD (WINAPI *COMDLG32_SHFree)(LPVOID);
extern HRESULT (WINAPI *COMDLG32_SHGetDataFromIDListA)(LPSHELLFOLDER psf, LPCITEMIDLIST pidl, int nFormat, LPVOID dest, int len);
extern BOOL (WINAPI *COMDLG32_SHGetFolderPathA)(HWND,int,HANDLE,DWORD,LPSTR);


/* PATH */
extern BOOL (WINAPI *COMDLG32_PathIsRootA)(LPCSTR x);
extern LPSTR (WINAPI *COMDLG32_PathFindFileNameA)(LPCSTR path);
extern DWORD (WINAPI *COMDLG32_PathRemoveFileSpecA)(LPSTR fn);
extern BOOL (WINAPI *COMDLG32_PathMatchSpecW)(LPCWSTR x, LPCWSTR y);
extern LPSTR (WINAPI *COMDLG32_PathAddBackslashA)(LPSTR path);
extern BOOL (WINAPI *COMDLG32_PathCanonicalizeA)(LPSTR pszBuf, LPCSTR pszPath);
extern int (WINAPI *COMDLG32_PathGetDriveNumberA)(LPCSTR lpszPath);
extern BOOL (WINAPI *COMDLG32_PathIsRelativeA) (LPCSTR lpszPath);
extern LPSTR (WINAPI *COMDLG32_PathFindNextComponentA)(LPCSTR pszPath);
extern LPWSTR (WINAPI *COMDLG32_PathAddBackslashW)(LPWSTR lpszPath);
extern LPSTR (WINAPI *COMDLG32_PathFindExtensionA)(LPCVOID lpszPath);
extern BOOL (WINAPI *COMDLG32_PathAddExtensionA)(LPSTR  pszPath,LPCSTR pszExtension);
#endif

#endif

