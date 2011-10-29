/* $Id: kFilePE.cpp,v 1.5 2002-02-24 02:47:26 bird Exp $
 *
 * kFilePE - PE files.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/******************************************************************************
*   Header Files                                                              *
******************************************************************************/
#include <string.h>

#include "MZexe.h"
#include "PEexe.h"

#include "kTypes.h"
#include "kError.h"
#include "kFile.h"
#include "kFileFormatBase.h"
#include "kFileInterfaces.h"
#include "kFilePE.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#if 0
kFilePE kFilePE((kFile*)NULL);
#endif


/**
 * Constructs a kFilePE object for a file.
 * @param     pFile     File to create object from.
 * @remark    throws errorcode
 */
kFilePE::kFilePE(kFile *pFile) throw (kError) :
    kFileFormatBase(pFile),
    pvBase(NULL),
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
    long                offPEHdr = 0;
    IMAGE_NT_HEADERS pehdr;

    /* read dos-header (If tehre is one) */
    pFile->setThrowOnErrors();
    pFile->readAt(&doshdr, sizeof(doshdr), 0);
    if (doshdr.e_magic == IMAGE_DOS_SIGNATURE)
    {
        if (doshdr.e_lfanew <= sizeof(doshdr))
            throw(kError(kError::INVALID_EXE_SIGNATURE));
        offPEHdr = doshdr.e_lfanew;
    }

    /* read pe headers and do minor verifications */
    pFile->readAt(&pehdr, sizeof(pehdr), offPEHdr);
    if (pehdr.Signature != IMAGE_NT_SIGNATURE)
        throw(kError(kError::INVALID_EXE_SIGNATURE));
    if (   pehdr.FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER)
        || pehdr.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
        throw(kError(kError::BAD_EXE_FORMAT));

            /* create mapping */
   pvBase = new char [pehdr.OptionalHeader.SizeOfImage];
   if (pvBase == NULL)
       throw(kError(kError::NOT_ENOUGH_MEMORY));
   memset(pvBase, 0, pehdr.OptionalHeader.SizeOfImage);

                /*
                printf("%ld\n", pehdr.OptionalHeader.SizeOfHeaders);
                printf("%ld\n", sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * pehdr.FileHeader.NumberOfSections);
   kASSERT(pehdr.OptionalHeader.SizeOfHeaders ==
                       sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * pehdr.FileHeader.NumberOfSections);
                */
   try
                {
       /* read optional header */
       pFile->readAt(pvBase, pehdr.OptionalHeader.SizeOfHeaders, 0);

                    /* read sections */
                    int i;
                    for (i = 0; i < pehdr.FileHeader.NumberOfSections; i++)
                    {
           unsigned long  cbSection;
                        PIMAGE_SECTION_HEADER pSectionHdr =
                        #if 0
               IMAGE_FIRST_SECTION(((unsigned long)pvBase + ((PIMAGE_DOS_HEADER)pvBase)->e_lfanew));
                        #else
               (PIMAGE_SECTION_HEADER) ( (unsigned long)pvBase + doshdr.e_lfanew + sizeof(IMAGE_NT_HEADERS) );
                        #endif
                        pSectionHdr += i;

           cbSection = KMIN(pSectionHdr->Misc.VirtualSize, pSectionHdr->SizeOfRawData);
           if (cbSection)
               pFile->readAt((char*)pvBase + pSectionHdr->VirtualAddress,
                             cbSection, pSectionHdr->PointerToRawData);
       }

                    /* set pointers */
                    if (*(unsigned short*)pvBase == IMAGE_DOS_SIGNATURE)
                    {
                        pDosHdr = (PIMAGE_DOS_HEADER)pvBase;
           pFileHdr = (PIMAGE_FILE_HEADER)((char*)pvBase + pDosHdr->e_lfanew + 4);
                    }
                    else
           pFileHdr = (PIMAGE_FILE_HEADER)((char*)pvBase + 4);

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
                   ((unsigned long*)&this->pExportDir)[i] = (int)pvBase + paDataDir[i].VirtualAddress;
                        }
                    }
                }
   catch (kError err)
   {
       delete(pvBase);
       pvBase = NULL;
       throw(err);
    }
}


/**
 * Destructor.
 */
kFilePE::~kFilePE() throw (kError)
{
    if (pvBase)
        delete(pvBase);
}


/**
 * Query for the module name.
 * @returns Success indicator. TRUE / FALSE.
 * @param   pszBuffer   Pointer to buffer which to put the name into.
 * @param   cchBuffer   Size of the buffer (defaults to 260 chars).
 */
KBOOL  kFilePE::moduleGetName(char *pszBuffer, int cchSize/* = 260*/)
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
KBOOL   kFilePE::exportFindFirst(kExportEntry *pExport)
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
KBOOL   kFilePE::exportFindNext(kExportEntry *pExport)
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
KBOOL kFilePE::exportLookup(unsigned long ulOrdinal, kExportEntry *pExport)
{
    kASSERT(!"not implemented.");
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
KBOOL kFilePE::exportLookup(const char *  pszName, kExportEntry *pExport)
{
    kASSERT(!"not implemented.");
    pszName = pszName;
    pExport = pExport;
    return FALSE;
}


/**
 * Mini dump function.
 */
KBOOL  kFilePE::dump(kFile *pOut)
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
                unsigned long * paulIndex;
                unsigned long * paulCallback;
                unsigned long   ulBorlandRVAFix = 0UL;

                /* Check if the addresses in the TLSDir is RVAs or real addresses */
                if (pTLSDir[i].StartAddressOfRawData > pOptHdr->ImageBase)
                    ulBorlandRVAFix = pOptHdr->ImageBase;

                j = 0;
                paulIndex    = (unsigned long *)((unsigned long)pTLSDir[i].AddressOfIndex - ulBorlandRVAFix + (unsigned long)this->pvBase);
                paulCallback = (unsigned long *)((unsigned long)pTLSDir[i].AddressOfCallBacks - ulBorlandRVAFix + (unsigned long)this->pvBase);
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

