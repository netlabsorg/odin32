/* $Id: undoccomctl32.h,v 1.1 2001-09-25 10:54:16 sandervl Exp $ */
#ifndef __WINE_WINE_UNDOCCOMCTL32_H__
#define __WINE_WINE_UNDOCCOMCTL32_H__

/**************************************************************************
 * The MRU-API is a set of functions to manipulate MRU(Most Recently Used)
 * lists.
 *
 * Stored in the reg. as a set of values under a single key.  Each item in the
 * list has a value name that is a single char. 'a' - 'z', '{', '|' or '}'.
 * The order of the list is stored with value name 'MRUList' which is a string
 * containing the value names (i.e. 'a', 'b', etc.) in the relevant order.
 */

typedef struct tagCREATEMRULIST
{
    DWORD  cbSize;        /* size of struct */
    DWORD  nMaxItems;     /* max no. of items in list */
    DWORD  dwFlags;       /* see below */
    HKEY   hKey;          /* root reg. key under which list is saved */
    LPCSTR lpszSubKey;    /* reg. subkey */
    PROC   lpfnCompare;   /* item compare proc */
} CREATEMRULIST, *LPCREATEMRULIST;

/* dwFlags */
#define MRUF_STRING_LIST  0 /* list will contain strings */
#define MRUF_BINARY_LIST  1 /* list will contain binary data */
#define MRUF_DELAYED_SAVE 2 /* only save list order to reg. is FreeMRUList */

/* If list is a string list lpfnCompare has the following prototype
 * int CALLBACK MRUCompareString(LPCSTR s1, LPCSTR s2)
 * for binary lists the prototype is
 * int CALLBACK MRUCompareBinary(LPCVOID data1, LPCVOID data2, DWORD cbData)
 * where cbData is the no. of bytes to compare.
 * Need to check what return value means identical - 0?
 */

typedef struct tagWINEMRUITEM
{
    DWORD          size;        /* size of data stored               */
    DWORD          itemFlag;    /* flags                             */
    BYTE           datastart;
} WINEMRUITEM, *LPWINEMRUITEM;

/* itemFlag */
#define WMRUIF_CHANGED   0x0001 /* this dataitem changed             */

typedef struct tagWINEMRULIST
{
    CREATEMRULIST  extview;     /* original create information       */
    DWORD          wineFlags;   /* internal flags                    */
    DWORD          cursize;     /* current size of realMRU           */
    LPSTR          realMRU;     /* pointer to string of index names  */
    LPWINEMRUITEM  *array;      /* array of pointers to data         */
                                /* in 'a' to 'z' order               */
} WINEMRULIST, *LPWINEMRULIST;

/* wineFlags */
#define WMRUF_CHANGED  0x0001   /* MRU list has changed              */

HANDLE WINAPI CreateMRUListA (LPCREATEMRULIST lpcml);
DWORD  WINAPI FreeMRUListA (HANDLE hMRUList);
INT    WINAPI AddMRUData (HANDLE hList, LPCVOID lpData, DWORD cbData);
INT    WINAPI AddMRUStringA(HANDLE hList, LPCSTR lpszString);
BOOL   WINAPI DelMRUString(HANDLE hList, INT nItemPos);
INT    WINAPI FindMRUData (HANDLE hList, LPCVOID lpData, DWORD cbData, LPINT lpRegNum);
INT    WINAPI FindMRUStringA (HANDLE hList, LPCSTR lpszString, LPINT lpRegNum);
HANDLE WINAPI CreateMRUListLazyA (LPCREATEMRULIST lpcml, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4);
INT    WINAPI EnumMRUListA(HANDLE hList, INT nItemPos, LPVOID lpBuffer,DWORD nBufferSize);



#endif /* __WINE_WINE_UNDOCCOMCTL32_H__ */
