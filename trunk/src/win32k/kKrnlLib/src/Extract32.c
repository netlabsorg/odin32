/* $Id: Extract32.c,v 1.2 2002-12-19 01:49:07 bird Exp $
 *
 * This is used to get opcodes for a kernel.
 * (Was 32bit part of mixed program, now everything is 32-bit.)
 *
 * Copyright (c) 2001-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define SEL_FLATMASK        0x01fff0000
#define SEL_FLAT_SHIFT      0x0d
#define SEL_LDT_RPL3        0x07

#define SelToFlat(sel, off) \
    (PVOID)( (((unsigned)(sel) << SEL_FLAT_SHIFT) & SEL_FLAT_MASK) + (unsigned)(off))

#define FlatToSel(flataddr) \
    (PVOID)( ( (((unsigned)(flataddr) << 3) & 0xfff80000) | (SEL_LDT_RPL3 << 16) ) | ((unsigned)(flataddr) & 0xffff) )

#define SSToDS(a) ((void*)(a))


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <kLib/format/LXexe.h>

#define INCL_BASE
#include <os2.h>
#define INCL_OS2KRNL_ALL
#include "os2krnl.h"

#include "krnlImportTable.h"
#include "krnlPrivate.h"

#include <stdio.h>
#include <string.h>



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#if !defined(QS_MTE) || defined(QS_MODVER)
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
*   Global Variables                                                           *
*******************************************************************************/
int             cObjects = 14;
OTE             aKrnlOTE[42];
HMODULE         hmodKrnl = NULLHANDLE;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int  kernelInit(int iTest, int argc, char **argv);


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
 * This function retrives the opcodes for all the kernel functions which
 * was found. The usual validation is done.
 *
 * @returns 0 on succes.
 *          error code with description in pszKrnlFile16 on failure.
 * @param   pszKrnlFile16   Pointer to buffer which on entry holds the
 *                          kernel filename. On failure this will describe
 *                          the problem which occured.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark  This function is called from 16-bit code.
 *          Assumes that Init32bitCrt is called.
 */
int GetOpcodes(char * pszKrnlFile, unsigned cSymObjects)
{
    int  rc = 0;
    char *  argv[4] = {NULL, NULL, NULL, NULL};
    argv[2] = pszKrnlFile;


    /*
     * Init the kernel.
     */
    if (kernelInit(1, 3, argv))
    {
        /*
         * Validate object count.
         */
        if (cObjects == (int)cSymObjects)
        {
            int i;
            for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
            {
                int     cb = 5;
                int     iObj;

                if (!aImportTab[i].fFound || EPTVar(aImportTab[i]))
                    continue;

                /*
                 * Check segment limits.
                 */
                iObj = aImportTab[i].iObject;
                if (aImportTab[i].offObject >= aKrnlOTE[iObj].ote_size)
                {
                    fprintf(stderr, "Func %d offset out of object %d (0x%x >= 0x%x)\n",
                            i, aImportTab[i].offObject, aKrnlOTE[iObj].ote_size);
                    rc = 1;
                    break;
                }

                /*
                 * Verify function prolog if code.
                 */
                if ((aKrnlOTE[iObj].ote_flags & (OBJBIGDEF | OBJEXEC)) == (OBJBIGDEF | OBJEXEC))
                {   /* 32-bit */
                    cb = krnlInterpretProlog32((char*)aKrnlOTE[iObj].ote_base + aImportTab[i].offObject);
                }
                else if ((aKrnlOTE[iObj].ote_flags & (OBJBIGDEF | OBJEXEC)) == (OBJEXEC))
                {   /* 16-bit */
                    cb = krnlInterpretProlog16((char*)aKrnlOTE[iObj].ote_base + aImportTab[i].offObject);
                }
                else
                {
                    fprintf(stderr, "Func %d isn't in a code segment.\n", i);
                    rc = 2;
                    break;
                }

                if (cb < 5)
                {
                    fprintf(stderr, "Func %d unknown prolog (obj=0x%x off=0x%x)\n",
                            i, iObj, aImportTab[i].offObject);
                    rc = 3;
                    break;
                }

                /*
                 * Get the opcode.
                 */
                aImportTab[i].chOpcode = *(unsigned char *)(aKrnlOTE[iObj].ote_base + aImportTab[i].offObject);
            } /* for */
        }
        else
        {
            fprintf(stderr, "Invalid object count (%d != %d)\n",
                    cObjects, aImportTab[0].iObject + 1);
            rc = 4;
        }
    }

    /*
     * free kernel and return
     */
    DosFreeModule(hmodKrnl);

    return rc;
}



