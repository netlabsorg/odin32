/*
 * kFilePE - PE files.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
/* emx fixups */
#ifdef __EMX__
    #define __stdcall
     #define max(a,b) (((a) > (b)) ? (a) : (b))
     #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

/******************************************************************************
*   Header Files                                                              *
******************************************************************************/
#ifdef __EMX__
#define INT INT_
#define PCHAR PCHAR_
#endif
#include <os2.h>
#ifdef __EMX__
#undef PCHAR
#undef INT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <peexe.h>
#include "kFileFormatBase.h"
#include "kFilePe.h"


/**
 * Constructs a kFilePE object for a file.
 * @param     phFile  File to create object from.
 * @remark    throws errorcode (TODO: errorhandling.)
 */
kFilePE::kFilePE(FILE *phFile) throw(int) : pvBase(NULL),
    pDosHdr(NULL), pFileHdr(NULL), pOptHdr(NULL), paDataDir(NULL), paSectionHdr(NULL),
    pExportDir(NULL)
{
    IMAGE_DOS_HEADER doshdr;

    /* read dos-header - assumes there is one */
    if (!fseek(phFile, 0, SEEK_SET)
        && fread(&doshdr, sizeof(doshdr), 1, phFile) == 1
        && doshdr.e_magic == IMAGE_DOS_SIGNATURE
        && doshdr.e_lfanew > sizeof(doshdr)
        )
    {
        IMAGE_NT_HEADERS pehdr;

        /* read pe headers */
        if (!fseek(phFile, doshdr.e_lfanew, SEEK_SET)
            && fread(&pehdr, sizeof(pehdr), 1, phFile) == 1
            && pehdr.Signature == IMAGE_NT_SIGNATURE
            && pehdr.FileHeader.SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER)
            && pehdr.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC)
        {
            /* create mapping */
            pvBase = malloc((size_t)pehdr.OptionalHeader.SizeOfImage);
            if (pvBase != NULL)
            {
                memset(pvBase, 0, (size_t)pehdr.OptionalHeader.SizeOfImage);
                /*
                printf("%ld\n", pehdr.OptionalHeader.SizeOfHeaders);
                printf("%ld\n", sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * pehdr.FileHeader.NumberOfSections);
                assert(pehdr.OptionalHeader.SizeOfHeaders ==
                       sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * pehdr.FileHeader.NumberOfSections);
                */
                if (!fseek(phFile, 0, SEEK_SET)
                    && fread(pvBase, (size_t)pehdr.OptionalHeader.SizeOfHeaders, 1, phFile) == 1
                    )
                {
                    /* read sections */
                    for (int i = 0; i < pehdr.FileHeader.NumberOfSections; i++)
                    {
                        ULONG  cbSection;
                        PIMAGE_SECTION_HEADER pSectionHdr =
                        #if 0
                            IMAGE_FIRST_SECTION(((ULONG)pvBase + ((PIMAGE_DOS_HEADER)pvBase)->e_lfanew));
                        #else
                            (PIMAGE_SECTION_HEADER) ( (ULONG)pvBase + doshdr.e_lfanew + sizeof(IMAGE_NT_HEADERS) );
                        #endif
                        pSectionHdr += i;

                        cbSection = min(pSectionHdr->Misc.VirtualSize, pSectionHdr->SizeOfRawData);
                        if (fseek(phFile, pSectionHdr->PointerToRawData, SEEK_SET)
                            ||
                            fread((void*)((ULONG)pvBase + pSectionHdr->VirtualAddress), (size_t)cbSection, 1, phFile) != 1
                            )
                        {
                            /* error */
                            free(pvBase);
                            pvBase = NULL;
                            throw(6);
                        }
                    }

                    /* set pointers */
                    if (*(unsigned short*)pvBase == IMAGE_DOS_SIGNATURE)
                    {
                        pDosHdr = (PIMAGE_DOS_HEADER)pvBase;
                        pFileHdr = (PIMAGE_FILE_HEADER)((DWORD)pvBase + pDosHdr->e_lfanew + 4);
                    }
                    else
                        pFileHdr = (PIMAGE_FILE_HEADER)((DWORD)pvBase + 4);

                    pOptHdr = (PIMAGE_OPTIONAL_HEADER)((int)pFileHdr + sizeof(*pFileHdr));
                    paDataDir = (PIMAGE_DATA_DIRECTORY)((int)pOptHdr + pFileHdr->SizeOfOptionalHeader
                                                        - pOptHdr->NumberOfRvaAndSizes*sizeof(*paDataDir));
                    paSectionHdr = (PIMAGE_SECTION_HEADER)((int)paDataDir +
                                                           pOptHdr->NumberOfRvaAndSizes*sizeof(*paDataDir));

                    if (paDataDir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
                        pExportDir = (PIMAGE_EXPORT_DIRECTORY)((int)pvBase + paDataDir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
                }
                else
                    throw(4);
            }
            else
                throw(3);
        }
        else
            throw(2);
    }
    else
        throw(1);
}


/**
 * Destructor.
 */
kFilePE::~kFilePE()
{
    if (pvBase)
        delete pvBase;
}


/**
 * Query for the module name.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pszBuffer  Pointer to buffer which to put the name into.
 */
BOOL  kFilePE::queryModuleName(char *pszBuffer)
{
    if (pExportDir && pExportDir->Name)
        strcpy(pszBuffer, (char*)((int)pExportDir->Name + (int)pvBase));
    else
        return FALSE;

    return TRUE;
}


/**
 * Finds the first exports.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 * @remark
 */
BOOL  kFilePE::findFirstExport(PEXPORTENTRY pExport)
{
    if (pExportDir && pExportDir->NumberOfFunctions)
    {
        memset(pExport, 0, sizeof(EXPORTENTRY));
        pExport->ulOrdinal = pExportDir->Base - 1;
        return findNextExport(pExport);
    }

    return FALSE;
}


/**
 * Finds the next export.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 * @remark
 */
BOOL  kFilePE::findNextExport(PEXPORTENTRY pExport)
{
    if (pExportDir && pExportDir->NumberOfFunctions)
    {
        void **ppv = (void**)((int)pExportDir->AddressOfFunctions + (int)pvBase);

        ++pExport->ulOrdinal -= pExportDir->Base;
        while (ppv[pExport->ulOrdinal] == NULL && pExport->ulOrdinal < pExportDir->NumberOfFunctions)
            pExport->ulOrdinal++;

        if (pExport->ulOrdinal < pExportDir->NumberOfFunctions)
        {
            int iName = 0;
            unsigned short *pawNameOrdinals = (unsigned short *)
                ((int)pExportDir->AddressOfNameOrdinals + (int)pvBase);

            /* look for name */
            while (iName < (int)pExportDir->NumberOfNames &&
                   pawNameOrdinals[iName] != pExport->ulOrdinal)
                iName++;
            if (iName < (int)pExportDir->NumberOfNames)
                strcpy(&pExport->achName[0],
                       (char*)((int)pvBase + ((int*)((int)pvBase + (int)pExportDir->AddressOfNames))[iName]));
            else
                pExport->achName[0] = '\0';
            pExport->ulOrdinal += pExportDir->Base;
        }
        else
            return FALSE;
    }
    else
        return FALSE;

    pExport->achIntName[0] = '\0';
    pExport->pv = NULL;
    return TRUE;
}


