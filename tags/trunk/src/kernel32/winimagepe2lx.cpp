/* $Id: winimagepe2lx.cpp,v 1.11 2000-06-28 18:08:35 sandervl Exp $ */

/*
 * Win32 PE2LX Image base class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998-1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS          /* DOS Error values */
#define INCL_DOSPROFILE         /* DosQuerySysState (Toolkit 4.5) */
#define INCL_DOSMODULEMGR       /* DOS Module management */

#define ALIGN(a, alignment) (((a) + (alignment - 1UL)) & ~(alignment - 1UL))


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2wrap.h>            //Odin32 OS/2 api wrappers

#include <malloc.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>

#include <win32type.h>
#include <misc.h>
#include <winimagebase.h>
#include <winimagepe2lx.h>

#define DBG_LOCALLOG    DBG_winimagepe2lx
#include "dbglocal.h"

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#ifndef QS_MTE
   /* From OS/2 Toolkit v4.5 (BSEDOS.H) */

   /* Global Record structure
    * Holds all global system information. Placed first in user buffer
    */
   typedef struct qsGrec_s {  /* qsGrec */
           ULONG         cThrds;
           ULONG         c32SSem;
           ULONG         cMFTNodes;
   }qsGrec_t;

   /*
    *      System wide MTE information
    *      ________________________________
    *      |       pNextRec                |----|
    *      |-------------------------------|    |
    *      |       hmte                    |    |
    *      |-------------------------------|    |
    *      |       ctImpMod                |    |
    *      |-------------------------------|    |
    *      |       ctObj                   |    |
    *      |-------------------------------|    |
    *      |       pObjInfo                |----|----------|
    *      |-------------------------------|    |          |
    *      |       pName                   |----|----|     |
    *      |-------------------------------|    |    |     |
    *      |       imported module handles |    |    |     |
    *      |          .                    |    |    |     |
    *      |          .                    |    |    |     |
    *      |          .                    |    |    |     |
    *      |-------------------------------| <--|----|     |
    *      |       "pathname"              |    |          |
    *      |-------------------------------| <--|----------|
    *      |       Object records          |    |
    *      |       (if requested)          |    |
    *      |_______________________________|    |
    *                                      <-----
    *      NOTE that if the level bit is set to QS_MTE, the base Lib record will be followed
    *      by a series of object records (qsLObj_t); one for each object of the
    *      module.
    */

   typedef struct qsLObjrec_s {  /* qsLOrec */
           ULONG         oaddr;  /* object address */
           ULONG         osize;  /* object size */
           ULONG         oflags; /* object flags */
   } qsLObjrec_t;

   typedef struct qsLrec_s {     /* qsLrec */
           void  FAR        *pNextRec;      /* pointer to next record in buffer */
           USHORT           hmte;           /* handle for this mte */
           USHORT           fFlat;          /* true if 32 bit module */
           ULONG            ctImpMod;       /* # of imported modules in table */
           ULONG            ctObj;          /* # of objects in module (mte_objcnt)*/
           qsLObjrec_t FAR  *pObjInfo;      /* pointer to per object info if any */
           UCHAR     FAR    *pName;         /* -> name string following struc */
   } qsLrec_t;



   /* Pointer Record Structure
    *      This structure is the first in the user buffer.
    *      It contains pointers to heads of record types that are loaded
    *      into the buffer.
    */

   typedef struct qsPtrRec_s {   /* qsPRec */
           qsGrec_t        *pGlobalRec;
           void            *pProcRec;      /* ptr to head of process records */
           void            *p16SemRec;     /* ptr to head of 16 bit sem recds */
           void            *p32SemRec;     /* ptr to head of 32 bit sem recds */
           void            *pMemRec;       /* ptr to head of shared mem recs */
           qsLrec_t        *pLibRec;       /* ptr to head of mte records */
           void            *pShrMemRec;    /* ptr to head of shared mem records */
           void            *pFSRec;        /* ptr to head of file sys records */
   } qsPtrRec_t;

#endif


