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
#include "kFile.h"
#include "kFileFormatBase.h"
#include "kInterfaces.h"
#include "kFilePe.h"

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#if 0
kFilePE kFilePE((kFile*)NULL);
#endif

/**
 * Constructs a kFilePE object for a file.
 * @param     pFile     File to create object from.
 * @remark    throws errorcode (TODO: errorhandling.)
 */
kFilePE::kFilePE(kFile *pFile) throw(int) : pvBase(NULL),
    pDosHdr(NULL), pFileHdr(NULL), pOptHdr(NULL), paDataDir(NULL), paSectionHdr(NULL),
    pExportDir(NULL),
    pImportDir(NULL),
    pRsrcDir(NULL),
    pBRelocDir(NULL),
    pDebugDir(NULL),
    pCopyright(NULL),
    pulGlobalPtr(NULL),
    pTLSDir(NULL),
    pLoadConfigDir(NULL),
    pBoundImportDir(NULL),
    pIATDir(NULL),
    pDelayImportDir(NULL)
{
    IMAGE_DOS_HEADER doshdr;

    /* read dos-header - assumes there is one */
    if (   pFile->readAt(&doshdr, sizeof(doshdr), 0)
        && doshdr.e_magic == IMAGE_DOS_SIGNATURE
        && doshdr.e_lfanew > sizeof(doshdr)
        )
    {
        IMAGE_NT_HEADERS pehdr;

        /* read pe headers */
        if (   pFile->readAt(&pehdr, sizeof(pehdr), doshdr.e_lfanew)
            && pehdr.Signature == IMAGE_NT_SIGNATURE
            && pehdr.FileHeader.SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER)
            && pehdr.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC)
        {
            /* create mapping */
            pvBase = calloc((size_t)pehdr.OptionalHeader.SizeOfImage, 1);
            if (pvBase != NULL)
            {
                /*
                printf("%ld\n", pehdr.OptionalHeader.SizeOfHeaders);
                printf("%ld\n", sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * pehdr.FileHeader.NumberOfSections);
                assert(pehdr.OptionalHeader.SizeOfHeaders ==
                       sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * pehdr.FileHeader.NumberOfSections);
                */
                if (pFile->readAt(pvBase, pehdr.OptionalHeader.SizeOfHeaders, 0))
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
                        if (    cbSection
                            &&  !pFile->readAt((char*)pvBase + pSectionHdr->VirtualAddress,
                                               cbSection,
                                               pSectionHdr->PointerToRawData)
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

                    //if (paDataDir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
                    //    pExportDir = (PIMAGE_EXPORT_DIRECTORY)((int)pvBase + paDataDir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
                    for (i = 0; i < pOptHdr->NumberOfRvaAndSizes && i < 16; i++)
                    {
                        if (paDataDir[i].VirtualAddress != 0)
                        {
                            if (paDataDir[i].VirtualAddress < pOptHdr->SizeOfImage)
                                ((PULONG)&this->pExportDir)[i] = (int)pvBase + paDataDir[i].VirtualAddress;
                            #ifdef DEBUG
                            else
                                fprintf(stderr, "bad directory pointer %d\n", i);
                            #endif
                        }
                    }
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
 * @returns Success indicator. TRUE / FALSE.
 * @param   pszBuffer   Pointer to buffer which to put the name into.
 * @param   cchBuffer   Size of the buffer (defaults to 260 chars).
 */
BOOL  kFilePE::moduleGetName(char *pszBuffer, int cchSize/* = 260*/)
{
    if (pExportDir && pExportDir->Name)
    {
        char *psz = (char*)((int)pExportDir->Name + (int)pvBase);
        int cch = strlen(psz) + 1;
        if (cch > cchSize)
            return FALSE;
        memcpy(pszBuffer, psz, cch);
    }
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
BOOL  kFilePE::exportFindFirst(kExportEntry *pExport)
{
    if (pExportDir && pExportDir->NumberOfFunctions)
    {
        memset(pExport, 0, sizeof(kExportEntry));
        pExport->ulOrdinal = pExportDir->Base - 1;
        return exportFindNext(pExport);
    }

    return FALSE;
}


/**
 * Finds the next export.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     pExport  Pointer to export structure.
 * @remark
 */
BOOL  kFilePE::exportFindNext(kExportEntry *pExport)
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

    pExport->ulAddress = pExport->iObject = pExport->ulOffset = ~0UL; /* FIXME/TODO */
    pExport->achIntName[0] = '\0';
    pExport->pv = NULL;
    return TRUE;
}


/**
 * Frees resources associated with the communicatin area.
 * It's not necessary to call this when exportFindNext has return FALSE.
 * (We don't allocate anything so it's not a problem ;-)
 * @param   pExport     Communication area which has been successfully
 *                      processed by findFirstExport.
 */
void kFilePE::exportFindClose(kExportEntry *pExport)
{
    pExport = pExport;
    return;
}


/**
 * Lookup information on a spesific export given by ordinal number.
 * @returns Success indicator.
 * @param   pExport     Communication area containing export information
 *                      on successful return.
 * @remark  stub
 */
BOOL kFilePE::exportLookup(unsigned long ulOrdinal, kExportEntry *pExport)
{
    assert(!"not implemented.");
    ulOrdinal = ulOrdinal;
    pExport = pExport;
    return FALSE;
}

/**
 * Lookup information on a spesific export given by name.
 * @returns Success indicator.
 * @param   pExport     Communication area containing export information
 *                      on successful return.
 * @remark  stub
 */
BOOL kFilePE::exportLookup(const char *  pszName, kExportEntry *pExport)
{
    assert(!"not implemented.");
    pszName = pszName;
    pExport = pExport;
    return FALSE;
}


/**
 * Mini dump function.
 */
BOOL  kFilePE::dump(kFile *pOut)
{
    int i,j,k;
    int c;

    /*
     * Dump sections.
     */
    pOut->printf("Sections\n"
                 "--------\n");
    for (i = 0; i < pFileHdr->NumberOfSections; i++)
    {
        pOut->printf("%2d  %-8.8s  VirtSize: 0x%08x  RVA: 0x%08x\n"
                     "    RawSize:  0x%08x  FileOffset: 0x%08x\n"
                     "    #Relocs:  0x%08x  FileOffset: 0x%08x\n"
                     "    #LineNbr: 0x%08x  FileOffset: 0x%08x\n"
                     "    Characteristics: 0x%08x\n",
                     i,
                     paSectionHdr[i].Name,
                     paSectionHdr[i].Misc.VirtualSize,
                     paSectionHdr[i].VirtualAddress,
                     paSectionHdr[i].PointerToRawData,
                     paSectionHdr[i].PointerToRawData,
                     paSectionHdr[i].NumberOfRelocations,
                     paSectionHdr[i].PointerToRelocations,
                     paSectionHdr[i].NumberOfLinenumbers,
                     paSectionHdr[i].PointerToLinenumbers,
                     paSectionHdr[i].Characteristics
                     );
    }
    pOut->printf("\n");


    /*
     * Dump the directories.
     */
    pOut->printf("Data Directory\n"
                 "--------------\n");
    for (i = 0; i < pOptHdr->NumberOfRvaAndSizes; i++)
    {
        static const char * apszDirectoryNames[] =
        {
            "Export",
            "Import",
            "Resource",
            "Exception",
            "Security",
            "Base Reloc",
            "Debug",
            "Copyright",
            "Global Ptr",
            "TLS",
            "Load Config",
            "Bound Import",
            "IAT",
            "Delay Import",
            "COM Descriptor",
            "unknown",
            "unknown"
        };

        pOut->printf("%2d  %-16s  Size: 0x%08x  RVA: 0x%08x\n",
                     i,
                     apszDirectoryNames[i],
                     pOptHdr->DataDirectory[i].Size,
                     pOptHdr->DataDirectory[i].VirtualAddress);
    }
    pOut->printf("\n");


    /*
     * Dump Import Directory if present.
     */
    if (pImportDir)
    {
        pOut->printf("Import Directory\n"
                     "----------------\n");

        PIMAGE_IMPORT_DESCRIPTOR pCur = pImportDir;
        while (pCur->u.Characteristics != 0)
        {
            pOut->printf("%s\n", (char*)pvBase + pCur->Name);
            pCur++;
        }
    }


    /*
     * Dump TLS directory if present
     */
    if (pTLSDir)
    {
        pOut->printf("TLS Directory\n"
                     "-------------\n");
        if (pOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size % sizeof(IMAGE_TLS_DIRECTORY))
            pOut->printf(" Warning! The size directory isn't a multiple of the directory struct!");

        c = (int)(pOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size / sizeof(IMAGE_TLS_DIRECTORY));
        for (i = 0; i < c; i++)
        {

            pOut->printf("%2d  StartAddressOfRawData %p\n"
                         "    EndAddressOfRawData   %p\n"
                         "    AddressOfIndex        %p\n"
                         "    AddressOfCallBacks    %p\n"
                         "    SizeOfZeroFill        %p\n"
                         "    Characteristics       %p\n",
                         i,
                         pTLSDir[i].StartAddressOfRawData,
                         pTLSDir[i].EndAddressOfRawData,
                         pTLSDir[i].AddressOfIndex,
                         pTLSDir[i].AddressOfCallBacks,
                         pTLSDir[i].SizeOfZeroFill,
                         pTLSDir[i].Characteristics);

            /* Print Callbacks */
            if (pTLSDir[i].AddressOfCallBacks)
            {
                PULONG  paulIndex;
                PULONG  paulCallback;
                ULONG   ulBorlandRVAFix = 0UL;

                /* Check if the addresses in the TLSDir is RVAs or real addresses */
                if (pTLSDir[i].StartAddressOfRawData > pOptHdr->ImageBase)
                    ulBorlandRVAFix = pOptHdr->ImageBase;

                j = 0;
                paulIndex    = (PULONG)((ULONG)pTLSDir[i].AddressOfIndex - ulBorlandRVAFix + (ULONG)this->pvBase);
                paulCallback = (PULONG)((ULONG)pTLSDir[i].AddressOfCallBacks - ulBorlandRVAFix + (ULONG)this->pvBase);
                if (*paulCallback)
                {
                    pOut->printf("    Callbacks:\n");
                    for (j = 0; paulCallback[j] != 0; j++)
                    {
                        pOut->printf("      %02d  Address: 0x%08x  Index: 0x%08x\n",
                                     paulIndex[j],
                                     paulCallback[j]);
                    }
                }
                else
                    pOut->printf("    (Empty callback array!)\n");
            }

        }

        pOut->printf("\n");
    }


    return TRUE;
}

