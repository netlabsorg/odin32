/* $Id: lxfile.cpp,v 1.1 1999-09-06 02:20:05 bird Exp $
 *
 * PE2LX/win32k - Implementation of the class LXFile.
 *
 * Copyright 1998-1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdarg.h>

#include "pe2lx.h"                      /* toplevel includefile */
/* Extra Ring0 stuff */
#include "OS2Krnl.h"
#include "ldrCalls.h"
#include "sprintf.h"
#include "Yield.h"

/* resource includes */
#include "accelerator.h"
#include "cursor.h"
#include "cursorgrp.h"
#include "dialog.h"
#include "icon.h"
#include "icongrp.h"
#include "menu.h"
#include "bitmap.h"
#include "rcdata.h"
#include "strings.h"

#include <versionos2.h>



/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** heximal(decimal) Buffer for hex(ULONG) */
char szHexBuffer[30];



/**
 * Print a number on the form "0xNUMINHEX (NUMINDEC)".
 * Example: hex(1024) returns "0x00000400 (1024)".
 *  @return pointer
 */
char *hex(ULONG num)
{
    sprintf(szHexBuffer, "0x%+08x (%lu)",num,num);
    return szHexBuffer;
}

/** PE Resource Types */
char *ResTypes[] =
      {"niks", "CURSOR", "BITMAP", "ICON", "MENU", "DIALOG", "STRING",
       "FONTDIR", "FONT", "ACCELERATOR", "RCDATA",  "MESSAGETABLE",
       "GROUP_CURSOR", "niks", "GROUP_ICON", "niks", "VERSION"};



#ifdef RING0_DEBUG_IN_RING3

    #define _ldrRead __ldrRead

    /** Ring3 repacement for the os2krnls _ldrRead function*/
    ULONG __ldrRead(
        SFN   hFile,
        ULONG ulOffset,
        PVOID pBuffer,
        ULONG ulFlags ,
        ULONG ulBytesToRead,
        PMTE  pMTE
        )
    {
        ULONG ulRead, ulMoved;
        APIRET rc;

        rc = DosSetFilePtr(hFile, ulOffset, FILE_BEGIN, &ulMoved);
        if (rc == NO_ERROR)
            rc = DosRead(hFile, pBuffer, ulBytesToRead, &ulRead);
        else
            cout << "DosSetFilePtr(hfile, " << ulOffset << ",..,..) failed with rc = " << rc << endl;
        return rc;
    }

#endif



/**
 * Constructor.
 */
LXFile::LXFile()
{
    cout << "LXFile::LXFile()" << endl;
    inited = 0;
    hFile = 0;
    nSections = 0;
    pObjectTable = NULL;
    pObjectPageTable = NULL;
    pFixupTable = NULL;
    pFixupRecordTable = NULL;

    //fixup stuff:
    fCrossed = FALSE;
    crossedpageaddress = 0;

    lastord = 0;
   fFirstIcon = FALSE;
}



/**
 * Destructor
 */
LXFile::~LXFile()
{
    cout << "LXFile::~LXFile()" << endl;
    if (pFixupTable)       free (pFixupTable);
    if (pFixupRecordTable) free (pFixupRecordTable);
    if (pObjectTable)      free (pObjectTable);
    if (pObjectPageTable)  free (pObjectPageTable);
}



/**
 * Initiating conversion of the PE file.
 * @param  hFile    Handle(SystemFileNumber) to the PE-file.
 * @param  pHdr     Pointer to the PE signature(if any) and the following 196 (size of a LX-header).
 * @param  PEOffset Offset, into the PE-file, of the PE signature. (That is offset of the data pHdr points to.)
 * @return Boolean. True if successful (converting of PE) else False.
 * @remark Should only be called one time! Object should not be used if init(...) failes!
 */