/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
#ifndef QS_MTE
   /* from OS/2 Toolkit v4.5 */

   APIRET APIENTRY DosQuerySysState(ULONG EntityList, ULONG EntityLevel, PID pid,
                                    TID tid, PVOID pDataBuf, ULONG cbBuf);
    #define QS_MTE         0x0004
#endif



/**
 * Constructor - creates an pe2lx image object from a module handle to a pe2lx module.
 * @param     hinstance   OS/2 module handle.
 * @param     fWin32k     TRUE:  Win32k module.
 *                        FALSE: Pe2Lx module.
 * @status    partially implemented.
 * @author    knut st. osmundsen, Sander van Leeuwen
 */
Win32Pe2LxImage::Win32Pe2LxImage(HINSTANCE hinstance, BOOL fWin32k)
    : Win32ImageBase(hinstance),
    paSections(NULL), cSections(0), pNtHdrs(NULL), fWin32k(fWin32k)
{
    setFullPath(szFileName);
}


/**
 * Free memory associated with this object.
 * @status    completely implemented.
 * @author    knut st. osmundsen, Sander van Leeuwen
 */
Win32Pe2LxImage::~Win32Pe2LxImage()
{
    cleanup();
}


/**
 * Initiates the object.
 * Must be called immediately after the object construction.
 * @returns   Success indicator, TRUE == success; FALSE = failure.
 * @sketch    Get section placement and sizes for this module. (paSections, cSections)
 *            Verify that there is at least one section - the header section.
 *            Locate the NT headers.
 *            Set pNtHdrs pointer.
 *            Validate the NT headers.
 *            Read the PE section table the set the RVAs in paSections.
 *            Locate and set the entrypoint.
 *            Locate the resource directory (if any). (pResRootDir, ulRVAResourceSection)
 *            TLS - FIXME!
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Object must be destroyed if failure!
 */
