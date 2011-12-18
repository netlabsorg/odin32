/* $Id: fontres.cpp,v 1.1 2004-01-11 11:42:13 sandervl Exp $ */


/*
 * GDI32 font apis
 *
 * Copyright 2003 Sander van Leeuwen (sandervl@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <dbglog.h>
#include <unicode.h>
#include <heapstring.h>
#include <neexe.h>
#include <objhandle.h>
#include <font.h>
#include <vmutex.h>
#include "ft2supp.h"

#include "oslibgpi.h"

#define DBG_LOCALLOG    DBG_fontres
#include "dbglocal.h"

#define TESTFONT_RESNAME    	"TESTFONT"

//just picking a reasonable maximum; don't want to mess with linked lists
#define MAX_PUBLICFONTS		32

typedef struct {
  char  szFaceName[32];
  char  szFontResName[260];
  char  szFileName[260];
  char *pFontData;
  DWORD dwFontSize;
} SCALABLEFONTS;


static SCALABLEFONTS    *publicfonts = NULL;
static VMutex fntmutex;

#define FntLock()	fntmutex.enter()
#define FntUnlock()     fntmutex.leave()


//******************************************************************************
// Remember the font filename for GetFontData if this is a font previously
// registered with AddFontResource
//******************************************************************************
BOOL RegisterFont(HFONT hFont, LPSTR lfFaceName)
{
    if(!publicfonts) {
        return FALSE;
    }

    FntLock();
    int i;
    for(i=0;i<MAX_PUBLICFONTS;i++) {
        if(strcmp(publicfonts[i].szFaceName, lfFaceName) == 0) {
#ifdef DEBUG
            dprintf(("Remember font file %s for font %x", publicfonts[i].szFileName, hFont));
            if(GetFileAttributesA(publicfonts[i].szFileName) == -1) {
                dprintf(("ERROR: font file does not exist anymore!!!!!!"));
            }
#endif
            ObjSetHandleGDI32Data(hFont, HNDL_FONT, (DWORD)&publicfonts[i]);
            break;
        }
    }
    FntUnlock();
    if(i == MAX_PUBLICFONTS) {
        return FALSE;
    }
    return TRUE;
}
//******************************************************************************
//******************************************************************************
static LPVOID ReadEntireFile(LPSTR lpszFileName, DWORD *lpdwSize = NULL)
{
    HFILE hFile = INVALID_HANDLE_VALUE;

    DWORD dwSize, dwBytesRead, ret;
    LPVOID lpFileData = NULL;

    hFile = CreateFileA(lpszFileName,
                        GENERIC_READ,      
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,                              
                        OPEN_EXISTING,
                        0, 
                        NULL);                             

    if(hFile == INVALID_HANDLE_VALUE) {
        DebugInt3();
        goto failure;
    }  
    //determine filesize
    dwSize = SetFilePointer(hFile, 0, NULL, FILE_END);
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

    //allocate memory to hold the entire file
    lpFileData = (char *)malloc(dwSize);
    if(lpFileData == NULL) {
        DebugInt3();
        goto failure; 
    }
    //and read it
    ret = ReadFile(hFile, lpFileData, dwSize, &dwBytesRead, NULL);
    if(ret == FALSE || dwBytesRead != dwSize) {
        DebugInt3();
        goto failure;
    }
    if(lpdwSize) *lpdwSize = dwSize;

    CloseHandle(hFile);
    return lpFileData;

failure:
    if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
    if(lpFileData) free(lpFileData);
    return NULL;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFontData(HDC hdc, DWORD dwTable,
                           DWORD dwOffset,
                           LPVOID lpvBuffer,
                           DWORD cbData)
{
    HFONT hFont;
    SCALABLEFONTS *font;

    dprintf(("GDI32: GetFontData, partially implemented"));

    hFont = GetCurrentObject(hdc, OBJ_FONT);
    if(hFont == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return GDI_ERROR;
    }
    font = (SCALABLEFONTS *)ObjQueryHandleGDI32Data(hFont, HNDL_FONT);
    if(font == NULL) {
        return FT2Module.Ft2GetFontData(hdc, dwTable, dwOffset, lpvBuffer, cbData);
    }

    if(font->pFontData == NULL) 
    {
        font->pFontData = (char *)ReadEntireFile(font->szFileName, &font->dwFontSize);
        if(!font->pFontData) return GDI_ERROR;
    }

    cbData = min(cbData, font->dwFontSize);
    if(dwTable == 0 && dwOffset == 0) {
        if(lpvBuffer == NULL) return font->dwFontSize;

        memcpy(lpvBuffer, font->pFontData, cbData);
        return cbData;
    }

    //todo: parse TTF structure (haven't seen anybody rely on that yet)

  return(GDI_ERROR);
}
//******************************************************************************
//TODO: supposed to support more than only .fon files
//******************************************************************************
int WIN32API AddFontResourceA(LPCSTR szFont)
{
    DWORD   ret, alignment, nrFonts, fontOrdinal, i;
    PIMAGE_DOS_HEADER pDosHdr = NULL;
    PIMAGE_OS2_HEADER pNEHdr;
    struct resource_typeinfo_s *pResInfo;
    struct resource_nameinfo_s *pNameInfo;
    LPFONTDIR16 pFontDir = NULL;
    char  *pFontNameId;
    char  *pResNameTable;
    char  *pFontPath, *newname = NULL, *extension;
    char   szFamily[32], szFace[32];

    dprintf(("GDI32: AddFontResourceA %s", szFont));

    if(GetFileAttributesA(szFont) == -1) {
        dprintf(("WARNING: Font res file doesn't exists"));
        SetLastError(ERROR_INVALID_PARAMETER); 
        return FALSE;
    }

    //Support for raw .ttf files as well
    newname = strupr(strdup(szFont));
    extension = strrchr(newname, '.');
    if (!strcmp(extension, ".TTF"))
    {    
	    //get font family and face names
	    OSLibGpiQueryFontName(newname, szFamily, szFace, sizeof(szFamily));
    }
    else
    {
	    pDosHdr = (PIMAGE_DOS_HEADER)ReadEntireFile((LPSTR)szFont);
	    if(!pDosHdr) return FALSE;
   
	    pNEHdr  = (PIMAGE_OS2_HEADER) ((char *)pDosHdr + pDosHdr->e_lfanew);
	    if(pDosHdr->e_magic != IMAGE_DOS_SIGNATURE || 
	       pNEHdr->ne_magic != IMAGE_OS2_SIGNATURE) {
	        dprintf(("Unknown file format"));
	        goto failure;
    }

	    pResInfo = (struct resource_typeinfo_s *)((char *)pNEHdr + pNEHdr->ne_rsrctab);
	    alignment = *(WORD *)pResInfo;
	    pResInfo = (struct resource_typeinfo_s *)((char *)pResInfo+sizeof(WORD));

	    //first resource is a font directory
	    if(pResInfo->type_id != NE_RSCTYPE_FONTDIR && pResInfo->count != 1) {
	        dprintf(("Unknown file format"));
	        goto failure;
	    }
	    pNameInfo = (struct resource_nameinfo_s *)(pResInfo+1);
	    pFontDir    = (LPFONTDIR16)((char *)pDosHdr + (pNameInfo->offset << alignment));
	    //first word = length followed by the string (not terminated)
	    pFontNameId = (char *)pResInfo - sizeof(WORD) + pNameInfo->id;
	
	    nrFonts     = *(WORD *)pFontDir;
	    fontOrdinal = *((WORD *)pFontDir+1);
	    pFontDir    = (LPFONTDIR16)((char *)pFontDir + 2*sizeof(WORD));

	    pResNameTable = (char *)pNEHdr + pNEHdr->ne_restab;

	    //second resource is the absolute path of the TTF file
	    pResInfo  = (struct resource_typeinfo_s *)(pNameInfo+1);
	    if(pResInfo->type_id != 0x80CC && pResInfo->count != 1) {
        	dprintf(("Unknown file format"));
	        goto failure;
	    }
	    pNameInfo = (struct resource_nameinfo_s *)(pResInfo+1);
	    pFontPath = ((char *)pDosHdr + (pNameInfo->offset << alignment));

            free(newname);
   
	    //GPI makes assumptions about the file based on the extention (d'oh)
	    newname = strdup(pFontPath);
	    extension = strrchr(newname, '.');
    
	    strcpy(extension, ".TTF");
	    CopyFileA(pFontPath, newname, FALSE);
    }
         	
    //Load the TrueType font
    ret = OSLibGpiLoadFonts(newname);
    if(ret == FALSE) {
        dprintf(("OSLibGpiLoadFonts %s failed!!", pFontPath));
        goto failure;
    }

    //add to the list of known public fonts that we added before
    if(publicfonts == NULL) {
        publicfonts = (SCALABLEFONTS*)malloc(MAX_PUBLICFONTS*sizeof(SCALABLEFONTS));
        if(publicfonts == NULL) {
            DebugInt3();
            goto failure;
        }
        memset(publicfonts, 0, MAX_PUBLICFONTS*sizeof(SCALABLEFONTS));
    }
    FntLock();
    for(i=0;i<MAX_PUBLICFONTS;i++) {
        if(publicfonts[i].szFaceName[0] == 0) {
            break;
        }
    }
    if(i == MAX_PUBLICFONTS) {
        FntUnlock();
        DebugInt3();
        goto failure;
    }
    strncpy(publicfonts[i].szFileName, newname, sizeof(publicfonts[i].szFileName));
    if (pFontDir)
      strncpy(publicfonts[i].szFaceName, pFontDir->szDeviceName, sizeof(publicfonts[i].szFaceName));
    else
      strncpy(publicfonts[i].szFaceName, szFace, sizeof(publicfonts[i].szFaceName));
    strncpy(publicfonts[i].szFontResName, szFont, sizeof(publicfonts[i].szFontResName));
    publicfonts[i].pFontData = NULL;
    FntUnlock();

    free(newname);
    free(pDosHdr);
    return 1;

failure:
    if(pDosHdr) free(pDosHdr);
    if(newname) free(newname);
    return 0;
}
//******************************************************************************
//******************************************************************************
int WIN32API AddFontResourceW(LPCWSTR szFont)
{
 char *astring = UnicodeToAsciiString((LPWSTR)szFont);
    int  ret;

    dprintf(("GDI32: AddFontResourceW"));
    ret = AddFontResourceA(astring);
    FreeAsciiString(astring);
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceA(LPCSTR lpszFont)
{
    BOOL ret;

    dprintf(("GDI32: RemoveFontResourceA %s", lpszFont));
    
    if(!publicfonts) {
        dprintf(("WARNING: font not found in our list!!"));
    return FALSE;
    }
    FntLock();
    int i;
    for(i=0;i<MAX_PUBLICFONTS;i++) {
        if(strcmp(publicfonts[i].szFontResName, lpszFont) == 0)  
        {
            ret = OSLibGpiUnloadFonts(publicfonts[i].szFileName);
            DeleteFileA(publicfonts[i].szFileName);

            if(publicfonts[i].pFontData) free(publicfonts[i].pFontData);
            publicfonts[i].pFontData     = 0;
            publicfonts[i].szFileName[0] = 0;
            publicfonts[i].szFaceName[0] = 0;
            break;
        }
    }
    FntUnlock();
    if(i == MAX_PUBLICFONTS) {
        dprintf(("WARNING: font not found in our list!!"));
// no need to trap on a warning        DebugInt3();
        return FALSE;
    }
    return ret;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveFontResourceW(LPCWSTR szFont)
{
 char *astring = UnicodeToAsciiString((LPWSTR)szFont);
    BOOL  ret;

    dprintf(("GDI32: RemoveFontResourceW"));
    ret = RemoveFontResourceA(astring);
    FreeAsciiString(astring);
    return ret;
}
/*****************************************************************************
 * Name      : BOOL CreateScalableFontResourceA
 * Purpose   : The CreateScalableFontResourceA function creates a font resource
 *             file for a scalable font.
 * Parameters: DWORD   fdwHidden       flag for read-only embedded font
 *             LPCSTR lpszFontRes     address of filename for font resource
 *             LPCSTR lpszFontFile    address of filename for scalable font
 *             LPCSTR lpszCurrentPath address of path to font file
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CreateScalableFontResourceA(DWORD fdwHidden,
                                          LPCSTR lpszFontRes,
                                          LPCSTR lpszFontFile,
                                          LPCSTR lpszCurrentPath)
{
    HRSRC   hRsrc;
    HINSTANCE hGdi32;
    HGLOBAL handle;
    DWORD   dwSize;
    LPVOID  lpData, lpCopy = NULL;
    HANDLE  hFile = 0;
    DWORD   ret, dwBytesWritten, alignment, nrFonts, fontOrdinal, len;
    PIMAGE_DOS_HEADER pDosHdr;
    PIMAGE_OS2_HEADER pNEHdr;
    struct resource_typeinfo_s *pResInfo;
    struct resource_nameinfo_s *pNameInfo;
    LPFONTDIR16 pFontDir;
    char  *pFontNameId;
    char  *pResNameTable;
    char  *pFontPath, *newname, *extension;
    char   szFamily[32], szFace[32];

    dprintf(("GDI32: CreateScalableFontResourceA %x %s %s %s partly implemented", fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath));

    if(lpszCurrentPath != NULL) {
        //figure out which possibilities exist
        DebugInt3();
        return FALSE;
    }

    //This function is supposed to create a font resource file (.fot; NE dll with
    //references to the TTF file (absolute path))

    if(GetFileAttributesA(lpszFontRes) != -1) {
        dprintf(("WARNING: Font res file already exists"));
        SetLastError(ERROR_INVALID_PARAMETER); 
        return FALSE;
    }

    //fetch our font resource file example
    hGdi32 = GetModuleHandleA("GDI32");
    hRsrc = FindResourceA( hGdi32, TESTFONT_RESNAME, RT_RCDATAA );
    if(!hRsrc) {
        dprintf(("WARNING: Test font file resource not found!!"));
        DebugInt3();
        return FALSE;
    }
    handle = LoadResource(hGdi32, hRsrc);
    dwSize = SizeofResource(hGdi32, hRsrc);
    lpData = LockResource(handle);
    if(!dwSize || !handle || !lpData) {
        dprintf(("WARNING: Test font file resource not loaded/locked!!"));
        DebugInt3();
        return FALSE;
    }
    lpCopy = malloc(dwSize);
    if(lpCopy == NULL) {
        DebugInt3();
        return FALSE;
    }
    memcpy(lpCopy, lpData, dwSize);

    pDosHdr = (PIMAGE_DOS_HEADER)lpCopy;
    pNEHdr  = (PIMAGE_OS2_HEADER) ((char *)pDosHdr + pDosHdr->e_lfanew);
    if(pDosHdr->e_magic != IMAGE_DOS_SIGNATURE || 
       pNEHdr->ne_magic != IMAGE_OS2_SIGNATURE) {
        dprintf(("Unknown file format"));
        goto failure;
    }

    pResInfo = (struct resource_typeinfo_s *)((char *)pNEHdr + pNEHdr->ne_rsrctab);
    alignment = *(WORD *)pResInfo;
    pResInfo = (struct resource_typeinfo_s *)((char *)pResInfo+sizeof(WORD));

    //first resource is a font directory
    if(pResInfo->type_id != NE_RSCTYPE_FONTDIR && pResInfo->count != 1) {
        dprintf(("Unknown file format"));
        goto failure;
    }
    pNameInfo = (struct resource_nameinfo_s *)(pResInfo+1);
    pFontDir    = (LPFONTDIR16)((char *)pDosHdr + (pNameInfo->offset << alignment));
    //first word = length followed by the string (not terminated)
    pFontNameId = (char *)pResInfo - sizeof(WORD) + pNameInfo->id;

    //GPI makes assumptions about the file based on the extention (d'oh)
    newname = strdup(lpszFontFile);
    extension = strrchr(newname, '.');
    
    strcpy(extension, ".TTF");
    CopyFileA(lpszFontFile, newname, FALSE);

    //get font family and face names
    OSLibGpiQueryFontName(newname, szFamily, szFace, sizeof(szFamily));

    DeleteFileA(newname);
    free(newname);

    nrFonts     = *(WORD *)pFontDir;
    fontOrdinal = *((WORD *)pFontDir+1);
    pFontDir    = (LPFONTDIR16)((char *)pFontDir + 2*sizeof(WORD));

    //copy family and face names to fontdir structure
    len = strlen(szFamily);
    strcpy(pFontDir->szDeviceName, szFamily);
    strcpy(pFontDir->szDeviceName + len + 1, szFace);

    dprintf(("Family %s, Face %s", szFamily, szFace));

    pResNameTable = (char *)pNEHdr + pNEHdr->ne_restab;

    //second resource is the absolute path of the TTF file
    pResInfo  = (struct resource_typeinfo_s *)(pNameInfo+1);
    if(pResInfo->type_id != 0x80CC && pResInfo->count != 1) {
        dprintf(("Unknown file format"));
        goto failure;
    }
    pNameInfo = (struct resource_nameinfo_s *)(pResInfo+1);
    pFontPath = ((char *)pDosHdr + (pNameInfo->offset << alignment));
    //overwrite absolute font path
    strcpy(pFontPath, lpszFontFile);
    pNameInfo->length = strlen(lpszFontFile)+1;

    hFile = CreateFileA(lpszFontRes,                      
                        GENERIC_READ | GENERIC_WRITE,      
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,                              
                        CREATE_NEW,
                        0, 
                        NULL);                             

    if(hFile == INVALID_HANDLE_VALUE) {
        DebugInt3();
        goto failure;
    }  
    ret = WriteFile(hFile, lpCopy, dwSize, &dwBytesWritten, NULL);
    if(ret == FALSE || dwBytesWritten != dwSize) {
        DebugInt3();
        goto failure;
    }    
    CloseHandle(hFile);

    if(lpCopy) free(lpCopy);

    SetLastError(ERROR_SUCCESS);
    return TRUE;

failure:
    if(hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        DeleteFileA(lpszFontRes);
    }
    if(lpCopy) free(lpCopy);
    SetLastError(ERROR_INVALID_PARAMETER); //not correct
  return FALSE;
}


/*****************************************************************************
 * Name      : BOOL CreateScalableFontResourceW
 * Purpose   : The CreateScalableFontResourceW function creates a font resource
 *             file for a scalable font.
 * Parameters: DWORD   fdwHidden       flag for read-only embedded font
 *             LPCSTR lpszFontRes     address of filename for font resource
 *             LPCSTR lpszFontFile    address of filename for scalable font
 *             LPCSTR lpszCurrentPath address of path to font file
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API CreateScalableFontResourceW(DWORD fdwHidden,
                                          LPCWSTR lpszFontRes,
                                          LPCWSTR lpszFontFile,
                                          LPCWSTR lpszCurrentPath)
{
  LPSTR lpszFontFileA = NULL, lpszFontResA = NULL, lpszCurrentPathA = NULL;

  dprintf(("GDI32: CreateScalableFontResourceW %x %ls %ls %ls not implemented", fdwHidden, lpszFontRes, lpszFontFile, lpszCurrentPath));

  STACK_strdupWtoA(lpszFontFile, lpszFontFileA);
  STACK_strdupWtoA(lpszFontRes, lpszFontResA);
  STACK_strdupWtoA(lpszCurrentPath, lpszCurrentPathA);
  return CreateScalableFontResourceA(fdwHidden, lpszFontResA, lpszFontFileA, lpszCurrentPathA);
}
//******************************************************************************
//******************************************************************************
