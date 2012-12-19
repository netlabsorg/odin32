/* $Id: pefile.h,v 1.7 2000-10-06 11:03:14 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __PEFILE_H__
#define __PEFILE_H__

#ifndef _OS2WIN_H
#include <os2wrap.h>
#include <win32type.h>
#include <peexe.h>
#include <stdlib.h> //min & max

#define IDS_ERRBADFILENAME  1000
#define IDR_CURSOR      1
#define IDR_BITMAP      2
#define IDR_ICON        3
#define IDR_MENU        4
#define IDR_DIALOG      5
#define IDR_STRING      6
#define IDR_FONTDIR     7
#define IDR_FONT        8
#define IDR_ACCELERATOR     9
#define IDR_RCDATA      10
#define IDR_MESSAGETABLE    11

#endif

    #define    NTRT_NEWRESOURCE      0x2000
    #define    NTRT_ERROR            0x7fff
    #define    NTRT_CURSOR           1
    #define    NTRT_BITMAP           2
    #define    NTRT_ICON             3
    #define    NTRT_MENU             4
    #define    NTRT_DIALOG           5
    #define    NTRT_STRING           6
    #define    NTRT_FONTDIR          7
    #define    NTRT_FONT             8
    #define    NTRT_ACCELERATORS     9
    #define    NTRT_RCDATA           10
    #define    NTRT_MESSAGETABLE     11
    #define    NTRT_GROUP_CURSOR     12
    #define    NTRT_GROUP_ICON       14
    #define    NTRT_VERSION          16
    #define    NTRT_NEWBITMAP        (NTRT_BITMAP|NTRT_NEWRESOURCE)
    #define    NTRT_NEWMENU          (NTRT_MENU|NTRT_NEWRESOURCE)
    #define    NTRT_NEWDIALOG        (NTRT_DIALOG|NTRT_NEWRESOURCE)

#define SIZE_OF_NT_SIGNATURE    sizeof (DWORD)
#define MAXRESOURCENAME     13

#define PEHEADEROFF(a) ((LPVOID)((BYTE *)a          +  \
             ((IMAGE_DOS_HEADER*)a)->e_lfanew    +  \
             SIZE_OF_NT_SIGNATURE))

#define OPTHEADEROFF(a) ((LPVOID)((BYTE *)a          +  \
             ((IMAGE_DOS_HEADER*)a)->e_lfanew    +  \
             SIZE_OF_NT_SIGNATURE            +  \
             sizeof (IMAGE_FILE_HEADER)))

#define SECTIONHDROFF(a) ((LPVOID)((BYTE *)a          +  \
             ((IMAGE_DOS_HEADER*)a)->e_lfanew    +  \
             SIZE_OF_NT_SIGNATURE            +  \
             sizeof (IMAGE_FILE_HEADER)      +  \
             sizeof (IMAGE_OPTIONAL_HEADER)))

#define NR_SECTIONS(a)	((int)((PIMAGE_FILE_HEADER)PEHEADEROFF (a))->NumberOfSections)

BOOL    GetPEFileHeader (LPVOID, PIMAGE_FILE_HEADER);
BOOL    GetPEOptionalHeader (LPVOID, PIMAGE_OPTIONAL_HEADER);
LPVOID  ImageDirectoryOffset (LPVOID, DWORD);
DWORD   ImageDirectorySize (LPVOID, DWORD);
BOOL    IsSectionType(LPVOID lpFile, PIMAGE_SECTION_HEADER psh, DWORD dwIMAGE_DIRECTORY);
BOOL    GetSectionHdrByName (LPVOID, PIMAGE_SECTION_HEADER, char *);
BOOL    GetSectionHdrByType (LPVOID, PIMAGE_SECTION_HEADER, int);
BOOL    GetSectionHdrByImageDir (LPVOID, DWORD, PIMAGE_SECTION_HEADER);
BOOL    GetSectionHdrByRVA (LPVOID lpFile, IMAGE_SECTION_HEADER *sh, ULONG rva);

int     GetNumberOfResources (LPVOID);
int     GetListOfResourceTypes (LPVOID, char **);

PIMAGE_IMPORT_MODULE_DIRECTORY GetImportFunctions (LPVOID lpFile, char *pszModule, char **szBase);

#endif