BOOL Win32Pe2LxImage::init()
{
    APIRET rc;

    /* Get section placement and sizes for this module. (paSections, cSections) */
    rc = getSections();
    if (rc != NO_ERROR)
    {
        dprintf(("Win32Pe2LxImage::Win32Pe2LxImage: error - getSection failed with rc=%d\n",
                 rc));
        return FALSE;
    }

    /* Verify that there is at least one section - the header section. */
    if (cSections < 1)
    {
        dprintf(("Win32Pe2LxImage::Win32Pe2LxImage: no header section, cSections is 0\n"));
        return FALSE;
    }

    /* Locate the NT headers. */
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)paSections[0].ulAddress;
    if ((pDosHdr->e_magic != IMAGE_DOS_SIGNATURE
        || pDosHdr->e_lfanew < sizeof(IMAGE_DOS_HEADER)  /* Larger than 64 bytes */
        || pDosHdr->e_lfanew > 0x04000000UL              /* or less that 64MB. */
        )
        && !*(PDWORD)paSections[0].ulAddress == IMAGE_NT_SIGNATURE
        )
    {
        dprintf(("Win32Pe2LxImage::Win32Pe2LxImage: Not a pe2lx image!(?)\n"));
        return FALSE;
    }

    /* Set pNtHdrs pointer. */
    if (pDosHdr->e_magic == IMAGE_DOS_SIGNATURE)
        pNtHdrs = (PIMAGE_NT_HEADERS)(paSections[0].ulAddress + pDosHdr->e_lfanew);
    else
        pNtHdrs = (PIMAGE_NT_HEADERS)paSections[0].ulAddress;

    /* Validate the NT headers. */
    if (pNtHdrs->Signature != IMAGE_NT_SIGNATURE
        || pNtHdrs->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC
        || pNtHdrs->FileHeader.Machine != IMAGE_FILE_MACHINE_I386
        || (cSections != 1 /* all in one object */
            && pNtHdrs->FileHeader.NumberOfSections
               > cSections - 1 - (pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL ? 0 : 1) /* hdr section and stack */
            )
        /* TODO: add more tests? */
        )
    {
        dprintf(("Win32Pe2LxImage::Win32Pe2LxImage: Not a pe2lx image!(?)\n"));
        return FALSE;
    }

    /* set RVAs */
    rc = setSectionRVAs();
    if (rc != NO_ERROR)
    {
        dprintf(("Win32Pe2LxImage::Win32Pe2LxImage: setSectionRVAs failed with rc=%d\n", rc));
        return FALSE;
    }

    /* Locate and set the entrypoint. */
    setEntryPoint((ULONG)getPointerFromRVA(pNtHdrs->OptionalHeader.AddressOfEntryPoint));
    if (entryPoint == 0UL &&
        (pNtHdrs->OptionalHeader.AddressOfEntryPoint != NULL        /* getPointerFromRVA failed... */
        || !(pNtHdrs->FileHeader.Characteristics & IMAGE_FILE_DLL)) /* EXEs must have and entry point! */
        )
    {
        dprintf(("Win32Pe2LxImage::Win32Pe2LxImage: entrypoint is incorrect, AddrOfEP=0x%08x, entryPoint=0x%08x\n",
                 pNtHdrs->OptionalHeader.AddressOfEntryPoint, entryPoint));
        return FALSE;
    }

    /* Locate the resource directory (if any) */
    if (pNtHdrs->OptionalHeader.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_RESOURCE
        && pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress > 0UL)
    {
        ulRVAResourceSection = pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
        pResRootDir = (PIMAGE_RESOURCE_DIRECTORY)getPointerFromRVA(ulRVAResourceSection);
    }

    /* TLS - Thread Local Storage */
    if (pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress != 0UL
        && pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size != 0UL)
    {
        PIMAGE_TLS_DIRECTORY pTLSDir;
        pTLSDir = (PIMAGE_TLS_DIRECTORY)getPointerFromRVA(pNtHdrs->OptionalHeader.
                                                          DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].
                                                          VirtualAddress);

        if (pTLSDir != NULL)
        {
            PVOID pv;
            ULONG ulBorlandRVAFix = 0UL;

            pv = getPointerFromRVA(pTLSDir->StartAddressOfRawData);
            /*
             * Borland seems to have problems getting things right...
             * Needs to subtract image base to make the TLSDir "RVA"s real
             * RVAs before converting them to pointers.
             */
            if ((pv == NULL || pTLSDir->StartAddressOfRawData == 0UL)
                && pTLSDir->StartAddressOfRawData > this->pNtHdrs->OptionalHeader.ImageBase)
                {
                ulBorlandRVAFix = this->pNtHdrs->OptionalHeader.ImageBase;
                pv = getPointerFromRVA(pTLSDir->StartAddressOfRawData - ulBorlandRVAFix);
                }
            if (pv == NULL || pTLSDir->StartAddressOfRawData == 0UL)
            {
                eprintf(("Win32Pe2LxImage::init: invalid RVA to TLS StartAddressOfRawData - %#8x.\n",
                         pTLSDir->StartAddressOfRawData));
                return FALSE;
            }
            setTLSAddress(pv);
            setTLSInitSize(pTLSDir->EndAddressOfRawData - pTLSDir->StartAddressOfRawData);
            setTLSTotalSize(pTLSDir->EndAddressOfRawData - pTLSDir->StartAddressOfRawData + pTLSDir->SizeOfZeroFill);
            pv = getPointerFromRVA((ULONG)pTLSDir->AddressOfIndex - ulBorlandRVAFix);
            if (pv == NULL)
            {
                eprintf(("Win32Pe2LxImage::init: invalid RVA to TLS AddressOffIndex - %#8x.\n",
                         pTLSDir->AddressOfIndex));
                return FALSE;
            }
            setTLSIndexAddr((LPDWORD)pv);
            pv = getPointerFromRVA((ULONG)pTLSDir->AddressOfCallBacks - ulBorlandRVAFix);
            if (pv == NULL)
            {
                eprintf(("Win32Pe2LxImage::init: invalid RVA to TLS AddressOffIndex - %#8x.\n",
                         pTLSDir->AddressOfIndex));
                return FALSE;
            }
            setTLSCallBackAddr((PIMAGE_TLS_CALLBACK*)pv);
        }
        else
        {
            eprintf(("Win32Pe2LxImage::init: invalid RVA to TLS Dir - %#8x. (getPointerFromRVA failed)\n",
                     pNtHdrs->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress));
            return FALSE;
        }
    }
    return TRUE;
}



