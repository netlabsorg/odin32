/* $Id: os2fake-win.c,v 1.1 2001-03-14 20:14:35 bird Exp $
 * 
 * OS/2 Fake library for Win32.
 * 
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define VALIDPTR(pv) (((PVOID)pv) >= (PVOID)0x10000 && ((PVOID)pv) <= (PVOID)0xc0000000)



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <windows.h>
#include <string.h>
#include <stdio.h>

#include "os2fake.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static ULONG ConvertAttributes(DWORD dwFileAttributes);
static ULONG ConvertFileTime(PFILETIME pFileTime);

/**
 * Converts Win32 file attribute to OS/2 file attributes.
 * @returns OS/2 fileattributes.
 * @param   dwFileAttributes    Win32 fileattributes.
 */
ULONG ConvertAttributes(DWORD dwFileAttributes)
{
    static struct _ConvAttr
    {
        ULONG   ulWin;
        ULONG   ulOs2;
    }       aConvAttr[] = 
    {                                  
        {FILE_ATTRIBUTE_READONLY,   FILE_READONLY},
        {FILE_ATTRIBUTE_HIDDEN,     FILE_HIDDEN},
        {FILE_ATTRIBUTE_SYSTEM,     FILE_SYSTEM},
        {FILE_ATTRIBUTE_DIRECTORY,  FILE_DIRECTORY},
        {FILE_ATTRIBUTE_ARCHIVE,    FILE_ARCHIVED}
    };                                    
    ULONG   ulOS2Attr = 0;
    int     i;

    for (i = 0; i < sizeof(aConvAttr) / sizeof(aConvAttr[0]); i++)
        if (dwFileAttributes & aConvAttr[i].ulWin)
            ulOS2Attr |= aConvAttr[i].ulOs2;

    return ulOS2Attr;
}


/**
 * Converts Win32 filetime to OS/2 filetime.
 * @returns OS/2 filetime.
 * @param   pFileTime   Pointer to Win32 filetime.
 */
ULONG ConvertFileTime(PFILETIME pFileTime)
{
    ULONG       ulOS2FileTime;
    SYSTEMTIME  SystemTime;

    if (   FileTimeToSystemTime(pFileTime, &SystemTime)
        && SystemTime.wYear >= 1980 && SystemTime.wYear < (1980 + 0x7F))
    {
        ulOS2FileTime =    SystemTime.wDay
                        | (SystemTime.wMonth << 5)
                        | (((SystemTime.wYear - 1980) & 0x7F) << (5+4))
                        | ((SystemTime.wSecond / 2) << (16))
                        | (SystemTime.wMinute << (16+5))
                        | (SystemTime.wHour << (16+5+6));
    }
    else
        ulOS2FileTime = 0;

    return ulOS2FileTime;
}



APIRET OS2ENTRY         DosQueryPathInfo(
                            PCSZ        pszPathName,
                            ULONG       ulInfoLevel,
                            PVOID       pInfoBuf,
                            ULONG       cbInfoBuf)
{
    APIRET  rc;                         /* Return code. */

    if (!VALIDPTR(pszPathName))
    {
        fprintf(stderr, "DosQueryPathInfo: pszPathName is an invalid pointer - %p\n", pszPathName);
        return ERROR_INVALID_PARAMETER;
    }

    if (!VALIDPTR(pInfoBuf))
    {
        fprintf(stderr, "DosQueryPathInfo: pInfoBuf is an invalid pointer - %p\n", pInfoBuf);
        return ERROR_INVALID_PARAMETER;
    }


    rc = ERROR_INVALID_PARAMETER;
    switch (ulInfoLevel)
    {
        case FIL_QUERYFULLNAME:
        {
            LPTSTR   lpDummy;
            if (GetFullPathName(pszPathName, cbInfoBuf, pInfoBuf, &lpDummy) > 0)
                rc = NO_ERROR;
            else
                rc = GetLastError();
            break;
        }

        case FIL_STANDARD:
            if (cbInfoBuf == sizeof(FILESTATUS3))
            {
                WIN32_FILE_ATTRIBUTE_DATA   fad;
                
                if (GetFileAttributesEx(pszPathName, GetFileExInfoStandard, &fad)) //W98, NT4 and above.
                {
                    PFILESTATUS3    pfst3 = (PFILESTATUS3)(pInfoBuf);
                    
                    if (fad.nFileSizeHigh > 0)
                        rc = ERROR_BAD_LENGTH;
                    pfst3->cbFile = pfst3->cbFileAlloc = fad.nFileSizeLow;
                    pfst3->attrFile = ConvertAttributes(fad.dwFileAttributes);
                    *(PULONG)(&pfst3->fdateCreation) = ConvertFileTime(&fad.ftCreationTime);
                    *(PULONG)(&pfst3->fdateLastAccess) = ConvertFileTime(&fad.ftLastAccessTime);
                    *(PULONG)(&pfst3->fdateLastWrite) = ConvertFileTime(&fad.ftLastWriteTime);
                    rc = NO_ERROR;
                }
                else
                    rc = GetLastError();
            }
            else
                fprintf(stderr, "DosQueryPathInfo: FIL_STANDARD - invalid structure size (cbInfoBuf=%d)\n", cbInfoBuf);
            break;

        default:
            fprintf(stderr, "DosQueryPathInfo: ulInfoLevel=%d not supported\n", ulInfoLevel);
    }

    return rc;
}
            