BOOL LXFile::init(SFN hFile, PIMAGE_SIG_FILE_HEADER pHdr, ULONG PEOffset)
{
    ULONG rc;
    PIMAGE_SECTION_HEADER pSectionTable = NULL;
    int i,j;
    char *pszSectionName = (char*)malloc(IMAGE_SIZEOF_SHORT_NAME + 1);

    cout << "LXFile::init!" << endl;

    /* validate file header */
    if(!isPEFile(pHdr))
    {
        kprintf(("LXFile::init: Sorry. Not a pe file.\n"));
        return 0;
    }

    /* ok copy file handler, offset and pHdr*/
    LXFile::hFile = hFile;
    ulPEOffset = PEOffset;
    memcpy(&PEHdrs.Signature, &pHdr->Signature, sizeof(DWORD) );
    memcpy(&PEHdrs.FileHeader, &pHdr->FileHeader, sizeof(IMAGE_FILE_HEADER) );

    /* Exe or DLL? */
    if(PEHdrs.FileHeader.Characteristics & IMAGE_FILE_DLL)
        SetExeType(FALSE);
    else
        SetExeType(TRUE);

    /* Relocations ? */
    if(PEHdrs.FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
    {
        cout << "No fixups, might not run - yet!" << endl;
        SetNoFixups();
    }

    /* Read optional header */
    rc = _ldrRead(
        hFile,
        PEOffset + IMAGE_SIZEOF_FILE_HEADER + sizeof(DWORD)/*(sizeof signature)*/,
        &PEHdrs.OptionalHeader,
        0,
        sizeof(IMAGE_OPTIONAL_HEADER),
        NULL
        );

    if (rc != NO_ERROR)
    {
        kprintf(("Oops - error while reading optional header  rc=%d \n", rc));
        return FALSE;
    }

    if (PEHdrs.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
    {
        kprintf(("Oops - incorrect Magic word in optional header.\n"));
        return FALSE;
    }


    cout << "PE Optional header: " << endl;
    cout << "Preferred address : " << hex(PEHdrs.OptionalHeader.ImageBase) << endl;
    cout << "Base Of Code      : " << hex(PEHdrs.OptionalHeader.BaseOfCode) << endl;
    cout << "CodeSize          : " << hex(PEHdrs.OptionalHeader.SizeOfCode) << endl;
    cout << "Base Of Data      : " << hex(PEHdrs.OptionalHeader.BaseOfData) << endl;
    cout << "Data Size (uninit): " << hex(PEHdrs.OptionalHeader.SizeOfUninitializedData) << endl;
    cout << "Data Size (init)  : " << hex(PEHdrs.OptionalHeader.SizeOfInitializedData) << endl;
    cout << "Entry Point       : " << hex(PEHdrs.OptionalHeader.AddressOfEntryPoint) << endl;
    cout << "Section Alignment : " << hex(PEHdrs.OptionalHeader.SectionAlignment) << endl;
    cout << "Stack Reserve size: " << hex(PEHdrs.OptionalHeader.SizeOfStackReserve) << endl;
    cout << "Stack Commit size : " << hex(PEHdrs.OptionalHeader.SizeOfStackCommit) << endl;
    cout << "SizeOfHeapReserve : " << hex(PEHdrs.OptionalHeader.SizeOfHeapReserve) << endl;
    cout << "SizeOfHeapCommit  : " << hex(PEHdrs.OptionalHeader.SizeOfHeapCommit) << endl;
    cout << "FileAlignment     : " << hex(PEHdrs.OptionalHeader.FileAlignment) << endl;

    /* Module Type GUI/console (SetExeType must have been called before!) */
    if (PEHdrs.OptionalHeader.Subsystem != IMAGE_SUBSYSTEM_WINDOWS_GUI)
    {
        if (PEHdrs.OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
        {
            SetModuleType(SYSTEM_CHARACTER);
            //cout << "Console app" << endl;
        }
        else
        {
            cout << "Sorry subsystem ";
            switch (PEHdrs.OptionalHeader.Subsystem)
            {
                case IMAGE_SUBSYSTEM_NATIVE:
                    cout << "native";
                    break;
                case IMAGE_SUBSYSTEM_OS2_CUI:
                    cout << "os2 cui";
                    break;
                case IMAGE_SUBSYSTEM_POSIX_CUI:
                    cout << "posix";
                    break;
                default:
                    cout << "unknown" << PEHdrs.OptionalHeader.Subsystem;
            }
            cout << " is not supported - yet." << endl;
            return FALSE;
        }
    }
    else
    {
        SetModuleType(SYSTEM_GUI);
        //cout << "GUI app" << endl;
    }

    /* StackSize */
    SetStackSize(max(PEHdrs.OptionalHeader.SizeOfStackCommit, PEHdrs.OptionalHeader.SizeOfStackReserve));


    /* Sections */
    nSections = PEHdrs.FileHeader.NumberOfSections;
    ltassert( SetNrSections(nSections));

    /* read read section Table */
    pSectionTable = (PIMAGE_SECTION_HEADER)malloc(nSections*sizeof(IMAGE_SECTION_HEADER));  /* don't forget to free pSectionTable! */
    ltassert(pSectionTable != NULL);

    rc = _ldrRead(
        hFile,
        ulPEOffset+sizeof(IMAGE_NT_HEADERS),
        pSectionTable,
        0,
        nSections*sizeof(IMAGE_SECTION_HEADER),
        NULL
        );

    if (rc != NO_ERROR)
    {
        cout << endl << "Oops! error while reading section table, rc" << rc << endl;
        free(pSectionTable);
        return FALSE;
    }

    /* Create TIBSection ?! */
    if (!(options.noExeTIBFix && IsEXE))
        ltassert(CreateTIBSection(&PEHdrs.OptionalHeader));

    /* iterate thru the sections  */
    cout << endl;
    for (i = 0; i < nSections; i++)
    {
        memset(pszSectionName,0,9);
        strncpy(pszSectionName, pSectionTable[i].Name, IMAGE_SIZEOF_SHORT_NAME);

        cout << "Section Name:        " << pszSectionName << endl;
        cout << "Raw data size:       " << hex(pSectionTable[i].SizeOfRawData) << endl;
        cout << "Virtual Address:     " << hex(pSectionTable[i].VirtualAddress) << endl;
        cout << "Virtual Size:        " << hex(pSectionTable[i].Misc.VirtualSize) << endl;
        cout << "Pointer to raw data: " << hex(pSectionTable[i].PointerToRawData) << endl;
        cout << "Section flags:       " << hex(pSectionTable[i].Characteristics) << endl;
        /* ignore .reloc */
        if (strcmp(pszSectionName, ".reloc") == 0) {
            cout << ".reloc" << endl << endl;
            continue;
        }
        /* ignore .edata - later */
        if (strcmp(pszSectionName, ".edata") == 0) {
            cout << ".edata" << endl << endl;
            continue;
        }
        /* ignore .pdata - what is in this section?*/
        if (strcmp(pszSectionName, ".pdata") == 0) {
            cout << ".pdata" << endl << endl;
            continue;
        }
        /* ignore .rsrc - later */
        if (strcmp(pszSectionName, ".rsrc") == 0) {
            cout << ".rsrc" << endl << endl;
            continue;
        }
        /* ignore .debug */
        if (strcmp(pszSectionName, ".debug") == 0) {
            cout << ".rdebug" << endl << endl;
            continue;
        }

        /* import */
        if (strcmp(pszSectionName, ".idata") == 0 || PEIsImportDataSection(&pSectionTable[i]))
        {
            int type = SECTION_IMPORT;

            cout << "Import Data Section" << endl << endl;
            if(pSectionTable[i].Characteristics & IMAGE_SCN_CNT_CODE)
            {
                cout << "Also Code Section" << endl << endl;
                type |= SECTION_CODE;
            }
            StoreSection(
                pSectionTable[i].PointerToRawData,
                pSectionTable[i].SizeOfRawData,
                pSectionTable[i].Misc.VirtualSize,
                pSectionTable[i].VirtualAddress + PEHdrs.OptionalHeader.ImageBase,
                type);
            continue;
        }
        /* code section */
        if ((pSectionTable[i].Characteristics & IMAGE_SCN_CNT_CODE)
         //KSO Sun 1998-08-09: Borland does not alway set the CODE flag for its "CODE" section (TIB fix need to have the first section marked as code)
         || (pSectionTable[i].Characteristics & IMAGE_SCN_MEM_EXECUTE &&
            !(pSectionTable[i].Characteristics & (IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_CNT_INITIALIZED_DATA)) //KSO: make sure its not marked as a datasection
            )
         )
        {
            cout << "Code Section" << endl << endl;
            StoreSection(
                pSectionTable[i].PointerToRawData,
                pSectionTable[i].SizeOfRawData,
                pSectionTable[i].Misc.VirtualSize,
                pSectionTable[i].VirtualAddress + PEHdrs.OptionalHeader.ImageBase,
                SECTION_CODE);
            continue;
        }
        /* readonly data */
        if(!(pSectionTable[i].Characteristics & IMAGE_SCN_MEM_WRITE))  //read only data section
        {
            cout << "Read Only Data Section" << endl << endl;
            StoreSection(
                pSectionTable[i].PointerToRawData,
                pSectionTable[i].SizeOfRawData,
                pSectionTable[i].Misc.VirtualSize,
                pSectionTable[i].VirtualAddress + PEHdrs.OptionalHeader.ImageBase,
                SECTION_READONLYDATA);
            continue;
        }
        /* uninit. data */
        if(pSectionTable[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
        {
            cout << "Uninitialized Data Section" << endl << endl;
            StoreSection(
                pSectionTable[i].PointerToRawData,
                pSectionTable[i].SizeOfRawData,
                pSectionTable[i].Misc.VirtualSize,
                pSectionTable[i].VirtualAddress + PEHdrs.OptionalHeader.ImageBase,
                SECTION_UNINITDATA);
            continue;
        }
        /* init. data */
        if(pSectionTable[i].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
        {
            cout << "Initialized Data Section" << endl << endl;
            StoreSection(
                pSectionTable[i].PointerToRawData,
                pSectionTable[i].SizeOfRawData,
                pSectionTable[i].Misc.VirtualSize,
                pSectionTable[i].VirtualAddress + PEHdrs.OptionalHeader.ImageBase,
                SECTION_INITDATA);
            continue;
        }
        /* other  */
        if(pSectionTable[i].Characteristics & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ))
        {
            cout << "Other Section, stored as read/write uninit data" << endl << endl;
            StoreSection(
                pSectionTable[i].PointerToRawData,
                pSectionTable[i].SizeOfRawData,
                pSectionTable[i].Misc.VirtualSize,
                pSectionTable[i].VirtualAddress + PEHdrs.OptionalHeader.ImageBase,
                SECTION_UNINITDATA);
            continue;
        }
    }

    /*SvL: Align and/or concatenate code & data sections*/
   if (options.AlignSections)
        if (!AlignSections(&PEHdrs.OptionalHeader))
        {
            free(pSectionTable);
            return FALSE;
        }

   if (options.noExeTIBFix && IsEXE)
   {
      /* here we set the entrypoint for EXEs which don't have a TIBFix */
      ltasserthp2(SetEntryAddress(PEHdrs.OptionalHeader.AddressOfEntryPoint + PEHdrs.OptionalHeader.ImageBase), free(pSectionTable));
   }

    Yield();

    /* BASE RELOCATIONS */
    if (!ProcessFixups(pSectionTable))
    {
        free(pSectionTable);
        return FALSE;
    }

    Yield();

    /* EXPORTS */
    if (!ProcessExports(pSectionTable))
    {
        free(pSectionTable);
        return FALSE;
    }

    Yield();

    /* IMPORTS */
    if (!ProcessImports(pSectionTable))
    {
        free(pSectionTable);
        return FALSE;
    }

    Yield();

    /* RESOURCES */
    if (!ProcessResources(pSectionTable))
    {
        free(pSectionTable);
        return FALSE;
    }

    Yield();

    /* processing PE file is finished */
    /* no use for SectionTable any more - free the space */
    free(pSectionTable);
    pSectionTable = NULL;

    /* build LX file */
   rc =  PrepareLX();

    Yield();

    return rc;
}



/**
 * Build the virtual LX - file.
 *  @return True on success. False on failiure - object is then unusable.
 */
BOOL LXFile::PrepareLX()
{
    int rc;

    realintfixup  *currealrec;
    intfixup      *frec;
    namefixup     *imprec;

    int pagetblidx, i, j, k, idx, rawsize, objectsize;
    int nrpages, pagedataoffset;
    //BOOL fFirstCode = FALSE;
    int objcnt;



    //*************************************/
    //* Object Table && Object Page Table */
    //*************************************/
    pagetblidx            =  1;  //start met 1
    pagedataoffset        =  0;
    nrpages               =  GetNrPages();

    LXHdr.e32_objcnt      =  GetNrObjects2(); //NB! All Objects
    LXHdr.e32_mpages      =  0;

    pObjectTable             =  (struct o32_obj*)malloc((LXHdr.e32_objcnt)*sizeof(o32_obj));
    ltassert(pObjectTable != NULL);
    memset(pObjectTable,0,LXHdr.e32_objcnt*sizeof(o32_obj));

    //Stack is not contained within the "Object Page Table". [KSO Jun 24 1998 5:43am]
    //pObjectPageTable      =  (struct o32_map*)malloc((nrpages-(StackSize+0xFFF>>PAGE_SHIFT))*sizeof(o32_map));
    pObjectPageTable      =  (struct o32_map*)malloc(nrpages*sizeof(o32_map));
    ltassert(pObjectPageTable != NULL);
    //pObjectPageTable      =  (struct o32_map*)malloc((nrpages)*sizeof(o32_map));

    idx                   = -1;
    objcnt                =  1;

    /* section -> object */
    for ( i = 0; i < nrsections; i++)
    {
        pObjectTable[objcnt-1].o32_reserved =  0;
        pObjectTable[objcnt-1].o32_pagemap  =  pagetblidx;
        if (PESection[i].fInvalid)
            continue;

        switch (PESection[i].type)
        {
            case (SECTION_CODE | SECTION_TIBFIX): //KSO Aug 16 1998: TIBFixObject
            case (SECTION_CODE | SECTION_IMPORT):
            case SECTION_CODE:
                pObjectTable[objcnt-1].o32_flags = OBJREAD | NSEXRD | OBJBIGDEF;
                break;
            case SECTION_IMPORT:
                pObjectTable[objcnt-1].o32_flags = OBJREAD | OBJBIGDEF;
                break;
            case SECTION_INITDATA:
                pObjectTable[objcnt-1].o32_flags = OBJREAD | OBJWRITE | OBJBIGDEF;
                break;
            case SECTION_COMBINEDDATA:
                pObjectTable[objcnt-1].o32_flags = OBJREAD | OBJWRITE | OBJBIGDEF;
                break;
            case SECTION_RESOURCE_ORG:
            case SECTION_UNINITDATA:
                //SvL: Zero filled flag maps on Permanent flag, so ditch it
                pObjectTable[objcnt-1].o32_flags = OBJREAD | OBJWRITE | OBJBIGDEF; //read/write/big
                break;
            case SECTION_READONLYDATA:
                pObjectTable[objcnt-1].o32_flags = OBJREAD | OBJBIGDEF;
                break;
        }
        //OS/2 doesn't seem to mind if the sizes of all the pages in an object
        //exceeds the object size....
        if (PESection[i].type == SECTION_UNINITDATA)
            objectsize = PESection[i].virtualsize;  //rawsize == 0
        else
            objectsize = PESection[i].rawsize;

        pObjectTable[objcnt-1].o32_mapsize  = PESection[i].nrpages;
        pObjectTable[objcnt-1].o32_mapsize += PESection[i].nrinvalidpages;
        pObjectTable[objcnt-1].o32_base     = PESection[i].address;
        pObjectTable[objcnt-1].o32_size     = pObjectTable[objcnt-1].o32_mapsize*PAGE_SIZE;

        rawsize = PESection[i].rawsize;
        for ( j = 0; j < pObjectTable[objcnt-1].o32_mapsize; j++)
        {
            idx = pagetblidx+j-1;
            pObjectPageTable[idx].o32_pagedataoffset = pagedataoffset;

            if (j < PESection[i].nrinvalidpages)
            {
                pObjectPageTable[idx].o32_pagesize       = 0;
                //SvL: Invalid pages are not properly handled by the OS/2 loader -> make 'em zeroed
                pObjectPageTable[idx].o32_pageflags      = ZEROED;
            }
            else
            {
                if (PESection[i].type == SECTION_COMBINEDDATA)
                {
                    //KSO Aug 16 1998: TIBFixObject - some code removed
                    int didx = j - PESection[i].nrinvalidpages;
                    pObjectPageTable[idx].o32_pagesize  = datapage[didx].size;
                    pObjectPageTable[idx].o32_pageflags = datapage[didx].flags;
                    pagedataoffset += pObjectPageTable[idx].o32_pagesize;
                    if (pObjectPageTable[idx].o32_pageflags == 0)
                        LXHdr.e32_mpages++;
                }
                else
                    if (PESection[i].type == SECTION_UNINITDATA || rawsize == 0)
                {
                    pObjectPageTable[idx].o32_pagesize       = 0;
                    pObjectPageTable[idx].o32_pagedataoffset = pagedataoffset;
                    pObjectPageTable[idx].o32_pageflags      = ZEROED;
                }
                else //KSO Aug 16 1998: no more TIBFix code to think about
                {
                    if (rawsize > PAGE_SIZE)
                        pObjectPageTable[idx].o32_pagesize = PAGE_SIZE;
                    else
                        pObjectPageTable[idx].o32_pagesize = rawsize;
                    rawsize                            -= pObjectPageTable[idx].o32_pagesize;
                    pObjectPageTable[idx].o32_pageflags = 0;
                    LXHdr.e32_mpages++;
                    pagedataoffset                     += pObjectPageTable[idx].o32_pagesize;
                }
            }
        }

        if (IsEXE && i == nrsections - 1)
        {//SvL: 25-7-'97 Add stack pages to last data object
            LXHdr.e32_stackobj     = objcnt;
            LXHdr.e32_autodata     = LXHdr.e32_stackobj;
            pObjectTable[objcnt-1].o32_flags    |= OBJWRITE;
            pObjectTable[objcnt-1].o32_size     += StackSize;
            LXHdr.e32_esp          = pObjectTable[objcnt-1].o32_size; //highest address in object
        }

        pagetblidx += pObjectTable[objcnt-1].o32_mapsize;
        objcnt++;
    }//for("nrsections")

    /* fix entry for TIBFixObject */
    if (!(options.noExeTIBFix && IsEXE))
    {
      int iPESection     = GetTIBSectionIdx(); //
        LXHdr.e32_eip      = 0;
        LXHdr.e32_startobj = GetTIBObjectNo();
        ltassert(LXHdr.e32_startobj != -1 && iPESection!= -1);
        TIBEntry = PEHdrs.OptionalHeader.AddressOfEntryPoint //RVA
            + PEHdrs.OptionalHeader.ImageBase
            - (PESection[iPESection].address + TIBOffCall + SIZE_TIBCALL);
    }
    else
    {
        /* no TIB stuff at all */
        SetEntryAddress(PEHdrs.OptionalHeader.AddressOfEntryPoint + PEHdrs.OptionalHeader.ImageBase);
    }



    //***********************************************/
    //* Resource Table && Object Page Table (cont.) */
    //***********************************************/
    if (nrresources)
    {
        //Correct count in case there are string tables which aren't completely filled
        nrresources = curresource;
        //Objects are ALL counted! //LXHdr.e32_objcnt++;
        pObjectTable[objcnt-1].o32_pagemap   = pagetblidx;
        pObjectTable[objcnt-1].o32_flags     = NSSHARED | OBJREAD | OBJRSRC | OBJBIGDEF;
        pObjectTable[objcnt-1].o32_mapsize   = ResSection.rawsize >> PAGE_SHIFT;

        pObjectTable[objcnt-1].o32_base      = 0;   //SvL: Set to 0 in PM exes (plf)
        pObjectTable[objcnt-1].o32_size      = ResSection.rawsize;

        for ( j = 0; j < pObjectTable[objcnt-1].o32_mapsize; j++)
        {
            idx = pagetblidx + j - 1;
            pObjectPageTable[idx].o32_pagedataoffset = pagedataoffset;
            pObjectPageTable[idx].o32_pagesize       = PAGE_SIZE;
            pObjectPageTable[idx].o32_pageflags      = 0;
            LXHdr.e32_mpages++;
            pagedataoffset                          += PAGE_SIZE;
        }
        if ((pObjectTable[objcnt-1].o32_size & 0xFFF) != 0)
        {
            pObjectTable[objcnt-1].o32_mapsize++;
            LXHdr.e32_mpages++;
            idx++;
            pObjectPageTable[idx].o32_pagedataoffset = pagedataoffset;
            pObjectPageTable[idx].o32_pagesize       = (USHORT)(pObjectTable[objcnt-1].o32_size & 0xFFF);
            pObjectPageTable[idx].o32_pageflags      = 0;
            pagedataoffset                          += pObjectPageTable[idx].o32_pagesize;
        }

        pagetblidx  += pObjectTable[objcnt-1].o32_mapsize;

        //fill in object nr's in resource table entries
        for ( i = 0; i < nrresources; i++)
        {
            os2resource[i].obj = LXHdr.e32_objcnt;
        }
    }// if (nrresources)

    /* Object Page Table finished */
    //object page records offset:
    LXHdr.e32_objmap    = LXHdr.e32_objtab + LXHdr.e32_objcnt*sizeof(pObjectTable[objcnt-1]);
    //size tables vanaf object table t/m impproc (eigenlijk pagesum ook..)
    LXHdr.e32_ldrsize   = LXHdr.e32_objcnt*sizeof(pObjectTable[objcnt-1]);
    //LXHdr.e32_ldrsize  += (nrpages-(StackSize+0xFFF>>PAGE_SHIFT))*sizeof(o32_map);
    LXHdr.e32_ldrsize  += nrpages*sizeof(o32_map);

    //resource table:
    LXHdr.e32_rsrctab   = LXHdr.e32_ldrsize + LXHdr.e32_objtab;
    LXHdr.e32_rsrccnt   = nrresources;
    LXHdr.e32_ldrsize  += nrresources*sizeof(rsrc32);
    LXHdr.e32_restab    = LXHdr.e32_rsrctab + nrresources*sizeof(rsrc32);



    //**************************************/
    //* Resident Name Table && Entry Table */
    //**************************************/
   #if 1 //KSO Aug 16 1998: exports from exes and dll without exports: TODO cleanup!
   if (exports)
   {
        //write DLL export information
        int len = 0;

        //save resident name table
        if (exports)
        {
            len = curexport - exports + 1;  // + 0 byte to signal end of table
            LXHdr.e32_ldrsize += len;
        }

        //save entry table
        LXHdr.e32_enttab = LXHdr.e32_restab + len;
        //TODO: Forwarders!
        len = (int)curexpb - (int)expbundle + 1; // + 0 byte to signal end of table
        if (len > 1)
            LXHdr.e32_ldrsize += len;
   }
   else
    {
        int modlen;
      //save resident name table
        //KSO: modname has right format - Jun 16 1998 4:27pm
        modlen = (char)modulename[0];
        modlen += 1 + 2 + 1; //len byte + ord word + 0 byte

        //save entry table
        LXHdr.e32_enttab    = LXHdr.e32_restab + modlen; //points to 0 byte
        LXHdr.e32_ldrsize  += 1 + modlen;
    }

    #else

    if (IsEXE == TRUE)
    {
        int modlen;
        //save resident name table
        //KSO: modname has right format - Jun 16 1998 4:27pm
        modlen = (char)modulename[0];
        modlen += 1 + 2 + 1; //len byte + ord word + 0 byte

        //save entry table
        LXHdr.e32_enttab    = LXHdr.e32_restab + modlen; //points to 0 byte
        LXHdr.e32_ldrsize  += 1 + modlen;
    }
    else
    {
        //write DLL export information
        int len = 0;

        //save resident name table
        if (exports)
        {
            len = curexport - exports + 1;  // + 0 byte to signal end of table
            LXHdr.e32_ldrsize += len;
        }

        //save entry table
        LXHdr.e32_enttab = LXHdr.e32_restab + len;
        //TODO: Forwarders!
        len = (int)curexpb - (int)expbundle + 1; // + 0 byte to signal end of table
        if (len > 1)
            LXHdr.e32_ldrsize += len;
    }
    #endif


    Yield();


    //********************/
    //* Fixup Page Table */
    //********************/
    //#pages * offset naar import fixups + 1 eind offset
    LXHdr.e32_fpagetab = LXHdr.e32_ldrsize  + LXHdr.e32_objtab;
    LXHdr.e32_frectab  = LXHdr.e32_fpagetab + (nrpages + 1) * sizeof(int);
    pFixupTable        = (int*)malloc((nrpages + 1) * sizeof(int));
    ltassert(pFixupTable != NULL)

    for ( i = 0; i <= nrpages; i++)
        pFixupTable[i] = 0;

    //dump alle fixup records in 1 memory block en bepaal offsets in fixup page table
    frec              = &intfixuprec[0];
    imprec            = &impfixuprec[0];
    pFixupRecordTable = (realintfixup*)malloc(
          (nrintfixups + 1)            * sizeof(realintfixup) //KSO Aug 16 1998: "+1": internal entrypointfixup
        + (nrimpfixups + EXTRA_FIXUPS) * sizeof(realnamefixup)
        );
    ltassert2(pFixupRecordTable != NULL, free(pFixupTable);pFixupTable = NULL);

    /* KSO Aug 16 1998: we need the pageno of the TIBSection */
    int TIBPageNo = GetTIBSectionIdx();
    ltassert(TIBPageNo != -1);
    TIBPageNo = GetPageNoByVA(PESection[TIBPageNo].address);

    currealrec = pFixupRecordTable;
    j          = 0;
    k          = 0;
    for (i = 0; i < nrpages; i++)
    {
        pFixupTable[i] = (int)currealrec - (int)pFixupRecordTable;
        if (i == TIBPageNo && !(IsEXE && options.noExeTIBFix))
        {
            //add kernel32:registerresourceinfo import
            realordfixup *kerord = (realordfixup *)currealrec;
            kerord->nr_stype   = 0x07;           //32 bits offset
            kerord->nr_flags   = 0x01 | 0x40;   //import by ordinal + mod offset = 16 bits
            kerord->r32_soff   = TIBOffKerImport;
            kerord->r32_objmod = kernel32Object + 1;
            if (IsEXE)
                kerord->ord     = ORD_REGISTERRESOURCEINFO;
            else
                kerord->ord      = ORD_REGISTERDLL;

            currealrec = (realintfixup *)((int)currealrec + sizeof(realnamefixup));
            ltasserthp(currealrec); //pFixupTable & pFixupRecordTable will be freed by the destructor
            if (!IsEXE)
            {
                kerord = (realordfixup *)currealrec;
                kerord->nr_stype   = 0x08;           //32 bits self referencing offset
                kerord->nr_flags   = 0x01 | 0x40;   //import by ordinal + mod offset = 16 bits
                kerord->r32_soff   = DLL_OFF_KERIMP2;
                kerord->r32_objmod = kernel32Object + 1;
                kerord->ord        = ORD_KERNEL32DLLEXITLIST;
                currealrec = (realintfixup *)((int)currealrec + sizeof(realnamefixup));
            }

            /* add entrypoint internal fixup */
            realintfixup *pEntInt = currealrec;
            pEntInt->nr_stype   = 0x08; //self referencing
            pEntInt->nr_flags   = 0x00 | 0x10 | 0x40;
            pEntInt->r32_soff   = TIBOffEntry;
            pEntInt->targetaddr = PEHdrs.OptionalHeader.ImageBase + PEHdrs.OptionalHeader.AddressOfEntryPoint;
            pEntInt->targetobj  = GetObjectNoByVA(pEntInt->targetaddr);
            ltassert(pEntInt->targetobj != -1);
            pEntInt->targetaddr -= pObjectTable[pEntInt->targetobj - 1].o32_base;
            currealrec++;
        }// if (i == 0)

        //SvL: 16-9-'97, for multiple import pages
        while (k < nrimpfixups && imprec->srcpage == i)
        {
            //sizeof(realordfixup) == sizeof(realnamefixup)
            memcpy(currealrec, imprec, sizeof(realnamefixup));
            currealrec = (realintfixup *)((int)currealrec + sizeof(realnamefixup));
            ltasserthp(currealrec); //pFixupTable & pFixupRecordTable will be freed by the destructor
            imprec++;
            ltasserthp(imprec); //pFixupTable & pFixupRecordTable will be freed by the destructor
            k++;
        }
        while (j < nrintfixups && frec->srcpage == i)
        {
            memcpy(currealrec, frec, sizeof(realintfixup));
            //  cout << "src address " << frec->r32_soff << " srcpage " << frec->srcpage << " target " << frec->targetaddr << " frec " << (int)frec << endl;
            frec++;
            ltasserthp(frec); //pFixupTable & pFixupRecordTable will be freed by the destructor
            currealrec++;
            ltasserthp(currealrec); //pFixupTable & pFixupRecordTable will be freed by the destructor
            j++;
        }

    }//for
    pFixupTable[nrpages] = (int)currealrec - (int)pFixupRecordTable;

    //Add extra kernel32 imports and evt. internal entrypoint-fixup
    if (IsEXE)
        nrimpfixups++;
    else
        nrimpfixups += EXTRA_FIXUPS;
    nrintfixups += 1; //KSO Aug 16 1998: internal entrypoint fixup

    /* impfixuprec and intfixuprec are not needed any more */
    if (impfixuprec != NULL)
    {
        free(impfixuprec);
        impfixuprec = NULL;
    }
    if (intfixuprec != NULL)
    {
        free(intfixuprec);
        intfixuprec = NULL;
    }

    //fixup page + record + import name + module table size
    LXHdr.e32_fixupsize = (nrpages+1)*sizeof(int)
        + (ULONG)currealrec - (ULONG)pFixupRecordTable
        + impmodulesize
        + impnamesize;

    //size tables vanaf object table t/m impproc (eigenlijk pagesum ook..)
    LXHdr.e32_ldrsize  += LXHdr.e32_fixupsize;

    //offset naar module name array
    LXHdr.e32_impmod    = LXHdr.e32_frectab + ((ULONG)currealrec - (ULONG)pFixupRecordTable);
    //offset naar proc name array
    LXHdr.e32_impproc   = LXHdr.e32_impmod + impmodulesize;


    Yield();


    //*************/
    //* Code/Data */
    //*************/
    //pointer naar begin code/data
    if (options.lxoffset)
        ulPEOffset = LXHEADER_OFFSET;

    LXHdr.e32_datapage    = LXHdr.e32_impproc + impnamesize + ulPEOffset; //relative to begin of file

    ULONG ulCurrentOffset = LXHdr.e32_datapage;
    for (i = 0; i < nrsections; i++)
    {
        //zero filled pages, niet nodig om op te slaan
        if (PESection[i].type != SECTION_UNINITDATA && PESection[i].rawdata)
        {
            PESection[i].lxoffset = ulCurrentOffset;
            ulCurrentOffset += PESection[i].rawsize;
        }
    }



    //*************/
    //* Resources */
    //*************/
    if (nrresources)
        ResSection.lxoffset = ulCurrentOffset;

    cout << "LXFile::PrepareLX is finished" << endl;
    inited = TRUE;

    #ifndef RELEASE
        //Lets have a look at how much mem is used by this module...
        if (options.fLogging)
            queryMemoryUsage();
    #endif

    return (TRUE);
}



/**
 * Process Fixups in a PE file.
 * @param  pSectionTable  Pointer to Section Table.
 *  @return TRUE on success. FALSE on failiure - object is then unusable.
 */
BOOL LXFile::ProcessFixups(PIMAGE_SECTION_HEADER pSectionTable)
{
    /* BASE RELOCATIONS */
    int i,j,rc;
    ULONG VARead;

    ULONG offRel = PEImageDirectoryOffset (pSectionTable, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    ltassert(SetNrOff32Fixups((offRel > 0) ? PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size/2 : 0)); //size/2 is approixmate... or?

    if ( offRel != 0 && PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size > 0)
    {
        PIMAGE_BASE_RELOCATION pRelocations, prel, end;
        int count;
        char *page;

        /* page currently processed */
        char *pPage = (char*)malloc(PAGE_SIZE * 2);
        ltassert(pPage != NULL);

        /* allocate memory for base reallocations */
        pRelocations = (PIMAGE_BASE_RELOCATION)malloc(PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);
        ltasserthp2(pRelocations, free(pPage));

        /* read them */
        rc = _ldrRead(
            hFile,
            offRel,
            pRelocations,
            0,
            PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size,
            NULL
            );
        if (rc != NO_ERROR)
        {
            cout << "Error while trying to read base relocatations. rc=" << rc << endl;
            free(pRelocations);
            free(pPage);
            return FALSE;
        }

        prel = pRelocations;
        end = (PIMAGE_BASE_RELOCATION)((ULONG)pRelocations + PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);
        j = 1;
        VARead = ~0;
        while (prel < end && prel->VirtualAddress)
        {
            AddOff32FinishCrossPageFixup();
            page = (char *)prel + (int)prel->VirtualAddress;
            count  = (prel->SizeOfBlock - 8)/2;
            cout.setf(ios::hex, ios::basefield);
            cout << "Page " << j << " Address " << (ULONG)prel->VirtualAddress << " Count " << count << endl;
            cout.setf(ios::dec, ios::basefield);

            /* page buffer */
            if (VARead == prel->VirtualAddress)
                memcpy(pPage, pPage+PAGE_SIZE, PAGE_SIZE);
            else
            {
                /*read it */
                rc = ReadPageByAddress(
                    PEHdrs.OptionalHeader.ImageBase + prel->VirtualAddress,
                    pPage
                    );

                if (rc != NO_ERROR && rc != -2)
                {
                    cout << "error reading the page. rc=" << rc << endl;
                    return 0;
                }
            }
            /* process all fixups for this page */
            for (i = 0; i < count; i++)
            {
                int type   = prel->TypeOffset[i] >> 12;
                int offset = prel->TypeOffset[i] & 0xFFF;
                switch (type)
                {
                    case IMAGE_REL_BASED_ABSOLUTE:
                        //cout << "absolute fixup; unused" << endl;
                        break;  //skip

                    case IMAGE_REL_BASED_HIGHLOW:
                        //cout << "address " << hex(offset) << " type " << type; // << endl;

                        /* check if this is an crosspage fixup */
                        if (offset > PAGE_SIZE-4)
                        {
                            /* read folowing page */
                            VARead = prel->VirtualAddress + PAGE_SIZE;
                            rc = ReadPageByAddress(
                                PEHdrs.OptionalHeader.ImageBase + VARead,
                                pPage + PAGE_SIZE
                                );
                            if ( rc != NO_ERROR && rc != -2 )
                            {
                                cout << "warning: could not read next page - cross page fixup have incorrect target address. rc=" << rc << endl;
                                VARead = ~0;
                                memset(pPage+PAGE_SIZE, 0, 4);
                            }
                        }
                        rc = AddOff32Fixup(PEHdrs.OptionalHeader.ImageBase + prel->VirtualAddress + offset, pPage);
                        //cout << endl;
                        break;

                    case IMAGE_REL_BASED_HIGH:
                    case IMAGE_REL_BASED_LOW:
                    case IMAGE_REL_BASED_HIGHADJ:
                    case IMAGE_REL_BASED_MIPS_JMPADDR:
                    default:
                        cout << "Unknown/unsupported fixup type!" << endl;
                        break;
                }//switch
            }//for

            /* check if the next page is the one we read */
            /* we must read ahead because of fixups that overlap pages */
            prel = (PIMAGE_BASE_RELOCATION)((ULONG)prel + prel->SizeOfBlock);
            j++;
        }//while
        AddOff32FinishCrossPageFixup(); //catch evt. crosspagefixup on the last page with fixup(s). This may work...
        free(pPage);
        free(pRelocations);
    }

    return TRUE;
}



/**
 * Process Exports in a PE file.
 *  @return True on success. False on failiure - object is then unusable.
 * @param  pSectionTable  Pointer to Section Table.
 */
BOOL LXFile::ProcessExports(PIMAGE_SECTION_HEADER pSectionTable)
{
    int rc, i;

    /* EXPORTS */
    /* get section header and pointer to data directory for .edata section */
    //if (PEHdrs.FileHeader.Characteristics & IMAGE_FILE_DLL) - this won't do for Borland exes!!
    //{
    PCHAR pExportSection       = NULL;
    PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
    PIMAGE_SECTION_HEADER pESH = PEGetSectionHdrByImageDir(pSectionTable, IMAGE_DIRECTORY_ENTRY_EXPORT);
    ULONG offExportDirectory   = PEImageDirectoryOffset   (pSectionTable, IMAGE_DIRECTORY_ENTRY_EXPORT);

    if ( offExportDirectory > 0 && pESH != NULL)
    {
        /* allocate memory for exportsection */
        pExportSection = (PCHAR)malloc(max(pESH->Misc.VirtualSize, pESH->SizeOfRawData));
        ltassert(pExportSection != NULL);

        /* read exportsection */
        rc = _ldrRead(
            hFile,
            pESH->PointerToRawData,
            pExportSection,
            0,
            max(pESH->Misc.VirtualSize, pESH->SizeOfRawData),
            NULL
            );
        if (rc != NO_ERROR)
        {
            cout << "Error while trying to read exportsection. rc=" << rc << endl;
            free(pExportSection);
            free(pSectionTable);
            return FALSE;
        }

        pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((ULONG)pExportSection +  (offExportDirectory - pESH->PointerToRawData)); //??
        ltasserthp2(pExportDirectory,free(pExportSection));


        int    *ptrNames;
        int    *ptrAddress;
        USHORT *ptrOrd;

        cout << "Exported Functions: " << endl;
        ptrOrd =(USHORT *)((ULONG)pExportDirectory->AddressOfNameOrdinals - (ULONG)pESH->VirtualAddress + (ULONG)pExportSection);
        ptrNames   = (int *)((int)pExportDirectory->AddressOfNames - (ULONG)pESH->VirtualAddress + (ULONG)pExportSection);
        ptrAddress = (int *)((int)pExportDirectory->AddressOfFunctions - (ULONG)pESH->VirtualAddress + (ULONG)pExportSection);
        ltasserthp2(ptrOrd,    free(pExportSection));
        ltasserthp2(ptrNames,  free(pExportSection));
        ltasserthp2(ptrAddress,free(pExportSection));
        ltassert2(SetNrExtFixups(max(pExportDirectory->NumberOfNames, pExportDirectory->NumberOfFunctions)), free(pExportSection));


        int     ord;
        int     RVAExport;
        char   *name;

        for ( i = 0; i < pExportDirectory->NumberOfNames; i++)
        {
            ord       = ptrOrd[i] + pExportDirectory->Base;
            name      = (char *)((ULONG)ptrNames[i] - (ULONG)pESH->VirtualAddress + (ULONG)pExportSection);
            ltasserthp2(name, free(pExportSection));

            RVAExport = ptrAddress[ptrOrd[i]];
            //      RVAExport = ptrAddress[i];
#ifdef FORWARDERS
            if (RVAExport < pESH->VirtualAddress || RVAExport > pESH->VirtualAddress + pESH->SizeOfRawData)
            {
#endif
                //points to code (virtual address relative to oh.ImageBase
                cout << "address 0x";
                cout.setf(ios::hex, ios::basefield);
                cout << RVAExport;
                cout.setf(ios::dec, ios::basefield);
                cout << " " << name << "@" << ord << endl;
                ltassert2(AddNameExport(PEHdrs.OptionalHeader.ImageBase + RVAExport, name, ord), free(pExportSection));
#ifdef FORWARDERS

            }
            else
            {//forwarder
                char *forward = (char *)((ULONG)RVAExport -
                                                 (ULONG)pESH->VirtualAddress +
                                                 (ULONG)pExportSection);
                ltasserthp2(forward, free(pExportSection));
                cout << RVAExport << " " << name << " @" << ord << " is forwarder to " << (int)forward << endl;
                //ltassert2(AddForwarder(name, ord, forward), free(pExportSection);
            }
#endif
        }//for

        for ( i = 0; i < max(pExportDirectory->NumberOfNames, pExportDirectory->NumberOfFunctions); i++)
        {
            ord = pExportDirectory->Base + i;    //Correct??
            RVAExport = ptrAddress[i];
#ifdef FORWARDERS
            if (RVAExport < pESH->VirtualAddress || RVAExport > pESH->VirtualAddress + pESH->SizeOfRawData)
            {
#endif
                if (RVAExport)
                {
                    //points to code (virtual address relative to oh.ImageBase
                    cout << "ord " << ord << " at 0x";
                    cout.setf(ios::hex, ios::basefield);
                    cout << RVAExport  << endl;
                    cout.setf(ios::dec, ios::basefield);
                    ltassert2(AddOrdExport(PEHdrs.OptionalHeader.ImageBase + RVAExport, ord), free(pExportSection));
                }
#ifdef FORWARDERS
            }
            else
            {//forwarder or empty
                char *forward = (char *)((int)RVAExport -
                                                 (int)pESH->VirtualAddress +
                                                 (ULONG)pExportSection);
                ltasserthp2(forward, free(pExportSection));
                cout << "ord " << ord << " at 0x";
                cout.setf(ios::hex, ios::basefield);
                cout << RVAExport << " is forwarder to 0x" << (int)forward << endl;
                cout.setf(ios::dec, ios::basefield);
                //ltassert2(AddForwarder(NULL, ord, forward), free(pExportSection));
            }
#endif
        }//for

        free(pExportSection);
    }


    /* realloc exports */
    if (exports != NULL)
    {
        char *pTmp;
        pTmp = (char*)realloc(exports, curexport - exports + 1);
        if(pTmp != NULL)
        {
            curexport += pTmp-exports;
            exports = pTmp;
        }
        else
            kprintf(("LXFile::PrepareLX:realloc(exports, curexport-exports+1 failed.\n"));
    }

    return TRUE;
}



/**
 * Process Imports in a PE file.
 * @param  pFile  Pointer to start of PE-file.
 *  @return True on success. False on failiure - object is then unusable.
 * @algo   1) get module names and store them<br>
 *           a) check dwRVAModuleName is within .idata seg - if not find section<br>
 *         2) iterate thru functions of each module<br>
 *           a) check OriginalFirstThunk is not 0 and that it points to a RVA.<br>
 *           b) if not a) borland-styled PE-file - ARG!!! check FirstThunk<br>
 *           c) check OriginalFirstThunk/FirstThunk ok RVAs and find right section<br>
 *           d) store ordinal/name import<br>
 *         3) finished<br>
 * @remark Should now detect most Borland styled files including the GifCon32.exe and
 *         loader32 from SoftIce. (Stupid Borland!!!)<br>
 *         KSO [Jul 22 1998 2:44am]<br>
 *        <I>This function has a sister function in (pe.cpp)ProcessImports - update always both</I>
 */
BOOL LXFile::ProcessImports(PIMAGE_SECTION_HEADER pSH)
{
    PIMAGE_IMPORT_DESCRIPTOR pID;
    PIMAGE_SECTION_HEADER    pshID;
    PCHAR                    pSectionID = NULL;
    PIMAGE_SECTION_HEADER    pshExtra = NULL;
    PCHAR                    pSectionExtra = NULL;
    PIMAGE_OPTIONAL_HEADER   pOH;

    int    i,j;
    BOOL   fBorland = 0;
    int    cModules;
    char  *pszModules;
    char  *pszCurModule;
    char  *pszTmp;
    ULONG *pulImport;
   ULONG  ulCurFixup;
    int    Size;


    /* 1) get module names */
    if ((pshID = PEGetSectionHdrByImageDir(pSH, IMAGE_DIRECTORY_ENTRY_IMPORT)) == NO_ERROR)
        return FALSE;

    pSectionID = (char*)PEReadSection(pshID);   //remeber to free this
    ltassert(pSectionID != NULL);

    pID = (PIMAGE_IMPORT_DESCRIPTOR)(
        (ULONG)pSectionID
        + PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
        - pshID->VirtualAddress
        );
    ltasserthp2(pID, free(pSectionID))

        //calc size of module list
    i = Size = cModules = 0;
    while (pID[i].Name != 0)
    {
            //test RVA inside ID-Section
        if (pID[i].Name >= pshID->VirtualAddress && pID[i].Name < pshID->VirtualAddress + max(pshID->Misc.VirtualSize, pshID->SizeOfRawData))
            pszTmp = (char*)(pID[i].Name - pshID->VirtualAddress + pSectionID);
        else
        {
            //is the "Extra"-section already found or do we have to find it?
            if (pshExtra == 0 || pID[i].Name < pshExtra->VirtualAddress || pID[i].Name >= pshExtra->VirtualAddress + max(pshExtra->Misc.VirtualSize, pshExtra->SizeOfRawData))
            {
                if ((pshExtra = PEGetSectionHdrByRVA(pSH, pID[i].Name)) != NULL)
                {
                    if (pSectionExtra != NULL) free(pSectionExtra);
                    if ((pSectionExtra = (char*)PEReadSection(pshExtra)) == NULL)
                    {
                        free(pSectionID);
                        return FALSE;
                    }
                }
                else
                {
                    free(pSectionID);
                    if (pSectionExtra != NULL) free(pSectionExtra);
                    return FALSE;
                }
            }
            pszTmp = (char*)(pID[i].Name - pshExtra->VirtualAddress + pSectionExtra);
        }
        Size += strlen(pszTmp) + 1;
        i++;
        cModules++;
    }

    pszModules = (char*)malloc(Size);
    assert(pszModules != NULL);
    j = 0;
    for (i = 0; i < cModules; i++)
    {
            //test RVA inside ID-Section
        if (pID[i].Name >= pshID->VirtualAddress && pID[i].Name < pshID->VirtualAddress + max(pshID->Misc.VirtualSize, pshID->SizeOfRawData))
            pszTmp = (char*)(pID[i].Name - pshID->VirtualAddress + pSectionID);
        else
        {
            //is the "Extra"-section already found or do we have to find it?
            if (pshExtra == 0 || pID[i].Name < pshExtra->VirtualAddress || pID[i].Name >= pshExtra->VirtualAddress + max(pshExtra->Misc.VirtualSize, pshExtra->SizeOfRawData))
            {
                if ((pshExtra = PEGetSectionHdrByRVA(pSH, pID[i].Name)) != NULL)
                {
                    if (pSectionExtra != NULL) free(pSectionExtra);
                    if ((pSectionExtra = (char*)PEReadSection(pshExtra)) == NULL)
                    {
                        free(pSectionID);
                        return FALSE;
                    }
                }
                else
                {
                    free(pSectionID);
                    if (pSectionExtra != NULL) free(pSectionExtra);
                    return FALSE;
                }
            }
            pszTmp = (char*)(pID[i].Name - pshExtra->VirtualAddress + pSectionExtra);
        }
        strcpy(pszModules+j, pszTmp);
        j += strlen(pszTmp) + 1;
    }
    cout << endl;
    if (fBorland)
        cout << "Borland-styled PE-File." << endl;
        //Store modules
    cout << cModules << " imported Modules: " << endl;
    StoreImportModules(pszModules, cModules);


    /* 2) functions */
    pszCurModule = pszModules;
    pOH = &PEHdrs.OptionalHeader;
    for (i = 0; i < cModules; i++)
    {
        cout << "Module " << pszCurModule << endl;
        //  a) check that OriginalFirstThunk not is 0 and look for Borland-styled PE
        if (i == 0)
        {
            //heavy borland-style test - assume array of thunks is within that style does not change
            if ((ULONG)pID[i].u.OriginalFirstThunk == 0
                ////|| (ULONG)pID[i].OriginalFirstThunk < pOH->SizeOfImage
                || (ULONG)pID[i].u.OriginalFirstThunk < pshID->VirtualAddress
                || (ULONG)pID[i].u.OriginalFirstThunk >= pshID->VirtualAddress + max(pshID->Misc.VirtualSize, pshID->SizeOfRawData)
                || (ULONG)pID[i].u.OriginalFirstThunk >= pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
                   && (ULONG)pID[i].u.OriginalFirstThunk < sizeof(*pID)*cModules + pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
                //   && (ULONG)pID[i].OriginalFirstThunk < pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size + pOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
                )
                fBorland = TRUE;
        }

        //light borland-style test
        if (pID[i].u.OriginalFirstThunk == 0 || fBorland)
            pulImport = (ULONG*)pID[i].FirstThunk;
        else
            pulImport = (ULONG*)pID[i].u.OriginalFirstThunk;

        //  b) check if RVA ok
        if (!(pulImport > 0 && (ULONG)pulImport < pOH->SizeOfImage))
        {
            cout << "Invalid RVA " << hex((ULONG)pulImport) << endl;
            break;
        }

        // check section
        if ((ULONG)pulImport < pshID->VirtualAddress || (ULONG)pulImport >= pshID->VirtualAddress + max(pshID->Misc.VirtualSize, pshID->SizeOfRawData))
        {
            if (pshExtra == 0 || (ULONG)pulImport < pshExtra->VirtualAddress || (ULONG)pulImport >= pshExtra->VirtualAddress + max(pshExtra->Misc.VirtualSize, pshExtra->SizeOfRawData))
            {
                if ((pshExtra = PEGetSectionHdrByRVA(pSH, (ULONG)pulImport)) != NULL)
                {
                    if (pSectionExtra != NULL) free(pSectionExtra);
                    if ((pSectionExtra = (char*)PEReadSection(pshExtra)) == NULL)
                    {
                        free(pSectionID);
                        cout << "warning: could not read section for Thunk RVA " << hex((ULONG)pulImport) << endl;
                        return FALSE;
                    }
                }
                else
                {
                    free(pSectionID);
                    if (pSectionExtra != NULL) free(pSectionExtra);
                    cout << "warning: could not find section for Thunk RVA " << hex((ULONG)pulImport) << endl;
                    return FALSE;
                }
            }
            pulImport  = (PULONG)((ULONG)pulImport - pshExtra->VirtualAddress + (ULONG)pSectionExtra);
        }
        else
            pulImport  = (PULONG)((ULONG)pulImport - pshID->VirtualAddress + (ULONG)pSectionID);
        j          = 0;
        ulCurFixup = (ULONG)pID[i].FirstThunk + pOH->ImageBase;
        while (pulImport[j] != 0)
        {
            if (pulImport[j] & IMAGE_ORDINAL_FLAG)
            {   //ordinal
                cout.setf(ios::hex, ios::basefield);
                cout << "0x" << ulCurFixup << " Imported function " << pszCurModule << "@" << (pulImport[j] & ~IMAGE_ORDINAL_FLAG) << endl;
                cout.setf(ios::dec, ios::basefield);
                StoreImportByOrd(i, pulImport[j] & ~IMAGE_ORDINAL_FLAG, ulCurFixup);
            }
            else
            {   //name
                    //check
                if (pulImport[j] < pshID->VirtualAddress || pulImport[j] >= pshID->VirtualAddress + max(pshID->Misc.VirtualSize, pshID->SizeOfRawData))
                {
                    if (pshExtra == 0 || pulImport[j] < pshExtra->VirtualAddress || pulImport[j] >= pshExtra->VirtualAddress + max(pshExtra->Misc.VirtualSize, pshExtra->SizeOfRawData))
                    {
                        if ((pshExtra = PEGetSectionHdrByRVA(pSH, (ULONG)pulImport)) != NULL)
                        {
                            if (pSectionExtra != NULL) free(pSectionExtra);
                            if ((pSectionExtra = (char*)PEReadSection(pshExtra)) == NULL)
                            {
                                free(pSectionID);
                                cout << "warning: could not read section for Import Name RVA " << hex(pulImport[j]) << endl;
                                break; //return FALSE;
                            }
                        }
                        else
                        {
                            free(pSectionID);
                            if (pSectionExtra != NULL) free(pSectionExtra);
                            cout << "warning: could not find section for Thunk RVA " << hex(pulImport[j]) << endl;
                            break; //return FALSE;
                        }
                    }
                    pulImport[j] += (ULONG)pSectionExtra - pshExtra->VirtualAddress;
                }
                else
                    pulImport[j] += (ULONG)pSectionID - pshID->VirtualAddress;

                cout.setf(ios::hex, ios::basefield);
                cout << "0x" << ulCurFixup << " Imported function " << (char*)(pulImport[j] + 2) << endl;
                cout.setf(ios::dec, ios::basefield);
                StoreImportByName(pszCurModule, i, (char *)(pulImport[j] + 2), ulCurFixup);
            }
         ulCurFixup += sizeof(IMAGE_THUNK_DATA);
            j++;
        }

        pszCurModule += strlen(pszCurModule) + 1;
        cout << endl;
    }//for (i = 0; i < cModules; i++)

    free(pszModules);
    return TRUE;
}



/**
 * Processes the resources in the PE-file.
 * @param  pSectionTable  Pointer to sectiontable
 * @return TRUE on success. FALSE on failure.
 */
BOOL LXFile::ProcessResources(PIMAGE_SECTION_HEADER pSectionTable)
{
    /* RESOURCES */
    PIMAGE_SECTION_HEADER pRSH;
    PIMAGE_RESOURCE_DIRECTORY       prdRoot, prdType;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
    PIMAGE_RESOURCE_DATA_ENTRY      pData;
    ULONG                           offType;
    ULONG                           offRoot;
    int i,j,id,rc = TRUE;

    pRSH = PEGetSectionHdrByName(pSectionTable, ".rsrc"); // maybe we should check the ImageDirectory instead?
    if (pRSH != NULL)
    {

        cout << "Resource section: " << endl;
        cout << "Raw data size:       " << hex(pRSH->SizeOfRawData) << endl;
        cout << "Virtual Address:     " << hex(pRSH->VirtualAddress) << endl;
        cout << "Pointer to raw data: " << hex(pRSH->PointerToRawData) << endl;
        cout << "Section flags:       " << hex(pRSH->Characteristics) << endl << endl;

        /* get root directory of resource tree */
        offRoot = PEImageDirectoryOffset(pSectionTable, IMAGE_DIRECTORY_ENTRY_RESOURCE);

        if (offRoot != 0)
        {

            /* allocate and read resource section */
            if (PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress != pRSH->VirtualAddress)
                cout << "Info: Resource directory does not start at beginning of .rsrc section." << endl;
            /*alloc*/
            //prdRoot = (PIMAGE_RESOURCE_DIRECTORY)malloc(max(PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size, min(pRSH->VirtualAddress, pRSH->SizeOfRawData)));
            prdRoot = (PIMAGE_RESOURCE_DIRECTORY)malloc(PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size);
            ltassert(prdRoot != NULL);
            /*read*/
            rc = _ldrRead(
                hFile,
                offRoot,
                prdRoot,
                0,
                //max(PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size, min(pRSH->VirtualAddress, pRSH->SizeOfRawData)),
                PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size, //until this crashes keep it!
                NULL
                );
            if (rc != NULL)
            {
                cout << "error while reading resourceses, rc=" << rc <<endl;
                free(prdRoot);
                return FALSE;
            }

            cout << "Resources: " << endl;
            ltassert2(SetResourceSize(max(pRSH->Misc.VirtualSize, pRSH->SizeOfRawData)), free(prdRoot));
            ltassert2(SetNrResources(PEGetNumberOfResources(prdRoot)), free(prdRoot));  //these functions allocate much memory!

            /* set pointer to first resource type entry */
            prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdRoot + sizeof (IMAGE_RESOURCE_DIRECTORY));
            ltasserthp2(prde, free(prdRoot));
            rc = TRUE;
            //SvL: Store all resource ids so we can safely allocate unique ids for name ids
            for ( i = 0; i < prdRoot->NumberOfNamedEntries + prdRoot->NumberOfIdEntries && rc; i++)
            {
                /* locate directory or each resource type */
                offType = prde->u2.OffsetToData;
                prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)prdRoot + (offType & ~0x80000000));
             ltasserthp2(prdType, free(prdRoot);OS2Icon::DestroyAll();OS2Cursor::DestroyAll());
                if (i < prdRoot->NumberOfNamedEntries)
                {
                    //SvL: 30-10-'97, high bit is set, so clear to get real offset
                    prde->u1.Name &= ~0x80000000;
                    char *resname = UnicodeToAscii(*(WCHAR *)((int)prdRoot + (int)prde->u1.Name), (WCHAR *)((int)prdRoot + (int)prde->u1.Name + sizeof(WCHAR)));   // first word = string length

                    for (j = 0; j < (sizeof(ResTypes) / sizeof(ResTypes[0])); j++)
                    {
                        if (strcmp(resname, ResTypes[j]) == 0)
                            break;
                    }
                    if (j == (sizeof(ResTypes) / sizeof(ResTypes[0])))
                        id = NTRT_RCDATA;
                    else
                        id = j;
                }
                else
                    id = prde->u1.Id;

                ltassert2(offType & 0x80000000, free(prdRoot);OS2Icon::DestroyAll();OS2Cursor::DestroyAll());
                rc = StoreIdResSubDir(prdType, 1, prdRoot, pRSH->VirtualAddress, id, 0);

                /* increment to next entry */
                prde++;
            }

            /* set pointer to first resource type entry */
            prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdRoot + sizeof (IMAGE_RESOURCE_DIRECTORY));
          ltasserthp2(prde, free(prdRoot);OS2Icon::DestroyAll();OS2Cursor::DestroyAll());

            /* loop through all resource directory entry types */
            //eerste level -> types
            //tweede level -> names
            //derde  level -> language
            cout << "Name entries: " << endl;
            cout << "ID entries: " << endl;
            for (i=0; i < prdRoot->NumberOfNamedEntries+prdRoot->NumberOfIdEntries && rc; i++)
            {
                /* locate directory or each resource type */
                offType = prde->u2.OffsetToData;
                prdType = (PIMAGE_RESOURCE_DIRECTORY)((int)prdRoot + (offType & ~0x80000000));
             ltasserthp2(prdType, free(prdRoot);OS2Icon::DestroyAll();OS2Cursor::DestroyAll());

                if (i<prdRoot->NumberOfNamedEntries)
                {
                    //SvL: 30-10-'97, high bit is set, so clear to get real offset
                    prde->u1.Name &= ~0x80000000;
                    char *resname = UnicodeToAscii(*(WCHAR *)((int)prdRoot + (int)prde->u1.Name), (WCHAR *)((int)prdRoot + (int)prde->u1.Name + sizeof(WCHAR)));   // first word = string length

                    cout << "Type " << resname << endl;
                    for (j=0; j < (sizeof(ResTypes) / sizeof(ResTypes[0])); j++)
                    {
                        if (strcmp(resname, ResTypes[j]) == 0)
                            break;
                    }
                    if (j == (sizeof(ResTypes) / sizeof(ResTypes[0])))
                        id = NTRT_RCDATA;
                    else
                        id = j;
                }
                else
                {
                    if (prde->u1.Id < (sizeof(ResTypes) / sizeof(ResTypes[0])))
                        cout << "Type " << ResTypes[prde->u1.Id] << endl;
                    else
                        cout << "Custom Type " << prde->u1.Id << endl;
                    id = prde->u1.Id;
                }

                if (offType & 0x80000000)
                {//subdirectory?
                    rc = ProcessResSubDir(prdType, 1, prdRoot, pRSH->VirtualAddress, id, 0);
                }
                else
                {
                    pData = (PIMAGE_RESOURCE_DATA_ENTRY)prdType;
                    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    cout << "Resource Data entry of size " << hex(pData->Size) << endl;
                    cout << "Resource Data RVA " << hex(pData->OffsetToData) << endl;
                    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    ltassert2(offType & 0x80000000, free(prdRoot);OS2Icon::DestroyAll();OS2Cursor::DestroyAll());
                }

                /* increment to next entry */
                prde++;
            ltasserthp2(prde, free(prdRoot);OS2Icon::DestroyAll();OS2Cursor::DestroyAll());
            }

            free(prdRoot);
        }
        OS2Icon::DestroyAll();
        OS2Cursor::DestroyAll();
    }


    if (rc)
    {//if all went ok...
        rc = SaveConvertedNames();

        /* freeing now unused memory */
        free(cvtname);
        cvtname = NULL;
        free(resids);
        resids = NULL;
        free(orgrestable);
        orgrestable = NULL;


        //done with resources
        if (nrresources)
        {
            ResSection.virtualsize = ResSection.curoff;
            ResSection.rawsize     = ResSection.curoff;
        }


        /* do some resizing */
        if ((void*)ResSection.rawdata != NULL && rc)
        {
            void *pTmp;
            cout << "resizing: " << _msize((void*)ResSection.rawdata) << " ?? " << ResSection.rawsize << endl;
            pTmp = realloc((void*)ResSection.rawdata, ResSection.rawsize);
            if (pTmp != NULL)
                ResSection.rawdata = (ULONG)pTmp;
            else
                cout << "LXFile::PrepareLX: warning - could not reallocate ResSection.rawdata." << endl;
        }
    }

    return rc;
}



/**
 * Check if an valid PE-Header is presented.
 * @param  pHdr Is a pointer to the PE Signature and the following Fileheader.<br>
 *         Size: 196 bytes (size of an LX-header)
 */
BOOL LXFile::isPEFile(PIMAGE_SIG_FILE_HEADER pHdr)
{
    ltassert(pHdr != NULL);

    /*signature*/
    if (pHdr->Signature != IMAGE_NT_SIGNATURE)
    {
        //kprintf(("LXFile::isPEFile: Invalid PE signature\n"));
        return FALSE;
    }

    if (!(pHdr->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE))
    {
        kprintf(("LXFile::isPEFile: Not a valid PE file!\n"));
        return FALSE;
    }

    if (pHdr->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)
    {
        kprintf(("LXFile::isPEFile: Not i386 code.\n"));
        return FALSE;
    }

    if (pHdr->FileHeader.Characteristics & IMAGE_FILE_SYSTEM)
    {
        kprintf(("LXFile::isPEFile: Sorry. Can't convert file system drivers.\n"));
        return FALSE;
    }

    if (pHdr->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER))
    {
        kprintf(("LXFile::isPEFile: Incorrect optional header size(%d).\n", pHdr->FileHeader.SizeOfOptionalHeader));
        return FALSE;
    }

  return TRUE;
}



/**
 * Query if psh is a importdata section.
 * @param  psh  Pointer to sectionheader.
 * @return TRUE if psh is an importdata section else FALSE.
 */
BOOL LXFile::PEIsImportDataSection(PIMAGE_SECTION_HEADER psh)
{
    DWORD VAImageDir;

    /* locate specific image directory's relative virtual address */
    VAImageDir = PEHdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    return psh->VirtualAddress <= VAImageDir  &&  psh->VirtualAddress + psh->SizeOfRawData > VAImageDir;
}



/**
 * Get offset to section(data) - by a Data Directory entry.
 * @param  psh                Pointer to Section Table.
 * @param  dwIMAGE_DIRECTORY  Datadirectory index.
 * @return On success a offset to the section data, else NULL.
 */
DWORD LXFile::PEImageDirectoryOffset(PIMAGE_SECTION_HEADER psh, DWORD dwIMAGE_DIRECTORY)
{
    int          i = 0;
    DWORD        VAImageDir;

    /* must be 0 thru (NumberOfRvaAndSizes-1) */
    if (dwIMAGE_DIRECTORY >= PEHdrs.OptionalHeader.NumberOfRvaAndSizes)
        return NULL;

    /* locate specific image directory's relative virtual address */
    VAImageDir = PEHdrs.OptionalHeader.DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress;

    /* locate section containing image directory */
    while (i++ < nSections)
    {
        if (psh->VirtualAddress <= VAImageDir && psh->VirtualAddress + psh->SizeOfRawData > VAImageDir)
            break;
        psh++;
    }

    if (i > nSections)
        return NULL;

    /* return image directory file offset  */
    return (VAImageDir - psh->VirtualAddress + psh->PointerToRawData);
}



/**
 * Get pointer to section header - by a Data Directory entry.
 * @param  psh                Pointer to Section Table.
 * @param  dwIMAGE_DIRECTORY  Datadirectory index.
 * @return On success a pointer to the sectionheader, else NULL pointer.
 */
PIMAGE_SECTION_HEADER LXFile::PEGetSectionHdrByImageDir(PIMAGE_SECTION_HEADER psh, DWORD dwIMAGE_DIRECTORY)
{
    int    i = 0;
    DWORD  VAImageDir;

    /* must be 0 thru (NumberOfRvaAndSizes-1) */
    if (dwIMAGE_DIRECTORY >= PEHdrs.OptionalHeader.NumberOfRvaAndSizes)
        return FALSE;

    /* locate specific image directory's relative virtual address */
    VAImageDir = (ULONG)PEHdrs.OptionalHeader.DataDirectory[dwIMAGE_DIRECTORY].VirtualAddress;

    /* locate section containing image directory */
    while (i++ < nSections)
    {
        if (psh->VirtualAddress <= VAImageDir  &&  psh->VirtualAddress + psh->SizeOfRawData > VAImageDir)
            break;
        psh++;
    }
    if (i > nSections)
        return NULL;

    return psh;
}



/**
 * Get pointer to section header - by the name of the section.
 * @param  psh        Pointer to Section Table.
 * @param  szSection  Name of the section to find the header of.
 * @return On success a pointer to the sectionheader, else NULL pointer.
 */
PIMAGE_SECTION_HEADER LXFile::PEGetSectionHdrByName(IMAGE_SECTION_HEADER *psh, char *szSection)
{
    int i;

    /* find the section by name */
    i = 0;
    while (i < nSections && strcmp(psh[i].Name, szSection) != 0)
        i++;

    if (i < nSections)
        return &psh[i];
    else
        return NULL;
}



/**
 * Get pointer to section header - by a RVA in that section.
 * @param  psh  Pointer to Section Table.
 * @param  rva  RVA address into a section.
 * @return On success a pointer to the sectionheader, else NULL pointer.
 */
PIMAGE_SECTION_HEADER LXFile::PEGetSectionHdrByRVA(IMAGE_SECTION_HEADER *psh, ULONG rva)
{
    int i;

    /* find the section by name */
    i = 0;
    while (i < nSections
        && !(rva >= psh[i].VirtualAddress && rva < psh[i].VirtualAddress + max(psh[i].Misc.VirtualSize, psh[i].SizeOfRawData))
        )
        i++;

    if (i < nSections)
        return &psh[i];
    else
        return NULL;
}



/**
 * Read Section based on section header.
 * @param  pSH  Pointer to section header of the section to be read.
 * @return Pointer to Section data on success, else NULL pointer.
 * @remark <b>Do not forget to free the return pointer!</b>
 */
PVOID LXFile::PEReadSection(PIMAGE_SECTION_HEADER pSH)
{
    PCHAR pSection = NULL;
    ULONG rc;
    ULONG ulRSize; //Rawsize
    ULONG ulVSize; //Virtualsize

    if(pSH != NULL)
    {
        //warning: Watcom sets .idata-sections virtual size to 0
        ulRSize  = minnotnull(pSH->SizeOfRawData, pSH->Misc.VirtualSize);
        ulVSize  =        max(pSH->SizeOfRawData, pSH->Misc.VirtualSize);

        pSection = (char*)malloc(ulVSize);
        ltassert(pSection != NULL);

        //pad to end of section
        memset(pSection + ulRSize, 0, ulVSize - ulRSize);

        rc = _ldrRead(
            hFile,
            pSH->PointerToRawData,
            pSection,
            0,
            ulRSize,
            NULL
            );

        if (rc != NO_ERROR)
        {
            //warning: Sectionname is not necessarily null terminated (if 8 chars long..)
            char szName[IMAGE_SIZEOF_SHORT_NAME+1];
            memset(SSToDS(szName),0,IMAGE_SIZEOF_SHORT_NAME+1);
            strncpy((char*)SSToDS(szName), pSH->Name, 8);
            cout << "error while reading section " << szName << ", rc=" << rc << endl;
            free(pSection);
            pSection = NULL;
        }
    }

    return (PVOID)pSection;
}



/**
 * Determine the total number of resources in the section
 * @param  prdRoot  Pointer to root node in the resource directory.
 * @return Number of resources in the directory.
 */
int LXFile::PEGetNumberOfResources(PIMAGE_RESOURCE_DIRECTORY prdRoot)
{
    PIMAGE_RESOURCE_DIRECTORY          prdType;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY    prde;
    int                                nCnt=0, i, j, id;
    char                              *resname;

    /* set pointer to first resource type entry */
    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((ULONG)prdRoot + sizeof (IMAGE_RESOURCE_DIRECTORY));

    /* loop through all resource directory entry types */
    for ( i = 0; i < prdRoot->NumberOfNamedEntries + prdRoot->NumberOfIdEntries;  i++)
    {
        ltasserthp(prde);

        /* locate directory or each resource type */
        prdType = (PIMAGE_RESOURCE_DIRECTORY)((ULONG)prdRoot + (prde->u2.OffsetToData & ~0x80000000));
        ltasserthp(prdType);

        if (i < prdRoot->NumberOfNamedEntries)
        {
            //SvL: 30-10-'97, high bit is set, so clear to get real offset
            prde->u1.Name &= ~0x80000000;
            for (j = 0; j < (sizeof(ResTypes) / sizeof(ResTypes[0])); j++)
            {
                resname = UnicodeToAscii( *(WCHAR *)((ULONG)prdRoot + (ULONG)prde->u1.Name), (WCHAR *)((ULONG)prdRoot + (ULONG)prde->u1.Name + sizeof(WCHAR)));    // first word = string length
                if (strcmp(resname, ResTypes[j]) == 0)
                    break;
            }
            if (j == (sizeof(ResTypes) / sizeof(ResTypes[0])))
            {
                //SvL: 30-10-'97, not found = custom resource type (correct?)
                id = NTRT_RCDATA;
            }
            else
                id = j;
        }
        else
            id = prde->u1.Id;


        /* increment count of name'd and ID'd resources in directory */
        if (id == NTRT_STRING)
        {
            //String tables can contain up to 16 individual resources!
            nCnt += prdType->NumberOfNamedEntries*16 + prdType->NumberOfIdEntries*16;
        }
        else
        {
            //Only icon groups are stored as resources in the LX file
            //Icon groups contain one or more icons
            if (id != NTRT_ICON)
                nCnt += prdType->NumberOfNamedEntries + prdType->NumberOfIdEntries;
        }
        /* increment to next entry */
        prde++;
    }

    return nCnt;
}



/**
 * @remark Have similar function in pe.cpp - remeber to update both.<br>
 *         No support for multiple languages; just select the first one
 *         present and ignore the rest
 */
BOOL LXFile::ProcessResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType, int level,
              PIMAGE_RESOURCE_DIRECTORY prdRoot, int VirtualAddress,
              int type, int id)
{
    PIMAGE_RESOURCE_DIRECTORY       prdType2;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
    PIMAGE_RESOURCE_DIR_STRING_U    pstring;
    PIMAGE_RESOURCE_DATA_ENTRY      pData;
    ULONG                           offType2;
    int i;
    BOOL                            rc = TRUE;;

    switch (level)
    {
        case 1:
            cout << "Names.." << endl;
            break;
        case 2:
            cout << "Language.." << endl;
            break;
    }
    cout << "Subdir with " << prdType->NumberOfNamedEntries << " Name entries" << endl;
    cout << "Subdir with " << prdType->NumberOfIdEntries << " id entries" << endl;

    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdType + sizeof (IMAGE_RESOURCE_DIRECTORY));
    for ( i = 0; i < prdType->NumberOfNamedEntries + prdType->NumberOfIdEntries && rc; i++)
    {
        ltasserthp(prde);

        /* locate directory or each resource type */
        offType2 = prde->u2.OffsetToData;
        prdType2 = (PIMAGE_RESOURCE_DIRECTORY)((int)prdRoot + (offType2 & ~0x80000000));
        ltasserthp(prdType2);

        if (i < prdType->NumberOfNamedEntries)
        {//name or id entry?
            if (prde->u1.s.NameIsString) //unicode directory string /*PLF Sat  97-06-21 22:30:35*/
                prde->u1.Name &= ~0x80000000;
            pstring = (PIMAGE_RESOURCE_DIR_STRING_U)((int)prdRoot + (int)prde->u1.Name);
            ltasserthp(pstring);
            cout << "Name = " << UnicodeToAscii(pstring->Length, pstring->NameString) << endl;
            if (level == 1)
            {
                id = ConvertNametoId(UnicodeToAscii(pstring->Length, pstring->NameString));
            }
        }
        else
        {
            cout << "Id " << prde->u1.Id << endl;
            if (level == 1)
                id = prde->u1.Id;
        }

        if (offType2 & 0x80000000)
        {//subdirectory?
            rc = ProcessResSubDir(prdType2, 2, prdRoot, VirtualAddress, type, id);
        }
        else
        {
            pData = (PIMAGE_RESOURCE_DATA_ENTRY)prdType2;
            cout << "Resource Data entry of size " << hex(pData->Size) << endl;
            cout << "Resource Data RVA " << hex(pData->OffsetToData - VirtualAddress) << endl;
            if (pData->Size)
            {//winamp17 winzip archive has resource with size 0
                switch (type)
                {
                    case NTRT_MENU:
                        rc = ShowMenu(*this, id, (MenuHeader *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size, 437);
                        break;
                    case NTRT_ICON:
                        new OS2Icon(id, (WINBITMAPINFOHEADER *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        rc = OS2Icon::GetIcon(id) != NULL;
                        break;
                    case NTRT_BITMAP:
                  //KSO Oct 8 1998: testing
                        rc = ShowBitmap(*this, id, (WINBITMAPINFOHEADER *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                  /*rc = StoreResource(id, RT_BITMAP, pData->Size + ((int)prdRoot + pData->OffsetToData - VirtualAddress) - (int)pData,
                                                 (char*)pData); */
                    break;
                    case NTRT_GROUP_ICON:
                        rc = ShowGroupIcon(*this, id, (IconHeader *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_CURSOR:
                        new OS2Cursor(id, (CursorComponent *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        rc = OS2Cursor::GetCursor(id) != NULL;
                        break;
                    case NTRT_GROUP_CURSOR:
                        rc = ShowGroupCursor(*this, id, (CursorHeader *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_DIALOG:
                        rc = ShowDialog(*this, id, (DialogBoxHeader *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size, 437);
                        break;
                    case NTRT_VERSION:
                        //Store version resource as OS/2 RT_RCDATA resource
                        //to retrieve the original for win32's version apis
                        //TODO: Only supports one version resource (who would want to use more??)
                        SetVersionResourceId(id);
                    case NTRT_RCDATA:
                        rc = ShowRCData(*this, id, (char *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    case NTRT_STRING:
                        //String format: tables of 16 strings stored as one resource
                        //upper 12 bits of resource id passed by user determines block (res id)
                        //lower 4 bits are an index into the string table
                        //Best solution is to split the strings up and store them as RCDATA
                        rc = ShowStrings(*this, id, (char *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size, 437);
                        break;
                    case NTRT_ACCELERATORS:
                        rc = ShowAccelerator(*this, id, (WINACCEL *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                    default:
                        cout << "!!Unsupported resource type!! : " << type << endl;
                        //Just save it as rcdata
                        rc = ShowRCData(*this, id, (char *)((char *)prdRoot + pData->OffsetToData - VirtualAddress), pData->Size);
                        break;
                }//switch
            }//if size > 0
        }
        if (level == 2) break;   //only support one language!

        prde++;
        cout << endl;
    }

    return rc;
}



/**
 * @remark Have similar function in pe.cpp - remeber to update both.
 */
BOOL LXFile::StoreIdResSubDir(PIMAGE_RESOURCE_DIRECTORY prdType, int level,
              PIMAGE_RESOURCE_DIRECTORY prdRoot, int VirtualAddress,
              int type, int id)
{
    PIMAGE_RESOURCE_DIRECTORY       prdType2;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY prde;
    ULONG                           offType2;
    int i, j;


    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdType + sizeof (IMAGE_RESOURCE_DIRECTORY));

    for (i = 0; i < prdType->NumberOfNamedEntries + prdType->NumberOfIdEntries; i++)
    {
        ltasserthp(prde);

        /* locate directory or each resource type */
        offType2 = prde->u2.OffsetToData;
        prdType2 = (PIMAGE_RESOURCE_DIRECTORY)((int)prdRoot + (offType2 & ~0x80000000));
        ltasserthp(prdType2);

        if (i >= prdType->NumberOfNamedEntries)
        {//name or id entry?
            if (level == 1)
                id = prde->u1.Id;
        }
        else
        {
            prde++;
            continue;   //skip name ids
        }

        if (offType2 & 0x80000000)
        {//subdirectory?
            ltassert(StoreIdResSubDir(prdType2, 2, prdRoot, VirtualAddress, type, id));
        }
        else
        {
            if (type == NTRT_STRING)
            {
                for (j = 0; j < 16; j++)
                    ltassert(StoreResourceId((id-1)*16+j));
            }
            else
                ltassert(StoreResourceId(id));
        }
        if (level == 2)
            break;   //only support one language!
        prde++;
    }
    return TRUE;
}



/**
 * Add 32-bit offset fixup.
 * @param  address  Address(RVA) of the fixup
 * @param  pPage    Pointer to the data of the page which address refers to.
 * @return NO_ERROR on success or ignorable errors.<br>
 *         -1 on error<br>
 *         -2 on crosspagefixup<br>
 * @remark This function replaces the AddOff32Fixup(int address) found in
 *         LXHeaderSuper for LXFile - the one in LXHeaderSuper shall
 *         <b>not</b> be used.<br>
 *         Errors may be ignored - if you dare.
 */
int LXFile::AddOff32Fixup(int address, char *pPage)
{
    intfixup  *pFixRec;
    int        i;
    int        no;
    int        targetaddr;
    int        rc = NO_ERROR;

    pFixRec = &intfixuprec[nrintfixups];

    cltassert(nrintfixups < nrofintfixups);

    pFixRec->nr_stype   = 0x07;               //32 bits offset fixup
    pFixRec->nr_flags   = 0x00 | 0x10 | 0x40; //internal fixup + 32 bits offset + 16 bits object nr
    pFixRec->r32_soff   = (SHORT)(address & 0xFFF);

    no = GetObjectNoByVA(address);
    if (no == -1)
    {
        cout << "Oops, fixup error 1...." << endl;
        return -1;
    }
    pFixRec->srcpage  = (USHORT)GetPageNoByVA(address);
    pFixRec->srcobj   = (USHORT)no;

    /*KSO: get targetaddress*/
    targetaddr = *(int*)&pPage[pFixRec->r32_soff];
    //cout << " targetaddr: " << hex(targetaddr);

    i  = GetSectionIdxByVA(targetaddr);
    no = GetObjectNoByVA(targetaddr);
    if (i == -1 || no == -1)
    {
        cout << "Oops, fixup error 2....(" << (ULONG)targetaddr << ")" << endl;
        return -1;
    }
    pFixRec->targetaddr = targetaddr - PESection[i].address;    //relatief
    pFixRec->targetobj  = (USHORT)no;

    //SvL: Check for fixups that cross page boundaries
    //     If I read the lx doc right, you need to add two fixups in
    //     this case, one for original page and one for original page + 1
    //KSO: Hopefully it will now work even if the last page contains such a fixup!
    if (pFixRec->r32_soff + 4 > PAGE_SIZE)
    {
        memcpy((char *)&crossedpage, (char *)pFixRec, sizeof(crossedpage));
        //crossed page boundary -> negative offset in next page
        crossedpage.r32_soff  = (SHORT)((pFixRec->r32_soff & 0xFFF) - PAGE_SIZE);
        crossedpage.srcpage   = pFixRec->srcpage+1;
        crossedpageaddress    = (address + PAGE_SIZE) & ~0xFFF;
        fCrossed              = TRUE;
        rc = -2;
    }

    nrintfixups++;

    return rc;
}



/**
 * Adds the crosspagefixup if present.
 * @return  NO_ERROR on success<br>
 *          Something else on error.
 */
int LXFile::AddOff32FinishCrossPageFixup(void)
{
    intfixup  *pFixRec;
    pFixRec = &intfixuprec[nrintfixups];

    cltassert(nrintfixups < nrofintfixups);

    //Check if we need to insert a crossed page fixup
    if (fCrossed == TRUE)
    {
        memcpy((char *)pFixRec, (char *)&crossedpage, sizeof(crossedpage));
        fCrossed           = FALSE;
        crossedpageaddress = 0;
        nrintfixups++;
        pFixRec++;
        //cout << "CrossPageFixup: Finished." << endl;
    }

    return NO_ERROR;
}



#ifdef MIN
    #undef MIN
#endif
#define MIN(a,b)  (a <= b ? a : b)
#define POST                                         \
        ulOffset += ulToRead;                          \
        ulBytesToRead -= ulToRead;                     \
        pBuffer = (PVOID)((ULONG)pBuffer + ulToRead);  \
        if (ulBytesToRead == 0)                        \
        {                                              \
            *pulBytesRead -= ulBytesToRead;             \
            return rc;                                  \
        }



/**
 * Read data from the virtual LX-file.
 * @param  ulOffset       Offset (into the virtual lx file) of the data to read
 * @param  pBuffer        Pointer to buffer where data is to be put.
 * @param  ulBytesToRead  Bytes to be read.
 * @param  pulBytesRead   Bytes actualy read. (Out)
 * @param  ulFlag             Flags from and to the _ldrRead call.
 * @return NO_ERROR if successful something else if not.
 * @remark Should only be called one time! Object should not be used if init(...) failes!
 */
ULONG LXFile::read(ULONG ulOffset, PVOID pBuffer, ULONG ulBytesToRead, PULONG pulBytesRead, ULONG ulFlag)
{
    ULONG ulToRead = 0;
    int i;
    int rc = NO_ERROR;

    cltassert((ULONG)pulBytesRead > 0x10000);
    cltassert((ULONG)pBuffer > 0x10000);

    cout.setf(ios::hex, ios::basefield);
    cout << "LXFile::read(0x" << ulOffset << ", 0x" << pBuffer << ", 0x" << ulBytesToRead << ");" << endl;
    cout.setf(ios::dec, ios::basefield);

    if (!inited)
    {
        kprintf(("LXFile::read: object not inited!"));
        *pulBytesRead = 0;
        return ERROR_READ_FAULT;
    }

    *pulBytesRead = ulBytesToRead;


    /* PRE LXHdr */
    if (ulOffset < ulPEOffset /*LXHEADER_OFFSET*/)
    {
        if (ulOffset == 0) cout << "LXFile::read: NEHdr at offset "<< hex(ulOffset) << endl ;
        /* the loader will never request this */
        if(!options.ourstub)
        {
            /* original stub */
            ulToRead = MIN(ulBytesToRead, ulPEOffset /*LXHEADER_OFFSET*/ - ulOffset);
            rc = _ldrRead(hFile, ulOffset, pBuffer, 0, ulToRead, NULL);
        }
        else
        {   /* our stub */
            /* correct PE offset in MZ header */
            ulToRead = MIN(ulBytesToRead, LXHEADER_OFFSET - ulOffset);
            if (ulOffset < 0x40 )
                *(PULONG)(achDosHeader+0x3c) = ulPEOffset;
            memcpy(pBuffer,(PVOID)((ULONG)achDosHeader + ulOffset), ulToRead);

            if (ulPEOffset > LXHEADER_OFFSET)
            {
                POST;
                ulToRead = MIN(ulBytesToRead,ulPEOffset - ulOffset);
                memset(pBuffer,0,ulToRead);
            }
        }
        POST;
    }


    /* LXHdr */
    if (!rc && ulOffset < ulPEOffset /*LXHEADER_OFFSET*/ + sizeof(e32_exe))
    {
        if (ulOffset == ulPEOffset /*LXHEADER_OFFSET*/) cout << "LXFile::read: LXHdr at offset "<< hex(ulOffset) << endl ;
        ulToRead = MIN(ulBytesToRead, sizeof(e32_exe)-(ulOffset - ulPEOffset /*LXHEADER_OFFSET*/));
        //printf("LXHdr: %d %d\n", ulOffset, ulToRead);
        memcpy(pBuffer,(PVOID)((ULONG)&LXHdr + ulOffset - ulPEOffset /*LXHEADER_OFFSET*/), ulToRead);
        POST;
    }


    /* the folowing section uses offset relative to linear EXE header */
    ulOffset -= ulPEOffset /*LXHEADER_OFFSET*/;


    /* Object Table */
    if (!rc && ulOffset < LXHdr.e32_objmap)
    {
        if (ulOffset == LXHdr.e32_objtab) cout << "LXFile::read: Object Table at offset "<< hex(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << endl ;
        ulToRead = MIN(ulBytesToRead, LXHdr.e32_objmap - ulOffset);
        //cout << "LXFile::read: Object Table: ulOffset " << ulOffset << " ulToRead " << ulToRead << endl;
        memcpy(pBuffer, (PVOID)((ULONG)pObjectTable + ulOffset - LXHdr.e32_objtab), ulToRead);
        POST;
    }


    /* Object Page Table */
    if (!rc && ulOffset < LXHdr.e32_rsrctab)
    {
        if (ulOffset == LXHdr.e32_objmap) cout << "LXFile::read: Object Page Table at offset "<< hex(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << endl ;
        ulToRead = MIN(ulBytesToRead, LXHdr.e32_rsrctab - ulOffset);
        //cout << "LXFile::read: Object Page Table: ulOffset " << ulOffset << " ulToRead " << ulToRead << endl;
        memcpy(pBuffer, (PVOID)((ULONG)pObjectPageTable + ulOffset - LXHdr.e32_objmap), ulToRead);
        POST;
    }


    /* Resource Table */
    if (!rc && ulOffset < LXHdr.e32_restab)
    {
        if (ulOffset == LXHdr.e32_rsrctab) cout << "LXFile::read: Resource Table at offset "<< hex(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << endl ;
        ulToRead = MIN(ulBytesToRead, LXHdr.e32_restab - ulOffset);
        //cout << "LXFile::read: Resource Table: ulOffset " << ulOffset << " ulToRead " << ulToRead << " Relativ Offset "<< (void*)(ulOffset - LXHdr.e32_rsrctab) << endl;
        memcpy(pBuffer, (PVOID)((ULONG)os2resource + ulOffset - LXHdr.e32_rsrctab), ulToRead);
        POST;
    }


    /* Resident Name Table && Entry Table */
    if (!rc && ulOffset < LXHdr.e32_fpagetab)
    {
        if (ulOffset < LXHdr.e32_enttab)
        {
            if (ulOffset == LXHdr.e32_restab) cout << "LXFile::read: Resident Name Table at offset "<< hex(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << endl ;
            ulToRead = MIN(ulBytesToRead, LXHdr.e32_enttab - ulOffset);
            //cout << "Resident Name Table: ulOffset " << (void*)(ulOffset + ulPEOffset /*LXHEADER_OFFSET*/) << " ulToRead " << MIN(ulBytesToRead, LXHdr.e32_enttab - ulOffset) << endl;
            memcpy(pBuffer, (PVOID)((ULONG)((exports!=NULL)?exports : modulename)+ulOffset-LXHdr.e32_restab) , ulToRead);
            POST
        }

        if (ulOffset < LXHdr.e32_fpagetab && LXHdr.e32_restab < LXHdr.e32_enttab)
        {
            if (ulOffset == LXHdr.e32_enttab) cout << "LXFile::read: Entry Table at offset "<< hex(ulOffset) << endl ;
            ulToRead = MIN(ulBytesToRead, LXHdr.e32_fpagetab - ulOffset);
            //cout << "Entry Table: ulOffset " << (void*)(ulOffset + ulPEOffset /*LXHEADER_OFFSET*/) << " ulToRead " << MIN(ulBytesToRead, LXHdr.e32_fpagetab - ulOffset) << endl;
            if (expbundle != NULL)
                memcpy(pBuffer, (PVOID)((ULONG)expbundle + ulOffset - LXHdr.e32_enttab), ulToRead);
            else
                memset(pBuffer, 0, ulToRead);
            POST;
        }
    }


    /* FixupTable */
    if (!rc && ulOffset < LXHdr.e32_frectab)
    {
        if (ulOffset == LXHdr.e32_fpagetab) cout << "LXFile::read: Fixup Table at offset "<< hex(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << endl ;
        ulToRead = MIN(ulBytesToRead, LXHdr.e32_frectab - ulOffset);
        cout << "LXFile::read: FixupTable: ulOffset " << (void*)(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << " ulToRead " << ulToRead << " Relativ Offset "<< (void*)(ulOffset - LXHdr.e32_fpagetab) << endl;
        memcpy(pBuffer, (PVOID)((ULONG)pFixupTable + ulOffset - LXHdr.e32_fpagetab), ulToRead);
        POST;
    }


    /* Fixup Record Table */
    if (!rc && ulOffset < LXHdr.e32_impmod)
    {
        if (ulOffset == LXHdr.e32_frectab) cout << "LXFile::read: Fixup Record Table at offset "<< hex(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << endl ;
        ulToRead = MIN(ulBytesToRead, LXHdr.e32_impmod - ulOffset);
        memcpy(pBuffer, (PVOID)((ULONG)pFixupRecordTable + ulOffset - LXHdr.e32_frectab), ulToRead);
        POST;
    }


    /* Import Module Name Table */
    if (!rc && ulOffset < LXHdr.e32_impproc)
    {
        if (ulOffset == LXHdr.e32_impmod) cout << "LXFile::read: Import Module Name Table at offset "<< hex(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << endl ;
        ulToRead = MIN(ulBytesToRead, LXHdr.e32_impproc - ulOffset);
        //kprintf("Import Module Name Table: offset %d relative offset %d  ulToRead %d\n", ulOffset, ulOffset - LXHdr.e32_impmod , ulToRead);
        memcpy(pBuffer, (PVOID)((ULONG)impmodules + ulOffset - LXHdr.e32_impmod), ulToRead);
        POST;
    }


    /* Import Procedure Name Table */
    if (!rc && ulOffset < LXHdr.e32_datapage - ulPEOffset /*LXHEADER_OFFSET*/)
    {
        if (ulOffset == LXHdr.e32_impproc) cout << "LXFile::read: Import Procedure Name Table at offset "<< hex(ulOffset+ulPEOffset /*LXHEADER_OFFSET*/) << endl ;
        ulToRead = MIN(ulBytesToRead, LXHdr.e32_datapage - ulPEOffset /*LXHEADER_OFFSET*/ - ulOffset );
        //kprintf("Import Procedure Name Table: offset %d relative offset %d  ulToRead %d\n", ulOffset, ulOffset - LXHdr.e32_impproc , ulToRead);
        memcpy(pBuffer, (PVOID)((ULONG)impnames + ulOffset - LXHdr.e32_impproc), ulToRead);
        POST;
    }


    /* CODE/DATA are relative file start offset */
    ulOffset += ulPEOffset /*LXHEADER_OFFSET*/;
    if (!rc)
    {
        NextSection:
        //cout << "Section - ";
        /* Determin which section no. */
        i = 0;
        while (i < nrsections
            &&
            !(PESection[i].rawsize != 0
              && PESection[i].type != SECTION_UNINITDATA
              && ulOffset < (PESection[i].lxoffset + PESection[i].rawsize)
              && ulOffset >= PESection[i].lxoffset
            )/*!*/
            )
            i++;

        if ( i >= nrsections )
        {
            /* find section nearest to ulOffset */
            int iTmp;
            //cout << "attempt find next section" << endl;
            iTmp = 0;
            i = 0;
            //cout << endl<< "ulOffset:" << ulOffset<< endl;
            while (i < nrsections)
            {
                //cout << "i="<< i << " lxoffset:" << PESection[i].lxoffset<<endl;
                if (PESection[i].lxoffset > ulOffset
                    && (PESection[i].lxoffset < PESection[iTmp].lxoffset || PESection[iTmp].lxoffset < ulOffset)
                    )
                    iTmp = i;
                i++;
            }
            i = (iTmp > 0 || PESection[iTmp].lxoffset > ulOffset) ? iTmp : nrsections;

            if (i < nrsections)
            {
                //fill gap
                ulToRead = MIN(ulBytesToRead, (PESection[i].lxoffset - ulOffset));
                cout << "alignment - fill " << ulToRead << "bytes." << endl;
                memset(pBuffer, 0xBB, ulToRead);
                POST;
            cout << "Section ";
            }
        }

        if (i < nrsections)
        {
            if (ulOffset == PESection[i].lxoffset) cout << "LXFile::read: Section no."<< i << " at offset "<< hex(ulOffset) << endl;

            /* read */
            ulToRead = MIN(ulBytesToRead, (PESection[i].lxoffset + PESection[i].rawsize) - ulOffset);
            //cout << i << ": offset " << ulOffset << " relative offset " << ulOffset - PESection[i].lxoffset << " ulToRead " << ulToRead << endl;
            if (!(PESection[i].type & SECTION_TIBFIX))
            {
                rc = _ldrRead(
                    hFile,
                    PESection[i].rawdata + (ulOffset - PESection[i].lxoffset),
                    pBuffer,
                    0,
                    ulToRead,
                    NULL
                    );
            }
            else
            {
                AdjustTIB();
                memcpy(pBuffer, (void*)((ULONG)szTIBFix + (ulOffset - PESection[i].lxoffset)), ulToRead);
            }
            if (rc || ulToRead == ulBytesToRead)
                return rc;

            /* read crosses section boundrary */
            ulOffset += ulToRead;
            ulBytesToRead -= ulToRead;
            pBuffer = (void*)((ULONG)pBuffer + ulToRead);
            //cout << "rec" << endl;
            //return read(ulOffset, pBuffer, ulBytesToRead);
            goto NextSection;
        }


        /* resource */
        if (!rc && nrresources > 0)
        {
            //cout << "debug: nrresources         " << nrresources << endl;
            //cout << "debug: ulOffset            " << ulOffset << endl;
            //cout << "debug: ResSection.lxoffset " << ResSection.lxoffset << endl;
            //cout << "debug: ResSection.rawsize  " << ResSection.rawsize << endl;
            //cout << "debug: ResSection.rawsize  " << ResSection.virtualsize << endl;
            if (!rc && ulOffset < ResSection.lxoffset + ResSection.rawsize)
            {
                if (ulOffset == ResSection.lxoffset) cout << "LXFile::read: ResSection at offset "<< hex(ulOffset) << endl;
                ulToRead = MIN(ulBytesToRead, ResSection.lxoffset + ResSection.rawsize - ulOffset);
                //cout << "LXFile::read: ResSection: ulOffset " << ulOffset << " relative offset " << ulOffset -ResSection.lxoffset << " ulToRead " << ulToRead << endl;
                memcpy(pBuffer, (char*)ResSection.rawdata + (ulOffset - ResSection.lxoffset), ulToRead);
                POST;
            }
        }

        /*
            While debugging: If this is an partly finished request we pad the last part of it and return NO_ERROR;
            Else: fail!
        */
        #ifdef RING0_DEBUG_IN_RING3
        if (!rc && *pulBytesRead > ulBytesToRead)
        {
            /*pad end*/
            ulToRead = ulBytesToRead;
            cout << "LXFile::read: WARNING! had to pad end of request!"<< endl;
            memset((void*)((ULONG)pBuffer+ulToRead), 0xCC, ulBytesToRead-ulToRead);
            *pulBytesRead -= ulBytesToRead;
            return -2;
        }
        #endif

    }

    return ERROR_READ_FAULT;
}



/**
 * Read a page from the PE-file based on the RVA on the page.
 * @param  address  RVA of the page to be read.
 * @param  pPage    Pointer to output buffer - size: one page.
 * @return NO_ERROR on success. Something else on failure.
 */
int LXFile::ReadPageByAddress(ULONG address, void *pPage)
{
    int SectNo,rc;
    ULONG ulOffset;
    ULONG ulToRead;

    /* find which section */
    SectNo = GetOriginalSectionIdxByVA(address);
    if (SectNo == -1)
    {
        cout << "LXFile::ReadPageByAddress: GetOriginalSectionIdxByVA(" << address << ") == -1 !oops!" << endl;
        return -1;
    }

    ///* check if this is our TIBFix section */
   //ulOffset = SectNo == GetTIBSectionIdx() ? ulOffset = 1 : ulOffset = 0;
    ulOffset = 0;

    /* check if not uninited datasection */
    if (PESection[SectNo].rawdata != ~0)
    {
        ulOffset = PESection[SectNo].rawdata + (address - (PESection[SectNo].address + (PESection[SectNo].nrinvalidpages + ulOffset)*PAGE_SIZE));
        ulToRead = MIN(PAGE_SIZE, PESection[SectNo].rawsize - (ulOffset - PESection[SectNo].rawdata));
    }
    else
    {
        memset(pPage , 0, PAGE_SIZE);
        return NO_ERROR;
    }

    /* check if last page */
    if (ulToRead > 0)
    {
        /* read page */
        rc = _ldrRead(
            hFile,
            ulOffset,
            pPage,
            0,
            ulToRead,
            NULL
            );
    }
    else
        if (ulToRead > 0 && ulToRead >= -PAGE_SIZE)
        {
            ulToRead = 0;
            rc = -2; //signal this is less or exactly than one page beyond the last page (used in ProcessFixups)
        }
        else
            return -1; //error!

    /* pad the rest of the page buffer */
    if (ulToRead < PAGE_SIZE)
        memset((void*)((ULONG)pPage + ulToRead), 0, PAGE_SIZE - ulToRead);

    return rc;
}



/**
 * Query the total amount of memory used by this object.
 * If logging is enabled it will display a list which shows how much
 * memory is consumed by each datamember.
 *
 * @return  Memory used in bytes.
 * @remark  May only be called after init() is completed
 */
int LXFile::queryMemoryUsage()
{
    int tsize, size;

    ltassert(inited);

    tsize = 0;
    cout << "LXFile::queryMemoryUsage() :" <<endl;
    cout << "----------------------------" <<endl;

    tsize += size = sizeof(LXFile);
    cout << "   LXfile                            " << hex(size) << "bytes" << endl;


    if( pObjectTable != NULL)
        tsize += size =_msize(pObjectTable);
    else
        size = 0;
    cout << "   LXfile::pObjectTable              " << hex(size) << "bytes" << endl;

    if( pObjectPageTable != NULL)
        tsize += size =_msize(pObjectPageTable);
    else
        size = 0;
    cout << "   LXfile::pObjectPageTable          " << hex(size) << "bytes" << endl;

    if( pFixupTable != NULL)
        tsize += size =_msize(pFixupRecordTable);
    else
        size = 0;
    cout << "   LXfile::pFixupTable               " << hex(size) << "bytes" << endl;

    if( pFixupRecordTable != NULL)
        tsize += size =_msize(pFixupRecordTable);
    else
        size = 0;
    cout << "   LXfile::pFixupRecordTable         " << hex(size) << "bytes" << endl;


    if( PESection != NULL)
        tsize += size =_msize(PESection);
    else
        size = 0;
    cout << "   LXHeaderSuper::PESection          " << hex(size) << "bytes" << endl;

    if( datapage != NULL)
        tsize += size =_msize(datapage);
    else
        size = 0;
    cout << "   LXHeaderSuper::datapage           " << hex(size) << "bytes" << endl;

    if( impmodules != NULL)
        tsize += size =_msize(impmodules);
    else
        size = 0;
    cout << "   LXHeaderSuper::impmodules         " << hex(size) << "bytes" << endl;

    if( impnames != NULL)
        tsize += size =_msize(impnames);
    else
        size = 0;
    cout << "   LXHeaderSuper::impnames           " << hex(size) << "bytes" << endl;

    if( impfixuprec != NULL)
        tsize += size =_msize(impfixuprec);
    else
        size = 0;
    cout << "   LXHeaderSuper::impfixuprec        " << hex(size) << "bytes" << endl;

    if( intfixuprec != NULL)
        tsize += size =_msize(intfixuprec);
    else
        size = 0;
    cout << "   LXHeaderSuper::intfixuprec        " << hex(size) << "bytes" << endl;

    if( os2resource != NULL)
        tsize += size =_msize(os2resource);
    else
        size = 0;
    cout << "   LXHeaderSuper::os2resource        " << hex(size) << "bytes" << endl;

    if( cvtname != NULL)
        tsize += size =_msize(cvtname);
    else
        size = 0;
    cout << "   LXHeaderSuper::cvtname            " << hex(size) << "bytes" << endl;

    if( resids != NULL)
        tsize += size =_msize(resids);
    else
        size = 0;
    cout << "   LXHeaderSuper::resids             " << hex(size) << "bytes" << endl;

    if( orgrestable != NULL)
        tsize += size =_msize(orgrestable);
    else
        size = 0;
    cout << "   LXHeaderSuper::orgrestable        " << hex(size) << "bytes" << endl;

    if( exports != NULL)
        tsize += size =_msize(exports);
    else
        size = 0;
    cout << "   LXHeaderSuper::exports            " << hex(size) << "bytes" << endl;

    if( expbundle != NULL)
        tsize += size =_msize(expbundle);
    else
        size = 0;
    cout << "   LXHeaderSuper::expbundle          " << hex(size) << "bytes" << endl;

    if( (void*)ResSection.rawdata != NULL)
        tsize += size =_msize((void*)ResSection.rawdata);
    else
        size = 0;
    cout << "   LXHeaderSuper::ResSection.rawdata " << hex(size) << "bytes" << endl;



/*
    if(  != NULL)
        tsize += size =_msize();
    else
        size = 0;
    cout << "   LXHeaderSuper::                   " << hex(size) << "bytes" << endl;

*/


    /*end*/
    cout << "   Total memory usage                " << hex(tsize) << "bytes" << endl;

    return tsize;
}



#ifndef __DEBUG_ALLOC__

    /**
     * No debug new operator
     */
    void *LXFile::operator new(size_t a)
    {
        return malloc((ULONG)a);
    }



    /**
     * No debug delete operator
     */
    void LXFile::operator delete(void *p)
    {
        free(p);
    }

#else

    /**
     * Debug new operator
     */
    void *LXFile::operator new(size_t a, const char *file, unsigned int line)
    {
        void* pTmp = malloc(a);
        #ifdef ALLOC_VERBOSE
        cout << "LXFile::operator new("<<a<<","<<file<<","<<line<<") -->"<< pTmp<<endl;
        #endif
        return pTmp;
    }



    /**
     * Debug delete operator
     */
    void LXFile::operator delete(void *p, const char *file, unsigned int line)
    {
        #ifdef ALLOC_VERBOSE
        cout << "LXFile::operator delete("<<p<<","<<file<<","<<line<<");"<<endl;
        #endif
        free(p);
    }

#endif



/**
 * Builds a LX (EXE) header to present to the LDRQAppType call
 * @param  pHdr  Pointer to the buffer in which the LX-header is made.
 */
void LXFile::buildQAppTypeHeader(struct e32_exe *pHdr)
{
    BOOL fEXE;

    vltassert(pHdr);

    fEXE = !(((PIMAGE_FILE_HEADER)((ULONG)pHdr+sizeof(DWORD)))->Characteristics & 0x2000);

    memset((void*)pHdr,0,sizeof(struct e32_exe));

    pHdr->e32_magic[0] = E32MAGIC1;
    pHdr->e32_magic[1] = E32MAGIC2;
    pHdr->e32_border   = E32LEBO;
    pHdr->e32_worder   = E32LEWO;
    pHdr->e32_level    = E32LEVEL;
    pHdr->e32_cpu      = E32CPU386;
    pHdr->e32_os       = 0x01;          //OS/2
    pHdr->e32_ver       = 0;                //who cares which version...
    pHdr->e32_mflags   = 0x300 | 0x8000*!fEXE;
    pHdr->e32_mpages   = 1;
    pHdr->e32_startobj = 1;             //code
   pHdr->e32_eip      = 0;
   pHdr->e32_stackobj = 3;
   pHdr->e32_esp      = 0x101000;
    pHdr->e32_pagesize = 4096;          //default
    pHdr->e32_pageshift= 0;             //default (doet watcom zo)
    pHdr->e32_fixupsize= 0x2c5f;
    pHdr->e32_fixupsum = 0;             //no checksum supported
    pHdr->e32_ldrsize  =    0x3507;
    pHdr->e32_ldrsum   = 0;             //no checksum supported
    pHdr->e32_objtab   = sizeof(struct e32_exe);
    pHdr->e32_objcnt   = 4;
    pHdr->e32_objmap   = 0x124;
    pHdr->e32_itermap  = 0;
    pHdr->e32_rsrctab  = 0x184;
    pHdr->e32_rsrccnt  = 0x90;
    /* exe! */
    pHdr->e32_restab   = 0x964;
    pHdr->e32_enttab   = 0x96b;
    /* */
    pHdr->e32_dirtab   = 0;
    pHdr->e32_dircnt   = 0;
    pHdr->e32_fpagetab = 0x96c;
    pHdr->e32_frectab  = 0xda0;
    pHdr->e32_impmod   = 0x305a;
    pHdr->e32_impmodcnt= 0x0006;
    pHdr->e32_impproc  = 0x3085;
    pHdr->e32_pagesum  = 0;
    pHdr->e32_datapage = 0x364b;
    pHdr->e32_preload  = 0;
    pHdr->e32_nrestab  = 0;
    pHdr->e32_cbnrestab= 0;
    pHdr->e32_nressum  = 0;
    pHdr->e32_autodata = 3;
    pHdr->e32_debuginfo= 0;
    pHdr->e32_debuglen = 0;
    pHdr->e32_instpreload=0;
    pHdr->e32_instdemand =0;
    pHdr->e32_heapsize =0;
    pHdr->e32_stacksize=0;

}



/**
 * Apply changes to the TIBFix code for this object.
 * @remark  This code is connected to LXHeaderSuper::SaveConvertedNames
 */
void LXFile::AdjustTIB()
{
    /* 4th paramenter for RegisterResourceInfo/RegisterDLL */
    *(LONG *)&szTIBFix[TIBOffPe2lxVer] = PE2LX_VERSION;

    /* 3rd paramenter for RegisterResourceInfo/RegisterDLL */
    *(LONG *)&szTIBFix[TIBOffVerResId] = VersionResourceId;

    /* 2nd paramenter for RegisterResourceInfo/RegisterDLL */
    if (nrcvtnames)
        *(LONG*)&szTIBFix[TIBOffName] = cvtnametableid;
    else
        *(LONG*)&szTIBFix[TIBOffName] = 0x77777777;

    /* 1st parameter for RegisterResourceInfo/RegisterDLL */
    if (nrorgres)
        *(LONG*)&szTIBFix[TIBOffName+5] = (fConsole << 24) | orgrestableid;
    else
        *(LONG*)&szTIBFix[TIBOffName+5] = (fConsole << 24) | 0x888888;

    /* Entry Point */
    *(PLONG)&szTIBFix[TIBOffEntry] = TIBEntry;
}



/**
 * Query if szFilename is the name of a present PE module in the tree of Nodes.
 * @param  szFilename  Module name or filename name evt. with partially or full path.
 * @return TRUE if szFilename is the tree else FALSE.
 */
BOOL LXFile::queryIsModuleName(const char *szFilename)
{
    char fname[CCHMAXPATH];
    char *pfname;
    pfname = (char*)SSToDS(fname);
    strcpy(pfname, szFilename);
    UpCase(pfname);
    pfname = StripExtension(StripPath((char*)SSToDS(fname)));

    return strlen(pfname) == modulename[0] && strncmp(pfname, &modulename[1], modulename[0]) == 0;
}



/**
 * Queries the size of the virtual LX-File.
 * @return  Virtual LX-File size in bytes.
 */
ULONG LXFile::queryFileSize()
{
    if (nrresources > 0)
        return ResSection.lxoffset + ResSection.rawsize;
    else
        if (LXHdr.e32_objcnt != nrsections && nrsections > 1 && PESection != NULL)  //COMBINEDDATA
            return PESection[nrsections-2].lxoffset + PESection[nrsections-2].rawsize;
        else
            if  (nrsections > 0 && PESection != NULL)
            return PESection[nrsections-1].lxoffset + PESection[nrsections-1].rawsize;
            else
                return ~0;
}