/**
 * Gets the object layout for this module. Creates the paSections array.
 * @returns   OS2 errorcode. (NO_ERROR == success)
 * @sketch    Allocate output buffer.
 *            Call DosQuerySysState.
 *            IF buffer overflow THEN retry calling it with larger buffers, until buffer size is 1MB.
 *            IF success THEN
 *            BEGIN
 *                Find record for this module.
 *                IF record found THEN allocate memory for object table and copy it. (paSections, cSections)
 *            END
 * @status    Completely implemented.
 * @author    knut st. osmundsen
 */
ULONG  Win32Pe2LxImage::getSections()
{
    APIRET rc = NO_ERROR;
    qsPtrRec_t *    pPtrRec;
    ULONG           cbBuf = 65536;

    pPtrRec = (qsPtrRec_t *)malloc(cbBuf);
    if (pPtrRec != NULL)
    {
        rc = DosQuerySysState(QS_MTE, QS_MTE, getpid(), 0L, pPtrRec, cbBuf);
        while (cbBuf < 1024*1024 && rc == ERROR_BUFFER_OVERFLOW)
        {
            PVOID pv = pPtrRec;
            cbBuf +=  65536;
            pPtrRec = (qsPtrRec_t *)realloc(pv, cbBuf);
            if (pPtrRec != NULL)
                rc = DosQuerySysState(QS_MTE, QS_MTE, getpid(), 0L, pPtrRec, cbBuf);
            else
            {
                rc = ERROR_NOT_ENOUGH_MEMORY;
                free(pv);
            }
        }

        if (rc == NO_ERROR)
        {
            qsLrec_t *  pLrec = pPtrRec->pLibRec;
            while (pLrec != NULL)
            {
                /*
                 * Bug detected in OS/2 FP13. Probably a problem which occurs
                 * in _LDRSysMteInfo when qsCheckCache is calle before writing
                 * object info. The result is that the cache flushed and the
                 * attempt of updating the qsLrec_t next and object pointer is
                 * not done. This used to work earlier and on Aurora AFAIK.
                 *
                 * The fix for this problem is to check if the pObjInfo is NULL
                 * while the number of objects isn't 0 and correct this. pNextRec
                 * will also be NULL at this time. This will be have to corrected
                 * before we exit the loop or moves to the next record.
                 * There is also a nasty alignment of the object info... Hope
                 * I got it right. (This aligment seems new to FP13.)
                 */
                if (pLrec->pObjInfo == NULL /*&& pLrec->pNextRec == NULL*/ && pLrec->ctObj > 0)
                    {
                    pLrec->pObjInfo = (qsLObjrec_t*)(
                        (char*)pLrec
                        + ((sizeof(qsLrec_t)                     /* size of the lib record */
                           + pLrec->ctImpMod * sizeof(short)    /* size of the array of imported modules */
                           + strlen((char*)pLrec->pName) + 1    /* size of the filename */
                           + 3) & ~3));                          /* the size is align on 4 bytes boundrary */
                    pLrec->pNextRec = (qsLrec_t*)((char*)pLrec->pObjInfo
                                                   + sizeof(qsLObjrec_t) * pLrec->ctObj);
                    }
                if (pLrec->hmte == hinstance)
                    break;

                /*
                 * Next record
                 */
                pLrec = (qsLrec_t*)pLrec->pNextRec;
            }


            if (pLrec)
            {
                if (pLrec->pObjInfo != NULL)
                {
                    /* Allocate memory for paSections */
                    paSections = (PSECTION)malloc(pLrec->ctObj*sizeof(SECTION));
                    if (paSections != NULL)
                    {
                        /* objects -> section array */
                        for (int i = 0; i < pLrec->ctObj; i++)
                        {
                            paSections[i].ulRVA = ~0UL;
                            paSections[i].cbVirtual = pLrec->pObjInfo[i].osize;
                            paSections[i].ulAddress = pLrec->pObjInfo[i].oaddr;
                        }
                        cSections = pLrec->ctObj;
                    }
                    else
                        rc = ERROR_NOT_ENOUGH_MEMORY;
                }
                else
                {
                    rc = ERROR_BAD_EXE_FORMAT;
                    dprintf(("Win32Pe2LxImage::getSections: Error - no object table!\n"));
                }
            }
            else
                rc = ERROR_MOD_NOT_FOUND;
        }
        else
            dprintf(("DosQuerySysState - failed with rc=%d (cbBuf=%d)\n", rc, cbBuf));

        if (pPtrRec != NULL)
            free(pPtrRec);
    }
    else
        rc = ERROR_NOT_ENOUGH_MEMORY;

    return rc;
}