/**
 * test case 1: Load the specified kernel
 * other cases: Load running kernel.
 * @returns Success indicator. (true/false)
 * @param   iTest   Testcase number.
 * @param   argc    main argc
 * @param   argv    main argv
 * @status  completely implemented.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark  Nearly unchanged from Win32kTst.c.
 */
int     kernelInit(int iTest, int argc, char **argv)
{
    #pragma data_seg(BIGDATA32)
    static char     achBuffer[1024*256];
    char            szError[256];
    int             rc;
    char            szName[CCHMAXPATH];
    char *          pszSrcName;
    char *          pszTmp;
    ULONG           ulAction;
    HFILE           hFile;
    struct e32_exe* pe32 = (struct e32_exe*)(void*)&achBuffer[0];
    qsPtrRec_t *    pPtrRec = (qsPtrRec_t*)(void*)&achBuffer[0];
    qsLrec_t *      pLrec;
    int             i;
    FILESTATUS3     fsts3;

    /*
     * If not testcase 1, use the running kernel.
     */
    if (iTest != 1)
    {
        ULONG   ulBootDrv = 3;
        pszSrcName = "c:\\os2krnl";
        DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, SSToDS(&ulBootDrv), sizeof(ulBootDrv));
        pszSrcName[0] = (char)(ulBootDrv + 'a' - 1);
    }
    else
    {
        if (argc < 3)
        {
            fprintf(stderr, "Missing parameter!\n");
            return FALSE;
        }
        pszSrcName = argv[2];
    }

    /*
     * Make a temporary copy of the kernel.
     */
    if (DosScanEnv("TMP", &pszTmp) != NO_ERROR || pszTmp == NULL)
    {
        fprintf(stderr, "Environment variable TMP is not set.\n");
        return FALSE;
    }
    strcpy(szName, pszTmp);
    if (szName[strlen(pszTmp) - 1] != '\\' && szName[strlen(pszTmp) - 1] != '/')
        strcat(szName, "\\");
    strcat(szName, "os2krnl");

    if (!DosQueryPathInfo(szName, FIL_STANDARD, &fsts3, sizeof(fsts3))) /* delete any existing file (even read-only). */
    {
        fsts3.attrFile = FILE_ARCHIVED;
        DosSetPathInfo(szName, FIL_STANDARD, &fsts3, sizeof(fsts3), 0);
        DosForceDelete(szName);
    }
    rc = DosCopy(pszSrcName, szName, DCPY_EXISTING);
    if (rc != NO_ERROR)
    {
        if (rc != ERROR_FILE_NOT_FOUND && rc != ERROR_PATH_NOT_FOUND)
            fprintf(stderr, "Failed to copy %s to %s. rc=%d\n", pszSrcName, szName, rc);
        return FALSE;
    }
    if (DosQueryPathInfo(szName, FIL_STANDARD, &fsts3, sizeof(fsts3)) != NO_ERROR
        ||  !(fsts3.attrFile = FILE_ARCHIVED)
        ||  DosSetPathInfo(szName, FIL_STANDARD, &fsts3, sizeof(fsts3), 0) != NO_ERROR
        )
    {
        fprintf(stderr, "Failed to set attributes for %s.\n", szName);
        return FALSE;
    }

    /*
     * Patch the kernel.
     *      Remove the entrypoint.
     */
    ulAction = 0;
    rc = DosOpen(szName, &hFile, &ulAction, 0, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
                 NULL);
    if (rc != NO_ERROR)
    {
        fprintf(stderr, "Failed to open temporary kernel file. rc = %d\n", rc);
        return FALSE;
    }
    rc = DosRead(hFile, &achBuffer[0], 0x200, &ulAction);
    if (rc != NO_ERROR)
    {
        DosClose(hFile);
        fprintf(stderr, "Failed to read LX header from temporary kernel file.\n");
        return FALSE;
    }
    pe32 = (struct e32_exe*)(void*)&achBuffer[*(unsigned long*)(void*)&achBuffer[0x3c]];
    if (*(PUSHORT)pe32->e32_magic != E32MAGIC)
    {
        DosClose(hFile);
        fprintf(stderr, "Failed to read LX header from temporary kernel file (2).\n");
        return FALSE;
    }
    pe32->e32_eip = 0;
    pe32->e32_startobj = 0;
    pe32->e32_mflags &= ~(E32LIBTERM | E32LIBINIT);
    if ((rc = DosSetFilePtr(hFile, *(unsigned long*)(void*)&achBuffer[0x3c], FILE_BEGIN, &ulAction)) != NO_ERROR
        || (rc = DosWrite(hFile, pe32, sizeof(struct e32_exe), &ulAction)) != NO_ERROR)
    {
        DosClose(hFile);
        fprintf(stderr, "Failed to write patched LX header to temporary kernel file.\n");
        return FALSE;
    }
    DosClose(hFile);

    /*
     * Load the module.
     */
    rc = DosLoadModule(szError, sizeof(szError), szName, SSToDS(&hmodKrnl));
    if (rc != NO_ERROR && (rc != ERROR_INVALID_PARAMETER && hmodKrnl == NULLHANDLE))
    {
        fprintf(stderr, "Failed to load OS/2 kernel image %s.");
        return FALSE;
    }

    /*
     * Get object information.
     */
    rc = DosQuerySysState(QS_MTE, QS_MTE, 0L, 0L, pPtrRec, sizeof(achBuffer));
    if (rc != NO_ERROR)
    {
        fprintf(stderr, "DosQuerySysState failed with rc=%d.\n", rc);
        return FALSE;
    }

    pLrec = pPtrRec->pLibRec;
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
            pLrec->pObjInfo = (qsLObjrec_t*)(void*)(
                (char*)(void*)pLrec
                + ((sizeof(qsLrec_t)                            /* size of the lib record */
                   + pLrec->ctImpMod * sizeof(short)            /* size of the array of imported modules */
                   + strlen((char*)(void*)pLrec->pName) + 1     /* size of the filename */
                   + 3) & ~3));                                 /* the size is align on 4 bytes boundrary */
            pLrec->pNextRec = (qsLrec_t*)(void*)((char*)(void*)pLrec->pObjInfo
                                                 + sizeof(qsLObjrec_t) * pLrec->ctObj);
            }
        if (pLrec->hmte == hmodKrnl)
            break;

        /*
         * Next record
         */
        pLrec = (qsLrec_t*)pLrec->pNextRec;
    }

    if (pLrec == NULL)
    {
        fprintf(stderr, "DosQuerySysState(os2krnl): not found\n");
        return FALSE;
    }
    if (pLrec->pObjInfo == NULL)
    {
        fprintf(stderr, "DosQuerySysState(os2krnl): no object info\n");
        return FALSE;
    }

    /*
     * Fill the aKrnlOTE array.
     */
    for (i = 0; i < pLrec->ctObj; i++)
    {
        aKrnlOTE[i].ote_size    = pLrec->pObjInfo[i].osize;
        aKrnlOTE[i].ote_base    = pLrec->pObjInfo[i].oaddr;
        aKrnlOTE[i].ote_flags   = pLrec->pObjInfo[i].oflags;
        aKrnlOTE[i].ote_pagemap = i > 0 ? aKrnlOTE[i-1].ote_pagemap + aKrnlOTE[i-1].ote_mapsize : 0;
        aKrnlOTE[i].ote_mapsize = (pLrec->pObjInfo[i].osize + 0x0FFF) / 0x1000;
        aKrnlOTE[i].ote_sel     = (USHORT)FlatToSel(pLrec->pObjInfo[i].oaddr);
        aKrnlOTE[i].ote_hob     = 0;
    }
    cObjects = pLrec->ctObj;

    return TRUE;
}