APIRET OS2ENTRY         DosFindFirst(
                            PCSZ        pszFileSpec,
                            PHDIR       phdir,
                            ULONG       flAttribute,
                            PVOID       pFindBuf,
                            ULONG       cbFindBuf,
                            PULONG      pcFileNames,
                            ULONG       ulInfoLevel)
{
    WIN32_FIND_DATA FindData;           /* Win32 Find data (returned by FindFirstFile) */
    APIRET          rc;

    if (!VALIDPTR(pszFileSpec))
    {
        fprintf(stderr, "DosFindFirst: pszFileSpec - %p\n", pszFileSpec);
        return ERROR_INVALID_PARAMETER;
    }

    if (!VALIDPTR(phdir))
    {
        fprintf(stderr, "DosFindFirst: phdir - %p\n", phdir);
        return ERROR_INVALID_PARAMETER;
    }

    if (!VALIDPTR(pFindBuf))
    {
        fprintf(stderr, "DosFindFirst: pfindbuf - %p\n", pFindBuf);
        return ERROR_INVALID_PARAMETER;
    }

    if (!VALIDPTR(pcFileNames))
    {
        fprintf(stderr, "DosFindFirst: pcFileNames - %p\n", pcFileNames);
        return ERROR_INVALID_PARAMETER;
    }

    if (*phdir != HDIR_CREATE)
    {
        fprintf(stderr, "DosFindFirst: *phdir != HDIR_CREATE - 0x%08x\n", *phdir);
        return ERROR_INVALID_PARAMETER;
    }

    switch (ulInfoLevel)
    {
        case FIL_STANDARD:
            if (cbFindBuf < sizeof(FILEFINDBUF3))
            {
                fprintf(stderr, "DosFindFirst: unsupported buffer size - %d\n", cbFindBuf);
                return ERROR_INVALID_PARAMETER;
            }
            break;

        default:
            fprintf(stderr, "DosFindFirst: invalid infolevel %d\n", ulInfoLevel);
            return ERROR_INVALID_PARAMETER;
    }

    *phdir = (HDIR)FindFirstFile(pszFileSpec, &FindData);
    if (*phdir != (HDIR)INVALID_HANDLE_VALUE)
    {
        PFILEFINDBUF3   pfindbuf = (PFILEFINDBUF3)pFindBuf;
        
        memcpy(pfindbuf->achName, FindData.cFileName, pfindbuf->cchName = strlen(FindData.cFileName) + 1);
        pfindbuf->cbFile = pfindbuf->cbFileAlloc = FindData.nFileSizeHigh > 0 ? 0xffffffff : FindData.nFileSizeLow;
        pfindbuf->attrFile = ConvertAttributes(FindData.dwFileAttributes);
        *(PULONG)(&pfindbuf->fdateCreation) = ConvertFileTime(&FindData.ftCreationTime);
        *(PULONG)(&pfindbuf->fdateLastAccess) = ConvertFileTime(&FindData.ftLastAccessTime);
        *(PULONG)(&pfindbuf->fdateLastWrite) = ConvertFileTime(&FindData.ftLastWriteTime);
        pfindbuf->oNextEntryOffset = 0;
        *pcFileNames = 1;
        rc = NO_ERROR;
    }
    else
        rc = GetLastError();

    return rc;
}


APIRET OS2ENTRY         DosFindNext(
                            HDIR        hDir,
                            PVOID       pFindBuf,
                            ULONG       cbFindBuf,
                            PULONG      pcFileNames)
{
    WIN32_FIND_DATA FindData;           /* Win32 Find data (returned by FindFirstFile) */
    APIRET          rc;

    if (!VALIDPTR(pFindBuf))
    {
        fprintf(stderr, "DosFindNext: pfindbuf - %p\n", pFindBuf);
        return ERROR_INVALID_PARAMETER;
    }

    if (cbFindBuf < sizeof(FILEFINDBUF3))
    {
        fprintf(stderr, "DosFindNext: unsupported buffer size - %d\n", cbFindBuf);
        return ERROR_INVALID_PARAMETER;
    }

    if (!VALIDPTR(pcFileNames))
    {
        fprintf(stderr, "DosFindNext: pcFileNames - %p\n", pcFileNames);
        return ERROR_INVALID_PARAMETER;
    }

    if (FindNextFile((HANDLE)hDir, &FindData))
    {
        PFILEFINDBUF3   pfindbuf = (PFILEFINDBUF3)pFindBuf;
        
        memcpy(pfindbuf->achName, FindData.cFileName, pfindbuf->cchName = strlen(FindData.cFileName) + 1);
        pfindbuf->cbFile = pfindbuf->cbFileAlloc = FindData.nFileSizeHigh > 0 ? 0xffffffff : FindData.nFileSizeLow;
        pfindbuf->attrFile = ConvertAttributes(FindData.dwFileAttributes);
        *(PULONG)(&pfindbuf->fdateCreation) = ConvertFileTime(&FindData.ftCreationTime);
        *(PULONG)(&pfindbuf->fdateLastAccess) = ConvertFileTime(&FindData.ftLastAccessTime);
        *(PULONG)(&pfindbuf->fdateLastWrite) = ConvertFileTime(&FindData.ftLastWriteTime);
        pfindbuf->oNextEntryOffset = 0;
        *pcFileNames = 1;
        rc = NO_ERROR;
    }
    else
        rc = GetLastError();

    return rc;

}


APIRET OS2ENTRY         DosFindClose(
                            HDIR        hDir)
{
    if (FindClose((HANDLE)hDir))
        return NO_ERROR;
    return ERROR_INVALID_HANDLE;
}


                   