/**
 * Sets the ulRVA according to the original PE section table.
 * @returns   OS/2 errorcode. (NO_ERROR == success)
 * @sketch    DEBUG: Validate pNtHdrs
 *            Make pointer to start of PE section table.
 *            Set RVA for the header section.
 *            IF not all in one object exe? THEN
 *                Loop thru the sections in the PE section table.
 *                  Note: due to the header section: PE section no. + 1 == LX object no.
 *            ELSE
 *            BEGIN
 *                (try) Reallocate paSections to NumberOfSections + 3.
 *                Loop thru the PE sections and make paSections from them.
 *                Add final Stack or TIBFix+Stack section if necessary.
 *                Resize header section.
 *            END
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Must not be called before pNtHdrs is set.
 */
ULONG Win32Pe2LxImage::setSectionRVAs()
{
    #if DEBUG
    if (pNtHdrs == NULL)
    {
        DebugInt3();
        return ERROR_INVALID_PARAMETER;
    }
    #endif

    PIMAGE_SECTION_HEADER paPESections = (PIMAGE_SECTION_HEADER)
        ((unsigned)pNtHdrs + sizeof(*pNtHdrs) +
         (pNtHdrs->OptionalHeader.NumberOfRvaAndSizes - IMAGE_NUMBEROF_DIRECTORY_ENTRIES) * sizeof(IMAGE_DATA_DIRECTORY)
        );

    /* set RVA for the header section to 0UL. */
    paSections[0].ulRVA = 0UL;

    /* All in one object exe? */
    if (pNtHdrs->FileHeader.NumberOfSections < cSections)
    {
        /* loop thru the other sections */
        for (int i = 0; i < pNtHdrs->FileHeader.NumberOfSections; i++)
            paSections[i+1].ulRVA = paPESections[i].VirtualAddress;
    }
    else
    {   /* all in one object */
        /* (try) Reallocate paSections to NumberOfSections + 3. */
        PVOID pv = realloc(paSections, sizeof(paSections[0]) * (pNtHdrs->FileHeader.NumberOfSections + 3));
        if (pv != NULL)
        {
            paSections = (PSECTION)pv;

            /* loop thru the PE sections */
            for (int i = 0; i < pNtHdrs->FileHeader.NumberOfSections; i++)
            {
                if (paSections[0].cbVirtual < paPESections[i].VirtualAddress)
                {
                    dprintf(("Win32Pe2LxImage::setSectionRVAs: mismatch between section table and all-in-one-object"
                             "paSections[0].cbVirtual %#x  paPESections[i].VirtualAddress %#x\n",
                             paSections[0].cbVirtual, paPESections[i].VirtualAddress
                             ));
                    return ERROR_BAD_EXE_FORMAT;
                }
                paSections[i+1].ulRVA = paPESections[i].VirtualAddress;
                paSections[i+1].cbVirtual = max(paPESections[i].Misc.VirtualSize, paPESections[i].SizeOfRawData);
                paSections[i+1].ulAddress = paSections[0].ulAddress + paPESections[i].VirtualAddress;
            }
            cSections = pNtHdrs->FileHeader.NumberOfSections + 1;

            /* add final Stack or TIBFix+Stack section if necessary */
            if (paSections[0].cbVirtual > paSections[i].ulRVA + ALIGN(paSections[i].cbVirtual, 0x1000))
            {
                paSections[i+1].ulRVA = ~0UL;
                paSections[i+1].cbVirtual = paSections[0].cbVirtual - paSections[i].ulRVA + ALIGN(paSections[i].cbVirtual, 0x1000);
                paSections[i+1].ulAddress = paSections[i].ulAddress + ALIGN(paSections[i].cbVirtual, 0x1000);
                i++;
                cSections++;
            }

            /* resize header section */
            paSections[0].cbVirtual = paSections[1].ulRVA; /*....*/
        }
        else
            return ERROR_NOT_ENOUGH_MEMORY;
    }

    return NO_ERROR;
}


