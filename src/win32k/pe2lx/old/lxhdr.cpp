/* $Id: lxhdr.cpp,v 1.1 1999-10-06 18:50:53 bird Exp $
 *
 * PE2LX PE image interpreter
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998-1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*
 * NOTE: NOT RING0 RING3 only!
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define OPTHDROFFSET(a) ((LPVOID)((BYTE *)a + \
        ((PIMAGE_DOS_HEADER)a)->e_lfanew    + \
        SIZE_OF_NT_SIGNATURE                + \
        sizeof(IMAGE_FILE_HEADER)))



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "pe2lx.h"  //KSO Tue 2nd June 1998


/**
 * Global LXHeader Object
 */
LXHeader OS2Exe;



/**
 * Constructor
 */
LXHeader::LXHeader() : LXHeaderSuper()
{

}


/**
 * Destructor
 */
LXHeader::~LXHeader()
{
    //LXHeaderSuper::~LXHeaderSuper();
}



/**
 * Saves the LX-File.
 * @param filename  The name of the LX-file to be created.
 */
BOOL LXHeader::SaveNewExeFile(char *filename)
{
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = (PIMAGE_OPTIONAL_HEADER)OPTHDROFFSET(pBaseFile);
    HFILE  win32handle;
    ULONG  ulAction       = 0;        /* Action taken by DosOpen */
    ULONG  ulLocal        = 0;        /* File pointer position after DosSetFilePtr */
    ULONG  ulWrite;
    APIRET rc;
    struct o32_obj objtable;
    struct o32_map *objpage;
    int pagetblidx, i, j, k, idx, rawsize, objectsize;
    int nrpages, pagedataoffset;
    //BOOL fFirstCode = FALSE; //KSO Aug 16 1998: no TIB to think so much about any longer
    int *fixuptable;
    char modname[128+5];//KSO Aug 16 1998: +5 just to be sure
    intfixup      *frec;
    namefixup     *imprec;
    realintfixup  *frealrec, *currealrec;

    rc = DosOpen(filename,                              /* File path name */
                     &win32handle,                          /* File handle */
                     &ulAction,                             /* Action taken */
                     0L,                                        /* File primary allocation */
                     0L,                          /* File attribute */
                     OPEN_ACTION_CREATE_IF_NEW |
                     OPEN_ACTION_REPLACE_IF_EXISTS,     /* Open function type */
                     OPEN_FLAGS_NOINHERIT |
                     OPEN_SHARE_DENYNONE  |
                     OPEN_ACCESS_READWRITE,             /* Open mode of the file */
                     0L);                                       /* No extended attribute */

    if (rc != NO_ERROR)
    {
        cout << "DosOpen returned " << rc << endl;
        return (FALSE);
    }
    /* Move the file pointer back to the beginning of the file */
    DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

    rc = DosWrite(win32handle, achDosHeader, sizeof(achDosHeader), &ulWrite);
    if (rc != NO_ERROR)
    {
        cout << "DosWrite returned " << rc << endl;
        DosClose(win32handle);                    /* Close the file */
        return (FALSE);
    }
    //write preliminary lx header
    DosWrite(win32handle, &LXHdr, sizeof(LXHdr), &ulWrite);

    //First take care of the resources (if present), see GetNrPages()
    //works correctly
    if (nrresources)
    {
        ResSection.virtualsize = ResSection.curoff;
        ResSection.rawsize     = ResSection.curoff;
    } //done with resources

    pagetblidx            = 1;   //start met 1
    objtable.o32_reserved = 0;
    pagedataoffset        = 0;
    nrpages               = GetNrPages();
    //SvL: 13 aug 1998
    //objpage               = (struct o32_map *)malloc((nrpages-(StackSize+0xFFF>>PAGE_SHIFT))*sizeof(o32_map));
    objpage               = (struct o32_map *)malloc(nrpages*sizeof(o32_map));

    LXHdr.e32_objcnt      = GetNrObjects2();
    LXHdr.e32_mpages      = 0;
    idx                   = -1;
    int objcnt            = 1;

    for (i=0;i<nrsections;i++)
    {
        objtable.o32_pagemap = pagetblidx;
        if (PESection[i].fInvalid)   continue;

        switch (PESection[i].type)
        {
            case (SECTION_CODE | SECTION_TIBFIX): //KSO Aug 15 1998: TIBFixObject
            case (SECTION_CODE | SECTION_IMPORT):
            case SECTION_CODE:
                objtable.o32_flags = OBJREAD | NSEXRD | OBJBIGDEF;
                //if (fFirstCode == FALSE) //KSO Aug 16 1998: Entrypoint fixup
                //  LXHdr.e32_startobj = i+1;
                break;
            case SECTION_IMPORT:
                objtable.o32_flags = OBJREAD | OBJBIGDEF;
                break;
            case SECTION_INITDATA:
                objtable.o32_flags = OBJREAD | OBJWRITE | OBJBIGDEF;
                break;
            case SECTION_COMBINEDDATA:
                objtable.o32_flags = OBJREAD | OBJWRITE | OBJBIGDEF;
                //LXHdr.e32_startobj = 1;
                break;
            case SECTION_RESOURCE_ORG:
            case SECTION_UNINITDATA:
                //SvL: Zero filled flag maps on Permanent flag, so ditch it
                objtable.o32_flags = OBJREAD | OBJWRITE | OBJBIGDEF; //read/write/big
                break;
            case SECTION_READONLYDATA:
                objtable.o32_flags = OBJREAD | OBJBIGDEF;
                break;
        }
        //OS/2 doesn't seem to mind if the sizes of all the pages in an object
        //exceeds the object size....
        if (PESection[i].type == SECTION_UNINITDATA)
            objectsize = PESection[i].virtualsize;  //rawsize == 0
        else    objectsize = PESection[i].rawsize;

        objtable.o32_mapsize = PESection[i].nrpages;

        objtable.o32_mapsize += PESection[i].nrinvalidpages;
        objtable.o32_base     = PESection[i].address;
        objtable.o32_size     = objtable.o32_mapsize*PAGE_SIZE;

        rawsize = PESection[i].rawsize;
        for (j=0;j<objtable.o32_mapsize;j++)
        {
            idx = pagetblidx+j-1;
            objpage[idx].o32_pagedataoffset = pagedataoffset;

            if (j < PESection[i].nrinvalidpages)
            {
                objpage[idx].o32_pagesize    = 0;
                //SvL: Invalid pages are not properly handled by the OS/2 loader -> make 'em zeroed
                objpage[idx].o32_pageflags   = ZEROED;
            }
            else
            {
                if (PESection[i].type == SECTION_COMBINEDDATA)
                {
                    //KSO Aug 16 1998: There is no longer a TIBFix in COMBINEDDATA - code removed
                    int didx = j - PESection[i].nrinvalidpages;
                    objpage[idx].o32_pagesize  = datapage[didx].size;
                    objpage[idx].o32_pageflags = datapage[didx].flags;
                    pagedataoffset += objpage[idx].o32_pagesize;
                    if (objpage[idx].o32_pageflags == 0)
                        LXHdr.e32_mpages++;

                }
                else
                    if (PESection[i].type == SECTION_UNINITDATA || rawsize == 0)
                {
                    objpage[idx].o32_pagesize       = 0;
                    objpage[idx].o32_pagedataoffset = pagedataoffset;
                    objpage[idx].o32_pageflags      = ZEROED;
                }
                else
                {  //KSO Aug 16 1998: TIBFixObject - TIBFix code removed
                    if (rawsize > PAGE_SIZE)
                        objpage[idx].o32_pagesize = PAGE_SIZE;
                    else
                        objpage[idx].o32_pagesize = rawsize;
                    rawsize                    -= objpage[idx].o32_pagesize;
                    objpage[idx].o32_pageflags  = 0;
                    LXHdr.e32_mpages++;
                    pagedataoffset             += objpage[idx].o32_pagesize;
                }
            }
        }

        if (IsEXE && i == nrsections - 1)
        {//SvL: 25-7-'97 Add stack pages to last data object
            LXHdr.e32_stackobj     = objcnt;
            LXHdr.e32_autodata     = LXHdr.e32_stackobj;
            objtable.o32_flags    |= OBJWRITE;
            objtable.o32_size     += StackSize;
            LXHdr.e32_esp          = objtable.o32_size; //highest address in object
        }

        //Write object table entry to file
        DosWrite(win32handle, &objtable, sizeof(objtable), &ulWrite);

        pagetblidx    += objtable.o32_mapsize;
        objcnt++;
    }


    /* KSO Aug 15 1998: EntryPoint - TIBFixObject */
    // Sets entrypoint to TIBFixSection and changes the TIBFix call
    int iPESection     = GetTIBSectionIdx();
    LXHdr.e32_eip      = 0;
    LXHdr.e32_startobj = GetTIBObjectNo();
    ltassert(LXHdr.e32_startobj != -1 && iPESection != -1);
    *(PULONG)&szTIBFix[TIBOffEntry] = pOptionalHeader->AddressOfEntryPoint //RVA
        + pOptionalHeader->ImageBase
        - (PESection[iPESection].address + TIBOffCall + SIZE_TIBCALL);



    if (nrresources)
    {
        //Correct count in case there are string tables which aren't completely filled
        nrresources = curresource;
        //KSO: already done by GetNrObjects2 //LXHdr.e32_objcnt++;
        objtable.o32_pagemap   = pagetblidx;
        objtable.o32_flags     = NSSHARED | OBJREAD | OBJRSRC | OBJBIGDEF;
        objtable.o32_mapsize   = ResSection.rawsize >> PAGE_SHIFT;

        objtable.o32_base      = 0; //SvL: Set to 0 in PM exes (plf)
        objtable.o32_size      = ResSection.rawsize;

        for (j=0;j<objtable.o32_mapsize;j++)
        {
            idx = pagetblidx+j-1;
            objpage[idx].o32_pagedataoffset = pagedataoffset;
            objpage[idx].o32_pagesize       = PAGE_SIZE;
            objpage[idx].o32_pageflags      = 0;
            LXHdr.e32_mpages++;
            pagedataoffset           += PAGE_SIZE;
        }
        if ((objtable.o32_size & 0xFFF) != 0)
        {
            objtable.o32_mapsize++;
            LXHdr.e32_mpages++;
            idx++;
            objpage[idx].o32_pagedataoffset = pagedataoffset;
            objpage[idx].o32_pagesize       = (USHORT)(objtable.o32_size & 0xFFF);
            objpage[idx].o32_pageflags      = 0;
            pagedataoffset           += objpage[idx].o32_pagesize;
        }

        //Write resource object table to file
        DosWrite(win32handle, &objtable, sizeof(objtable), &ulWrite);
        pagetblidx    += objtable.o32_mapsize;

        //fill in object nr's in resource table entries
        for (i=0;i<nrresources;i++)
        {
            os2resource[i].obj = LXHdr.e32_objcnt;
        }
    }

    //Write object page table entries to file
    //DosWrite(win32handle, objpage, sizeof(*objpage)*(nrpages-(StackSize+0xFFF>>PAGE_SHIFT)), &ulWrite);
    DosWrite(win32handle, objpage, sizeof(*objpage)*nrpages, &ulWrite);

    //object page records offset:
    LXHdr.e32_objmap    = LXHdr.e32_objtab + LXHdr.e32_objcnt*sizeof(objtable);
    //size tables vanaf object table t/m impproc (eigenlijk pagesum ook..)
    //LXHdr.e32_ldrsize   = LXHdr.e32_objcnt*sizeof(objtable) + (nrpages-(StackSize+0xFFF>>PAGE_SHIFT))*sizeof(o32_map);
    LXHdr.e32_ldrsize   = LXHdr.e32_objcnt*sizeof(objtable) + nrpages*sizeof(o32_map);

    //resource table:
    LXHdr.e32_rsrctab   = LXHdr.e32_ldrsize + LXHdr.e32_objtab;
    LXHdr.e32_rsrccnt   = nrresources;
    if (nrresources)
    {
        DosWrite(win32handle, (char *)os2resource, nrresources*sizeof(rsrc32), &ulWrite);
    }
    LXHdr.e32_ldrsize  += nrresources*sizeof(rsrc32);

    //resident name table + entry table:
    LXHdr.e32_restab    = LXHdr.e32_rsrctab + nrresources*sizeof(rsrc32);


#if 1 //KSO Aug 16 1998: exports from exes and dll without exports: TODO cleanup!
    //write DLL/EXE export information
    if (exports)
    {
        int len = 0;

        //save resident name table
        if (exports)
        {
            len = curexport - exports + 1;  // + 0 byte to signal end of table
            rc = DosWrite(win32handle, exports, len, &ulWrite);
            LXHdr.e32_ldrsize += len;
        }

        //save entry table
        LXHdr.e32_enttab = LXHdr.e32_restab + len;
        //TODO: Forwarders!
        len = (int)curexpb - (int)expbundle + 1; // + 0 byte to signal end of table
        if (len > 1)
        {
            rc = DosWrite(win32handle, expbundle, len, &ulWrite);
            LXHdr.e32_ldrsize += len;
        }
    }
    else
    {
        int modlen;
        //save resident name table
        memset(modname, 0, sizeof(modname));
        strcpy(modname, modulename);
        modlen = strlen(&modname[1]);
        modlen += 1 + 2 + 1;    //len byte + ord word + 0 byte
        DosWrite(win32handle, modname, modlen, &ulWrite);

        //save entry table
        LXHdr.e32_enttab    = LXHdr.e32_restab + modlen; //points to 0 byte
        DosWrite(win32handle, "", 1, &ulWrite);
        LXHdr.e32_ldrsize  += 1 + modlen;
    }

#else

    if (IsEXE == TRUE)
    {
        int modlen;
        //save resident name table
        //KSO: modname has right format - Jun 16 1998 4:27pm
        memset(modname, 0, sizeof(modname));
        strcpy(modname, modulename);
        //strcpy(&modname[1], modulename);
        //UpCase(&modname[1]); // this is already done!
        modlen = strlen(&modname[1]);
        //modname[0] = (char)modlen;
        modlen += 1 + 2 + 1;    //len byte + ord word + 0 byte
        DosWrite(win32handle, modname, modlen, &ulWrite);

        //save entry table
        LXHdr.e32_enttab    = LXHdr.e32_restab + modlen;    //points to 0 byte
        modname[0] = 0;
        DosWrite(win32handle, modname, 1, &ulWrite);
        LXHdr.e32_ldrsize  += 1 + modlen;
    }
    else
    {//write DLL export information
        int len = 0;

        //save resident name table
        if (exports)
        {
            len = curexport - exports + 1;  // + 0 byte to signal end of table
            rc = DosWrite(win32handle, exports, len, &ulWrite);
            LXHdr.e32_ldrsize += len;
        }

        //save entry table
        LXHdr.e32_enttab = LXHdr.e32_restab + len;
        //TODO: Forwarders!
        len = (int)curexpb - (int)expbundle + 1; // + 0 byte to signal end of table
        if (len > 1)
        {
            rc = DosWrite(win32handle, expbundle, len, &ulWrite);
            LXHdr.e32_ldrsize += len;
        }
    }
#endif

    /* FIXUPS */
    //#pages * offset naar import fixups + 1 eind offset
    LXHdr.e32_fpagetab  = LXHdr.e32_ldrsize + LXHdr.e32_objtab;
    LXHdr.e32_frectab   = LXHdr.e32_fpagetab + (nrpages+1)*sizeof(int);
    fixuptable = (int *)malloc((nrpages+1)*sizeof(int));
    for (i=0;i<nrpages+1;i++)   fixuptable[i] = 0;

    //dump alle fixup records in 1 memory block en bepaal offsets in fixup page table
    frec       = &intfixuprec[0];
    imprec     = &impfixuprec[0];
    //KSO Aug 15 1998: TIBFixObject - "+ 1" (internal entrypoint fixup)
    frealrec   = (realintfixup *)malloc(
                                                    (nrintfixups + 1)            * sizeof(realintfixup)
                                                  + (nrimpfixups + EXTRA_FIXUPS) * sizeof(realnamefixup)
                                                  );
    /* KSO Aug 16 1998: we need the pageno of the TIBSection */
    int TIBPageNo = GetTIBSectionIdx();
    ltassert(TIBPageNo != 1);
    TIBPageNo = GetPageNoByVA(PESection[TIBPageNo].address);

    currealrec = frealrec;
    j          = 0;
    k          = 0;
    for (i=0;i<nrpages;i++)
    {
        fixuptable[i] = (int)currealrec - (int)frealrec;
        if (i == TIBPageNo)
        {//add kernel32:registerresourceinfo import
            realordfixup *kerord = (realordfixup *)currealrec;
            kerord->nr_stype   = 0x07;      //32 bits offset
            kerord->nr_flags   = 0x01 | 0x40;   //import by ordinal + mod offset = 16 bits
            kerord->r32_soff   = TIBOffKerImport;
            kerord->r32_objmod = kernel32Object + 1;
            if (IsEXE)
                kerord->ord        = ORD_REGISTERRESOURCEINFO;
            else
                kerord->ord          = ORD_REGISTERDLL;

            currealrec = (realintfixup *)((int)currealrec + sizeof(realnamefixup));
            if (!IsEXE)
            {
                kerord = (realordfixup *)currealrec;
                kerord->nr_stype   = 0x08;      //32 bits self referencing offset
                kerord->nr_flags   = 0x01 | 0x40;   //import by ordinal + mod offset = 16 bits
                kerord->r32_soff   = DLL_OFF_KERIMP2;
                kerord->r32_objmod = kernel32Object + 1;
                kerord->ord        = ORD_KERNEL32DLLEXITLIST;
                currealrec = (realintfixup *)((int)currealrec + sizeof(realnamefixup));
            }

            /* add entrypoint internal fixup */
            realintfixup *pEntInt = currealrec;
            int SectNo;
            pEntInt->nr_stype    = 0x08; //self referencing
            pEntInt->nr_flags    = 0x00 | 0x10 | 0x40;
            pEntInt->r32_soff    = TIBOffEntry;
            pEntInt->targetaddr  = pOptionalHeader->ImageBase + pOptionalHeader->AddressOfEntryPoint;
            pEntInt->targetobj   = GetObjectNoByVA(pEntInt->targetaddr);
            ltassert(pEntInt->targetobj != 0);
            SectNo               = GetSectionIdxByVA(pEntInt->targetaddr);
            ltassert(SectNo != -1);
            pEntInt->targetaddr -= PESection[SectNo].address;
            currealrec++;
        }

        //SvL: 16-9-'97, for multiple import pages
        while (k < nrimpfixups && imprec->srcpage == i)
        {
            //sizeof(realordfixup) == sizeof(realnamefixup)
            memcpy(currealrec, imprec, sizeof(realnamefixup));
            currealrec = (realintfixup *)((int)currealrec + sizeof(realnamefixup));
            imprec++;
            k++;
        }
        while (j < nrintfixups && frec->srcpage == i)
        {
            memcpy(currealrec, frec, sizeof(realintfixup));
            //cout << "src address " << frec->r32_soff << " srcpage " << frec->srcpage << " target " << frec->targetaddr << " frec " << (int)frec << endl;
            frec++;
            currealrec++;
            j++;
        }
    }
    fixuptable[nrpages] = (int)currealrec - (int)frealrec;

    //Add extra kernel32 imports
    if (IsEXE)
        nrimpfixups++;
    else    nrimpfixups += EXTRA_FIXUPS;

    nrintfixups += 1; //KSO Aug 15 1998: TIBFixObject internal entrypoint fixup

    //Write fixup page table
    DosWrite(win32handle, fixuptable, (nrpages+1)*sizeof(int), &ulWrite);
    //Write fixup record table
    DosWrite(win32handle, frealrec, (int)currealrec - (int)frealrec, &ulWrite); //KSO Aug 15 1998: calc of fixupsize.

    //fixup page + record + import name + module table size
    LXHdr.e32_fixupsize = (nrpages+1)*sizeof(int) + (int)currealrec - (int)frealrec + impmodulesize + impnamesize;  //KSO Aug 15 1998: calc of fixupsize.

    //size tables vanaf object table t/m impproc (eigenlijk pagesum ook..)
    LXHdr.e32_ldrsize  += LXHdr.e32_fixupsize;

    //offset naar module name array
    LXHdr.e32_impmod    = LXHdr.e32_frectab + (int)currealrec - (int)frealrec;  //KSO Aug 15 1998: calc of fixupsize.
    //offset naar proc name array
    LXHdr.e32_impproc   = LXHdr.e32_impmod + impmodulesize;
    //Write Import Module array
    DosWrite(win32handle, impmodules, impmodulesize, &ulWrite);
    //Write Import Procedure Name array
    DosWrite(win32handle, impnames, impnamesize, &ulWrite);

    //pointer naar begin code/data
    LXHdr.e32_datapage  = LXHdr.e32_impproc + impnamesize + LXHEADER_OFFSET; //vanaf begin ipv lxheader

    free(frealrec);
    free(fixuptable);


    for (i=0;i<nrsections;i++)
    {
        //zero filled pages, niet nodig om op te slaan
        if (PESection[i].type != SECTION_UNINITDATA && PESection[i].rawdata)
            DosWrite(win32handle, PESection[i].rawdata, PESection[i].rawsize, &ulWrite);
          //KSO Aug 16 1998: Removed some TIB-code
    }
    if (nrresources)
        DosWrite(win32handle, ResSection.rawdata, ResSection.rawsize, &ulWrite);

    DosSetFilePtr(win32handle, LXHEADER_OFFSET, FILE_BEGIN, &ulLocal);
    //write final lx header
    DosWrite(win32handle, &LXHdr, sizeof(LXHdr), &ulWrite);

    free(objpage);
    DosClose(win32handle);                    /* Close the file */

    return (TRUE);
}