/**
 * Frees memory used by this object.
 * When an exception is to be thrown, this function is called first to clean up
 * the object. Base class(es) are automatically clean up by theire destructor(s).
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
VOID  Win32Pe2LxImage::cleanup()
{
    if (paSections != NULL)
    {
        free(paSections);
        paSections = NULL;
        cSections = 0;
    }
}


/**
 * Converts a RVA to an pointer.
 * @returns   Pointer matching the given RVA, NULL on error.
 * @param     ulRVA  An address relative to the imagebase of the original PE image.
 *                   If this is 0UL NULL is returned.
 * @sketch    DEBUG: validate state, paSections != NULL
 *            IF ulRVA is 0 THEN return NULL
 *            LOOP while more section left and ulRVA is not within section
 *                next section
 *            IF section matching ulRVA is not found THEN fail.
 *            return pointer matching RVA.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Should not be called until getSections has returned successfully.
 *            RVA == 0 is ignored.
 */
PVOID  Win32Pe2LxImage::getPointerFromRVA(ULONG ulRVA)
{
    int i;
    #ifdef DEBUG
        if (paSections == NULL)
            return NULL;
    #endif

    if (ulRVA == 0UL)
        return NULL;

    i = 0;
    while (i < cSections &&
           !(paSections[i].ulRVA <= ulRVA && paSections[i].ulRVA + paSections[i].cbVirtual > ulRVA)) /* ALIGN on page too? */
        i++;

    if (i >= cSections)
        return NULL;

    return (PVOID)(ulRVA - paSections[i].ulRVA + paSections[i].ulAddress);
}

/**
 * Gets pointer to an exported procedure by procedure name.
 * @returns   Address of exported procedure. 0UL if not found.
 * @param     name  Exported procedure name.
 * @status    completely implemented.
 * @author    Sander van Leeuwen
 * @remark
 */
ULONG Win32Pe2LxImage::getApi(char *name)
{
    APIRET      rc;
    ULONG       ulApiAddr;

    rc = DosQueryProcAddr(hinstance, 0, name, (PFN *)&ulApiAddr);
    return rc == NO_ERROR ? ulApiAddr : 0;
}


/**
 * Gets pointer to an exported procedure by ordinal.
 * @returns   Pointer to an exported procedure. 0UL if not found.
 * @param     ordinal  Export ordinal number.
 * @status    completely implemented.
 * @author    Sander van Leeuwen
 * @remark    FIXME:
 *            This function should be implemented for both Exe and Dll images!
 *            It could be done similar in both peldr image and pe2lx images by
 *            accessing PE structures.
 */
ULONG Win32Pe2LxImage::getApi(int ordinal)
{
    APIRET      rc;
    ULONG       ulApiAddr;

    rc = DosQueryProcAddr(hinstance, ordinal, NULL, (PFN *)&ulApiAddr);

    return rc == NO_ERROR ? ulApiAddr : 0;
}
